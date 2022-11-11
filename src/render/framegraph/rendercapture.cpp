// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/qrendercapture_p.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

RenderCapture::RenderCapture()
    : FrameGraphNode(FrameGraphNode::RenderCapture, QBackendNode::ReadWrite)
{

}

void RenderCapture::requestCapture(const QRenderCaptureRequest &request)
{
    QMutexLocker lock(&m_mutex);
    m_requestedCaptures.push_back(request);
}

// called by render view initializer job
bool RenderCapture::wasCaptureRequested() const
{
    QMutexLocker lock(&m_mutex);
    return m_requestedCaptures.size() > 0 && isEnabled();
}

// called by render view initializer job
QRenderCaptureRequest RenderCapture::takeCaptureRequest()
{
    Q_ASSERT(!m_requestedCaptures.isEmpty());
    return m_requestedCaptures.takeFirst();
}

void RenderCapture::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QRenderCapture *node = qobject_cast<const QRenderCapture *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    const QRenderCapturePrivate *d = static_cast<const QRenderCapturePrivate *>(QFrameGraphNodePrivate::get(node));
    const auto newPendingsCaptures = Qt3DCore::moveAndClear(d->m_pendingRequests);
    if (newPendingsCaptures.size() > 0) {
        m_requestedCaptures.append(newPendingsCaptures);
        markDirty(AbstractRenderer::FrameGraphDirty);
    }

    if (firstTime)
        markDirty(AbstractRenderer::FrameGraphDirty);
}

// called by render thread
void RenderCapture::addRenderCapture(int captureId, const QImage &image)
{
    QMutexLocker lock(&m_mutex);
    auto data = RenderCaptureDataPtr::create();
    data.data()->captureId = captureId;
    data.data()->image = image;
    m_renderCaptureData.push_back(data);
}

// called to send render capture in main thread
void RenderCapture::syncRenderCapturesToFrontend(Qt3DCore::QAspectManager *manager)
{
    auto *frontend = manager->lookupNode(peerId());
    if (!frontend)
        return;
    QRenderCapturePrivate *dfrontend = static_cast<QRenderCapturePrivate *>(Qt3DCore::QNodePrivate::get(frontend));

    QMutexLocker lock(&m_mutex);
    for (const RenderCaptureDataPtr &data : std::as_const(m_renderCaptureData)) {
        QPointer<QRenderCaptureReply> reply = dfrontend->takeReply(data.data()->captureId);
        // Note: QPointer has no operator bool, we must use isNull() to check it
        if (!reply.isNull()) {
            dfrontend->setImage(reply, data.data()->image);
            emit reply->completed();
        }
    }
    m_renderCaptureData.clear();
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
