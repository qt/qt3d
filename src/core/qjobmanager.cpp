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

#include "qjobmanager.h"

#include "weaverjob_p.h"

#include <threadweaver.h>
#include <dependencypolicy.h>
#include <thread.h>

#include <QAtomicInt>
#include <QDebug>
#include <QThread>

namespace Qt3D {

QJobManager::QJobManager(QObject *parent)
    : QJobManagerInterface(parent)
    , m_weaver(new ThreadWeaver::Queue(this))
{
    m_weaver->setMaximumNumberOfThreads(QThread::idealThreadCount());
}

void QJobManager::initialize()
{
}

void QJobManager::enqueueJobs(const QVector<QJobPtr> &jobQueue)
{
    // Convert QJobs to ThreadWeaver::Jobs
    QHash<QJob *, QSharedPointer<WeaverJob> > jobsMap;
    Q_FOREACH (const QJobPtr &job, jobQueue) {
        QSharedPointer<WeaverJob> weaverJob(new WeaverJob);
        weaverJob->m_job = job;
        jobsMap.insert(job.data(), weaverJob);
    }

    // Resolve dependencies
    Q_FOREACH (const QJobPtr &job, jobQueue) {
        const QVector<QWeakPointer<QJob> > &deps = job->dependencies();

        Q_FOREACH (const QWeakPointer<QJob> &dep, deps) {
            QSharedPointer<WeaverJob> weaverDep = jobsMap.value(dep.data());

            if (weaverDep) {
                ThreadWeaver::DependencyPolicy &dependencyPolicy = ThreadWeaver::DependencyPolicy::instance();
                QSharedPointer<WeaverJob> weaverJob = jobsMap.value(job.data());
                dependencyPolicy.addDependency(weaverJob, weaverDep);
            }
        }
    }

    Q_FOREACH (const QJobPtr &job, jobQueue) {
        QSharedPointer<WeaverJob> weaverJob = jobsMap.value(job.data());
        m_weaver->enqueue(weaverJob);
    }
}

void QJobManager::waitForAllJobs()
{
    m_weaver->finish();
}

void QJobManager::SynchronizedJob::run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
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

void QJobManager::waitForPerThreadFunction(JobFunction func, void *arg)
{
    const int threadCount = m_weaver->maximumNumberOfThreads();
    QAtomicInt atomicCount(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        QJobManager::SynchronizedJobPtr syncJob(new QJobManager::SynchronizedJob(func, arg, &atomicCount));
        m_weaver->enqueue(syncJob);
    }

    m_weaver->finish();
}

} // namespace Qt3D
