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
QFrameGraphPrivate::QFrameGraphPrivate(QFrameGraph *qq)
    : QComponentPrivate(qq)
    , m_activeFrameGraph(Q_NULLPTR)
{

}

void QFrameGraph::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QFrameGraph *other = static_cast<const QFrameGraph*>(ref);
    setActiveFrameGraph(qobject_cast<QFrameGraphNode *>(QNode::clone(other->d_func()->m_activeFrameGraph)));
}

/*!
 * \class Qt3D::FrameGraph
 *
 * \brief Component that has an activeFrameGraph property that should
 * reference the root FrameGraphItem of a frame graph tree. The Entity
 * that contains a FrameGraph property defines the rendering method to
 * be used by the renderer.
 *
 * Note that only a single FrameGraph can be active at any moment.
 *
 * \since 5.3
 */

QFrameGraph::QFrameGraph(QNode *parent)
    : QComponent(*new QFrameGraphPrivate(this), parent)
{
}

/*! \internal */
QFrameGraph::QFrameGraph(QFrameGraphPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
 *  \property Qt3D::QFrameGraph::activeFrameGraph
 *
 * Holds the current activeFrameGraph root node.
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
        d->m_activeFrameGraph = activeFrameGraph;
        emit activeFrameGraphChanged();

        if (!activeFrameGraph->parent())
            activeFrameGraph->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
        // TO DO : Add QChangeArbiterNotification
        }
    }
}

} // Qt3D

QT_END_NAMESPACE
