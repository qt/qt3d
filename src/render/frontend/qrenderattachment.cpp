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
#include "qrenderattachment_p.h"
#include "qrenderattachment.h"
#include "qtexture.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QRenderAttachmentPrivate::QRenderAttachmentPrivate(QRenderAttachment *qq)
    : QNodePrivate(qq)
    , m_texture(Q_NULLPTR)
    , m_type(QRenderAttachment::ColorAttachment0)
    , m_mipLevel(0)
    , m_layer(0)
    , m_face(QRenderAttachment::CubeMapNegativeX)
{
}

void QRenderAttachmentPrivate::copy(const QNodePrivate *ref)
{
    QNodePrivate::copy(ref);
    const QRenderAttachmentPrivate *rA = static_cast<const QRenderAttachmentPrivate *>(ref);
    m_type = rA->m_type;
}

QRenderAttachment::QRenderAttachment(QNode *parent)
    : QNode(*new QRenderAttachmentPrivate(this), parent)
{
}

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
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("type"));
            change->setValue(type);
            d->notifyObservers(change);
        }
    }
}

QRenderAttachment::RenderAttachmentType QRenderAttachment::type() const
{
    Q_D(const QRenderAttachment);
    return d->m_type;
}

void QRenderAttachment::setTexture(QTexture *texture)
{
    Q_D(QRenderAttachment);
    if (texture != d->m_texture) {
        d->m_texture = texture;
        emit textureChanged();

        // Handle inline declaration
        if (!texture->parent())
            texture->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("texture"));
            // TO DO: Send a clone once Texture is a QNode subclass
            change->setValue(QVariant::fromValue(texture));
            d->notifyObservers(change);
        }
    }
}

QTexture *QRenderAttachment::texture() const
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
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("mipLevel"));
            change->setValue(level);
            d->notifyObservers(change);
        }
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
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("layer"));
            change->setValue(layer);
            d->notifyObservers(change);
        }
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
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("face"));
            change->setValue(face);
            d->notifyObservers(change);
        }
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
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, this));
            change->setPropertyName(QByteArrayLiteral("name"));
            change->setValue(name);
            d->notifyObservers(change);
        }
    }
}

QString QRenderAttachment::name() const
{
    Q_D(const QRenderAttachment);
    return d->m_name;
}

QNode *QRenderAttachment::doClone() const
{
    Q_D(const QRenderAttachment);
    QRenderAttachment *clone = new QRenderAttachment();;
    clone->d_func()->copy(d_func());
    clone->setTexture(qobject_cast<QTexture *>(QNodePrivate::get(d->m_texture)->clone()));
    return clone;
}

} // Qt3D

QT_END_NAMESPACE
