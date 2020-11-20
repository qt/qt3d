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

#ifndef QT3DRENDER_TEXTUREIMAGEDATA_H
#define QT3DRENDER_TEXTUREIMAGEDATA_H

#include <Qt3DRender/qt3drender_global.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtOpenGL/qopengltexture.h>
#else
#include <QtGui/qopengltexture.h>
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
