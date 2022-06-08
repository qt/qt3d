// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "statesetnode_p.h"

#include <Qt3DRender/qrenderstateset.h>
#include <Qt3DRender/private/qrenderstateset_p.h>
#include <Qt3DRender/private/genericstate_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

StateSetNode::StateSetNode()
    : FrameGraphNode(FrameGraphNode::StateSet)
{
}

StateSetNode::~StateSetNode()
{
}

QList<QNodeId> StateSetNode::renderStates() const
{
    return m_renderStates;
}

void StateSetNode::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QRenderStateSet *node = qobject_cast<const QRenderStateSet *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    auto stateIds = qIdsForNodes(node->renderStates());
    std::sort(std::begin(stateIds), std::end(stateIds));
    if (m_renderStates != stateIds) {
        m_renderStates = stateIds;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

void StateSetNode::addRenderState(QNodeId renderStateId)
{
    if (!m_renderStates.contains(renderStateId))
        m_renderStates.push_back(renderStateId);
}

void StateSetNode::removeRenderState(QNodeId renderStateId)
{
    m_renderStates.removeOne(renderStateId);
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
