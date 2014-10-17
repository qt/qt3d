/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaspectmanager_p.h"
#include "qabstractaspect.h"
#include "qabstractaspect_p.h"
#include "qchangearbiter_p.h"
// TODO Make the kind of job manager configurable (e.g. ThreadWeaver vs Intel TBB)
#include "qjobmanager.h"
#include "qaspectjobmanagerinterface.h"
#include "qscheduler.h"
#include "qtickclock.h"
#include "qentity.h"

#include "corelogging.h"
#include <QEventLoop>
#include <QThread>
#include <QWaitCondition>
#include <QSurface>


QT_BEGIN_NAMESPACE

namespace Qt3D {

QAspectManager::QAspectManager(QObject *parent)
    : QObject(parent)
    , m_root(Q_NULLPTR)
    , m_surface(Q_NULLPTR)
    , m_scheduler(new QScheduler(this))
    , m_jobManager(new QJobManager(this))
    , m_changeArbiter(new QChangeArbiter(this))
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
    qDeleteAll(m_aspects);
}

void QAspectManager::setRoot(QNode *rootObject)
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    QEntity *root = qobject_cast<QEntity *>(rootObject);

    if (!root)
        qWarning() << "Root object is not an Entity";

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

// Called before register aspect
void QAspectManager::setSurface(QSurface *surface)
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    m_surface = surface;
    Q_FOREACH (QAbstractAspect *aspect, m_aspects)
        aspect->onInitialize(m_surface);
}

/*!
 * Registers a new \a aspect.
 */
void QAspectManager::registerAspect(QAbstractAspect *aspect)
{
    qCDebug(Aspects) << "Registering aspect";

    if (aspect != Q_NULLPTR) {
        m_aspects.append(aspect);
        QAbstractAspectPrivate::get(aspect)->m_jobManager = m_jobManager;
        QAbstractAspectPrivate::get(aspect)->m_arbiter = m_changeArbiter;
        // Register sceneObserver with the QChangeArbiter
        m_changeArbiter->registerSceneObserver(aspect);
        if (m_surface != Q_NULLPTR)
            aspect->onInitialize(m_surface);
    }
    else {
        qCWarning(Aspects) << "Failed to register aspect";
    }
    qCDebug(Aspects) << "Completed registering aspect";
}

QSurface *QAspectManager::surface() const
{
    return m_surface;
}

void QAspectManager::exec()
{
    // Gentlemen, start your engines
    QEventLoop eventLoop;

    //    QElapsedTimer timer;
    //    timer.start();
    //    qint64 t(0);
    QTickClock tickClock;
    tickClock.start();

    // Enter the main loop
    while (!m_terminated.load())
    {
        // Process any pending events, waiting for more to arrive if queue is empty
        eventLoop.processEvents(QEventLoop::WaitForMoreEvents, 16);

        // Only enter main render loop once the renderer and other aspects are initialized
        while (m_runMainLoop.load())
        {
            // Update the clocks (just main clock for now).
            // TODO: Add additional clocks
            qint64 t = tickClock.waitForNextTick();
            //        qDebug() << "t =" << t / 1000000;
            //            const qint64 t1 = timer.nsecsElapsed();
            //            const qint64 dt = t1 - t;
            //            t = t1;
            //            qDebug() << "dt =" << dt;

            // For each Aspect
            // Ask them to launch set of jobs for the current frame
            // Updates matrices, bounding volumes, render bins ...
            m_scheduler->update(t);

            // Distribute accumulated changes
            m_changeArbiter->syncChanges();

            // Process any pending events
            eventLoop.processEvents();
        }
    }
}

void QAspectManager::quit()
{
    m_runMainLoop.fetchAndStoreOrdered(0);
    m_terminated.fetchAndStoreOrdered(1);
}

const QList<QAbstractAspect *> &QAspectManager::aspects() const
{
    return m_aspects;
}

QAspectJobManagerInterface *QAspectManager::jobManager() const
{
    return m_jobManager;
}

QChangeArbiter *QAspectManager::changeArbiter() const
{
    return m_changeArbiter;
}

} // namespace Qt3D

QT_END_NAMESPACE

