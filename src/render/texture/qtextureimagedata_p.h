// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_TEXTUREDATA_P_H
#define QT3DRENDER_TEXTUREDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtextureimagedata.h"
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <functional>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QTextureImageDataPrivate
{
public:
    QTextureImageDataPrivate();

    void setData(const QByteArray &data, int blockSize, bool isCompressed);
    void setData(const QByteArray &data,
                 std::function<QByteArray(QByteArray, int, int, int)> dataExtractor,
                 bool isCompressed);

    bool setCompressedFile(const QString &source);

    QByteArray data(int layer, int face, int mipmapLevel) const;

    int m_width;
    int m_height;
    int m_depth;
    int m_layers;
    int m_faces;
    int m_mipLevels;
    int m_blockSize;
    int m_alignment;

    QOpenGLTexture::Target m_target;
    QOpenGLTexture::TextureFormat m_format;
    QOpenGLTexture::PixelFormat m_pixelFormat;
    QOpenGLTexture::PixelType m_pixelType;

    bool m_isCompressed;
    // ### Qt 6
    // QTextureImageData was originally written with assumptions around the internal data format
    // matching dds layout. This is an ugly, but easy, way to add basic ktx support without any
    // public API changes. Consider https://codereview.qt-project.org/#/c/178474/ for Qt 6.
    bool m_isKtx;
    QByteArray m_data;
    std::function<QByteArray(QByteArray rawData, int layer, int face, int mipmapLevel)> m_dataExtractor;

    static QTextureImageDataPrivate *get(QTextureImageData *imageData);

private:
    int ddsLayerSize() const;
    int ddsFaceSize() const;
    int mipmapLevelSize(int level) const;
    QByteArray ktxData(int layer, int face, int mipmapLevel) const;
};

} // namespace Qt3DRender


QT_END_NAMESPACE

#endif // QT3DRENDER_TEXTUREDATA_P_H
