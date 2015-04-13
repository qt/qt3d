/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#include "qthreadpooler_p.h"
#include "qthreadpooler_p_p.h"
#include "jobrunner_p.h"
#include "dependencyhandler_p.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QThreadPoolerPrivate
    \internal
*/
QThreadPoolerPrivate::QThreadPoolerPrivate(QThreadPooler *qq)
    : QObjectPrivate(),
      m_mutex(new QMutex(QMutex::NonRecursive)),
      m_runningThreads(0)
{
    q_ptr = qq;
}

QThreadPoolerPrivate::~QThreadPoolerPrivate()
{
    Q_FOREACH (QSharedPointer<TaskInterface> task, m_taskQueue)
        task->setDependencyHandler(Q_NULLPTR);
    delete m_dependencyHandler;

    delete m_mutex;
}

void QThreadPoolerPrivate::shutdown()
{
    m_jobFinished.wakeAll();

    // When shutting down a signal is send for jobrunners to exit run() loop
    // on next round. Sometimes the jobrunner is busy doing still the clean up
    // tasks and isn't waiting the release of WaitCondition. Repeat the waking
    // process max tryOuts.
    const int tryOuts = 2;

    Q_FOREACH (JobRunner *jr, m_workers) {
        if (!jr->isFinished()) {
            for (int i = 0; i < tryOuts; i++) {
                m_jobAvailable.wakeAll();
                if (jr->wait(100))
                    break;
            }
        }
    }
}


bool QThreadPoolerPrivate::isQueueEmpty()
{
    return m_taskQueue.isEmpty();
}

void QThreadPoolerPrivate::incRunningThreads()
{
    m_runningThreads += 1;
}

void QThreadPoolerPrivate::decRunningThreads()
{
    m_runningThreads -= 1;

    // Sanity check
    if (m_runningThreads < 0)
        m_runningThreads = 0;
}

void QThreadPoolerPrivate::createRunners(int threadCount)
{
    Q_Q(QThreadPooler);

    for (int i = 0; i < threadCount; i++) {
        JobRunner *jr = new JobRunner(q);

        jr->setMutex(m_mutex);
        jr->setWaitConditions(&(m_jobAvailable));
        jr->start();

        m_workers.append(jr);
    }
}

int QThreadPoolerPrivate::maxThreadCount() const
{
    return m_maxThreadCount;
}

void QThreadPoolerPrivate::setMaxThreadCount(int threadCount)
{

    m_maxThreadCount = threadCount;
    createRunners(m_maxThreadCount);
}

/////////////////////////////////////////////////

QThreadPooler::QThreadPooler(QObject *parent)
    : QObject(*new QThreadPoolerPrivate(this), parent)
{
}

QThreadPooler::~QThreadPooler()
{
    Q_D(QThreadPooler);

    emit shuttingDown();
    d->m_jobAvailable.wakeAll();

    d->shutdown();
}

int QThreadPooler::maxThreadCount() const
{
    Q_D(const QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

    return d->maxThreadCount();
}

void QThreadPooler::setMaxThreadCount(int threadCount)
{
    Q_D(QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

    d->setMaxThreadCount(threadCount);
}

QSharedPointer<TaskInterface> QThreadPooler::nextTask()
{
    Q_D(QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

    QSharedPointer<TaskInterface> task;
    int queueSize = d->m_taskQueue.size();
    for (int i = 0; i < queueSize; i++) {
        const QSharedPointer<TaskInterface> &candidate = d->m_taskQueue.at(i);
        if (!hasDependencies(candidate)) {
            task = candidate;
            // Increment running thread counter before removing item from queue
            // so that isIdle test keeps up
            d->incRunningThreads();
            d->m_taskQueue.removeAt(i);

            break;
        }
    }

    return task;
}

bool QThreadPooler::hasDependencies(const QSharedPointer<TaskInterface> &task)
{
    DependencyHandler *handler = task->dependencyHandler();
    if (handler)
        return handler->hasDependency(task);

    return false;
}

void QThreadPooler::enqueueTask(const QSharedPointer<TaskInterface> &task)
{
    Q_D(QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

    d->m_taskQueue.append(task);
    d->m_jobAvailable.wakeAll();
}

void QThreadPooler::flush()
{
    Q_D(QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

#ifdef QT_NO_DEBUG
    const int waitTime = 50;
#else
    const int waitTime = 500;
#endif

    while (!isIdling()) {
        if (d->m_jobFinished.wait(d->m_mutex, waitTime) == false)
            d->m_jobAvailable.wakeAll();
    }
}

bool QThreadPooler::isIdling()
{
    Q_D(QThreadPooler);

    return d->isQueueEmpty() && d->m_runningThreads == 0;
}

void QThreadPooler::startRunning()
{
    Q_D(QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

    d->incRunningThreads();
}

void QThreadPooler::stopRunning()
{
    Q_D(QThreadPooler);

    const QMutexLocker locker(d->m_mutex);

    d->decRunningThreads();
    d->m_jobFinished.wakeAll();
}

void QThreadPooler::setDependencyHandler(DependencyHandler *handler)
{
    Q_D(QThreadPooler);

    d->setDependencyHandler(handler);
}

} // namespace Qt3D

QT_END_NAMESPACE
