// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstracttextureimage.h"
#include "qabstracttextureimage_p.h"
#include <Qt3DRender/qtextureimagedatagenerator.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QTextureImageDataGenerator::~QTextureImageDataGenerator() = default;

/*!
    \class Qt3DRender::QTextureImageDataGenerator
    \inmodule Qt3DRender
    \since 5.7
    \brief Provides texture image data for QAbstractTextureImage.

    QTextureImageDataGenerator is a data provider for QAbstractTexture.
    QTextureImageDataGenerator can be used to expand Qt3D with more ways to load
    texture image data as well as support user-defined formats and formats Qt3D
    does not natively support. The data is returned by the QTextureImageDataPtr
    which contains the data that will be loaded to the texture.
    QTextureImageDataGenerator is executed by Aspect jobs in the backend.
 */
/*!
    \typedef Qt3DRender::QTextureImageDataPtr
    \relates Qt3DRender::QTextureImageDataGenerator

    Shared pointer to \l QTextureImageData.
*/

/*!
    \fn Qt3DRender::QTextureImageDataPtr Qt3DRender::QTextureImageDataGenerator::operator()()

    Implement the method to return the texture image data.
*/

/*!
    \fn bool Qt3DRender::QTextureImageDataGenerator::operator ==(const QTextureImageDataGenerator &other) const

    Implement the method to compare this texture data generator to \a other.
    Returns a boolean that indicates whether the \l QAbstractTextureImage needs to reload
    the \l QTextureImageData.
*/

QAbstractTextureImagePrivate::QAbstractTextureImagePrivate()
    : QNodePrivate(),
      m_mipLevel(0),
      m_layer(0),
      m_face(QAbstractTexture::CubeMapPositiveX)
{
}

QAbstractTextureImagePrivate::~QAbstractTextureImagePrivate()
{
}

QTextureImageDataGeneratorPtr QAbstractTextureImagePrivate::dataGenerator() const
{
    Q_Q(const QAbstractTextureImage);
    return q->dataGenerator();
}

/*!
    \qmltype AbstractTextureImage
    \instantiates Qt3DRender::QAbstractTextureImage
    \inherits Node
    \inqmlmodule Qt3D.Render
    \qmlabstract
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture image.
*/

/*!
    \class Qt3DRender::QAbstractTextureImage
    \inmodule Qt3DRender
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture image.

    QAbstractTextureImage should be used as the means of providing image data to a
    QAbstractTexture. It contains the necessary information: mipmap
    level, layer, cube face load at the proper place data into an OpenGL texture.

    The actual data is provided through a QTextureImageDataGenerator that will be
    executed by Aspect jobs in the backend. QAbstractTextureImage should be
    subclassed to provide a functor and eventual additional properties needed by
    the functor to load actual data.

    \note: QAbstractTextureImage should never be shared. Expect crashes, undefined
    behavior at best if this rule is not respected.
 */

/*!
   \fn Qt3DRender::QTextureImageDataGeneratorPtr Qt3DRender::QAbstractTextureImage::dataGenerator() const

    Implement this method to return the QTextureImageDataGeneratorPtr instance,
    which will provide the data for the texture image.
*/

/*!
    Constructs a new QAbstractTextureImage instance with \a parent as parent.
 */
QAbstractTextureImage::QAbstractTextureImage(QNode *parent)
    : QNode(*new QAbstractTextureImagePrivate, parent)
{
}

/*! \internal */
QAbstractTextureImage::~QAbstractTextureImage()
{
}


/*!
    \qmlproperty int Qt3D.Render::AbstractTextureImage::mipLevel

    Holds the mipmap level of the texture image.
 */

/*!
    \property Qt3DRender::QAbstractTextureImage::mipLevel

    Holds the mipmap level of the texture image.
 */
int QAbstractTextureImage::mipLevel() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_mipLevel;
}

/*!
    \qmlproperty int Qt3D.Render::AbstractTextureImage::layer

    Holds the layer of the texture image.
 */

/*!
    \property Qt3DRender::QAbstractTextureImage::layer

    \return the layer of the texture image.
 */
int QAbstractTextureImage::layer() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_layer;
}

/*!
    \qmlproperty enumeration Qt3D.Render::AbstractTextureImage::face

    Holds the cube map face of the texture image.

    \value CubeMapPositiveX 0x8515   GL_TEXTURE_CUBE_MAP_POSITIVE_X
    \value CubeMapNegativeX 0x8516   GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    \value CubeMapPositiveY 0x8517   GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    \value CubeMapNegativeY 0x8518   GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    \value CubeMapPositiveZ 0x8519   GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    \value CubeMapNegativeZ 0x851A   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

    \note The cube map face has a meaning only for
    \l [CPP] {Qt3DRender::QAbstractTexture::}{TargetCubeMap} and
    \l [CPP] {Qt3DRender::QAbstractTexture::}{TargetCubeMapArray}.
 */

/*!
    \property Qt3DRender::QAbstractTextureImage::face

    Holds the cube map face of the texture image.

    \note The cube map face has a meaning only for
    \l {QAbstractTexture::}{TargetCubeMap} and
    \l {QAbstractTexture::}{TargetCubeMapArray}.
 */
QAbstractTexture::CubeMapFace QAbstractTextureImage::face() const
{
    Q_D(const QAbstractTextureImage);
    return d->m_face;
}

/*!
 * Sets the mip level of a texture to \a level.
 * \param level
 */
void QAbstractTextureImage::setMipLevel(int level)
{
    Q_D(QAbstractTextureImage);
    if (level != d->m_mipLevel) {
        d->m_mipLevel = level;
        emit mipLevelChanged(level);
    }
}

/*!
 * Sets the layer of a texture to \a layer.
 * \param layer
 */
void QAbstractTextureImage::setLayer(int layer)
{
    Q_D(QAbstractTextureImage);
    if (layer != d->m_layer) {
        d->m_layer = layer;
        emit layerChanged(layer);
    }
}

/*!
 * Sets the texture image face to \a face.
 * \param face
 */
void QAbstractTextureImage::setFace(QAbstractTexture::CubeMapFace face)
{
    Q_D(QAbstractTextureImage);
    if (face != d->m_face) {
        d->m_face = face;
        emit faceChanged(face);
    }
}

/*!
    Triggers an update of the data generator that is sent to the backend.
 */
void QAbstractTextureImage::notifyDataGeneratorChanged()
{
    Q_D(QAbstractTextureImage);
    d->update();
}

/*! \internal */
QAbstractTextureImage::QAbstractTextureImage(QAbstractTextureImagePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qabstracttextureimage.cpp"
