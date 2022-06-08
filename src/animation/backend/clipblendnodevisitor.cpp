// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "clipblendnodevisitor_p.h"
#include <Qt3DAnimation/private/managers_p.h>
#include <Qt3DAnimation/private/clipblendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

/*!
    \class ClipBlendNodeVisitor
    \internal
    Visits a blend tree in either pre- or post-order manner, optionally taking care
    to only visit the nodes that are dependencies of evaluating the blend
    tree.
*/
ClipBlendNodeVisitor::ClipBlendNodeVisitor(ClipBlendNodeManager *manager,
                                           TraversalOrder order,
                                           NodeFilter filter)
    : m_manager(manager)
    , m_order(order)
    , m_filter(filter)
{
}

void ClipBlendNodeVisitor::traverse(Qt3DCore::QNodeId rootId,
                                    const VisitFunction &visitFunction) const
{
    ClipBlendNode *node = m_manager->lookupNode(rootId);
    if (node != nullptr) {
        switch (m_order) {
        case PreOrder: {
            switch (m_filter) {
            case VisitAllNodes:
                visitPreOrderAllNodes(node, visitFunction);
                break;

            case VisitOnlyDependencies:
                visitPreOrderDependencyNodes(node, visitFunction);
                break;
            }

            break;
        }

        case PostOrder: {
            switch (m_filter) {
            case VisitAllNodes:
                visitPostOrderAllNodes(node, visitFunction);
                break;

            case VisitOnlyDependencies:
                visitPostOrderDependencyNodes(node, visitFunction);
                break;
            }

            break;
        }
        }
    }
}

/*!
    \internal
    Leaf to root traversal (Pre-order traversal) visiting all nodes even if
    they will not participate in current evaluation of the blend tree.
*/
void ClipBlendNodeVisitor::visitPreOrderAllNodes(ClipBlendNode *node,
                                                 const VisitFunction &visitFunction) const
{
    visitFunction(node);
    const Qt3DCore::QNodeIdVector childIds = node->allDependencyIds();
    for (const Qt3DCore::QNodeId &childId: childIds) {
        ClipBlendNode *childNode = m_manager->lookupNode(childId);
        if (childNode != nullptr)
            visitPreOrderAllNodes(childNode, visitFunction);
    }
}

/*!
    \internal
    Leaf to root traversal (Post-order traversal) visiting all nodes even if
    they will not participate in current evaluation of the blend tree.
*/
void ClipBlendNodeVisitor::visitPostOrderAllNodes(ClipBlendNode *node,
                                                  const VisitFunction &visitFunction) const
{
    const Qt3DCore::QNodeIdVector childIds = node->allDependencyIds();
    for (const Qt3DCore::QNodeId &childId: childIds) {
        ClipBlendNode *childNode = m_manager->lookupNode(childId);
        if (childNode != nullptr)
            visitPostOrderAllNodes(childNode, visitFunction);
    }
    visitFunction(node);
}

/*!
    \internal
    Leaf to root traversal (Pre-order traversal) visiting only nodes required
    to evaluate the blend tree given its current state.
*/
void ClipBlendNodeVisitor::visitPreOrderDependencyNodes(ClipBlendNode *node,
                                                        const VisitFunction &visitFunction) const
{
    visitFunction(node);
    const Qt3DCore::QNodeIdVector childIds = node->currentDependencyIds();
    for (const Qt3DCore::QNodeId &childId: childIds) {
        ClipBlendNode *childNode = m_manager->lookupNode(childId);
        if (childNode != nullptr)
            visitPreOrderDependencyNodes(childNode, visitFunction);
    }
}

/*!
    \internal
    Leaf to root traversal (Post-order traversal) visiting only nodes required
    to evaluate the blend tree given its current state.
*/
void ClipBlendNodeVisitor::visitPostOrderDependencyNodes(ClipBlendNode *node,
                                                         const VisitFunction &visitFunction) const
{
    const Qt3DCore::QNodeIdVector childIds = node->currentDependencyIds();
    for (const Qt3DCore::QNodeId &childId: childIds) {
        ClipBlendNode *childNode = m_manager->lookupNode(childId);
        if (childNode != nullptr)
            visitPostOrderDependencyNodes(childNode, visitFunction);
    }
    visitFunction(node);
}

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
