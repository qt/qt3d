// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
