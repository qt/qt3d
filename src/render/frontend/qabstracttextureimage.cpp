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
    \class Qt3D::QAbstractTextureImagePrivate
    \internal
*/

/*!
    \qmltype AbstractTextureImage
    \instantiates Qt3D::QAbstractTextureImage
    \inherits Node
    \inqmlmodule Qt3D.Renderer
    \qmlabstract
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture image.
*/

/*!
    \class QAbstractTextureImage
    \inmodule Qt3DRenderer
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture image.

    QAbstractTextureImage should be used as the means of providing image data to a
    QAbstractTextureProvider. It contains the necessary information: mipmap
    level, layer, cube face load at the proper place data into an OpenGL texture.

    The actual data is provided through a QTextureDataFunctor that will be
    executed by Aspect jobs in the backend. QAbstractTextureImage should be
    subclassed to provide a functor and eventual additional properties needed by
    the functor to load actual data.

    \note: QAbstractTextureImage should never be shared. Expect crashes, undefined
    behavior at best if this rule is not respected.
 */

/*!
    Constructs a new Qt3D::QAbstractTextureImage instance with \a parent as parent.
 */
QAbstractTextureImage::QAbstractTextureImage(QNode *parent)
    : QNode(*new QAbstractTextureImagePrivate, parent)
{
}

/*!
  The destructor.
 */
QAbstractTextureImage::~QAbstractTextureImage()
{
    Q_ASSERT_X(QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3D::QAbstractTextureImage subclass didn't call QNode::cleanup in its destructor");
}


/*!
    \qmlproperty int Qt3D.render::AbstractTextureImage::mipmapLevel

    Holds the mipmap level of the texture image.
 */

/*!
    \property Qt3D::QAbstractTextureImage::mipmapLevel

    Holds the mipmap level of the texture image.
 */
int QAbstractTextureImage::mipmapLevel() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_mipmapLevel;
}

/*!
    \qmlproperty int Qt3D.Renderer::AbstractTextureImage::layer

    Holds the layer of the texture image.
 */

/*!
    \property Qt3D::QAbstractTextureImage::layer

    Holds the layer of the texture image.
 */
int QAbstractTextureImage::layer() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_layer;
}

/*!
    \qmlproperty enumeration Qt3D.Renderer::AbstractTextureImage::cubeMapFace

    Holds the cube map face of the texture image.

    \value CubeMapPositiveX 0x8515   GL_TEXTURE_CUBE_MAP_POSITIVE_X
    \value CubeMapNegativeX 0x8516   GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    \value CubeMapPositiveY 0x8517   GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    \value CubeMapNegativeY 0x8518   GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    \value CubeMapPositiveZ 0x8519   GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    \value CubeMapNegativeZ 0x851A   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

    \note The cube map face has a meaning only for
    \l {QAbstractTextureProvider::}{TargetCubeMap} and
    \l {QAbstractTextureProvider::}{TargetCubeMapArray}.
 */

/*!
    \property Qt3D::QAbstractTextureImage::cubeMapFace

    Holds the cube map face of the texture image.

    \note The cube map face has a meaning only for
    \l {QAbstractTextureProvider::}{TargetCubeMap} and
    \l {QAbstractTextureProvider::}{TargetCubeMapArray}.
 */
QAbstractTextureProvider::CubeMapFace QAbstractTextureImage::cubeMapFace() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_face;
}

void QAbstractTextureImage::setMipmapLevel(int level)
{
    Q_D(QAbstractTextureImage);
    if (level != d->m_mipmapLevel) {
        d->m_mipmapLevel = level;
        emit mipmapLevelChanged();
    }
}

void QAbstractTextureImage::setLayer(int layer)
{
    Q_D(QAbstractTextureImage);
    if (layer != d->m_layer) {
        d->m_layer = layer;
        emit layerChanged();
    }
}

void QAbstractTextureImage::setCubeMapFace(QAbstractTextureProvider::CubeMapFace face)
{
    Q_D(QAbstractTextureImage);
    if (face != d->m_face) {
        d->m_face = face;
        emit cubeMapFaceChanged();
    }
}

/*!
    Triggers an update of the data functor that is sent to the backend.
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

/*!
  Copies \a ref into this object.
 */
void QAbstractTextureImage::copy(const QNode *ref)
{
    const QAbstractTextureImage *imageRef = static_cast<const QAbstractTextureImage *>(ref);
    d_func()->m_face = imageRef->cubeMapFace();
    d_func()->m_layer = imageRef->layer();
    d_func()->m_mipmapLevel = imageRef->mipmapLevel();
}

/*! \internal */
QAbstractTextureImage::QAbstractTextureImage(QAbstractTextureImagePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // Qt3D

QT_END_NAMESPACE
