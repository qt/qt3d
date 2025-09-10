// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_CLIPBLENDNODEVISITOR_P_H
#define QT3DANIMATION_ANIMATION_CLIPBLENDNODEVISITOR_P_H

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


#include <functional>
#include <Qt3DCore/qnodeid.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class ClipBlendNodeManager;
class ClipBlendNode;

using VisitFunction = std::function<void (ClipBlendNode *)>;

class Q_AUTOTEST_EXPORT ClipBlendNodeVisitor
{
public:
    enum TraversalOrder {
        PreOrder,
        PostOrder
    };

    enum NodeFilter {
        VisitAllNodes,
        VisitOnlyDependencies
    };

    explicit ClipBlendNodeVisitor(ClipBlendNodeManager *manager,
                                  TraversalOrder order = PostOrder,
                                  NodeFilter filter = VisitAllNodes);

    void traverse(Qt3DCore::QNodeId rootId, const VisitFunction &visitFunction) const;

private:
    void visitPreOrderAllNodes(ClipBlendNode *node, const VisitFunction &visitFunction) const;
    void visitPostOrderAllNodes(ClipBlendNode *node, const VisitFunction &visitFunction) const;

    void visitPreOrderDependencyNodes(ClipBlendNode *node, const VisitFunction &visitFunction) const;
    void visitPostOrderDependencyNodes(ClipBlendNode *node, const VisitFunction &visitFunction) const;

    ClipBlendNodeManager *m_manager;
    TraversalOrder m_order;
    NodeFilter m_filter;
};

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_CLIPBLENDNODEVISITOR_P_H
