/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qabstracttextureimage.h"
#include "qabstracttextureimage_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractTextureImage
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture image.

    Qt3D::QAbstractTextureImage should be used as the mean of providing image data to a
    Qt3D::QAbstractTextureProvider. It contains the necessary informations: mipmap
    level, layer, cube face load at the proper place data into an OpenGL texture.

    The actual data is provided through a QTextureDataFunctor that will be
    executed by Aspect jobs in the backend. Qt3D::QAbstractTextureImage should be
    subclassed to provide a functor and eventual additional properties needed by
    the functor to load actual data.

    \note: QAbstractTextureImage should never be shared. Expect crashes, undefined
    behavior at best if this rule is not respected.
 */

/*!
    Constructs a new Qt3D::QAbstractTextureImage instance with \a parent as parent.
 */
QAbstractTextureImage::QAbstractTextureImage(QNode *parent)
    : QNode(*new QAbstractTextureImagePrivate(this), parent)
{
}

QAbstractTextureImage::~QAbstractTextureImage()
{
}

/*!
    \return the mipmal level of the texture image.
 */
int QAbstractTextureImage::mipmapLevel() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_mipmapLevel;
}

/*!
    \return the layer of the texture image.
 */
int QAbstractTextureImage::layer() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_layer;
}

/*!
    \return the cube map face of the texture image.
 */
QAbstractTextureProvider::CubeMapFace QAbstractTextureImage::cubeMapFace() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_face;
}

/*!
    Sets the mipmap level of the texture image to \a level.
 */
void QAbstractTextureImage::setMipmapLevel(int level)
{
    Q_D(QAbstractTextureImage);
    if (level != d->m_mipmapLevel) {
        d->m_mipmapLevel = level;
        emit mipmapLevelChanged();
    }
}

/*!
    Sets the layer of the texture image to \a layer.
    \note: has a meaning only for Target2DArray, TargetCubeMapArray and Target3D
    Qt3D::QAbstractTextureProvider.
 */
void QAbstractTextureImage::setLayer(int layer)
{
    Q_D(QAbstractTextureImage);
    if (layer != d->m_layer) {
        d->m_layer = layer;
        emit layerChanged();
    }
}

/*!
    Sets the cube map face of the texture image to \a face.
    \note: has a meaning only for TargetCubeMap and TargetCubeMapArray
    Qt3D::QAbstractTextureProvider.
 */
void QAbstractTextureImage::setCubeMapFace(QAbstractTextureProvider::CubeMapFace face)
{
    Q_D(QAbstractTextureImage);
    if (face != d->m_face) {
        d->m_face = face;
        emit cubeMapFaceChanged();
    }
}

/*!
    Triggers an update of the data functor that is sent to the backend
 */
void QAbstractTextureImage::update()
{
    Q_D(QAbstractTextureImage);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        change->setPropertyName("dataFunctor");
        change->setValue(QVariant::fromValue(dataFunctor()));
        d->notifyObservers(change);
    }
}

void QAbstractTextureImage::copy(const QNode *ref)
{
    const QAbstractTextureImage *imageRef = static_cast<const QAbstractTextureImage *>(ref);
    d_func()->m_face = imageRef->cubeMapFace();
    d_func()->m_layer = imageRef->layer();
    d_func()->m_mipmapLevel = imageRef->mipmapLevel();
}

QAbstractTextureImage::QAbstractTextureImage(QAbstractTextureImagePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // Qt3D

QT_END_NAMESPACE
