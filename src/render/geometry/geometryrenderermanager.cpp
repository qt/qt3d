// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

QList<Qt3DCore::QNodeId> GeometryRendererManager::dirtyGeometryRenderers()
{
    QList<Qt3DCore::QNodeId> vector(m_dirtyGeometryRenderers);
    m_dirtyGeometryRenderers.clear();
    return vector;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
