// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef GBUFFER_H
#define GBUFFER_H

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/QRenderTargetOutput>

class GBuffer : public Qt3DRender::QRenderTarget
{
public:
    explicit GBuffer(Qt3DCore::QNode *parent = 0);

    enum Attachments {
        Color = 0,
        Position,
        Normal,
        Depth,
        AttachmentsCount
    };

    Qt3DRender::QAbstractTexture *colorTexture() const;
    Qt3DRender::QAbstractTexture *positionTexture() const;
    Qt3DRender::QAbstractTexture *normalTexture() const;
    Qt3DRender::QAbstractTexture *depthTexture() const;

private:
    Qt3DRender::QAbstractTexture *m_textures[AttachmentsCount];
};

#endif // GBUFFER_H
