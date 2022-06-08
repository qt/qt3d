// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_BLITFRAMEBUFFER_P_H
#define QT3DRENDER_RENDER_BLITFRAMEBUFFER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qblitframebuffer_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT BlitFramebuffer : public FrameGraphNode
{
public:
    BlitFramebuffer();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::QNodeId sourceRenderTargetId() const;

    Qt3DCore::QNodeId destinationRenderTargetId() const;

    QRect sourceRect() const;

    QRect destinationRect() const;

    Qt3DRender::QRenderTargetOutput::AttachmentPoint sourceAttachmentPoint() const;

    Qt3DRender::QRenderTargetOutput::AttachmentPoint destinationAttachmentPoint() const;

    QBlitFramebuffer::InterpolationMethod interpolationMethod() const;

private:
    Qt3DCore::QNodeId m_sourceRenderTargetId;
    Qt3DCore::QNodeId m_destinationRenderTargetId;
    QRect m_sourceRect;
    QRect m_destinationRect;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint m_sourceAttachmentPoint;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint m_destinationAttachmentPoint;
    QBlitFramebuffer::InterpolationMethod m_interpolationMethod;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_BLITFRAMEBUFFER_H
