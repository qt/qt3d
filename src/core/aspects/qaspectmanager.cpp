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

#include "qaspectmanager.h"

#include "qabstractaspect.h"
#include "qchangearbiter.h"
// TODO Make the kind of job manager configurable (e.g. ThreadWeaver vs Intel TBB)
#include "qjobmanager.h"
#include "qjobmanagerinterface.h"
#include "qscheduler.h"
#include "qtickclock.h"
#include "qentity.h"

#include "corelogging.h"
#include <QEventLoop>
#include <QThread>
#include <QWaitCondition>
#include <QWindow>

#include <private/qaspectmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAspectManagerPrivate::QAspectManagerPrivate(QAspectManager *qq)
    : QObjectPrivate()
    , m_root(Q_NULLPTR)
    , m_window(Q_NULLPTR)
{
    m_runMainLoop.fetchAndStoreOrdered(0);
    m_terminated.fetchAndStoreOrdered(0);
    q_ptr = qq;
    qRegisterMetaType<QWindow*>("QWindow*");
}

QAspectManager::QAspectManager(QObject *parent)
    : QObject(*new QAspectManagerPrivate(this), parent)
{
    Q_D(QAspectManager);
    d->m_scheduler = new QScheduler(this);
    d->m_jobManager = new QJobManager(this);
    d->m_changeArbiter = new QChangeArbiter(this);
    qCDebug(Aspects) << Q_FUNC_INFO;
}

QAspectManager::~QAspectManager()
{
    Q_D(QAspectManager);
    delete d->m_changeArbiter;
    delete d->m_jobManager;
    delete d->m_scheduler;
}

QAspectManager::QAspectManager(QAspectManagerPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QAspectManager);
    d->m_scheduler = new QScheduler(this);
    d->m_jobManager = new QJobManager(this);
    d->m_changeArbiter = new QChangeArbiter(this);
    qCDebug(Aspects) << Q_FUNC_INFO;
}

void QAspectManager::initialize()
{
    Q_D(QAspectManager);
    qCDebug(Aspects) << Q_FUNC_INFO;
    d->m_jobManager->initialize();
    d->m_scheduler->setAspectManager(this);
    d->m_changeArbiter->initialize(d->m_jobManager);
}

void QAspectManager::shutdown()
{
    Q_D(QAspectManager);
    qCDebug(Aspects) << Q_FUNC_INFO;
    Q_FOREACH (QAbstractAspect *aspect, d->m_aspects) {
        aspect->unregisterAspect(d->m_root);
        aspect->cleanup();
        d->m_changeArbiter->unregisterSceneObserver(aspect->sceneObserver());
    }
    qDeleteAll(d->m_aspects);
}


void QAspectManager::setRoot(QObject *rootObject)
{
    Q_D(QAspectManager);
    qCDebug(Aspects) << Q_FUNC_INFO;

    QEntity *root = qobject_cast<QEntity *>(rootObject);

    if (!root)
        qWarning() << "Root object is not an Entity";

    if (root == d->m_root)
        return;

    if (d->m_root) {
        // Allow each aspect chance to cleanup any resources from this scene
        Q_FOREACH (QAbstractAspect *aspect, d->m_aspects)
            aspect->unregisterAspect(d->m_root);

        // Allow each aspect chance to cleanup any scene-independent resources
        Q_FOREACH (QAbstractAspect *aspect, d->m_aspects)
            aspect->cleanup();

        // Destroy all aspects
        qDeleteAll(d->m_aspects);
        d->m_aspects.clear();

        d->m_root = Q_NULLPTR;
    }

    d->m_root = root;

    if (d->m_root) {

        Q_FOREACH (QAbstractAspect *aspect, d->m_aspects)
            aspect->registerAspect(d->m_root);

        d->m_runMainLoop.fetchAndStoreOrdered(1);
    }
}

// Called before register aspect
void QAspectManager::setWindow(QWindow *window)
{
    Q_D(QAspectManager);
    qCDebug(Aspects) << Q_FUNC_INFO;
    d->m_window = window;
    // We need to create the window
    // Otherwise aspects won't be able to initialize the glContext
    // As show (which calls create) is only called after they're initialized
    d->m_window->create();
}

/*!
 * Registers a new \a aspect.
 */
void QAspectManager::registerAspect(QAbstractAspect *aspect)
{
    Q_D(QAspectManager);
    qCDebug(Aspects) << "Registering aspect";

    if (aspect != Q_NULLPTR) {
        d->m_aspects.append(aspect);
        aspect->initialize(this);
        // Register sceneObserver with the QChangeArbiter
        d->m_changeArbiter->registerSceneObserver(aspect->sceneObserver());
    }
    else {
        qCWarning(Aspects) << "Failed to register aspect";
    }
    qCDebug(Aspects) << "Completed registering aspect";
}

QWindow *QAspectManager::window() const
{
    Q_D(const QAspectManager);
    return d->m_window;
}

void QAspectManager::exec()
{
    Q_D(QAspectManager);
    // Gentlemen, start your engines
    QEventLoop eventLoop;

    //    QElapsedTimer timer;
    //    timer.start();
    //    qint64 t(0);
    QTickClock tickClock;
    tickClock.start();

    // Enter the main loop
    while (!d->m_terminated.load())
    {
        // Process any pending events, waiting for more to arrive if queue is empty
        eventLoop.processEvents(QEventLoop::WaitForMoreEvents, 16);

        // Only enter main render loop once the renderer and other aspects are initialized
        while (d->m_runMainLoop.load())
        {
            // Process any pending events
            eventLoop.processEvents();

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
            d->m_scheduler->update(t);

            // Distribute accumulated changes
            d->m_changeArbiter->syncChanges();
        }
    }
}

void QAspectManager::quit()
{
    Q_D(QAspectManager);
    d->m_runMainLoop.fetchAndStoreOrdered(0);
    d->m_terminated.fetchAndStoreOrdered(1);
}

const QList<QAbstractAspect *> &QAspectManager::aspects() const
{
    Q_D(const QAspectManager);
    return d->m_aspects;
}

QJobManagerInterface *QAspectManager::jobManager() const
{
    Q_D(const QAspectManager);
    return d->m_jobManager;
}

QChangeArbiter *QAspectManager::changeArbiter() const
{
    Q_D(const QAspectManager);
    return d->m_changeArbiter;
}

} // namespace Qt3D

QT_END_NAMESPACE

