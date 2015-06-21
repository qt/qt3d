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

#include "qframegraph.h"
#include "qframegraph_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DRenderer/qframegraphnode.h>
#include <Qt3DRenderer/private/qframegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QFrameGraphPrivate
    \internal
*/
QFrameGraphPrivate::QFrameGraphPrivate()
    : QComponentPrivate()
    , m_activeFrameGraph(Q_NULLPTR)
{

}

/*!
  \class FrameGraph
  \inmodule Qt3DRenderer
  \since 5.3

  \brief Defines the rendering method to be used by the renderer

  FrameGraph is the Component that has an activeFrameGraph property
  that should reference the root FrameGraphItem of a frame graph
  tree. The Entity that contains a FrameGraph property defines the
  rendering method to be used by the renderer.

  \note Only one FrameGraph can be active at any moment.
 */

/*!
    \qmltype FrameGraph
    \instantiates Qt3D::QFrameGraph
    \inherits Component3D
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief For OpenGL ...
*/

/*! \fn void Qt3D::QFrameGraph::copy(const QNode *ref)
  Copies the \a ref instance into this one.
 */
void QFrameGraph::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QFrameGraph *other = static_cast<const QFrameGraph*>(ref);
    setActiveFrameGraph(qobject_cast<QFrameGraphNode *>(QNode::clone(other->d_func()->m_activeFrameGraph)));
}

/*!
  \fn Qt3D::QFrameGraph::QFrameGraph(QNode *parent)
  Constructs a new QFrameGraph with the specified \a parent.
 */
QFrameGraph::QFrameGraph(QNode *parent)
    : QComponent(*new QFrameGraphPrivate, parent)
{
}

QFrameGraph::~QFrameGraph()
{
    QNode::cleanup();
}

/*! \internal */
QFrameGraph::QFrameGraph(QFrameGraphPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
  \qmlproperty Qt3D.Renderer::FrameGraphNode Qt3D.Renderer::FrameGraph::activeFrameGraph

  Holds the current activeFrameGraph root node.
*/

/*!
  \property Qt3D::QFrameGraph::activeFrameGraph

  Holds the current activeFrameGraph root node.
 */
QFrameGraphNode *QFrameGraph::activeFrameGraph() const
{
    Q_D(const QFrameGraph);
    return d->m_activeFrameGraph;
}

void QFrameGraph::setActiveFrameGraph(QFrameGraphNode *activeFrameGraph)
{
    Q_D(QFrameGraph);
    if (activeFrameGraph != d->m_activeFrameGraph) {

        if (!activeFrameGraph->parent())
            activeFrameGraph->setParent(this);

        d->m_activeFrameGraph = activeFrameGraph;
        emit activeFrameGraphChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
