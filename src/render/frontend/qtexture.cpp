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

#include "qtexture.h"

#include <texturedata.h>

#include <QImage>
#include <QDebug>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractTextureImagePrivate : public QNodePrivate
{
public:
    QAbstractTextureImagePrivate(QAbstractTextureImage *qq)
        : QNodePrivate(qq)
        , m_mipmapLevel(0)
        , m_layer(0)
        , m_face(QAbstractTextureProvider::CubeMapPositiveX)
    {
    }

    Q_DECLARE_PUBLIC(QAbstractTextureImage)

    int m_mipmapLevel;
    int m_layer;
    QAbstractTextureProvider::CubeMapFace m_face;
    QUrl m_source;
};

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

class QTextureImagePrivate: public QAbstractTextureImagePrivate
{
public:
    QTextureImagePrivate(QTextureImage *qq)
        : QAbstractTextureImagePrivate(qq)
    {
    }

    Q_DECLARE_PUBLIC(QTextureImage)
    QUrl m_source;
};

class QImageTextureDataFunctor : public QTextureDataFunctor
{
public:
    QImageTextureDataFunctor(const QUrl &url)
        : QTextureDataFunctor()
        , m_url(url)
    {}

    // Will be executed from within a QAspectJob
    TexImageDataPtr operator ()() Q_DECL_FINAL
    {
        return TexImageDataPtr();
    }

    bool operator ==(const QTextureDataFunctor &other) const Q_DECL_FINAL
    {
        const QImageTextureDataFunctor *otherFunctor = dynamic_cast<const QImageTextureDataFunctor*>(&other);
        return (otherFunctor != Q_NULLPTR && otherFunctor->m_url == m_url);
    }

private:
    QUrl m_url;
};

/*!
    \class Qt3D::QTextureImage
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture
    image from an image source.

    It contains the necessary information mipmap level, layer, cube face and
    source URL to load at the proper place data into an OpenGL texture.
 */

/*!
    Constructs a new Qt3D::QTextureImage instance with \a parent as parent.
 */
QTextureImage::QTextureImage(QNode *parent)
    : QAbstractTextureImage(*new QTextureImagePrivate(this), parent)
{
}

QTextureImage::~QTextureImage()
{
}

/*!
    \return the source url from which data will be loaded of the texture image.
 */
QUrl QTextureImage::source() const
{
    Q_D(const QTextureImage);
    return d->m_source;
}

/*!
    Sets the source url of the texture image to \a source.
 */
void QTextureImage::setSource(const QUrl &source)
{
    Q_D(QTextureImage);
    if (source != d->m_source) {
        d->m_source = source;
        emit sourceChanged();
    }
}

/*!
    \returns the a Qt3D::QTextureDataFunctorPtr functor to be used by the
    backend to load the texture image data into an OpenGL texture object.
 */
QTextureDataFunctorPtr QTextureImage::dataFunctor() const
{
    return QTextureDataFunctorPtr(new QImageTextureDataFunctor(source()));
}

void QTextureImage::copy(const QNode *ref)
{
    QAbstractTextureImage::copy(ref);
    const QTextureImage *img = static_cast<const QTextureImage *>(ref);
    d_func()->m_source = img->source();
}

} // namespace Qt3D

QT_END_NAMESPACE
