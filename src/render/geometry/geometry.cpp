// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "geometry_p.h"
#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qgeometry.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Geometry::Geometry()
    : BackendNode(ReadWrite)
    , m_geometryDirty(false)
{
}

Geometry::~Geometry()
{
}

void Geometry::cleanup()
{
    QBackendNode::setEnabled(false);
    m_attributes.clear();
    m_geometryDirty = false;
    m_boundingPositionAttribute = Qt3DCore::QNodeId();
    m_min = QVector3D();
    m_max = QVector3D();
}

void Geometry::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QGeometry *node = qobject_cast<const QGeometry *>(frontEnd);
    if (!node)
        return;

    m_geometryDirty |= firstTime;

    QNodeIdVector attribs = qIdsForNodes(node->attributes());
    std::sort(std::begin(attribs), std::end(attribs));
    if (m_attributes != attribs) {
        m_attributes = attribs;
        m_geometryDirty = true;
    }

    if ((node->boundingVolumePositionAttribute() && node->boundingVolumePositionAttribute()->id() != m_boundingPositionAttribute) ||
        // Note: doesn't set dirtyness as this parameter changing doesn't need a new VAO update.
        (!node->boundingVolumePositionAttribute() && !m_boundingPositionAttribute.isNull())) {
        m_boundingPositionAttribute = node->boundingVolumePositionAttribute() ? node->boundingVolumePositionAttribute()->id() : QNodeId{};
    }

    markDirty(AbstractRenderer::GeometryDirty);
}

void Geometry::unsetDirty()
{
    m_geometryDirty = false;
}

// Called from calcboundingvolumejob (in a QtConcurrent thead (can't send
// update changes from such a thread))
void Geometry::updateExtent(const QVector3D &min, const QVector3D &max)
{
    // Send notification to frontend
    if (m_min != min) {
        m_min = min;
    }

    if (m_max != max) {
        m_max = max;
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
