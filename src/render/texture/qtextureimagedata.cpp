// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtextureimagedata_p.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QTextureImageDataPrivate::QTextureImageDataPrivate()
    : m_width(-1)
    , m_height(-1)
    , m_depth(-1)
    , m_layers(-1)
    , m_faces(-1)
    , m_mipLevels(-1)
    , m_blockSize(-1)
    , m_alignment(1)
    , m_target(QOpenGLTexture::Target2D)
    , m_format(QOpenGLTexture::NoFormat)
    , m_pixelFormat(QOpenGLTexture::RGBA)
    , m_pixelType(QOpenGLTexture::UInt8)
    , m_isCompressed(false)
    , m_isKtx(false)
{
}

QByteArray QTextureImageDataPrivate::ktxData(int layer, int face, int mipmapLevel) const
{
    Q_ASSERT(layer >= 0 && layer < m_layers &&
            face >= 0 && face < m_faces &&
            mipmapLevel >= 0 && mipmapLevel < m_mipLevels);

    int offset = 0;
    for (int i = 0; i < mipmapLevel; i++)
        offset += (mipmapLevelSize(i) * m_faces * m_layers) + 4;
    const int selectedMipmapLevelSize = mipmapLevelSize(mipmapLevel);
    offset += (selectedMipmapLevelSize * m_faces * layer) + (selectedMipmapLevelSize * face) + 4;

    return QByteArray::fromRawData(m_data.constData() + offset, selectedMipmapLevelSize);
}

QByteArray QTextureImageDataPrivate::data(int layer, int face, int mipmapLevel) const
{
    if (layer < 0 || layer >= m_layers ||
            face < 0 || face >= m_faces ||
            mipmapLevel < 0 || mipmapLevel >= m_mipLevels) {
        qWarning() << Q_FUNC_INFO << "Requesting texture data for invalid layer, face or mipMapLevel";
        return QByteArray();
    }

    if (m_dataExtractor)
        return m_dataExtractor(m_data, layer, face, mipmapLevel);

    if (m_isKtx)
        return ktxData(layer, face, mipmapLevel);

    int offset = layer * ddsLayerSize() + face * ddsFaceSize();

    for (int i = 0; i < mipmapLevel; i++)
        offset += mipmapLevelSize(i);

    return QByteArray::fromRawData(m_data.constData() + offset, mipmapLevelSize(mipmapLevel));
}

QTextureImageDataPrivate *QTextureImageDataPrivate::get(QTextureImageData *imageData)
{
    return imageData->d_func();
}

void QTextureImageDataPrivate::setData(const QByteArray &data,
                                       int blockSize,
                                       bool isCompressed)
{
    m_isCompressed = isCompressed;
    m_data = data;
    m_blockSize = blockSize;
}

void QTextureImageDataPrivate::setData(const QByteArray &data,
                                       std::function<QByteArray(QByteArray rawData, int layer, int face, int mipmapLevel)> dataExtractor,
                                       bool isCompressed)
{
    m_isCompressed = isCompressed;
    m_data = data;
    m_dataExtractor = dataExtractor;
}

int QTextureImageDataPrivate::ddsLayerSize() const
{
    return m_faces * ddsFaceSize();
}

int QTextureImageDataPrivate::ddsFaceSize() const
{
    int size = 0;

    for (int i = 0; i < m_mipLevels; i++)
        size += mipmapLevelSize(i);

    return size;
}

// XXX check if this works for ETC1 compression
int QTextureImageDataPrivate::mipmapLevelSize(int level) const
{
    int w = qMax(m_width >> level, 1);
    int h = qMax(m_height >> level, 1);
    int d = qMax(m_depth >> level, 1);

    if (m_isCompressed)
        return ((w + 3) / 4) * ((h + 3) / 4) * m_blockSize * d;
    else
        return w * h * m_blockSize * d;
}

/*!
    \class Qt3DRender::QTextureImageData
    \inmodule Qt3DRender
    \since 5.5
    \brief QTextureImageData stores data representing a texture.
 */

/*!
  Constructs a new Qt3DRender::QTextureImageData.
*/
QTextureImageData::QTextureImageData()
    : d_ptr(new QTextureImageDataPrivate())
{
}

/*! \internal */
QTextureImageData::QTextureImageData(QTextureImageDataPrivate &dd)
    : d_ptr(&dd)
{
}

/*! \internal */
QTextureImageData::~QTextureImageData()
{
    cleanup();
    delete d_ptr;
}

QTextureImageData &QTextureImageData::operator=(const QTextureImageData &other)
{
    Q_D(QTextureImageData);
    *d = *other.d_ptr;
    return *this;
}

/*!
  Remove stored texture data and return the object to its initial state
 */
void QTextureImageData::cleanup() noexcept
{
    Q_D(QTextureImageData);
    d->m_width = -1;
    d->m_height = -1;
    d->m_depth = -1;
    d->m_layers = -1;
    d->m_faces = -1;
    d->m_mipLevels = -1;
    d->m_blockSize = 0;
    d->m_alignment = 1;
    d->m_isCompressed = false;
    d->m_data.clear();
}

/*!
  \return true if the stored texture is in a compressed format
 */
bool QTextureImageData::isCompressed() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_isCompressed;
}

/*!
  \return the width of the stored texture
*/
int QTextureImageData::width() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_width;
}

/*!
  \return the height of the stored texture
 */
int QTextureImageData::height() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_height;
}

/*!
  \return the depth of the stored texture
 */
int QTextureImageData::depth() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_depth;
}

/*!
  \return the number of layers in the stored texture
 */
int QTextureImageData::layers() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_layers;
}

/*!
  \return the number of mip levels in the stored texture
 */
int QTextureImageData::mipLevels() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_mipLevels;
}

/*!
  \return the number of faces in the stored texture
 */
int QTextureImageData::faces() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_faces;;
}

/*!
 * Sets the width to \a width.
 * \param setWidth
 */
void QTextureImageData::setWidth(int width) noexcept
{
    Q_D(QTextureImageData);
    d->m_width = width;
}

/*!
 * Sets the height to \a height.
 * \param setHeight
 */
void QTextureImageData::setHeight(int height) noexcept
{
    Q_D(QTextureImageData);
    d->m_height = height;
}

/*!
 * Sets the depth to \a depth.
 * \param setDepth
 */
void QTextureImageData::setDepth(int depth) noexcept
{
    Q_D(QTextureImageData);
    d->m_depth = depth;
}

/*!
 * Sets the layers to \a layers.
 * \param setLayers
 */
void QTextureImageData::setLayers(int layers) noexcept
{
    Q_D(QTextureImageData);
    d->m_layers = layers;
}

/*!
 * Sets the mip levels to \a mipLevels.
 * \param setMipLevels
 */
void QTextureImageData::setMipLevels(int mipLevels) noexcept
{
    Q_D(QTextureImageData);
    d->m_mipLevels = mipLevels;
}

/*!
 * Sets the faces to \a faces.
 * \param setFaces
 */
void QTextureImageData::setFaces(int faces) noexcept
{
    Q_D(QTextureImageData);
    d->m_faces = faces;
}

/*!
 * Sets the \a alignment requirements for the image.
 */
void QTextureImageData::setAlignment(int alignment) noexcept
{
    Q_D(QTextureImageData);
    d->m_alignment = alignment;
}

/*!
  \return the alignment requirement for the image.
 */
int QTextureImageData::alignment() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_alignment;
}

/*!
  \return the target for the stored texture.
 */
QOpenGLTexture::Target QTextureImageData::target() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_target;
}

/*!
  \return the format of the stored texture.
 */
QOpenGLTexture::TextureFormat QTextureImageData::format() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_format;
}

/*!
 * Sets the target to \a target.
 */
void QTextureImageData::setTarget(QOpenGLTexture::Target target) noexcept
{
    Q_D(QTextureImageData);
    d->m_target = target;
}

/*!
 * Sets the format to \a format.
 */
void QTextureImageData::setFormat(QOpenGLTexture::TextureFormat format) noexcept
{
    Q_D(QTextureImageData);
    d->m_format = format;
}

/*!
 * Sets the pixel format to \a pixelFormat.
 */
void QTextureImageData::setPixelFormat(QOpenGLTexture::PixelFormat pixelFormat) noexcept
{
    Q_D(QTextureImageData);
    d->m_pixelFormat = pixelFormat;
}

/*!
 * Sets the pixel type to \a pixelType.
 */
void QTextureImageData::setPixelType(QOpenGLTexture::PixelType pixelType) noexcept
{
    Q_D(QTextureImageData);
    d->m_pixelType = pixelType;
}

/*!
  Copies the image \a image as raw data within this object.
 */
void QTextureImageData::setImage(const QImage &image)
{
    Q_D(QTextureImageData);
    d->m_width = image.width();
    d->m_height = image.height();
    d->m_depth = 1;
    d->m_faces = 1;
    d->m_layers = 1;
    d->m_mipLevels = 1;
    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);
    Q_ASSERT_X(glImage.bytesPerLine() == (glImage.width() * glImage.depth() + 7) / 8,
               "QTextureImageData::setImage", "glImage is not packed"); // QTBUG-48330
    d->m_blockSize = 4;
    QByteArray imageBytes((const char*) glImage.constBits(), glImage.sizeInBytes());
    setData(imageBytes, d->m_blockSize, false);
    d->m_format = QOpenGLTexture::RGBA8_UNorm;
    d->m_pixelFormat = QOpenGLTexture::RGBA;
    d->m_pixelType = QOpenGLTexture::UInt8;
    d->m_target = QOpenGLTexture::Target2D;
}

/*!
  Stores the data \a data with blocksize \a blockSize and if the data to be stored is compressed \a isCompressed.
 */
void QTextureImageData::setData(const QByteArray &data, int blockSize, bool isCompressed)
{
    Q_D(QTextureImageData);
    d->setData(data, blockSize, isCompressed);
}

void QTextureImageData::setData(const QByteArray &data, std::function<QByteArray(QByteArray data, int layer, int face, int mipmapLevel)> dataExtractor, bool isCompressed)
{
    Q_D(QTextureImageData);
    d->setData(data, dataExtractor, isCompressed);
}

/*!
  \return the raw image data for the texture at layer \a layer, face \a face and mipmapLevel \a mipmapLevel.
 */
QByteArray QTextureImageData::data(int layer, int face, int mipmapLevel) const
{
    Q_D(const QTextureImageData);
    return d->data(layer, face, mipmapLevel);
}

/*!
  \return the pixel format of the stored texture.
 */
QOpenGLTexture::PixelFormat QTextureImageData::pixelFormat() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_pixelFormat;
}

/*!
  \return the pixel type of the stored texture.
 */
QOpenGLTexture::PixelType QTextureImageData::pixelType() const noexcept
{
    Q_D(const QTextureImageData);
    return d->m_pixelType;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
