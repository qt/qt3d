/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_TEXTUREIMAGEDATA_H
#define QT3DRENDER_TEXTUREIMAGEDATA_H

#include <Qt3DRender/qt3drender_global.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  include <QOpenGLTexture>
#else
#  include <QtGui/qopengltexture.h>
#endif
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

    void cleanup() Q_DECL_NOTHROW;

    bool isCompressed() const Q_DECL_NOTHROW;

    int width() const Q_DECL_NOTHROW;
    int height() const Q_DECL_NOTHROW;
    int depth() const Q_DECL_NOTHROW;

    int layers() const Q_DECL_NOTHROW;
    int mipLevels() const Q_DECL_NOTHROW;
    int faces() const Q_DECL_NOTHROW;

    void setWidth(int width) Q_DECL_NOTHROW;
    void setHeight(int height) Q_DECL_NOTHROW;
    void setDepth(int depth) Q_DECL_NOTHROW;

    void setLayers(int layers) Q_DECL_NOTHROW;
    void setMipLevels(int mipLevels) Q_DECL_NOTHROW;
    void setFaces(int faces) Q_DECL_NOTHROW;

    QOpenGLTexture::Target target() const Q_DECL_NOTHROW;
    QOpenGLTexture::TextureFormat format() const Q_DECL_NOTHROW;

    QOpenGLTexture::PixelFormat pixelFormat() const Q_DECL_NOTHROW;
    QOpenGLTexture::PixelType pixelType() const Q_DECL_NOTHROW;

    void setTarget(QOpenGLTexture::Target target) Q_DECL_NOTHROW;
    void setFormat(QOpenGLTexture::TextureFormat format) Q_DECL_NOTHROW;

    void setPixelFormat(QOpenGLTexture::PixelFormat pixelFormat) Q_DECL_NOTHROW;
    void setPixelType(QOpenGLTexture::PixelType pixelType) Q_DECL_NOTHROW;

    void setImage(const QImage &);
    void setData(const QByteArray &data,
                 int blockSize,
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
