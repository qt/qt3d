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
