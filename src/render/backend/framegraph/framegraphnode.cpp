/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "framegraphnode.h"
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/framegraphmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

FrameGraphNode::FrameGraphNode()
    : m_renderer(Q_NULLPTR)
    , m_nodeType(InvalidNodeType)
    , m_enabled(true)
{
}

FrameGraphNode::FrameGraphNode(FrameGraphNodeType nodeType)
    : m_nodeType(nodeType)
    , m_enabled(true)
{
}

FrameGraphNode::~FrameGraphNode()
{
}

void FrameGraphNode::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void FrameGraphNode::setHandle(HFrameGraphNode handle)
{
    m_handle = handle;
}

void FrameGraphNode::setParentHandle(HFrameGraphNode parentHandle)
{
    m_parentHandle = parentHandle;
    FrameGraphNode **parent = m_renderer->frameGraphManager()->data(m_parentHandle);
    if (parent != Q_NULLPTR && !(*parent)->m_childrenHandles.contains(m_handle))
        (*parent)->m_childrenHandles.append(m_handle);
}

void FrameGraphNode::appendChildHandle(HFrameGraphNode childHandle)
{
    if (!m_childrenHandles.contains(childHandle)) {
        FrameGraphNode **child = m_renderer->frameGraphManager()->data(childHandle);
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
    FrameGraphNode **parent = m_renderer->frameGraphManager()->data(m_parentHandle);
    if (parent != Q_NULLPTR)
        return *parent;
    return Q_NULLPTR;
}

QList<FrameGraphNode *> FrameGraphNode::children() const
{
    QList<FrameGraphNode *> children;

    Q_FOREACH (HFrameGraphNode handle, m_childrenHandles) {
        FrameGraphNode **child = m_renderer->frameGraphManager()->data(handle);
        if (child != Q_NULLPTR)
            children << *child;
    }
    return children;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
