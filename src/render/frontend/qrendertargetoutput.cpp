/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qrendertargetoutput_p.h"
#include "qrendertargetoutput.h"
#include "qtexture.h"
#include <Qt3DCore/qnodepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QRenderTargetOutputPrivate::QRenderTargetOutputPrivate()
    : QNodePrivate()
    , m_texture(Q_NULLPTR)
    , m_attachmentPoint(QRenderTargetOutput::Color0)
    , m_mipLevel(0)
    , m_layer(0)
    , m_face(QAbstractTexture::CubeMapNegativeX)
{
}

void QRenderTargetOutput::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QRenderTargetOutput *rA = static_cast<const QRenderTargetOutput*>(ref);
    d_func()->m_attachmentPoint = rA->d_func()->m_attachmentPoint;
    setTexture(qobject_cast<QAbstractTexture *>(QNode::clone(rA->d_func()->m_texture)));
}

QRenderTargetOutput::QRenderTargetOutput(QNode *parent)
    : QNode(*new QRenderTargetOutputPrivate, parent)
{
}

/*! \internal */
QRenderTargetOutput::QRenderTargetOutput(QRenderTargetOutputPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QRenderTargetOutput::setAttachmentPoint(QRenderTargetOutput::AttachmentPoint attachmentPoint)
{
    Q_D(QRenderTargetOutput);
    if (attachmentPoint != d->m_attachmentPoint) {
        d->m_attachmentPoint = attachmentPoint;
        emit attachmentPointChanged(attachmentPoint);
    }
}

QRenderTargetOutput::AttachmentPoint QRenderTargetOutput::attachmentPoint() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_attachmentPoint;
}

void QRenderTargetOutput::setTexture(QAbstractTexture *texture)
{
    Q_D(QRenderTargetOutput);
    if (texture != d->m_texture) {
        d->m_texture = texture;

        // Handle inline declaration
        if (!texture->parent())
            texture->setParent(this);
        emit textureChanged(texture);
    }
}

QAbstractTexture *QRenderTargetOutput::texture() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_texture;
}

void QRenderTargetOutput::setMipLevel(int level)
{
    Q_D(QRenderTargetOutput);
    if (d->m_mipLevel != level) {
        d->m_mipLevel = level;
        emit mipLevelChanged(level);
    }
}

int QRenderTargetOutput::mipLevel() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_mipLevel;
}

void QRenderTargetOutput::setLayer(int layer)
{
    Q_D(QRenderTargetOutput);
    if (d->m_layer != layer) {
        d->m_layer = layer;
        emit layerChanged(layer);
    }
}

int QRenderTargetOutput::layer() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_layer;
}

void QRenderTargetOutput::setFace(QAbstractTexture::CubeMapFace face)
{
    Q_D(QRenderTargetOutput);
    if (d->m_face != face) {
        d->m_face = face;
        emit faceChanged(face);
    }
}

QAbstractTexture::CubeMapFace QRenderTargetOutput::face() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_face;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
