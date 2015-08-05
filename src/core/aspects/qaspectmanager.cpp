/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaspectmanager_p.h"
#include "qabstractaspect.h"
#include "qabstractaspect_p.h"
#include "qchangearbiter_p.h"
// TODO Make the kind of job manager configurable (e.g. ThreadWeaver vs Intel TBB)
#include "qaspectjobmanager_p.h"
#include "qabstractaspectjobmanager_p.h"
#include "qentity.h"

#include <Qt3DCore/qservicelocator.h>

#include <Qt3DCore/private/qtickclockservice_p.h>
#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qscheduler_p.h>
#include <Qt3DCore/private/qtickclock_p.h>
#include <Qt3DCore/qabstractframeadvanceservice.h>
#include <QEventLoop>
#include <QThread>
#include <QWaitCondition>
#include <QSurface>


QT_BEGIN_NAMESPACE

namespace Qt3D {

QAspectManager::QAspectManager(QObject *parent)
    : QObject(parent)
    , m_root(Q_NULLPTR)
    , m_scheduler(new QScheduler(this))
    , m_jobManager(new QAspectJobManager(this))
    , m_changeArbiter(new QChangeArbiter(this))
    , m_serviceLocator(new QServiceLocator())
    , m_waitForEndOfExecLoop(0)
{
    qRegisterMetaType<QSurface *>("QSurface*");
    m_runMainLoop.fetchAndStoreOrdered(0);
    m_terminated.fetchAndStoreOrdered(0);
    qCDebug(Aspects) << Q_FUNC_INFO;
}

QAspectManager::~QAspectManager()
{
    delete m_changeArbiter;
    delete m_jobManager;
    delete m_scheduler;
}

bool QAspectManager::isShuttingDown() const
{
    return !m_runMainLoop.load();
}

void QAspectManager::initialize()
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    m_jobManager->initialize();
    m_scheduler->setAspectManager(this);
    m_changeArbiter->initialize(m_jobManager);
}

void QAspectManager::shutdown()
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    Q_FOREACH (QAbstractAspect *aspect, m_aspects) {
        aspect->onCleanup();
        m_changeArbiter->unregisterSceneObserver(aspect);
    }
    // Aspects must be deleted in the Thread they were created in
}

void QAspectManager::setRootEntity(Qt3D::QEntity *root)
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    if (root == m_root)
        return;

    if (m_root) {
        // Allow each aspect chance to cleanup any resources from this scene
        Q_FOREACH (QAbstractAspect *aspect, m_aspects) {
            aspect->onCleanup();
        }
    }

    m_root = root;

    if (m_root) {
        Q_FOREACH (QAbstractAspect *aspect, m_aspects)
            aspect->registerAspect(m_root);
        m_runMainLoop.fetchAndStoreOrdered(1);
    }
}

// Should be called after aspects are registered
void QAspectManager::setData(const QVariantMap &data)
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    m_data = data;
    Q_FOREACH (QAbstractAspect *aspect, m_aspects)
        aspect->onInitialize(m_data);
}

/*!
 * Registers a new \a aspect.
 */
void QAspectManager::registerAspect(QAbstractAspect *aspect)
{
    qCDebug(Aspects) << "Registering aspect";

    if (aspect != Q_NULLPTR) {
        m_aspects.append(aspect);
        QAbstractAspectPrivate::get(aspect)->m_aspectManager = this;
        QAbstractAspectPrivate::get(aspect)->m_jobManager = m_jobManager;
        QAbstractAspectPrivate::get(aspect)->m_arbiter = m_changeArbiter;
        // Register sceneObserver with the QChangeArbiter
        m_changeArbiter->registerSceneObserver(aspect);
        aspect->onInitialize(m_data);
    }
    else {
        qCWarning(Aspects) << "Failed to register aspect";
    }
    qCDebug(Aspects) << "Completed registering aspect";
}

QVariantMap QAspectManager::data() const
{
    return m_data;
}

void QAspectManager::exec()
{
    // Gentlemen, start your engines
    QEventLoop eventLoop;

    // Enter the main loop
    while (!m_terminated.load())
    {
        // Process any pending events, waiting for more to arrive if queue is empty
        eventLoop.processEvents(QEventLoop::WaitForMoreEvents, 16);

        // Retrieve the frame advance service. Defaults to timer based if there is no renderer.
        QAbstractFrameAdvanceService *frameAdvanceService =
                m_serviceLocator->service<QAbstractFrameAdvanceService>(QServiceLocator::FrameAdvanceService);

        // Start the frameAdvanceService if we're about to enter the running loop
        bool needsShutdown = false;
        if (m_runMainLoop.load()) {
            needsShutdown = true;
            frameAdvanceService->start();

            // We are about to enter the main loop. Give aspects a chance to do any last
            // pieces of initialization
            qCDebug(Aspects) << "Calling onStartup() for each aspect";
            Q_FOREACH (QAbstractAspect *aspect, m_aspects)
                aspect->onStartup();
        }

        // Only enter main render loop once the renderer and other aspects are initialized
        while (m_runMainLoop.load())
        {
            qint64 t = frameAdvanceService->waitForNextFrame();

            // For each Aspect
            // Ask them to launch set of jobs for the current frame
            // Updates matrices, bounding volumes, render bins ...
            m_scheduler->update(t);

            // Distribute accumulated changes
            m_changeArbiter->syncChanges();

            // Process any pending events
            eventLoop.processEvents();
        }

        if (needsShutdown) {
            // Give aspects a chance to perform any shutdown actions. This may include unqueuing
            // any blocking work on the main thread that could potentially deadlock during shutdown.
            qCDebug(Aspects) << "Calling onShutdown() for each aspect";
            Q_FOREACH (QAbstractAspect *aspect, m_aspects)
                aspect->onShutdown();
        }
    }
    qCDebug(Aspects) << Q_FUNC_INFO << "Exiting event loop";

    m_waitForEndOfExecLoop.release(1);
}

void QAspectManager::quit()
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    m_runMainLoop.fetchAndStoreOrdered(0);
    m_terminated.fetchAndStoreOrdered(1);

    // Allow the Aspect thread to proceed in case it's locked by the
    // FrameAdvanceService <=> it is still in the running loop
    QAbstractFrameAdvanceService *advanceFrameService = m_serviceLocator->service<QAbstractFrameAdvanceService>(QServiceLocator::FrameAdvanceService);
    if (advanceFrameService)
        advanceFrameService->stop();

    // We need to wait for the QAspectManager exec loop to terminate
    m_waitForEndOfExecLoop.acquire(1);
    qCDebug(Aspects) << Q_FUNC_INFO << "Exited event loop";
}

const QList<QAbstractAspect *> &QAspectManager::aspects() const
{
    return m_aspects;
}

QAbstractAspectJobManager *QAspectManager::jobManager() const
{
    return m_jobManager;
}

QChangeArbiter *QAspectManager::changeArbiter() const
{
    return m_changeArbiter;
}

QServiceLocator *QAspectManager::serviceLocator() const
{
    return m_serviceLocator.data();
}

} // namespace Qt3D

QT_END_NAMESPACE

