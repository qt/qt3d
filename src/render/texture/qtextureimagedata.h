// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_TEXTUREIMAGEDATA_H
#define QT3DRENDER_TEXTUREIMAGEDATA_H

#include <Qt3DRender/qt3drender_global.h>
#include <QtOpenGL/qopengltexture.h>
#include <QtGui/QImage>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureImageDataPrivate;

class Q_3DRENDERSHARED_EXPORT QTextureImageData
{
public:
    QTextureImageData();
    ~QTextureImageData();

    QTextureImageData &operator=(const QTextureImageData &other);

    void cleanup() noexcept;

    bool isCompressed() const noexcept;

    int width() const noexcept;
    int height() const noexcept;
    int depth() const noexcept;

    int layers() const noexcept;
    int mipLevels() const noexcept;
    int faces() const noexcept;

    void setWidth(int width) noexcept;
    void setHeight(int height) noexcept;
    void setDepth(int depth) noexcept;

    void setLayers(int layers) noexcept;
    void setMipLevels(int mipLevels) noexcept;
    void setFaces(int faces) noexcept;

    int alignment() const noexcept;
    void setAlignment(int alignment) noexcept;

    QOpenGLTexture::Target target() const noexcept;
    QOpenGLTexture::TextureFormat format() const noexcept;

    QOpenGLTexture::PixelFormat pixelFormat() const noexcept;
    QOpenGLTexture::PixelType pixelType() const noexcept;

    void setTarget(QOpenGLTexture::Target target) noexcept;
    void setFormat(QOpenGLTexture::TextureFormat format) noexcept;

    void setPixelFormat(QOpenGLTexture::PixelFormat pixelFormat) noexcept;
    void setPixelType(QOpenGLTexture::PixelType pixelType) noexcept;

    void setImage(const QImage &);
    void setData(const QByteArray &data,
                 int blockSize,
                 bool isCompressed = false);

    void setData(const QByteArray &data,
                 std::function<QByteArray(QByteArray rawData, int layer, int face, int mipmapLevel)>,
                 bool isCompressed = false);

    QByteArray data(int layer = 0, int face = 0, int mipmapLevel = 0) const;
protected:
    QTextureImageData(QTextureImageDataPrivate &dd);

private:
    Q_DECLARE_PRIVATE(QTextureImageData)
    QTextureImageDataPrivate *d_ptr;
};

typedef QSharedPointer<QTextureImageData> QTextureImageDataPtr;

} // namespace Qt3DRender


QT_END_NAMESPACE

#endif // QT3DRENDER_TEXTUREIMAGEDATA_H
