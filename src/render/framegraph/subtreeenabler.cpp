/****************************************************************************
**
** Copyright (C) 2019 Ford Motor Company
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
