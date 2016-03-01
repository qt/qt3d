/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QRENDERATTACHMENT_H
#define QT3DRENDER_QRENDERATTACHMENT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTextureProvider;
class QRenderAttachmentPrivate;

class QT3DRENDERSHARED_EXPORT QRenderAttachment : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(RenderAttachmentType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTextureProvider *texture READ texture WRITE setTexture NOTIFY textureChanged)
    Q_PROPERTY(int mipLevel READ mipLevel WRITE setMipLevel NOTIFY mipLevelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(CubeMapFace face READ face WRITE setFace NOTIFY faceChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    enum RenderAttachmentType {
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
    Q_ENUM(RenderAttachmentType)

    enum CubeMapFace {
        CubeMapPositiveX = 0x8515,  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        CubeMapNegativeX = 0x8516,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        CubeMapPositiveY = 0x8517,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        CubeMapNegativeY = 0x8518,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        CubeMapPositiveZ = 0x8519,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        CubeMapNegativeZ = 0x851A   // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    Q_ENUM(CubeMapFace)

    explicit QRenderAttachment(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QRenderAttachment();

    RenderAttachmentType type() const;
    QAbstractTextureProvider *texture() const;
    int mipLevel() const;
    int layer() const;
    CubeMapFace face() const;
    QString name() const;

public Q_SLOTS:
    void setType(RenderAttachmentType type);
    void setTexture(QAbstractTextureProvider *texture);
    void setMipLevel(int level);
    void setLayer(int layer);
    void setFace(CubeMapFace face);
    void setName(const QString &name);

Q_SIGNALS:
    void typeChanged(RenderAttachmentType type);
    void textureChanged(QAbstractTextureProvider *texture);
    void mipLevelChanged(int mipLevel);
    void layerChanged(int layer);
    void faceChanged(CubeMapFace face);
    void nameChanged(const QString &name);

protected:
    QRenderAttachment(QRenderAttachmentPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QRenderAttachment)
    QT3D_CLONEABLE(QRenderAttachment)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QRenderAttachment::RenderAttachmentType)

#endif // QT3DRENDER_QRENDERATTACHMENT_H
