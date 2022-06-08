// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
