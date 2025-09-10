// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaspectjobmanager_p.h"

#include <QtCore/QAtomicInt>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QFuture>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qthreadpooler_p.h>
#include <Qt3DCore/private/task_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAspectJobManager::QAspectJobManager(QAspectManager *parent)
    : QAbstractAspectJobManager(parent)
    , m_threadPooler(new QThreadPooler(this))
    , m_aspectManager(parent)
{
}

QAspectJobManager::~QAspectJobManager()
{
}

void QAspectJobManager::initialize()
{
}

// Adds all Aspect Jobs to be processed for a frame
void QAspectJobManager::enqueueJobs(const std::vector<QAspectJobPtr> &jobQueue)
{
    auto systemService = m_aspectManager ? m_aspectManager->serviceLocator()->systemInformation() : nullptr;
    if (systemService)
        systemService->writePreviousFrameTraces();

    // Convert QJobs to Tasks
    QHash<QAspectJob *, AspectTaskRunnable *> tasksMap;
    QList<RunnableInterface *> taskList;
    taskList.reserve(jobQueue.size());
    for (const QAspectJobPtr &job : jobQueue) {
        AspectTaskRunnable *task = new AspectTaskRunnable(systemService);
        task->m_job = job;
        tasksMap.insert(job.data(), task);

        taskList << task;
    }

    for (const QAspectJobPtr &job : jobQueue) {
        const std::vector<QWeakPointer<QAspectJob> > &deps = job->dependencies();
        AspectTaskRunnable *taskDepender = tasksMap.value(job.data());

        int dependerCount = 0;
        for (const QWeakPointer<QAspectJob> &dep : deps) {
            AspectTaskRunnable *taskDependee = tasksMap.value(dep.toStrongRef().data());
            // The dependencies here are not hard requirements, i.e., the dependencies
            // not in the jobQueue should already have their data ready.
            if (taskDependee) {
                taskDependee->m_dependers.append(taskDepender);
                ++dependerCount;
            }
        }

        taskDepender->m_dependerCount += dependerCount;
    }

    m_threadPooler->mapDependables(taskList);
}

// Wait for all aspects jobs to be completed
int QAspectJobManager::waitForAllJobs()
{
    return m_threadPooler->waitForAllJobs();
}

void QAspectJobManager::waitForPerThreadFunction(JobFunction func, void *arg)
{
    const int threadCount = QAspectJobManager::idealThreadCount();
    QAtomicInt atomicCount(threadCount);

    QList<RunnableInterface *> taskList;
    for (int i = 0; i < threadCount; ++i) {
        SyncTaskRunnable *syncTask = new SyncTaskRunnable(func, arg, &atomicCount);
        taskList << syncTask;
    }

    QFuture<void> future = m_threadPooler->mapDependables(taskList);
    future.waitForFinished();
}


int QAspectJobManager::idealThreadCount()
{
    static int jobCount = 0;
    if (jobCount)
        return jobCount;

    const QByteArray maxThreadCount = qgetenv("QT3D_MAX_THREAD_COUNT");
    if (!maxThreadCount.isEmpty()) {
        bool conversionOK = false;
        const int maxThreadCountValue = maxThreadCount.toInt(&conversionOK);
        if (conversionOK) {
            jobCount = maxThreadCountValue;
            return jobCount;
        }
    }

    jobCount = QThread::idealThreadCount();
    return jobCount;
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qaspectjobmanager_p.cpp"
