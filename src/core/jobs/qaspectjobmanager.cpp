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

#include "qaspectjobmanager.h"
#include "qaspectjobmanager_p.h"
#include "task_p.h"
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
    \class Qt3D::QAspectJobManagerPrivate
    \internal
*/
QAspectJobManagerPrivate::QAspectJobManagerPrivate()
    : QAbstractAspectJobManagerPrivate()
#ifdef THREAD_WEAVER
    , m_weaver(Q_NULLPTR)
#endif
{
}

QAspectJobManagerPrivate::~QAspectJobManagerPrivate()
{
    delete m_dependencyHandler;
}

QAspectJobManager::QAspectJobManager(QObject *parent)
    : QAbstractAspectJobManager(*new QAspectJobManagerPrivate, parent)
{
    Q_D(QAspectJobManager);
#ifdef THREAD_WEAVER
    d->m_weaver = new ThreadWeaver::Queue(this);
    d->m_weaver->setMaximumNumberOfThreads(QThread::idealThreadCount());
#else
    d->m_threadPooler = new QThreadPooler(this);
    d->m_dependencyHandler = new DependencyHandler();
    d->m_threadPooler->setDependencyHandler(d->m_dependencyHandler);
#endif
}

/*! \internal */
QAspectJobManager::QAspectJobManager(QAspectJobManagerPrivate &dd, QObject *parent)
    : QAbstractAspectJobManager(dd, parent)
{
    Q_D(QAspectJobManager);
#ifdef THREAD_WEAVER
    d->m_weaver = new ThreadWeaver::Queue(this);
    d->m_weaver->setMaximumNumberOfThreads(QThread::idealThreadCount());
#else
    d->m_threadPooler = new QThreadPooler(this);
    d->m_dependencyHandler = new DependencyHandler();
    d->m_threadPooler->setDependencyHandler(d->m_dependencyHandler);
#endif
}

void QAspectJobManager::initialize()
{
}

void QAspectJobManager::enqueueJobs(const QVector<QAspectJobPtr> &jobQueue)
{
    Q_D(QAspectJobManager);

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
        d->m_weaver->enqueue(weaverJob);
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
                taskDepender->setDependencyHandler(d->m_dependencyHandler);
                taskDependee->setDependencyHandler(d->m_dependencyHandler);
            }
        }
    }
    d->m_dependencyHandler->addDependencies(qMove(dependencyList));

    d->m_threadPooler->mapDependables(taskList);
#endif
}

void QAspectJobManager::waitForAllJobs()
{
    Q_D(QAspectJobManager);
#ifdef THREAD_WEAVER
    d->m_weaver->finish();
#else
    QFutureWatcher<void> futureWatcher;
    futureWatcher.setFuture(d->m_threadPooler->future());
    futureWatcher.waitForFinished();
#endif
}

void QAspectJobManager::waitForPerThreadFunction(JobFunction func, void *arg)
{
    Q_D(QAspectJobManager);

#ifdef THREAD_WEAVER
    const int threadCount = d->m_weaver->maximumNumberOfThreads();
    QAtomicInt atomicCount(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        SynchronizedJobPtr syncJob(new SynchronizedJob(func, arg, &atomicCount));
        d->m_weaver->enqueue(syncJob);
    }

    d->m_weaver->finish();
#else
    const int threadCount = QThread::idealThreadCount();
    QAtomicInt atomicCount(threadCount);

    QVector<RunnableInterface *> taskList;
    for (int i = 0; i < threadCount; ++i) {
        SyncTaskRunnable *syncTask = new SyncTaskRunnable(func, arg, &atomicCount);
        taskList << syncTask;
    }

    QFuture<void> future = d->m_threadPooler->mapDependables(taskList);
    QFutureWatcher<void> futureWatcher;
    futureWatcher.setFuture(future);
    futureWatcher.waitForFinished();
#endif
}

} // namespace Qt3D

QT_END_NAMESPACE
