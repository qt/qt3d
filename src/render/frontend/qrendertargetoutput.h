// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERTARGETOUTPUT_H
#define QT3DRENDER_QRENDERTARGETOUTPUT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/QAbstractTexture>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;
class QRenderTargetOutputPrivate;

class Q_3DRENDERSHARED_EXPORT QRenderTargetOutput : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(AttachmentPoint attachmentPoint READ attachmentPoint WRITE setAttachmentPoint NOTIFY attachmentPointChanged)
    Q_PROPERTY(QAbstractTexture *texture READ texture WRITE setTexture NOTIFY textureChanged)
    Q_PROPERTY(int mipLevel READ mipLevel WRITE setMipLevel NOTIFY mipLevelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture::CubeMapFace face READ face WRITE setFace NOTIFY faceChanged)

public:
    enum AttachmentPoint {
        Color0 = 0,
        Color1,
        Color2,
        Color3,
        Color4,
        Color5,
        Color6,
        Color7,
        Color8,
        Color9,
        Color10,
        Color11,
        Color12,
        Color13,
        Color14,
        Color15,
        Depth,
        Stencil,
        DepthStencil
    };
    Q_ENUM(AttachmentPoint) // LCOV_EXCL_LINE

    explicit QRenderTargetOutput(Qt3DCore::QNode *parent = nullptr);
    ~QRenderTargetOutput();

    AttachmentPoint attachmentPoint() const;
    QAbstractTexture *texture() const;
    int mipLevel() const;
    int layer() const;
    QAbstractTexture::CubeMapFace face() const;

public Q_SLOTS:
    void setAttachmentPoint(AttachmentPoint attachmentPoint);
    void setTexture(QAbstractTexture *texture);
    void setMipLevel(int level);
    void setLayer(int layer);
    void setFace(QAbstractTexture::CubeMapFace face);

Q_SIGNALS:
    void attachmentPointChanged(AttachmentPoint attachmentPoint);
    void textureChanged(QAbstractTexture *texture);
    void mipLevelChanged(int mipLevel);
    void layerChanged(int layer);
    void faceChanged(QAbstractTexture::CubeMapFace face);

protected:
    explicit QRenderTargetOutput(QRenderTargetOutputPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderTargetOutput)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QRenderTargetOutput::AttachmentPoint) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QRENDERTARGETOUTPUT_H
