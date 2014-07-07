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

#include "abstractaspect.h"
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

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAspectManager::QAspectManager(QObject *parent)
    : QObject(parent)
    , m_root(0)
    , m_window(0)
    , m_scheduler(new QScheduler(this))
    , m_jobManager(new QJobManager(this))
    , m_changeArbiter(new QChangeArbiter(this))
    , m_runMainLoop(false)
{
    qRegisterMetaType<QWindow*>("QWindow*");
    qCDebug(Aspects) << Q_FUNC_INFO;
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
}


void QAspectManager::setRoot(QObject *rootObject, QWaitCondition *waitCondition)
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    QEntity *root = qobject_cast<QEntity *>(rootObject);

    if (!root)
        qWarning() << "Root object is not an Entity";

    if (root == m_root)
        return;

    if (m_root) {
        // Allow each aspect chance to cleanup any resources from this scene
        Q_FOREACH (AbstractAspect *aspect, m_aspects)
            aspect->unregisterAspect(m_root);

        // Allow each aspect chance to cleanup any scene-independent resources
        Q_FOREACH (AbstractAspect *aspect, m_aspects)
            aspect->cleanup();

        // Destroy all aspects
        qDeleteAll(m_aspects);
        m_aspects.clear();

        m_root = 0;
    }

    m_root = root;

    if (m_root) {

        Q_FOREACH (AbstractAspect *aspect, m_aspects)
            aspect->registerAspect(m_root);

        m_runMainLoop = true;
    }

    if (waitCondition)
        waitCondition->wakeOne();
}

// Called before register aspect
void QAspectManager::setWindow(QWindow *window)
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    m_window = window;
}

/*!
 * Registers a new \a aspect.
 */
void QAspectManager::registerAspect(QObject *aspect)
{
    qCDebug(Aspects) << Q_FUNC_INFO << "Registering aspect libraries";

    AbstractAspect *aspectImpl = Q_NULLPTR;
    if ((aspectImpl = qobject_cast<AbstractAspect*>(aspect)) != Q_NULLPTR) {
        m_aspects.append(aspectImpl);
        aspectImpl->initialize(this);
    }
    else {
       qCWarning(Aspects) << Q_FUNC_INFO << "Failed to register aspect";
    }
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
    while (1)
    {
        // Process any pending events, waiting for more to arrive if queue is empty
        eventLoop.processEvents(QEventLoop::WaitForMoreEvents, 16);

        // Only enter main render loop once the renderer and other aspects are initialized
        while (m_runMainLoop)
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
            m_scheduler->update(t);


            // Distribute accumulated changes
            m_changeArbiter->syncChanges();
        }
    }
}

} // namespace Qt3D

QT_END_NAMESPACE
