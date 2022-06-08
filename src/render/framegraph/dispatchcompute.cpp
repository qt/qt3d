// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "dispatchcompute_p.h"
#include <Qt3DRender/private/qdispatchcompute_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

DispatchCompute::DispatchCompute()
    : FrameGraphNode(FrameGraphNode::ComputeDispatch)
{
    m_workGroups[0] = 1;
    m_workGroups[1] = 1;
    m_workGroups[2] = 1;
}

DispatchCompute::~DispatchCompute()
{
}

void DispatchCompute::cleanup()
{
    m_workGroups[0] = 1;
    m_workGroups[1] = 1;
    m_workGroups[2] = 1;
}

void DispatchCompute::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QDispatchCompute *node = qobject_cast<const QDispatchCompute *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (m_workGroups[0] != node->workGroupX()) {
        m_workGroups[0] = node->workGroupX();
        markDirty(AbstractRenderer::FrameGraphDirty|AbstractRenderer::ComputeDirty);
    }
    if (m_workGroups[1] != node->workGroupY()) {
        m_workGroups[1] = node->workGroupY();
        markDirty(AbstractRenderer::FrameGraphDirty|AbstractRenderer::ComputeDirty);
    }
    if (m_workGroups[2] != node->workGroupZ()) {
        m_workGroups[2] = node->workGroupZ();
        markDirty(AbstractRenderer::FrameGraphDirty|AbstractRenderer::ComputeDirty);
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
