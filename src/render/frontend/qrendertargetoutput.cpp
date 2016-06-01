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

#include "qrendertargetoutput.h"
#include "qrendertargetoutput_p.h"
#include "qtexture.h"
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QRenderTargetOutput
 * \brief The QRenderTargetOutput class allows the specification of an attachment
 * of a render target (whether it is a color texture, a depth texture, etc... ).
 * \since 5.7
 * \inmodule Qt3DRender
 */

/*! \internal */
QRenderTargetOutputPrivate::QRenderTargetOutputPrivate()
    : QNodePrivate()
    , m_texture(nullptr)
    , m_attachmentPoint(QRenderTargetOutput::Color0)
    , m_mipLevel(0)
    , m_layer(0)
    , m_face(QAbstractTexture::CubeMapNegativeX)
{
}

/*!
 * The constructor creates a new QRenderTargetOutput::QRenderTargetOutput instance
 * with the specified \a parent.
 * \param parent
 */
QRenderTargetOutput::QRenderTargetOutput(QNode *parent)
    : QNode(*new QRenderTargetOutputPrivate, parent)
{
}

/*! \internal */
QRenderTargetOutput::~QRenderTargetOutput()
{
}

/*! \internal */
QRenderTargetOutput::QRenderTargetOutput(QRenderTargetOutputPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
 * Sets the attachment point to \a attachmentPoint.
 * \param attachmentPoint
 */
void QRenderTargetOutput::setAttachmentPoint(QRenderTargetOutput::AttachmentPoint attachmentPoint)
{
    Q_D(QRenderTargetOutput);
    if (attachmentPoint != d->m_attachmentPoint) {
        d->m_attachmentPoint = attachmentPoint;
        emit attachmentPointChanged(attachmentPoint);
    }
}

/*!
 * \return the current attachment point.
 */
QRenderTargetOutput::AttachmentPoint QRenderTargetOutput::attachmentPoint() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_attachmentPoint;
}

void QRenderTargetOutput::setTexture(QAbstractTexture *texture)
{
    Q_D(QRenderTargetOutput);
    if (texture != d->m_texture) {

        if (d->m_texture)
            d->unregisterDestructionHelper(d->m_texture);

        // Handle inline declaration
        if (!texture->parent())
            texture->setParent(this);

        d->m_texture = texture;

        // Ensures proper bookkeeping
        if (d->m_texture)
            d->registerDestructionHelper(d->m_texture, &QRenderTargetOutput::setTexture, d->m_texture);

        emit textureChanged(texture);
    }
}

/*!
 * \return the current texture.
 */
QAbstractTexture *QRenderTargetOutput::texture() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_texture;
}

/*!
 * Sets the required mip level to \a level.
 * \param level
 */
void QRenderTargetOutput::setMipLevel(int level)
{
    Q_D(QRenderTargetOutput);
    if (d->m_mipLevel != level) {
        d->m_mipLevel = level;
        emit mipLevelChanged(level);
    }
}

/*!
 * \return the current mip level.
 */
int QRenderTargetOutput::mipLevel() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_mipLevel;
}

/*!
 * Sets the required layer to \a layer.
 * \param layer
 */
void QRenderTargetOutput::setLayer(int layer)
{
    Q_D(QRenderTargetOutput);
    if (d->m_layer != layer) {
        d->m_layer = layer;
        emit layerChanged(layer);
    }
}

/*!
 * \return the current layer.
 */
int QRenderTargetOutput::layer() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_layer;
}

/*!
 * Sets the required cubemap face to \a face.
 * \param face
 */
void QRenderTargetOutput::setFace(QAbstractTexture::CubeMapFace face)
{
    Q_D(QRenderTargetOutput);
    if (d->m_face != face) {
        d->m_face = face;
        emit faceChanged(face);
    }
}

/*!
 * \return the current cubemap face.
 */
QAbstractTexture::CubeMapFace QRenderTargetOutput::face() const
{
    Q_D(const QRenderTargetOutput);
    return d->m_face;
}

Qt3DCore::QNodeCreatedChangeBasePtr QRenderTargetOutput::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QRenderTargetOutputData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QRenderTargetOutput);
    data.textureId = qIdForNode(texture());
    data.attachmentPoint = d->m_attachmentPoint;
    data.mipLevel = d->m_mipLevel;
    data.layer = d->m_layer;
    data.face = d->m_face;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
