// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "managers_p.h"

#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

FrameGraphManager::~FrameGraphManager()
{
    qDeleteAll(m_nodes);
}

bool FrameGraphManager::containsNode(Qt3DCore::QNodeId id) const
{
    return m_nodes.contains(id);
}

void FrameGraphManager::appendNode(Qt3DCore::QNodeId id, FrameGraphNode *node)
{
    m_nodes.insert(id, node);
}

FrameGraphNode* FrameGraphManager::lookupNode(Qt3DCore::QNodeId id) const
{
    const QHash<Qt3DCore::QNodeId, FrameGraphNode*>::const_iterator it = m_nodes.find(id);
    if (it == m_nodes.end())
        return nullptr;
    else
        return *it;
}

void FrameGraphManager::releaseNode(Qt3DCore::QNodeId id)
{
    auto node = m_nodes.take(id);
    if (node) {
        node->cleanup();
        delete node;
    }
}

void SkeletonManager::addDirtySkeleton(DirtyFlag dirtyFlag, HSkeleton skeletonHandle)
{
    switch (dirtyFlag) {
    case SkeletonDataDirty:
        m_dirtyDataSkeletons.push_back(skeletonHandle);
        break;

    case SkeletonTransformsDirty:
        m_dirtyTransformSkeletons.push_back(skeletonHandle);
        break;
    }
}

QList<HSkeleton> SkeletonManager::takeDirtySkeletons(DirtyFlag dirtyFlag)
{
    switch (dirtyFlag) {
    case SkeletonDataDirty:
        return Qt3DCore::moveAndClear(m_dirtyDataSkeletons);

    case SkeletonTransformsDirty:
        return Qt3DCore::moveAndClear(m_dirtyTransformSkeletons);
    }
    return { };
}

void JointManager::addDirtyJoint(Qt3DCore::QNodeId jointId)
{
    const HJoint jointHandle = lookupHandle(jointId);
    m_dirtyJoints.push_back(jointHandle);
}

void JointManager::removeDirtyJoint(Qt3DCore::QNodeId jointId)
{
    const HJoint jointHandle = lookupHandle(jointId);
    m_dirtyJoints.removeAll(jointHandle);
}

QList<HJoint> JointManager::dirtyJoints()
{
    return  Qt3DCore::moveAndClear(m_dirtyJoints);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
