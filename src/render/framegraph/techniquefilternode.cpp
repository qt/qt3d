// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "techniquefilternode_p.h"
#include "qfilterkey.h"
#include "qtechniquefilter.h"
#include <Qt3DRender/private/qtechniquefilter_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

TechniqueFilter::TechniqueFilter()
    : FrameGraphNode(FrameGraphNode::TechniqueFilter)
{
}

void TechniqueFilter::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QTechniqueFilter *node = qobject_cast<const QTechniqueFilter *>(frontEnd);
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

    auto filterIds = qIdsForNodes(node->matchAll());
    std::sort(std::begin(filterIds), std::end(filterIds));
    if (m_filters != filterIds) {
        m_filters = filterIds;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

QList<Qt3DCore::QNodeId> TechniqueFilter::parameters() const
{
    return m_parameterPack.parameters();
}

QList<Qt3DCore::QNodeId> TechniqueFilter::filters() const
{
    return m_filters;
}

void TechniqueFilter::appendFilter(QNodeId criterionId)
{
    if (!m_filters.contains(criterionId))
        m_filters.append(criterionId);
}

void TechniqueFilter::removeFilter(Qt3DCore::QNodeId criterionId)
{
    m_filters.removeOne(criterionId);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
