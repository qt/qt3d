// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "proximityfilter_p.h"
#include <Qt3DRender/private/qproximityfilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

ProximityFilter::ProximityFilter()
    : FrameGraphNode(FrameGraphNode::ProximityFilter)
    , m_distanceThreshold(0.0f)
{
}

void ProximityFilter::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QProximityFilter *node = qobject_cast<const QProximityFilter *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    const auto entityId = Qt3DCore::qIdForNode(node->entity());
    if (entityId != m_entityId) {
        m_entityId = entityId;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }

    if (node->distanceThreshold() != m_distanceThreshold) {
        m_distanceThreshold = node->distanceThreshold();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
