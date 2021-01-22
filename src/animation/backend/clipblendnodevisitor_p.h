/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

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
