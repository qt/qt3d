// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "gbuffer.h"

GBuffer::GBuffer(Qt3DCore::QNode *parent)
    : Qt3DRender::QRenderTarget(parent)
{
    const Qt3DRender::QAbstractTexture::TextureFormat formats[AttachmentsCount] = {
        Qt3DRender::QAbstractTexture::RGBA32F,
        // We use RGBA32F for the following two instead of a more fitting format because
        // OpenGL vendors might not support other formats
        Qt3DRender::QAbstractTexture::RGBA32F,
        Qt3DRender::QAbstractTexture::RGBA32F,
        Qt3DRender::QAbstractTexture::D32F
    };

    const Qt3DRender::QRenderTargetOutput::AttachmentPoint attachmentPoints[AttachmentsCount] = {
        Qt3DRender::QRenderTargetOutput::Color0,
        Qt3DRender::QRenderTargetOutput::Color1,
        Qt3DRender::QRenderTargetOutput::Color2,
        Qt3DRender::QRenderTargetOutput::Depth
    };

    for (int i = 0; i < AttachmentsCount; i++) {
        Qt3DRender::QRenderTargetOutput *output = new Qt3DRender::QRenderTargetOutput(this);

        m_textures[i] = new Qt3DRender::QTexture2D();
        m_textures[i]->setFormat(formats[i]);
        m_textures[i]->setWidth(1024);
        m_textures[i]->setHeight(1024);
        m_textures[i]->setGenerateMipMaps(false);
        m_textures[i]->setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::ClampToEdge));
        m_textures[i]->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
        m_textures[i]->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

        output->setTexture(m_textures[i]);
        output->setAttachmentPoint(attachmentPoints[i]);
        addOutput(output);
    }
}

Qt3DRender::QAbstractTexture *GBuffer::colorTexture() const
{
    return m_textures[Color];
}

Qt3DRender::QAbstractTexture *GBuffer::positionTexture() const
{
    return m_textures[Position];
}

Qt3DRender::QAbstractTexture *GBuffer::normalTexture() const
{
    return m_textures[Normal];
}

Qt3DRender::QAbstractTexture *GBuffer::depthTexture() const
{
    return m_textures[Depth];
}
