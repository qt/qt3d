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

#include "qtextureimagedata_p.h"
#include <QDebug>
#include <QFileInfo>
#include <QFile>

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
    , m_target(QOpenGLTexture::Target2D)
    , m_format(QOpenGLTexture::RGBA8_UNorm)
    , m_pixelFormat(QOpenGLTexture::RGBA)
    , m_pixelType(QOpenGLTexture::UInt8)
    , m_isCompressed(false)
{
}

QByteArray QTextureImageDataPrivate::data(int layer, int face, int mipmapLevel) const
{
    if (layer < 0 || layer >= m_layers ||
            face < 0 || face >= m_faces ||
            mipmapLevel < 0 || mipmapLevel >= m_mipLevels) {
        qWarning() << Q_FUNC_INFO << "Requesting texture data for invalid layer, face or mipMapLevel";
        return QByteArray();
    }

    int offset = layer * layerSize() + face * faceSize();

    for (int i = 0; i < mipmapLevel; i++)
        offset += mipmapLevelSize(i);

    return QByteArray::fromRawData(m_data.constData() + offset, mipmapLevelSize(mipmapLevel));
}

void QTextureImageDataPrivate::setData(const QByteArray &data,
                                       int blockSize,
                                       bool isCompressed)
{
    m_isCompressed = isCompressed;
    m_data = data;
    m_blockSize = blockSize;
}

int QTextureImageDataPrivate::layerSize() const
{
    return m_faces * faceSize();
}

int QTextureImageDataPrivate::faceSize() const
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

QTextureImageData::QTextureImageData()
    : d_ptr(new QTextureImageDataPrivate())
{
}

QTextureImageData::QTextureImageData(QTextureImageDataPrivate &dd)
    : d_ptr(&dd)
{
}

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

void QTextureImageData::cleanup() Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_width = -1;
    d->m_height = -1;
    d->m_depth = -1;
    d->m_layers = -1;
    d->m_faces = -1;
    d->m_mipLevels = -1;
    d->m_blockSize = 0;
    d->m_isCompressed = false;
    d->m_data.clear();
}

bool QTextureImageData::isCompressed() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_isCompressed;
}

int QTextureImageData::width() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_width;
}

int QTextureImageData::height() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_height;
}

int QTextureImageData::depth() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_depth;
}

int QTextureImageData::layers() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_layers;
}

int QTextureImageData::mipLevels() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_mipLevels;
}

int QTextureImageData::faces() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_faces;;
}

void QTextureImageData::setWidth(int width) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_width = width;
}

void QTextureImageData::setHeight(int height) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_height = height;
}

void QTextureImageData::setDepth(int depth) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_depth = depth;
}

void QTextureImageData::setLayers(int layers) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_layers = layers;
}

void QTextureImageData::setMipLevels(int mipLevels) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_mipLevels = mipLevels;
}

void QTextureImageData::setFaces(int faces) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_faces = faces;
}

QOpenGLTexture::Target QTextureImageData::target() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_target;
}

QOpenGLTexture::TextureFormat QTextureImageData::format() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_format;
}

void QTextureImageData::setTarget(QOpenGLTexture::Target target) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_target = target;
}

void QTextureImageData::setFormat(QOpenGLTexture::TextureFormat format) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_format = format;
}

void QTextureImageData::setPixelFormat(QOpenGLTexture::PixelFormat pixelFormat) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_pixelFormat = pixelFormat;
}

void QTextureImageData::setPixelType(QOpenGLTexture::PixelType pixelType) Q_DECL_NOTHROW
{
    Q_D(QTextureImageData);
    d->m_pixelType = pixelType;
}

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
    QByteArray imageBytes((const char*) glImage.constBits(), glImage.byteCount());
    setData(imageBytes, d->m_blockSize, false);
    d->m_format = QOpenGLTexture::RGBA8_UNorm;
    d->m_pixelFormat = QOpenGLTexture::RGBA;
    d->m_pixelType = QOpenGLTexture::UInt8;
    d->m_target = QOpenGLTexture::Target2D;
}

void QTextureImageData::setData(const QByteArray &data, int blockSize, bool isCompressed)
{
    Q_D(QTextureImageData);
    d->setData(data, blockSize, isCompressed);
}

QByteArray QTextureImageData::data(int layer, int face, int mipmapLevel) const
{
    Q_D(const QTextureImageData);
    return d->data(layer, face, mipmapLevel);
}

QOpenGLTexture::PixelFormat QTextureImageData::pixelFormat() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_pixelFormat;
}

QOpenGLTexture::PixelType QTextureImageData::pixelType() const Q_DECL_NOTHROW
{
    Q_D(const QTextureImageData);
    return d->m_pixelType;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
