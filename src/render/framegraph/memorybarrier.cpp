// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "memorybarrier_p.h"
#include <Qt3DRender/private/qmemorybarrier_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

MemoryBarrier::MemoryBarrier()
    : FrameGraphNode(FrameGraphNode::MemoryBarrier)
    , m_waitOperations(QMemoryBarrier::None)
{
}

MemoryBarrier::~MemoryBarrier()
{
}

QMemoryBarrier::Operations MemoryBarrier::waitOperations() const
{
    return m_waitOperations;
}

void MemoryBarrier::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QMemoryBarrier *node = qobject_cast<const QMemoryBarrier *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->waitOperations() != m_waitOperations) {
        m_waitOperations = node->waitOperations();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
