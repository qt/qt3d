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
#include "qrenderattachment_p.h"
#include "qrenderattachment.h"
#include "qtexture.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QRenderAttachmentPrivate
    \internal
*/
QRenderAttachmentPrivate::QRenderAttachmentPrivate()
    : QNodePrivate()
    , m_texture(Q_NULLPTR)
    , m_type(QRenderAttachment::ColorAttachment0)
    , m_mipLevel(0)
    , m_layer(0)
    , m_face(QRenderAttachment::CubeMapNegativeX)
{
}

void QRenderAttachment::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QRenderAttachment *rA = static_cast<const QRenderAttachment*>(ref);
    d_func()->m_type = rA->d_func()->m_type;
    setTexture(qobject_cast<QAbstractTextureProvider *>(QNode::clone(rA->d_func()->m_texture)));
}

QRenderAttachment::QRenderAttachment(QNode *parent)
    : QNode(*new QRenderAttachmentPrivate, parent)
{
}

QRenderAttachment::~QRenderAttachment()
{
    QNode::cleanup();
}

/*! \internal */
QRenderAttachment::QRenderAttachment(QRenderAttachmentPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QRenderAttachment::setType(QRenderAttachment::RenderAttachmentType type)
{
    Q_D(QRenderAttachment);
    if (type != d->m_type) {
        d->m_type = type;
        emit typeChanged();
    }
}

QRenderAttachment::RenderAttachmentType QRenderAttachment::type() const
{
    Q_D(const QRenderAttachment);
    return d->m_type;
}

void QRenderAttachment::setTexture(QAbstractTextureProvider *texture)
{
    Q_D(QRenderAttachment);
    if (texture != d->m_texture) {
        d->m_texture = texture;

        // Handle inline declaration
        if (!texture->parent())
            texture->setParent(this);
        emit textureChanged();
    }
}

QAbstractTextureProvider *QRenderAttachment::texture() const
{
    Q_D(const QRenderAttachment);
    return d->m_texture;
}

void QRenderAttachment::setMipLevel(int level)
{
    Q_D(QRenderAttachment);
    if (d->m_mipLevel != level) {
        d->m_mipLevel = level;
        emit mipLevelChanged();
    }
}

int QRenderAttachment::mipLevel() const
{
    Q_D(const QRenderAttachment);
    return d->m_mipLevel;
}

void QRenderAttachment::setLayer(int layer)
{
    Q_D(QRenderAttachment);
    if (d->m_layer != layer) {
        d->m_layer = layer;
        emit layerChanged();
    }
}

int QRenderAttachment::layer() const
{
    Q_D(const QRenderAttachment);
    return d->m_layer;
}

void QRenderAttachment::setFace(QRenderAttachment::CubeMapFace face)
{
    Q_D(QRenderAttachment);
    if (d->m_face != face) {
        d->m_face = face;
        emit faceChanged();
    }
}

QRenderAttachment::CubeMapFace QRenderAttachment::face() const
{
    Q_D(const QRenderAttachment);
    return d->m_face;
}

void QRenderAttachment::setName(const QString &name)
{
    Q_D(QRenderAttachment);
    if (d->m_name != name) {
        d->m_name = name;
        emit nameChanged();
    }
}

QString QRenderAttachment::name() const
{
    Q_D(const QRenderAttachment);
    return d->m_name;
}

} // Qt3D

QT_END_NAMESPACE
