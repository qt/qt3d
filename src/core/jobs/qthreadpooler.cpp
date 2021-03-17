/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qthreadpooler_p.h"
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QThreadPooler::QThreadPooler(QObject *parent)
    : QObject(parent)
    , m_futureInterface(nullptr)
    , m_mutex()
    , m_taskCount(0)
    , m_threadPool(QThreadPool::globalInstance())
    , m_totalRunJobs(0)
{
    m_threadPool->setMaxThreadCount(QThreadPooler::maxThreadCount());
    // Ensures that threads will never be recycled
    m_threadPool->setExpiryTimeout(-1);
}

QThreadPooler::~QThreadPooler()
{
    // Wait till all tasks are finished before deleting mutex
    QMutexLocker locker(&m_mutex);
    locker.unlock();
}

void QThreadPooler::enqueueTasks(const QVector<RunnableInterface *> &tasks)
{
    // The caller have to set the mutex
    const QVector<RunnableInterface *>::const_iterator end = tasks.cend();

    m_totalRunJobs = 0;
    for (QVector<RunnableInterface *>::const_iterator it = tasks.cbegin();
         it != end; ++it) {

        // Only AspectTaskRunnables are checked for dependencies.
        static const auto hasDependencies = [](RunnableInterface *task) -> bool {
            return (task->type() == RunnableInterface::RunnableType::AspectTask)
                    && (static_cast<AspectTaskRunnable *>(task)->m_dependerCount > 0);
        };

        if (!hasDependencies(*it) && !(*it)->reserved()) {
            (*it)->setReserved(true);
            if ((*it)->isRequired()) {
                (*it)->setPooler(this);
                m_threadPool->start((*it));
            } else {
                skipTask(*it);
            }
        }
    }
}

void QThreadPooler::skipTask(RunnableInterface *task)
{
    enqueueDepencies(task);

    if (currentCount() == 0) {
        if (m_futureInterface) {
            m_futureInterface->reportFinished();
            delete m_futureInterface;
        }
        m_futureInterface = nullptr;
    }

    delete task; // normally gets deleted by threadpool
}

void QThreadPooler::enqueueDepencies(RunnableInterface *task)
{
    release();

    if (task->type() == RunnableInterface::RunnableType::AspectTask) {
        AspectTaskRunnable *aspectTask = static_cast<AspectTaskRunnable *>(task);
        const auto &dependers = aspectTask->m_dependers;
        for (auto it = dependers.begin(); it != dependers.end(); ++it) {
            AspectTaskRunnable *dependerTask = static_cast<AspectTaskRunnable *>(*it);
            if (--dependerTask->m_dependerCount == 0) {
                if (!dependerTask->reserved()) {
                    dependerTask->setReserved(true);
                    if ((*it)->isRequired()) {
                        dependerTask->setPooler(this);
                        m_threadPool->start(dependerTask);
                    } else {
                        skipTask(*it);
                    }
                }
            }
        }
    }
}

void QThreadPooler::taskFinished(RunnableInterface *task)
{
    const QMutexLocker locker(&m_mutex);

    m_totalRunJobs++;

    enqueueDepencies(task);

    if (currentCount() == 0) {
        if (m_futureInterface) {
            m_futureInterface->reportFinished();
            delete m_futureInterface;
        }
        m_futureInterface = nullptr;
    }
}

QFuture<void> QThreadPooler::mapDependables(QVector<RunnableInterface *> &taskQueue)
{
    const QMutexLocker locker(&m_mutex);

    if (!m_futureInterface)
        m_futureInterface = new QFutureInterface<void>();
    if (!taskQueue.empty())
        m_futureInterface->reportStarted();

    acquire(taskQueue.size());
    enqueueTasks(taskQueue);

    return QFuture<void>(m_futureInterface);
}

int QThreadPooler::waitForAllJobs()
{
    future().waitForFinished();
    return m_totalRunJobs;
}

QFuture<void> QThreadPooler::future()
{
    const QMutexLocker locker(&m_mutex);

    if (!m_futureInterface)
        return QFuture<void>();
    else
        return QFuture<void>(m_futureInterface);
}

void QThreadPooler::acquire(int add)
{
    // The caller have to set the mutex

    m_taskCount.fetchAndAddOrdered(add);
}

void QThreadPooler::release()
{
    // The caller have to set the mutex

    m_taskCount.fetchAndAddOrdered(-1);
}

int QThreadPooler::currentCount() const
{
    // The caller have to set the mutex

    return m_taskCount.loadRelaxed();
}

int QThreadPooler::maxThreadCount()
{
    static int threadCount = 0;

    if (threadCount == 0) {
        threadCount = QThread::idealThreadCount();
        const QByteArray maxThreadCount = qgetenv("QT3D_MAX_THREAD_COUNT");
        if (!maxThreadCount.isEmpty()) {
            bool conversionOK = false;
            const int maxThreadCountValue = maxThreadCount.toInt(&conversionOK);
            if (conversionOK)
                threadCount = std::min(threadCount, maxThreadCountValue);
        }
    }

    return threadCount;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
