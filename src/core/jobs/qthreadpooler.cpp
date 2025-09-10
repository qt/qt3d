// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qthreadpooler_p.h"
#include "qaspectjobmanager_p.h"
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QThreadPooler::QThreadPooler(QObject *parent)
    : QObject(parent)
    , m_futureInterface(nullptr)
    , m_mutex()
    , m_taskCount(0)
    , m_threadPool(new QThreadPool(this))
    , m_totalRunJobs(0)
{
    m_threadPool->setMaxThreadCount(QAspectJobManager::idealThreadCount());
    // Ensures that threads will never be recycled
    m_threadPool->setExpiryTimeout(-1);
}

QThreadPooler::~QThreadPooler()
{
    // Wait till all tasks are finished before deleting mutex
    QMutexLocker locker(&m_mutex);
    locker.unlock();
}

void QThreadPooler::enqueueTasks(const QList<RunnableInterface *> &tasks)
{
    // The caller have to set the mutex
    const QList<RunnableInterface *>::const_iterator end = tasks.cend();

    m_totalRunJobs = 0;
    for (QList<RunnableInterface *>::const_iterator it = tasks.cbegin();
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

QFuture<void> QThreadPooler::mapDependables(QList<RunnableInterface *> &taskQueue)
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

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qthreadpooler_p.cpp"
