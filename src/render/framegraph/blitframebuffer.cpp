// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/qblitframebuffer_p.h>
#include <Qt3DRender/private/blitframebuffer_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

BlitFramebuffer::BlitFramebuffer()
    : FrameGraphNode(FrameGraphNode::BlitFramebuffer)
    , m_sourceRenderTargetId(Qt3DCore::QNodeId())
    , m_destinationRenderTargetId(Qt3DCore::QNodeId())
    , m_sourceRect(QRect())
    , m_destinationRect(QRect())
    , m_sourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0)
    , m_destinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0)
    , m_interpolationMethod(Qt3DRender::QBlitFramebuffer::Linear)
{
}

void BlitFramebuffer::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QBlitFramebuffer *node = qobject_cast<const QBlitFramebuffer *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->sourceRect().toRect() != m_sourceRect) {
        m_sourceRect = node->sourceRect().toRect();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->destinationRect().toRect() != m_destinationRect) {
        m_destinationRect = node->destinationRect().toRect();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->sourceAttachmentPoint() != m_sourceAttachmentPoint) {
        m_sourceAttachmentPoint = node->sourceAttachmentPoint();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->destinationAttachmentPoint() != m_destinationAttachmentPoint) {
        m_destinationAttachmentPoint = node->destinationAttachmentPoint();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->interpolationMethod() != m_interpolationMethod) {
        m_interpolationMethod = node->interpolationMethod();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    const QNodeId destinationNodeId = qIdForNode(node->destination());
    if (destinationNodeId != m_destinationRenderTargetId) {
        m_destinationRenderTargetId = destinationNodeId;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    const QNodeId sourceNodeId = qIdForNode(node->source());
    if (sourceNodeId != m_sourceRenderTargetId) {
        m_sourceRenderTargetId = sourceNodeId;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

Qt3DRender::QRenderTargetOutput::AttachmentPoint BlitFramebuffer::destinationAttachmentPoint() const
{
    return m_destinationAttachmentPoint;
}

QBlitFramebuffer::InterpolationMethod BlitFramebuffer::interpolationMethod() const
{
    return m_interpolationMethod;
}

Qt3DRender::QRenderTargetOutput::AttachmentPoint BlitFramebuffer::sourceAttachmentPoint() const
{
    return m_sourceAttachmentPoint;
}

QRect BlitFramebuffer::destinationRect() const
{
    return m_destinationRect;
}

QRect BlitFramebuffer::sourceRect() const
{
    return m_sourceRect;
}

Qt3DCore::QNodeId BlitFramebuffer::destinationRenderTargetId() const
{
    return m_destinationRenderTargetId;
}

Qt3DCore::QNodeId BlitFramebuffer::sourceRenderTargetId() const
{
    return m_sourceRenderTargetId;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
