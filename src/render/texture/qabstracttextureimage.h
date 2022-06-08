// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QABSTRACTTEXTUREIMAGE_H
#define QT3DRENDER_QABSTRACTTEXTUREIMAGE_H

#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTextureImagePrivate;

class Q_3DRENDERSHARED_EXPORT QAbstractTextureImage : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(int mipLevel READ mipLevel WRITE setMipLevel NOTIFY mipLevelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture::CubeMapFace face READ face WRITE setFace NOTIFY faceChanged)
public:
    explicit QAbstractTextureImage(Qt3DCore::QNode *parent = nullptr);
    virtual ~QAbstractTextureImage();

    int mipLevel() const;
    int layer() const;
    QAbstractTexture::CubeMapFace face() const;

public Q_SLOTS:
    void setMipLevel(int level);
    void setLayer(int layer);
    void setFace(QAbstractTexture::CubeMapFace face);

Q_SIGNALS:
    void mipLevelChanged(int mipLevel);
    void layerChanged(int layer);
    void faceChanged(QAbstractTexture::CubeMapFace face);

protected:
    void notifyDataGeneratorChanged();
    virtual QTextureImageDataGeneratorPtr dataGenerator() const = 0;
    explicit QAbstractTextureImage(QAbstractTextureImagePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractTextureImage)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTTEXTUREIMAGE_H
