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
#include "dependencyhandler_p.h"

#include <QtCore/QThreadPool>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QThreadPoolerPrivate
    \internal
*/
QThreadPooler::QThreadPooler(QObject *parent)
    : QObject(parent),
      m_futureInterface(Q_NULLPTR),
      m_mutex(new QMutex(QMutex::NonRecursive)),
      m_taskCount(0)
{
}

QThreadPooler::~QThreadPooler()
{
    // Wait till all tasks are finished before deleting mutex
    QMutexLocker locker(m_mutex);
    locker.unlock();

    delete m_mutex;
}

void QThreadPooler::setDependencyHandler(DependencyHandler *handler)
{
    m_dependencyHandler = handler;
    m_dependencyHandler->setMutex(m_mutex);
}

void QThreadPooler::enqueueTasks(QVector<RunnableInterface *> &tasks)
{
    // The caller have to set the mutex

    for (QVector<RunnableInterface *>::iterator it = tasks.begin();
         it != tasks.end(); it++) {
        if (!m_dependencyHandler->hasDependency((*it)) && !(*it)->reserved()) {
            (*it)->setReserved(true);
            (*it)->setPooler(this);
            QThreadPool::globalInstance()->start((*it));
        }
    }
}

void QThreadPooler::taskFinished(RunnableInterface *task)
{
    const QMutexLocker locker(m_mutex);

    release();

    QVector<RunnableInterface *> freedTasks;
    if (task->dependencyHandler())
        freedTasks = m_dependencyHandler->freeDependencies(task);
    if (freedTasks.size())
        enqueueTasks(freedTasks);

    if (currentCount() == 0) {
        if (m_futureInterface) {
            m_futureInterface->reportFinished();
            delete m_futureInterface;
        }
        m_futureInterface = Q_NULLPTR;
    }
}

QFuture<void> QThreadPooler::mapDependables(QVector<RunnableInterface *> &taskQueue)
{
    const QMutexLocker locker(m_mutex);

    if (!m_futureInterface)
        m_futureInterface = new QFutureInterface<void>();
    if (taskQueue.size())
        m_futureInterface->reportStarted();

    acquire(taskQueue.size());
    enqueueTasks(taskQueue);

    return QFuture<void>(m_futureInterface);
}

QFuture<void> QThreadPooler::future()
{
    const QMutexLocker locker(m_mutex);

    if (!m_futureInterface)
        return QFuture<void>();
    else
        return QFuture<void>(m_futureInterface);
}

void QThreadPooler::acquire(int add)
{
    // The caller have to set the mutex

    forever {
        int localCount = m_taskCount.load();
        if (m_taskCount.testAndSetOrdered(localCount, localCount + add))
            return;
    }
}

void QThreadPooler::release()
{
    // The caller have to set the mutex

    forever {
        int localCount = m_taskCount.load();

        // Task counter going below zero means coding errors somewhere.
        Q_ASSERT(localCount > 0);

        if (m_taskCount.testAndSetOrdered(localCount, localCount - 1))
            return;
    }
}

int QThreadPooler::currentCount()
{
    // The caller have to set the mutex

    return m_taskCount.load();
}

int QThreadPooler::maxThreadCount() const
{
    return QThreadPool::globalInstance()->maxThreadCount();
}

} // namespace Qt3D

QT_END_NAMESPACE
