/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QRENDERATTACHMENT_H
#define QT3D_QRENDERATTACHMENT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTexture;
class QRenderAttachmentPrivate;

class QT3DRENDERERSHARED_EXPORT QRenderAttachment : public QNode
{
    Q_OBJECT
    Q_ENUMS(RenderAttachmentType)
    Q_ENUMS(CubeMapFace)
    Q_PROPERTY(RenderAttachmentType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(Qt3D::QTexture *texture READ texture WRITE setTexture NOTIFY textureChanged)
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

    enum CubeMapFace {
        CubeMapPositiveX = 0x8515,  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        CubeMapNegativeX = 0x8516,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        CubeMapPositiveY = 0x8517,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        CubeMapNegativeY = 0x8518,  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        CubeMapPositiveZ = 0x8519,  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        CubeMapNegativeZ = 0x851A   // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    explicit QRenderAttachment(QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

    void setType(RenderAttachmentType type);
    RenderAttachmentType type() const;

    void setTexture(QTexture *texture);
    QTexture *texture() const;

    void setMipLevel(int level);
    int mipLevel() const;

    void setLayer(int layer);
    int layer() const;

    void setFace(CubeMapFace face);
    CubeMapFace face() const;

    void setName(const QString &name);
    QString name() const;

Q_SIGNALS:
    void typeChanged();
    void textureChanged();
    void mipLevelChanged();
    void layerChanged();
    void faceChanged();
    void nameChanged();

protected:
    Q_DECLARE_PRIVATE(QRenderAttachment)
    QRenderAttachment(QRenderAttachmentPrivate &dd, QNode *parent = 0);
    QNode *doClone(bool isClone = true) const Q_DECL_OVERRIDE;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QRENDERATTACHMENT_H
