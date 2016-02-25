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

#ifndef QT3DRENDER_QRENDERTARGETOUTPUT_H
#define QT3DRENDER_QRENDERTARGETOUTPUT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTextureProvider;
class QRenderTargetOutputPrivate;

class QT3DRENDERSHARED_EXPORT QRenderTargetOutput : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(AttachmentPoint attachmentPoint READ attachmentPoint WRITE setAttachmentPoint NOTIFY attachmentPointChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTextureProvider *texture READ texture WRITE setTexture NOTIFY textureChanged)
    Q_PROPERTY(int mipLevel READ mipLevel WRITE setMipLevel NOTIFY mipLevelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(CubeMapFace face READ face WRITE setFace NOTIFY faceChanged)

public:
    enum AttachmentPoint {
        ColorAttachment0 = 0,
        ColorAttachment1,
        ColorAttachment2,
        ColorAttachment3,
        ColorAttachment4,
        ColorAttachment5,
        ColorAttachment6,
        ColorAttachment7,
        ColorAttachment8,
        ColorAttachment9,
        ColorAttachment10,
        ColorAttachment11,
        ColorAttachment12,
        ColorAttachment13,
        ColorAttachment14,
        ColorAttachment15,
        DepthAttachment,
        StencilAttachment,
        DepthStencilAttachment
    };
    Q_ENUM(AttachmentPoint)

    enum CubeMapFace {
        CubeMapPositiveX = 0x8515,  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        CubeMapNegativeX = 0x8516,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        CubeMapPositiveY = 0x8517,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        CubeMapNegativeY = 0x8518,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        CubeMapPositiveZ = 0x8519,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        CubeMapNegativeZ = 0x851A   // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    Q_ENUM(CubeMapFace)

    explicit QRenderTargetOutput(Qt3DCore::QNode *parent = 0);
    ~QRenderTargetOutput();

    AttachmentPoint attachmentPoint() const;
    QAbstractTextureProvider *texture() const;
    int mipLevel() const;
    int layer() const;
    CubeMapFace face() const;

public Q_SLOTS:
    void setAttachmentPoint(AttachmentPoint attachmentPoint);
    void setTexture(QAbstractTextureProvider *texture);
    void setMipLevel(int level);
    void setLayer(int layer);
    void setFace(CubeMapFace face);

Q_SIGNALS:
    void attachmentPointChanged(AttachmentPoint attachmentPoint);
    void textureChanged(QAbstractTextureProvider *texture);
    void mipLevelChanged(int mipLevel);
    void layerChanged(int layer);
    void faceChanged(CubeMapFace face);

protected:
    QRenderTargetOutput(QRenderTargetOutputPrivate &dd, Qt3DCore::QNode *parent = 0);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QRenderTargetOutput)
    QT3D_CLONEABLE(QRenderTargetOutput)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QRenderTargetOutput::AttachmentPoint)

#endif // QT3DRENDER_QRENDERTARGETOUTPUT_H
