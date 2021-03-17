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

#include "task_p.h"

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QMutexLocker>

#include <Qt3DCore/private/qthreadpooler_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

RunnableInterface::~RunnableInterface()
{
}

// Aspect task

AspectTaskRunnable::AspectTaskRunnable(QSystemInformationService *service)
    : m_service(service)
    , m_pooler(nullptr)
    , m_id(0)
    , m_reserved(false)
{
}

AspectTaskRunnable::~AspectTaskRunnable()
{
}

bool AspectTaskRunnable::isRequired() const
{
    return m_job ? QAspectJobPrivate::get(m_job.data())->isRequired() : false;
}

void AspectTaskRunnable::run()
{
    if (m_job) {
        QAspectJobPrivate *jobD = QAspectJobPrivate::get(m_job.data());
        QTaskLogger logger(m_pooler ? m_service : nullptr, jobD->m_jobId, QTaskLogger::AspectJob);
        m_job->run();
    }

    // We could have an append sub task or something in here
    // So that a job can post sub jobs ?

    if (m_pooler)
        m_pooler->taskFinished(this);
}

// Synchronized task

SyncTaskRunnable::SyncTaskRunnable(QAbstractAspectJobManager::JobFunction func,
                                   void *arg, QAtomicInt *atomicCount)
    : m_func(func)
    , m_arg(arg)
    , m_atomicCount(atomicCount)
    , m_pooler(nullptr)
    , m_reserved(false)
    , m_id(0)
{
}

SyncTaskRunnable::~SyncTaskRunnable()
{
}

bool SyncTaskRunnable::isRequired() const
{
    return true;
}

void SyncTaskRunnable::run()
{
    // Call the function
    m_func(m_arg);

    // Decrement the atomic counter to let others know we've done our bit
    m_atomicCount->deref();

    // Wait for the other worker threads to be done
    while (m_atomicCount->loadRelaxed() > 0)
        QThread::currentThread()->yieldCurrentThread();

    if (m_pooler)
        m_pooler->taskFinished(this);
}

} // namespace Qt3DCore {

QT_END_NAMESPACE
