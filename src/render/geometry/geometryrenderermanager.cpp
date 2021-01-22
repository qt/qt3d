/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "geometryrenderermanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

GeometryRendererManager::GeometryRendererManager()
{
}

GeometryRendererManager::~GeometryRendererManager()
{
}

void GeometryRendererManager::addDirtyGeometryRenderer(Qt3DCore::QNodeId bufferId)
{
    if (!m_dirtyGeometryRenderers.contains(bufferId))
        m_dirtyGeometryRenderers.push_back(bufferId);
}

QVector<Qt3DCore::QNodeId> GeometryRendererManager::dirtyGeometryRenderers()
{
    QVector<Qt3DCore::QNodeId> vector(m_dirtyGeometryRenderers);
    m_dirtyGeometryRenderers.clear();
    return vector;
}

void GeometryRendererManager::requestTriangleDataRefreshForGeometryRenderer(const Qt3DCore::QNodeId geometryRenderer)
{
    if (!m_geometryRenderersRequiringTriangleRefresh.contains(geometryRenderer))
        m_geometryRenderersRequiringTriangleRefresh.push_back(geometryRenderer);
}

bool GeometryRendererManager::isGeometryRendererScheduledForTriangleDataRefresh(const Qt3DCore::QNodeId geometryRenderer)
{
    return m_geometryRenderersRequiringTriangleRefresh.contains(geometryRenderer);
}

QVector<Qt3DCore::QNodeId> GeometryRendererManager::geometryRenderersRequiringTriangleDataRefresh()
{
    QVector<Qt3DCore::QNodeId> vector(m_geometryRenderersRequiringTriangleRefresh);
    m_geometryRenderersRequiringTriangleRefresh.clear();
    return vector;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
