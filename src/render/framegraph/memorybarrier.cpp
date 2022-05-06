/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
