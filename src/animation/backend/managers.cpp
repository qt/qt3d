/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
