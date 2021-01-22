/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 Paul Lemire
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

#include "framegraphvisitor_p.h"

#include "framegraphnode_p.h"
#include "subtreeenabler_p.h"
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <QThreadPool>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

FrameGraphVisitor::FrameGraphVisitor(const FrameGraphManager *manager)
    : m_manager(manager)
{
    m_leaves.reserve(8);
}

QVector<FrameGraphNode *> FrameGraphVisitor::traverse(FrameGraphNode *root)
{
    m_leaves.clear();
    m_enablersToDisable.clear();

    Q_ASSERT_X(root, Q_FUNC_INFO, "The FrameGraphRoot is null");

    // Kick off the traversal
    Render::FrameGraphNode *node = root;
    if (node == nullptr)
        qCritical() << Q_FUNC_INFO << "FrameGraph is null";
    visit(node);
    return m_leaves;
}

// intended to be called after traverse
// (returns data that is captured during the traverse)
QVector<FrameGraphNode *> &&FrameGraphVisitor::takeEnablersToDisable()
{
    return std::move(m_enablersToDisable);
}

void FrameGraphVisitor::visit(Render::FrameGraphNode *node)
{
    if (node->nodeType() == Render::FrameGraphNode::SubtreeEnabler) {
        if (!node->isEnabled())
            return;
        if (static_cast<SubtreeEnabler*>(node)->enablement() == QSubtreeEnabler::SingleShot) {
            node->setEnabled(false);
            m_enablersToDisable.push_back(node);
        }
    }

    // Recurse to children (if we have any), otherwise if this is a leaf node,
    // initiate a rendering from the current camera
    const QVector<Qt3DCore::QNodeId> fgChildIds = node->childrenIds();

    for (const Qt3DCore::QNodeId fgChildId : fgChildIds)
        visit(m_manager->lookupNode(fgChildId));

    // Leaf node - create a RenderView ready to be populated
    // TODO: Pass in only framegraph config that has changed from previous
    // index RenderViewJob.
    if (fgChildIds.empty())
        m_leaves.push_back(node);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
