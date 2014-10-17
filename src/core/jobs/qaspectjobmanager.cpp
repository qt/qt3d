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

#include <threadweaver.h>
#include <dependencypolicy.h>
#include <thread.h>

#include "qaspectjobmanager.h"
#include "job.h"
#include "weaverjob_p.h"
#include "qaspectjobmanager_p.h"

#include <QAtomicInt>
#include <QDebug>
#include <QThread>


QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace {

class SynchronizedJob : public ThreadWeaver::Job
{
public:
    SynchronizedJob(QAspectJobManagerInterface::JobFunction func, void *arg, QAtomicInt *atomicCount)
        : m_func(func)
        , m_arg(arg)
        , m_atomicCount(atomicCount)
    {}

protected:
    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread) Q_DECL_OVERRIDE;

private:
    QAspectJobManagerInterface::JobFunction m_func;
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

QAspectJobManagerPrivate::QAspectJobManagerPrivate(QAspectJobManager *qq)
    : QObjectPrivate()
    , q_ptr(qq)
    , m_weaver(Q_NULLPTR)
{
}

QAspectJobManager::QAspectJobManager(QObject *parent)
    : QAspectJobManagerInterface(parent)
    , d_ptr(new QAspectJobManagerPrivate(this))
{
    Q_D(QAspectJobManager);
    d->m_weaver = new ThreadWeaver::Queue(this);
    d->m_weaver->setMaximumNumberOfThreads(QThread::idealThreadCount());
}

QAspectJobManager::QAspectJobManager(QAspectJobManagerPrivate &dd, QObject *parent)
    : QAspectJobManagerInterface(parent)
    , d_ptr(&dd)
{
    Q_D(QAspectJobManager);
    d->m_weaver = new ThreadWeaver::Queue(this);
    d->m_weaver->setMaximumNumberOfThreads(QThread::idealThreadCount());
}

void QAspectJobManager::initialize()
{
}

void QAspectJobManager::enqueueJobs(const QVector<QAspectJobPtr> &jobQueue)
{
    Q_D(QAspectJobManager);
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
}

void QAspectJobManager::waitForAllJobs()
{
    Q_D(QAspectJobManager);
    d->m_weaver->finish();
}

void QAspectJobManager::waitForPerThreadFunction(JobFunction func, void *arg)
{
    Q_D(QAspectJobManager);
    const int threadCount = d->m_weaver->maximumNumberOfThreads();
    QAtomicInt atomicCount(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        SynchronizedJobPtr syncJob(new SynchronizedJob(func, arg, &atomicCount));
        d->m_weaver->enqueue(syncJob);
    }

    d->m_weaver->finish();
}

} // namespace Qt3D

QT_END_NAMESPACE
