/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

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
