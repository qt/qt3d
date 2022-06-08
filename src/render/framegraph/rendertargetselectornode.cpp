// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "rendertargetselectornode_p.h"
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/private/qrendertargetselector_p.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/qrendertargetoutput.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RenderTargetSelector::RenderTargetSelector() :
    FrameGraphNode(FrameGraphNode::RenderTarget)
{
}

void RenderTargetSelector::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QRenderTargetSelector *node = qobject_cast<const QRenderTargetSelector *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    const QNodeId renderTargetId = qIdForNode(node->target());
    if (renderTargetId != m_renderTargetUuid) {
        m_renderTargetUuid = renderTargetId;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
