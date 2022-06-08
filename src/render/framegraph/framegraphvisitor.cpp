// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

std::vector<FrameGraphNode *> &&FrameGraphVisitor::traverse(FrameGraphNode *root)
{
    m_leaves.clear();
    m_enablersToDisable.clear();

    Q_ASSERT_X(root, Q_FUNC_INFO, "The FrameGraphRoot is null");

    // Kick off the traversal
    Render::FrameGraphNode *node = root;
    if (node == nullptr)
        qCritical() << Q_FUNC_INFO << "FrameGraph is null";
    visit(node);
    return std::move(m_leaves);
}

// intended to be called after traverse
// (returns data that is captured during the traverse)
std::vector<FrameGraphNode *> &&FrameGraphVisitor::takeEnablersToDisable()
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
    const QList<Qt3DCore::QNodeId> fgChildIds = node->childrenIds();

    for (const Qt3DCore::QNodeId &fgChildId : fgChildIds)
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
