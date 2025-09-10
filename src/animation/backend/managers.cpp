// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "managers_p.h"
#include <Qt3DAnimation/private/clipblendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

ClipBlendNodeManager::ClipBlendNodeManager()
{
}

ClipBlendNodeManager::~ClipBlendNodeManager()
{
    qDeleteAll(m_nodes);
}

bool ClipBlendNodeManager::containsNode(Qt3DCore::QNodeId id) const
{
    return m_nodes.contains(id);
}

void ClipBlendNodeManager::appendNode(Qt3DCore::QNodeId id, ClipBlendNode *node)
{
    m_nodes.insert(id, node);
}

ClipBlendNode *ClipBlendNodeManager::lookupNode(Qt3DCore::QNodeId id) const
{
    return m_nodes.value(id, nullptr);
}

void ClipBlendNodeManager::releaseNode(Qt3DCore::QNodeId id)
{
    delete m_nodes.take(id);
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
