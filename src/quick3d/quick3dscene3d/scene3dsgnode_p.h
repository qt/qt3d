// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_SCENE3DSGNODE_P_H
#define QT3DRENDER_SCENE3DSGNODE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick/QSGGeometryNode>

#include <scene3dsgmaterial_p.h>

QT_BEGIN_NAMESPACE

class QSGTexture;

namespace Qt3DRender {

class Scene3DSGNode : public QSGGeometryNode
{
public:
    Scene3DSGNode();
    ~Scene3DSGNode();

    void setTexture(QSGTexture *texture) noexcept
    {
        m_material.setTexture(texture);
        m_opaqueMaterial.setTexture(texture);
        markDirty(DirtyMaterial);
    }
    QSGTexture *texture() const noexcept { return m_material.texture(); }

    void setRect(const QRectF &rect, bool mirrorVertically = false);
    QRectF rect() const noexcept { return m_rect; }

    void show();

private:
    Scene3DSGMaterial m_material;
    Scene3DSGMaterial m_opaqueMaterial;
    QSGGeometry m_geometry;
    QRectF m_rect;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_SCENE3DSGNODE_P_H
