/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "layerfilternode_p.h"
#include "qlayerfilter.h"
#include <Qt3DRender/private/qlayerfilter_p.h>
#include <algorithm>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

LayerFilterNode::LayerFilterNode()
    : FrameGraphNode(FrameGraphNode::LayerFilter)
    , m_filterMode(QLayerFilter::AcceptAnyMatchingLayers)
{
}

void LayerFilterNode::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QLayerFilter *node = qobject_cast<const QLayerFilter *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (m_filterMode != node->filterMode()) {
        m_filterMode = node->filterMode();
        markDirty(AbstractRenderer::FrameGraphDirty|AbstractRenderer::LayersDirty);
    }

    auto layerIds = qIdsForNodes(node->layers());
    std::sort(std::begin(layerIds), std::end(layerIds));
    if (m_layerIds != layerIds) {
        m_layerIds = layerIds;
        markDirty(AbstractRenderer::FrameGraphDirty|AbstractRenderer::LayersDirty);
    }
}

QNodeIdVector LayerFilterNode::layerIds() const
{
    return m_layerIds;
}

void LayerFilterNode::setLayerIds(const QNodeIdVector &list)
{
    m_layerIds = list;
}

QLayerFilter::FilterMode LayerFilterNode::filterMode() const
{
    return m_filterMode;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
