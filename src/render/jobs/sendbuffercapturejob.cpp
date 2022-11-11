// Copyright (C) 2017 Juan José Casafranca
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "sendbuffercapturejob_p.h"

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qbuffer_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class SendBufferCaptureJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    SendBufferCaptureJobPrivate() {}
    ~SendBufferCaptureJobPrivate() {}

    void postFrame(Qt3DCore::QAspectManager *aspectManager) override;

    mutable QMutex m_mutex;
    QList<QPair<Qt3DCore::QNodeId, QByteArray>> m_buffersToCapture;
    QList<QPair<Qt3DCore::QNodeId, QByteArray>> m_buffersToNotify;
};

SendBufferCaptureJob::SendBufferCaptureJob()
    : Qt3DCore::QAspectJob(*new SendBufferCaptureJobPrivate)
    , m_nodeManagers(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::SendBufferCapture, 0)
}

SendBufferCaptureJob::~SendBufferCaptureJob()
{
}

// Called from SubmitRenderView while rendering
void SendBufferCaptureJob::addRequest(QPair<Qt3DCore::QNodeId, QByteArray> request)
{
    Q_D(SendBufferCaptureJob);
    QMutexLocker locker(&d->m_mutex);
    d->m_buffersToCapture.push_back(request);
}

// Called by aspect thread jobs to execute (we may still be rendering at this point)
bool SendBufferCaptureJob::hasRequests() const
{
    Q_D(const SendBufferCaptureJob);
    QMutexLocker locker(&d->m_mutex);
    return d->m_buffersToCapture.size() > 0;
}

void SendBufferCaptureJob::run()
{
    Q_ASSERT(m_nodeManagers);
    Q_D(SendBufferCaptureJob);
    QMutexLocker locker(&d->m_mutex);
    for (const QPair<Qt3DCore::QNodeId, QByteArray> &pendingCapture : std::as_const(d->m_buffersToCapture)) {
        Buffer *buffer = m_nodeManagers->bufferManager()->lookupResource(pendingCapture.first);
        // Buffer might have been destroyed between the time addRequest is made and this job gets run
        // If it exists however, it cannot be destroyed before this job is done running
        if (buffer != nullptr)
            buffer->updateDataFromGPUToCPU(pendingCapture.second);
    }
    d->m_buffersToNotify = Qt3DCore::moveAndClear(d->m_buffersToCapture);
}

void SendBufferCaptureJobPrivate::postFrame(Qt3DCore::QAspectManager *aspectManager)
{
    QMutexLocker locker(&m_mutex);
    const QList<QPair<Qt3DCore::QNodeId, QByteArray>> pendingSendBufferCaptures = Qt3DCore::moveAndClear(m_buffersToNotify);
    for (const auto &bufferDataPair : pendingSendBufferCaptures) {
        Qt3DCore::QBuffer *frontendBuffer = static_cast<decltype(frontendBuffer)>(aspectManager->lookupNode(bufferDataPair.first));
        if (!frontendBuffer)
            continue;
        Qt3DCore::QBufferPrivate *dFrontend = static_cast<decltype(dFrontend)>(Qt3DCore::QNodePrivate::get(frontendBuffer));
        // Calling frontendBuffer->setData would result in forcing a sync against the backend
        // which isn't necessary
        dFrontend->setData(bufferDataPair.second);
        Q_EMIT frontendBuffer->dataAvailable();
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
