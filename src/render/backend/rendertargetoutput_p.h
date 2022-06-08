// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERTARGETOUTPUT_H
#define QT3DRENDER_RENDER_RENDERTARGETOUTPUT_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/private/attachmentpack_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderTargetOutput;

namespace Render {

class AttachmentManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderTargetOutput : public BackendNode
{
public:
    RenderTargetOutput();

    Qt3DCore::QNodeId textureUuid() const;
    int mipLevel() const;
    int layer() const;
    QString name() const;
    QAbstractTexture::CubeMapFace face() const;
    QRenderTargetOutput::AttachmentPoint point() const;
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    Attachment *attachment();
    const Attachment *attachment() const;

private:
    Qt3DCore::QNodeId m_attachmentUuid;
    Attachment m_attachmentData;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERTARGETOUTPUT_H
