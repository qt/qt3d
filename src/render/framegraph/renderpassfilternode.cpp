// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderpassfilternode_p.h"
#include "qfilterkey.h"
#include "qrenderpassfilter.h"
#include <Qt3DRender/private/qrenderpassfilter_p.h>
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

RenderPassFilter::RenderPassFilter()
    : FrameGraphNode(FrameGraphNode::RenderPassFilter)
{
}

void RenderPassFilter::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QRenderPassFilter *node = qobject_cast<const QRenderPassFilter *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (firstTime)
        m_parameterPack.clear();

    auto parameters = qIdsForNodes(node->parameters());
    std::sort(std::begin(parameters), std::end(parameters));
    if (m_parameterPack.parameters() != parameters) {
        m_parameterPack.setParameters(parameters);
        markDirty(AbstractRenderer::FrameGraphDirty);
    }

    auto filterIds = qIdsForNodes(node->matchAny());
    std::sort(std::begin(filterIds), std::end(filterIds));
    if (m_filters != filterIds) {
        m_filters = filterIds;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}


QList<Qt3DCore::QNodeId> RenderPassFilter::filters() const
{
    return m_filters;
}

void RenderPassFilter::appendFilter(Qt3DCore::QNodeId criterionId)
{
    if (!m_filters.contains(criterionId))
        m_filters.append(criterionId);
}

void RenderPassFilter::removeFilter(Qt3DCore::QNodeId criterionId)
{
    m_filters.removeOne(criterionId);
}

QList<Qt3DCore::QNodeId> RenderPassFilter::parameters() const
{
    return m_parameterPack.parameters();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
