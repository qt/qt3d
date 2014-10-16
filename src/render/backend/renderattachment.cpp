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

#include <Qt3DRenderer/private/renderattachment_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qtexture.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderAttachment::RenderAttachment()
    : QBackendNode()
{
}

void RenderAttachment::updateFromPeer(QNode *peer)
{
    QRenderAttachment *attachment = static_cast<QRenderAttachment *>(peer);

    m_attachmentData.m_mipLevel = attachment->mipLevel();
    m_attachmentData.m_layer = attachment->layer();
    m_attachmentData.m_type = attachment->type();
    m_attachmentData.m_face = attachment->face();
    m_attachmentData.m_name = attachment->name();
    if (attachment->texture())
        m_attachmentData.m_textureUuid = attachment->texture()->uuid();
}

QUuid RenderAttachment::textureUuid() const
{
    return m_attachmentData.m_textureUuid;
}

int RenderAttachment::mipLevel() const
{
    return m_attachmentData.m_mipLevel;
}

int RenderAttachment::layer() const
{
    return m_attachmentData.m_layer;
}

QString RenderAttachment::name() const
{
    return m_attachmentData.m_name;
}

QRenderAttachment::CubeMapFace RenderAttachment::face() const
{
    return m_attachmentData.m_face;
}

QRenderAttachment::RenderAttachmentType RenderAttachment::type() const
{
    return m_attachmentData.m_type;
}

void RenderAttachment::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("type")) {
            m_attachmentData.m_type = static_cast<QRenderAttachment::RenderAttachmentType>(propertyChange->value().toInt());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("texture")) {
            m_attachmentData.m_textureUuid = propertyChange->value().toUuid();
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("mipLevel")) {
            m_attachmentData.m_mipLevel = propertyChange->value().toInt();
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("layer")) {
            m_attachmentData.m_layer = propertyChange->value().toInt();
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("face")) {
            m_attachmentData.m_face = static_cast<QRenderAttachment::CubeMapFace>(propertyChange->value().toInt());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("name")) {
            m_attachmentData.m_name = propertyChange->value().toString();
        }
    }
}

Attachment RenderAttachment::attachment() const
{
    return m_attachmentData;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
