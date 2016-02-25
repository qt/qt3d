/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "framegraphnode_p.h"
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/qframegraph.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

FrameGraphNode::FrameGraphNode()
    : BackendNode()
    , m_nodeType(InvalidNodeType)
    , m_enabled(true)
    , m_manager(Q_NULLPTR)
{
}

FrameGraphNode::FrameGraphNode(FrameGraphNodeType nodeType)
    : m_nodeType(nodeType)
    , m_enabled(true)
    , m_manager(Q_NULLPTR)
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
        m_parentId = parentId;
        FrameGraphNode *parent = m_manager->lookupNode(m_parentId);
        if (parent != Q_NULLPTR && !parent->m_childrenIds.contains(peerId()))
            parent->m_childrenIds.append(peerId());
    }
}

void FrameGraphNode::appendChildId(Qt3DCore::QNodeId childId)
{
    if (!m_childrenIds.contains(childId)) {
        FrameGraphNode *child = m_manager->lookupNode(childId);
        if (child != Q_NULLPTR) {
            m_childrenIds.append(childId);
            child->m_parentId = peerId();
        }
    }
}

void FrameGraphNode::removeChildId(Qt3DCore::QNodeId childId)
{
    if (m_childrenIds.contains(childId)) {
        FrameGraphNode *child = m_manager->lookupNode(childId);
        if (child != Q_NULLPTR) {
            child->m_parentId = Qt3DCore::QNodeId();
        }
        m_childrenIds.removeAll(childId);
    }
}

Qt3DCore::QNodeId FrameGraphNode::parentId() const
{
    return m_parentId;
}

QVector<Qt3DCore::QNodeId> FrameGraphNode::childrenIds() const
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
        if (child != Q_NULLPTR)
            children << child;
    }
    return children;
}

// TO DO: We need to rework that and probably add a RenderFrameGraph element
FrameGraphComponentFunctor::FrameGraphComponentFunctor(AbstractRenderer *renderer)
    : m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *FrameGraphComponentFunctor::create(Qt3DCore::QNode *frontend) const
{
    // TO DO: Ideally we should have a RenderFrameGraph component and use its setPeer method
    // to do that
    QFrameGraph *framegraph = static_cast<QFrameGraph *>(frontend);
    if (framegraph->activeFrameGraph() != Q_NULLPTR)
        m_renderer->setFrameGraphRoot(framegraph->activeFrameGraph()->id());
    return Q_NULLPTR;
}

Qt3DCore::QBackendNode *FrameGraphComponentFunctor::get(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    return Q_NULLPTR;
}

void FrameGraphComponentFunctor::destroy(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
