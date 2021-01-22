/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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

#include "trianglesextractor_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/triangleboundingvolume_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

TrianglesExtractor::TrianglesExtractor(GeometryRenderer *renderer, NodeManagers *manager)
    : TrianglesVisitor(manager)
    , m_renderer(renderer)
{
}

QVector<RayCasting::QBoundingVolume *> TrianglesExtractor::extract(const Qt3DCore::QNodeId id)
{
    qDeleteAll(m_volumes);
    apply(m_renderer, id);
    return m_volumes;
}

void TrianglesExtractor::visit(uint andx, const Vector3D &a, uint bndx, const Vector3D &b, uint cndx, const Vector3D &c)
{
    Q_UNUSED(andx); Q_UNUSED(bndx); Q_UNUSED(cndx);
    m_volumes.push_back(new TriangleBoundingVolume(m_nodeId, a, b, c));
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
