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

#include "task_p.h"
#include "dependencyhandler_p.h"
#include "qthreadpooler_p.h"

#include <QMutexLocker>

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

RunnableInterface::~RunnableInterface()
{
}

// Aspect task

AspectTaskRunnable::AspectTaskRunnable()
    : m_dependencyHandler(0),
      m_reserved(false)
{
}

AspectTaskRunnable::~AspectTaskRunnable()
{
}

void AspectTaskRunnable::run()
{
    if (m_job)
        m_job->run();

    if (m_pooler)
        m_pooler->taskFinished(this);
}

void AspectTaskRunnable::setDependencyHandler(DependencyHandler *handler)
{
    m_dependencyHandler = handler;
}

DependencyHandler *AspectTaskRunnable::dependencyHandler()
{
    return m_dependencyHandler;
}

// Synchronized task

SyncTaskRunnable::SyncTaskRunnable(QAbstractAspectJobManager::JobFunction func,
                                   void *arg, QAtomicInt *atomicCount)
    : m_func(func),
      m_arg(arg),
      m_atomicCount(atomicCount),
      m_pooler(Q_NULLPTR),
      m_reserved(false)
{
}

SyncTaskRunnable::~SyncTaskRunnable()
{
}

void SyncTaskRunnable::run()
{
    // Call the function
    m_func(m_arg);

    // Decrement the atomic counter to let others know we've done our bit
    m_atomicCount->deref();

    // Wait for the other worker threads to be done
    while (m_atomicCount->load() > 0)
        QThread::currentThread()->yieldCurrentThread();

    if (m_pooler)
        m_pooler->taskFinished(this);
}

void SyncTaskRunnable::setDependencyHandler(DependencyHandler *handler)
{
    Q_UNUSED(handler);
}

DependencyHandler *SyncTaskRunnable::dependencyHandler()
{
    return Q_NULLPTR;
}

} // namespace Qt3D {

QT_END_NAMESPACE
