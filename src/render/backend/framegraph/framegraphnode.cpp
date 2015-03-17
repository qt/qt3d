/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "framegraphnode_p.h"
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/qframegraph.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

FrameGraphNode::FrameGraphNode()
    : QBackendNode()
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

void FrameGraphNode::setHandle(HFrameGraphNode handle)
{
    m_handle = handle;
}

void FrameGraphNode::setParentHandle(HFrameGraphNode parentHandle)
{
    m_parentHandle = parentHandle;
    FrameGraphNode **parent = m_manager->data(m_parentHandle);
    if (parent != Q_NULLPTR && *parent != Q_NULLPTR && !(*parent)->m_childrenHandles.contains(m_handle))
        (*parent)->m_childrenHandles.append(m_handle);
}

void FrameGraphNode::appendChildHandle(HFrameGraphNode childHandle)
{
    if (!m_childrenHandles.contains(childHandle)) {
        FrameGraphNode **child = m_manager->data(childHandle);
        if (child != Q_NULLPTR) {
            m_childrenHandles.append(childHandle);
            (*child)->m_parentHandle = m_handle;
        }
    }
}

void FrameGraphNode::removeChildHandle(HFrameGraphNode childHandle)
{
    if (m_childrenHandles.contains(childHandle))
        m_childrenHandles.removeAll(childHandle);
}

HFrameGraphNode FrameGraphNode::handle() const
{
    return m_handle;
}

HFrameGraphNode FrameGraphNode::parentHandle() const
{
    return m_parentHandle;
}

QList<HFrameGraphNode> FrameGraphNode::childrenHandles() const
{
    return m_childrenHandles;
}

FrameGraphNode *FrameGraphNode::parent() const
{
    FrameGraphNode **parent = m_manager->data(m_parentHandle);
    if (parent != Q_NULLPTR)
        return *parent;
    return Q_NULLPTR;
}

QList<FrameGraphNode *> FrameGraphNode::children() const
{
    QList<FrameGraphNode *> children;

    Q_FOREACH (HFrameGraphNode handle, m_childrenHandles) {
        FrameGraphNode **child = m_manager->data(handle);
        if (child != Q_NULLPTR)
            children << *child;
    }
    return children;
}

// TO DO: We need to rework that and probably add a RenderFrameGraph element
FrameGraphComponentFunctor::FrameGraphComponentFunctor(Renderer *renderer)
    : m_renderer(renderer)
{
}

QBackendNode *FrameGraphComponentFunctor::create(QNode *frontend, const QBackendNodeFactory *) const
{
    // TO DO: Ideally we should have a RenderFrameGraph component and use its setPeer method
    // to do that
    QFrameGraph *framegraph = static_cast<QFrameGraph *>(frontend);
    if (framegraph->activeFrameGraph() != Q_NULLPTR)
        m_renderer->setFrameGraphRoot(framegraph->activeFrameGraph()->id());
    return Q_NULLPTR;
}

QBackendNode *FrameGraphComponentFunctor::get(const QNodeId &id) const
{
    Q_UNUSED(id);
    return Q_NULLPTR;
}

void FrameGraphComponentFunctor::destroy(const QNodeId &id) const
{
    Q_UNUSED(id);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
