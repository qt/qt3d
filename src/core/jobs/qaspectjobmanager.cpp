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

#include "qaspectjobmanager_p.h"
#include "task_p.h"
#include "qthreadpooler_p.h"
#include "dependencyhandler_p.h"

#include <QAtomicInt>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QtCore/QFuture>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAspectJobManager::QAspectJobManager(QObject *parent)
    : QAbstractAspectJobManager(parent)
    , m_threadPooler(new QThreadPooler(this))
    , m_dependencyHandler(new DependencyHandler)
{
    m_threadPooler->setDependencyHandler(m_dependencyHandler.data());
}

QAspectJobManager::~QAspectJobManager()
{
}

void QAspectJobManager::initialize()
{
}

void QAspectJobManager::enqueueJobs(const QVector<QAspectJobPtr> &jobQueue)
{
    // Convert QJobs to Tasks
    QHash<QAspectJob *, AspectTaskRunnable *> tasksMap;
    QVector<RunnableInterface *> taskList;
    taskList.reserve(jobQueue.size());
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
}

void QAspectJobManager::waitForAllJobs()
{
    m_threadPooler->future().waitForFinished();
}

void QAspectJobManager::waitForPerThreadFunction(JobFunction func, void *arg)
{
    const int threadCount = m_threadPooler->maxThreadCount();
    QAtomicInt atomicCount(threadCount);

    QVector<RunnableInterface *> taskList;
    for (int i = 0; i < threadCount; ++i) {
        SyncTaskRunnable *syncTask = new SyncTaskRunnable(func, arg, &atomicCount);
        taskList << syncTask;
    }

    QFuture<void> future = m_threadPooler->mapDependables(taskList);
    future.waitForFinished();
}

} // namespace Qt3DCore

QT_END_NAMESPACE
