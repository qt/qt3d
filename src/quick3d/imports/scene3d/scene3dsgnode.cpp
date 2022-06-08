// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scene3dsgnode_p.h"

#include <QtCore/qthread.h>

#include <scene3dlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DCore::Scene3DSGNode
    \internal

    \brief The Qt3DCore::Scene3DSGNode class is a simple QSGeometryNode subclass that
    uses a Qt3DCore::Scene3DMaterial

    The Qt3DCore::Scene3DSGNode allows to render a simple rectangle textured with a
    texture using premultiplied alpha.
 */
Scene3DSGNode::Scene3DSGNode()
    : QSGGeometryNode()
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
{
    setMaterial(&m_material);
    setOpaqueMaterial(&m_opaqueMaterial);
    setGeometry(&m_geometry);
    qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
}

Scene3DSGNode::~Scene3DSGNode()
{
    qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
    // The Scene3DSGNode is deleted by the QSGRenderThread when the SceneGraph
    // is terminated.
}

void Scene3DSGNode::setRect(const QRectF &rect, bool mirrorVertically)
{
    if (rect != m_rect) {
        m_rect = rect;
        // By default, map the item's bounding rect to normalized texture coordinates

        QRectF textureRect = QRectF(0.0f, 1.0f, 1.0f, -1.0f);
        if (mirrorVertically) {
            float tmp = textureRect.top();
            textureRect.setTop(textureRect.bottom());
            textureRect.setBottom(tmp);
        }

        QSGGeometry::updateTexturedRectGeometry(&m_geometry, m_rect, textureRect);
        markDirty(DirtyGeometry);
    }
}

void Scene3DSGNode::show()
{
    m_material.show();
    m_opaqueMaterial.show();
}

} // namespace Qt3DRender

QT_END_NAMESPACE
