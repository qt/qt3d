/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

void Scene3DSGNode::setRect(const QRectF &rect, const QRectF textureRect)
{
    if (rect != m_rect) {
        m_rect = rect;
        // By default, map the item's bounding rect to normalized texture coordinates
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
