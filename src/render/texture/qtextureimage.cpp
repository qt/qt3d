// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtextureimage.h"
#include "qtextureimage_p.h"
#include "qtexture_p.h"
#include "qabstracttextureimage_p.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QTextureImage
    \inmodule Qt3DRender
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture
    image from an image source.

    It contains the necessary information mipmap level, layer, cube face and
    source URL to load at the proper place data into an OpenGL texture.
 */

/*!
    \qmltype TextureImage
    \instantiates Qt3DRender::QTextureImage
    \inherits AbstractTextureImage
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief Encapsulates the necessary information to create an OpenGL texture
    image from an image source.
*/

/*!
    \enum QTextureImage::Status

    This  enumeration specifies the status values for texture image loading.

    \value None The texture image loading has not been started yet.
    \value Loading The texture image loading has started, but not finised.
    \value Ready The texture image loading has finished.
    \value Error The texture image loading confronted an error.
*/

/*!
    \qmlproperty enumeration TextureImage::status

    This property holds the status of the texture image loading.

    \list
    \li TextureImage.None
    \li TextureImage.Loading
    \li TextureImage.Ready
    \li TextureImage.Error
    \endlist
    \readonly
*/

/*!
    \property Qt3DRender::QTextureImage::source

    This property holds the source url from which data for the texture
    image will be loaded.
*/

/*!
    \property QTextureImage::status

    This property holds the status of the texture image loading.

    \list
    \li TextureImage.None
    \li TextureImage.Loading
    \li TextureImage.Ready
    \li TextureImage.Error
    \endlist
    \readonly
*/

/*!
    Constructs a new Qt3DRender::QTextureImage instance with \a parent as parent.
 */
QTextureImage::QTextureImage(QNode *parent)
    : QAbstractTextureImage(*new QTextureImagePrivate, parent)
{
}

/*! \internal */
QTextureImage::~QTextureImage()
{
}

/*!
    Returns the source url from which data for the texture image will be loaded.
 */
QUrl QTextureImage::source() const
{
    Q_D(const QTextureImage);
    return d->m_source;
}

/*!
    Returns the current status.
 */
QTextureImage::Status QTextureImage::status() const
{
    Q_D(const QTextureImage);
    return d->m_status;
}

/*!
 * Returns whether mirroring is enabled or not.
 */
bool QTextureImage::isMirrored() const
{
    Q_D(const QTextureImage);
    return d->m_mirrored;
}

/*!
  \qmlproperty url Qt3D.Render::TextureImage::source

  This property holds the source url from which data for the texture
  image will be loaded.
*/

/*!
    Sets the source url of the texture image to \a source.
    \note This internally triggers a call to update the data generator.
 */
void QTextureImage::setSource(const QUrl &source)
{
    Q_D(QTextureImage);
    if (source != d->m_source) {
        d->m_source = source;
        const bool blocked = blockNotifications(true);
        emit sourceChanged(source);
        blockNotifications(blocked);
        notifyDataGeneratorChanged();
    }
}

/*!
  \property Qt3DRender::QTextureImage::mirrored

  This property specifies whether the image should be mirrored when loaded. This
  is a convenience to avoid having to manipulate images to match the origin of
  the texture coordinates used by the rendering API. By default this property
  is set to true. This has no effect when using GPU compressed texture formats.

  \warning This property results in a performance price payed at runtime when
  loading uncompressed or CPU compressed image formats such as PNG. To avoid this
  performance price it is better to set this property to false and load texture
  assets that have been pre-mirrored.

  \note OpenGL specifies the origin of texture coordinates from the lower left
  hand corner whereas DirectX uses the the upper left hand corner.

  \note When using cube map texture you'll probably want mirroring disabled as
  the cube map sampler takes a direction rather than regular texture
  coordinates.
*/

/*!
  \qmlproperty bool Qt3D.Render::TextureImage::mirrored

  This property specifies whether the image should be mirrored when loaded. This
  is a convenience to avoid having to manipulate images to match the origin of
  the texture coordinates used by the rendering API. By default this property
  is set to true. This has no effect when using GPU compressed texture formats.

  \warning This property results in a performance price payed at runtime when
  loading uncompressed or CPU compressed image formats such as PNG. To avoid this
  performance price it is better to set this property to false and load texture
  assets that have been pre-mirrored.

  \note OpenGL specifies the origin of texture coordinates from the lower left
  hand corner whereas DirectX uses the the upper left hand corner.

  \note When using cube map texture you'll probably want mirroring disabled as
  the cube map sampler takes a direction rather than regular texture
  coordinates.
*/

/*!
    Sets mirroring to \a mirrored.
    \note This internally triggers a call to update the data generator.
 */
void QTextureImage::setMirrored(bool mirrored)
{
    Q_D(QTextureImage);
    if (mirrored != d->m_mirrored) {
        d->m_mirrored = mirrored;
        const bool blocked = blockNotifications(true);
        emit mirroredChanged(mirrored);
        blockNotifications(blocked);
        notifyDataGeneratorChanged();
    }
}

/*!
 * Sets the status to \a status.
 * \param status
 */
void QTextureImage::setStatus(Status status)
{
    Q_D(QTextureImage);
    if (status != d->m_status) {
        d->m_status = status;
        emit statusChanged(status);
    }
}

/*!
    Returns the Qt3DRender::QTextureImageDataGeneratorPtr functor to be used by the
    backend to load the texture image data into an OpenGL texture object.
 */
QTextureImageDataGeneratorPtr QTextureImage::dataGenerator() const
{
    return QTextureImageDataGeneratorPtr(new QImageTextureDataFunctor(source(), isMirrored()));
}

/*
    The constructor creates a new QImageTextureDataFunctor::QImageTextureDataFunctor
    instance with the specified \a url.
 */
QImageTextureDataFunctor::QImageTextureDataFunctor(const QUrl &url, bool mirrored)
    : QTextureImageDataGenerator()
    , m_url(url)
    , m_status(QTextureImage::None)
    , m_mirrored(mirrored)
{
    if (url.isLocalFile()) {
        QFileInfo info(url.toLocalFile());
        m_lastModified = info.lastModified();
    }
}

QTextureImageDataPtr QImageTextureDataFunctor::operator ()()
{
    if (!m_url.isValid())
        return QTextureImageDataPtr();

    // We assume that a texture image is going to contain a single image data
    // For compressed dds or ktx textures a warning should be issued if
    // there are layers or 3D textures
    if (!Qt3DCore::QDownloadHelperService::isLocal(m_url))
        qWarning() << "QTextureImage only supports local url";

    QTextureImageDataPtr data = TextureLoadingHelper::loadTextureData(m_url, false, m_mirrored);

    // Data failed to load
    // Still create an empty QTextureImage to avoid trying to reload it every frame
    if (!data)
        data = QTextureImageDataPtr::create();
    return data;
}

bool QImageTextureDataFunctor::operator ==(const QTextureImageDataGenerator &other) const
{
    const QImageTextureDataFunctor *otherFunctor = functor_cast<QImageTextureDataFunctor>(&other);

    // if its the same URL, but different modification times, its not the same image.
    return (otherFunctor != nullptr &&
            otherFunctor->m_url == m_url &&
            otherFunctor->m_lastModified == m_lastModified &&
            otherFunctor->m_mirrored == m_mirrored);
}

QUrl QImageTextureDataFunctor::url() const
{
    return m_url;
}

bool QImageTextureDataFunctor::isMirrored() const
{
    return m_mirrored;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qtextureimage.cpp"

