// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTEXTUREDATA_H
#define QT3DRENDER_QTEXTUREDATA_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/qtexturewrapmode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureDataPrivate;

class Q_3DRENDERSHARED_EXPORT QTextureData
{
public:
    QTextureData();
    ~QTextureData();

    QAbstractTexture::Target target() const;
    void setTarget(QAbstractTexture::Target target);

    QAbstractTexture::TextureFormat format() const;
    void setFormat(QAbstractTexture::TextureFormat);

    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

    int depth() const;
    void setDepth(int depth);

    int layers() const;
    void setLayers(int layers);

    bool isAutoMipMapGenerationEnabled() const;
    void setAutoMipMapGenerationEnabled(bool isAutoMipMapGenerationEnabled);

    float maximumAnisotropy() const;
    void setMaximumAnisotropy(float maximumAnisotropy);

    QAbstractTexture::Filter minificationFilter() const;
    void setMinificationFilter(QAbstractTexture::Filter filter);

    QAbstractTexture::Filter magnificationFilter() const;
    void setMagnificationFilter(QAbstractTexture::Filter filter);

    QTextureWrapMode::WrapMode wrapModeX() const;
    void setWrapModeX(QTextureWrapMode::WrapMode wrapModeX);

    QTextureWrapMode::WrapMode wrapModeY() const;
    void setWrapModeY(QTextureWrapMode::WrapMode wrapModeY);

    QTextureWrapMode::WrapMode wrapModeZ() const;
    void setWrapModeZ(QTextureWrapMode::WrapMode wrapModeZ);

    QAbstractTexture::ComparisonFunction comparisonFunction() const;
    void setComparisonFunction(QAbstractTexture::ComparisonFunction comparisonFunction);

    QAbstractTexture::ComparisonMode comparisonMode() const;
    void setComparisonMode(QAbstractTexture::ComparisonMode comparisonMode);

    QList<QTextureImageDataPtr> imageData() const;
    void addImageData(const QTextureImageDataPtr &imageData);

private:
    Q_DECLARE_PRIVATE(QTextureData)
    QTextureDataPrivate *d_ptr;
};

typedef QSharedPointer<QTextureData> QTextureDataPtr;

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTEXTUREDATA_H
