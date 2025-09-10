// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "framegraphnode_p.h"
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

FrameGraphNode::FrameGraphNode()
    : BackendNode()
    , m_nodeType(InvalidNodeType)
    , m_manager(nullptr)
{
}

FrameGraphNode::FrameGraphNode(FrameGraphNodeType nodeType, QBackendNode::Mode mode)
    : BackendNode(mode)
    , m_nodeType(nodeType)
    , m_manager(nullptr)
{
}

FrameGraphNode::~FrameGraphNode()
{
}

void FrameGraphNode::setFrameGraphManager(FrameGraphManager *manager)
{
    if (m_manager != manager)
        m_manager = manager;
}

FrameGraphManager *FrameGraphNode::manager() const
{
    return m_manager;
}

void FrameGraphNode::setParentId(Qt3DCore::QNodeId parentId)
{
    if (m_parentId != parentId) {
        // We already had a parent, tell it to abandon us
        if (!m_parentId.isNull()) {
            FrameGraphNode *parent = m_manager->lookupNode(m_parentId);
            if (parent != nullptr)
                parent->m_childrenIds.removeAll(peerId());
        }
        m_parentId = parentId;
        FrameGraphNode *parent = m_manager->lookupNode(m_parentId);
        if (parent != nullptr && !parent->m_childrenIds.contains(peerId()))
            parent->m_childrenIds.append(peerId());
    }
}

Qt3DCore::QNodeId FrameGraphNode::parentId() const
{
    return m_parentId;
}

QList<Qt3DCore::QNodeId> FrameGraphNode::childrenIds() const
{
    return m_childrenIds;
}

FrameGraphNode *FrameGraphNode::parent() const
{
    return m_manager->lookupNode(m_parentId);
}

QList<FrameGraphNode *> FrameGraphNode::children() const
{
    QList<FrameGraphNode *> children;
    children.reserve(m_childrenIds.size());

    for (Qt3DCore::QNodeId id : m_childrenIds) {
        FrameGraphNode *child = m_manager->lookupNode(id);
        if (child != nullptr)
            children << child;
    }
    return children;
}

void FrameGraphNode::cleanup()
{
    setParentId({});
    markDirty(AbstractRenderer::FrameGraphDirty);
}

void FrameGraphNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QFrameGraphNode *node = qobject_cast<const QFrameGraphNode *>(frontEnd);

    const auto parentId = Qt3DCore::qIdForNode(node->parentFrameGraphNode());
    if (parentId != m_parentId) {
        setParentId(parentId);
        // TO DO: Check if FrameGraphDirty wouldn't be enough here
        markDirty(AbstractRenderer::AllDirty);
    }

    if (node->isEnabled() != d_func()->m_enabled) {
        d_func()->m_enabled = node->isEnabled();
        markDirty(AbstractRenderer::FrameGraphDirty);
    }

    if (firstTime)
        markDirty(AbstractRenderer::FrameGraphDirty);
}



} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
