/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#include "gbuffer.h"

GBuffer::GBuffer(Qt3D::QNode *parent)
    : Qt3D::QRenderTarget(parent)
{
    const Qt3D::QAbstractTextureProvider::TextureFormat formats[AttachmentsCount] = {
        Qt3D::QAbstractTextureProvider::RGBA32F,
        Qt3D::QAbstractTextureProvider::RGB32F,
        Qt3D::QAbstractTextureProvider::RGB16F,
        Qt3D::QAbstractTextureProvider::D32F
    };

    const Qt3D::QRenderAttachment::RenderAttachmentType attachmentTypes[AttachmentsCount] = {
        Qt3D::QRenderAttachment::ColorAttachment0,
        Qt3D::QRenderAttachment::ColorAttachment1,
        Qt3D::QRenderAttachment::ColorAttachment2,
        Qt3D::QRenderAttachment::DepthAttachment
    };

    const QString attachmentNames[AttachmentsCount] = {
        QString::fromLatin1("color"),
        QString::fromLatin1("position"),
        QString::fromLatin1("normal"),
        QString::fromLatin1("depth")
    };

    for (int i = 0; i < AttachmentsCount; i++) {
        Qt3D::QRenderAttachment *attachment = new Qt3D::QRenderAttachment(this);

        m_textures[i] = new Qt3D::QTexture2D();
        m_textures[i]->setFormat(formats[i]);
        m_textures[i]->setWidth(1024);
        m_textures[i]->setHeight(1024);
        m_textures[i]->setGenerateMipMaps(false);
        m_textures[i]->setWrapMode(Qt3D::QTextureWrapMode(Qt3D::QTextureWrapMode::ClampToEdge));
        m_textures[i]->setMinificationFilter(Qt3D::QAbstractTextureProvider::Linear);
        m_textures[i]->setMagnificationFilter(Qt3D::QAbstractTextureProvider::Linear);

        attachment->setTexture(m_textures[i]);
        attachment->setType(attachmentTypes[i]);
        attachment->setName(attachmentNames[i]);

        addAttachment(attachment);
    }
}

Qt3D::QAbstractTextureProvider *GBuffer::colorTexture() const
{
    return m_textures[Color];
}

Qt3D::QAbstractTextureProvider *GBuffer::positionTexture() const
{
    return m_textures[Position];
}

Qt3D::QAbstractTextureProvider *GBuffer::normalTexture() const
{
    return m_textures[Normal];
}

Qt3D::QAbstractTextureProvider *GBuffer::depthTexture() const
{
    return m_textures[Depth];
}
