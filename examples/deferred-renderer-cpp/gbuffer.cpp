/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gbuffer.h"

GBuffer::GBuffer(Qt3D::QNode *parent)
    : Qt3D::QRenderTarget(parent)
{
    const Qt3D::QTexture::TextureFormat formats[AttachmentsCount] = {
        Qt3D::QTexture::RGBA32F,
        Qt3D::QTexture::RGB32F,
        Qt3D::QTexture::RGB16F,
        Qt3D::QTexture::D32F
    };

    const Qt3D::QRenderAttachment::RenderAttachmentType attachmentTypes[AttachmentsCount] = {
        Qt3D::QRenderAttachment::ColorAttachment0,
        Qt3D::QRenderAttachment::ColorAttachment1,
        Qt3D::QRenderAttachment::ColorAttachment2,
        Qt3D::QRenderAttachment::DepthAttachment
    };

    const QString attachmentNames[AttachmentsCount] = {
        QStringLiteral("color"),
        QStringLiteral("position"),
        QStringLiteral("normal"),
        QStringLiteral("depth")
    };

    for (int i = 0; i < AttachmentsCount; i++) {
        Qt3D::QRenderAttachment *attachment = new Qt3D::QRenderAttachment(this);

        m_textures[i] = new Qt3D::QTexture();
        m_textures[i]->setTarget(Qt3D::QTexture::Target2D);
        m_textures[i]->setFormat(formats[i]);
        m_textures[i]->setWidth(1024);
        m_textures[i]->setHeight(1024);
        m_textures[i]->setGenerateMipMaps(false);
        m_textures[i]->setWrapMode(Qt3D::QTexture::ClampToEdge);
        m_textures[i]->setMinificationFilter(Qt3D::QTexture::Linear);
        m_textures[i]->setMagnificationFilter(Qt3D::QTexture::Linear);

        attachment->setTexture(m_textures[i]);
        attachment->setType(attachmentTypes[i]);
        attachment->setName(attachmentNames[i]);

        addAttachment(attachment);
    }
}

Qt3D::QTexture *GBuffer::colorTexture() const
{
    return m_textures[Color];
}

Qt3D::QTexture *GBuffer::positionTexture() const
{
    return m_textures[Position];
}

Qt3D::QTexture *GBuffer::normalTexture() const
{
    return m_textures[Normal];
}

Qt3D::QTexture *GBuffer::depthTexture() const
{
    return m_textures[Depth];
}
