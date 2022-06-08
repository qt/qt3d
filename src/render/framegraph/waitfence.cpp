// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "waitfence_p.h"
#include <Qt3DRender/private/qwaitfence_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

WaitFence::WaitFence()
    : FrameGraphNode(FrameGraphNode::WaitFence)
{
    m_data.handleType = QWaitFence::NoHandle;
    m_data.waitOnCPU = false;
    m_data.timeout = std::numeric_limits<quint64>::max();
}

WaitFence::~WaitFence()
{
}

void WaitFence::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QWaitFence *node = qobject_cast<const QWaitFence *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->handleType() != m_data.handleType) {
        m_data.handleType = node->handleType();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->handle() != m_data.handle) {
        m_data.handle = node->handle();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->timeout() != m_data.timeout) {
        m_data.timeout = node->timeout();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
    if (node->waitOnCPU() != m_data.waitOnCPU) {
        m_data.waitOnCPU = node->waitOnCPU();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
