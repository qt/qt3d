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

#include "techniquefilternode_p.h"
#include "qfilterkey.h"
#include "qtechniquefilter.h"
#include <Qt3DRender/private/qtechniquefilter_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

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

QVector<Qt3DCore::QNodeId> TechniqueFilter::parameters() const
{
    return m_parameterPack.parameters();
}

QVector<Qt3DCore::QNodeId> TechniqueFilter::filters() const
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
