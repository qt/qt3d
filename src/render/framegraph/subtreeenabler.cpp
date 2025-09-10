// Copyright (C) 2019 Ford Motor Company
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "subtreeenabler_p.h"
#include <Qt3DRender/private/qsubtreeenabler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

SubtreeEnabler::SubtreeEnabler()
    : FrameGraphNode(FrameGraphNode::SubtreeEnabler, FrameGraphNode::ReadWrite)
{
}

void SubtreeEnabler::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QSubtreeEnabler *node = qobject_cast<const QSubtreeEnabler *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    const auto enablement = node->enablement();
    if (enablement != m_enablement) {
        m_enablement = enablement;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

} //Render

} //Qt3DRender

QT_END_NAMESPACE
