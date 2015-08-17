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

#ifdef THREAD_WEAVER
#include <threadweaver.h>
#include <dependencypolicy.h>
#include <thread.h>
#include "job.h"
#include "weaverjob_p.h"
#endif

#include "qaspectjobmanager_p.h"
#include "task_p.h"
#include "qthreadpooler_p.h"
#include "dependencyhandler_p.h"

#include <QAtomicInt>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>

QT_BEGIN_NAMESPACE

namespace Qt3D {

#ifdef THREAD_WEAVER
namespace {

class SynchronizedJob : public ThreadWeaver::Job
{
public:
    SynchronizedJob(QAbstractAspectJobManager::JobFunction func, void *arg, QAtomicInt *atomicCount)
        : m_func(func)
        , m_arg(arg)
        , m_atomicCount(atomicCount)
    {}

protected:
    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) Q_DECL_OVERRIDE;

private:
    QAbstractAspectJobManager::JobFunction m_func;
    void *m_arg;
    QAtomicInt *m_atomicCount;
};

typedef QSharedPointer<SynchronizedJob> SynchronizedJobPtr;

void SynchronizedJob::run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
{
    Q_UNUSED(self);
    Q_CHECK_PTR(m_func);
    Q_CHECK_PTR(thread);

    // Call the function
    m_func(m_arg);

    // Decrement the atomic counter to let others know we've done our bit
    m_atomicCount->deref();

    // Wait for the other worker threads to be done
    while (m_atomicCount->load() > 0)
        thread->yieldCurrentThread();
}

} // anonymous
#endif

/*!
    \class Qt3D::QAspectJobManager
    \internal
*/
QAspectJobManager::QAspectJobManager(QObject *parent)
    : QAbstractAspectJobManager(parent)
#ifdef THREAD_WEAVER
    , m_weaver(Q_NULLPTR)
#endif
    , m_threadPooler(new QThreadPooler(this))
    , m_dependencyHandler(new DependencyHandler)
{
#ifdef THREAD_WEAVER
    m_weaver = new ThreadWeaver::Queue(this);
    m_weaver->setMaximumNumberOfThreads(QThread::idealThreadCount());
#else
    m_threadPooler->setDependencyHandler(m_dependencyHandler.data());
#endif
}

QAspectJobManager::~QAspectJobManager()
{
}

void QAspectJobManager::initialize()
{
}

void QAspectJobManager::enqueueJobs(const QVector<QAspectJobPtr> &jobQueue)
{
#ifdef THREAD_WEAVER
    // Convert QJobs to ThreadWeaver::Jobs
    QHash<QAspectJob *, QSharedPointer<WeaverJob> > jobsMap;
    Q_FOREACH (const QAspectJobPtr &job, jobQueue) {
        QSharedPointer<WeaverJob> weaverJob(new WeaverJob);
        weaverJob->m_job = job;
        jobsMap.insert(job.data(), weaverJob);
    }

    // Resolve dependencies
    Q_FOREACH (const QAspectJobPtr &job, jobQueue) {
        const QVector<QWeakPointer<QAspectJob> > &deps = job->dependencies();

        Q_FOREACH (const QWeakPointer<QAspectJob> &dep, deps) {
            QSharedPointer<WeaverJob> weaverDep = jobsMap.value(dep.data());

            if (weaverDep) {
                ThreadWeaver::DependencyPolicy &dependencyPolicy = ThreadWeaver::DependencyPolicy::instance();
                QSharedPointer<WeaverJob> weaverJob = jobsMap.value(job.data());
                dependencyPolicy.addDependency(weaverJob, weaverDep);
            }
        }
    }

    Q_FOREACH (const QAspectJobPtr &job, jobQueue) {
        QSharedPointer<WeaverJob> weaverJob = jobsMap.value(job.data());
        m_weaver->enqueue(weaverJob);
    }
#else
    // Convert QJobs to Tasks
    QHash<QAspectJob *, AspectTaskRunnable *> tasksMap;
    QVector<RunnableInterface *> taskList;
    Q_FOREACH (const QAspectJobPtr &job, jobQueue) {
        AspectTaskRunnable *task = new AspectTaskRunnable();
        task->m_job = job;
        tasksMap.insert(job.data(), task);

        taskList << task;
    }

    // Resolve dependencies
    QVector<Dependency> dependencyList;

    Q_FOREACH (const QSharedPointer<QAspectJob> &job, jobQueue) {
        const QVector<QWeakPointer<QAspectJob> > &deps = job->dependencies();

        Q_FOREACH (const QWeakPointer<QAspectJob> &dep, deps) {
            AspectTaskRunnable *taskDependee = tasksMap.value(dep.data());

            if (taskDependee) {
                AspectTaskRunnable *taskDepender = tasksMap.value(job.data());
                dependencyList.append(Dependency(taskDepender, taskDependee));
                taskDepender->setDependencyHandler(m_dependencyHandler.data());
                taskDependee->setDependencyHandler(m_dependencyHandler.data());
            }
        }
    }
    m_dependencyHandler->addDependencies(qMove(dependencyList));

    m_threadPooler->mapDependables(taskList);
#endif
}

void QAspectJobManager::waitForAllJobs()
{
#ifdef THREAD_WEAVER
    m_weaver->finish();
#else
    QFutureWatcher<void> futureWatcher;
    futureWatcher.setFuture(m_threadPooler->future());
    futureWatcher.waitForFinished();
#endif
}

void QAspectJobManager::waitForPerThreadFunction(JobFunction func, void *arg)
{
#ifdef THREAD_WEAVER
    const int threadCount = m_weaver->maximumNumberOfThreads();
    QAtomicInt atomicCount(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        SynchronizedJobPtr syncJob(new SynchronizedJob(func, arg, &atomicCount));
        m_weaver->enqueue(syncJob);
    }

    m_weaver->finish();
#else
    const int threadCount = m_threadPooler->maxThreadCount();
    QAtomicInt atomicCount(threadCount);

    QVector<RunnableInterface *> taskList;
    for (int i = 0; i < threadCount; ++i) {
        SyncTaskRunnable *syncTask = new SyncTaskRunnable(func, arg, &atomicCount);
        taskList << syncTask;
    }

    QFuture<void> future = m_threadPooler->mapDependables(taskList);
    QFutureWatcher<void> futureWatcher;
    futureWatcher.setFuture(future);
    futureWatcher.waitForFinished();
#endif
}

} // namespace Qt3D

QT_END_NAMESPACE
