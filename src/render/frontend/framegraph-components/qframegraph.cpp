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

#include "qframegraph.h"
#include "qframegraph_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DRenderer/qframegraphitem.h>
#include <Qt3DRenderer/private/qframegraphitem_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QFrameGraphPrivate::QFrameGraphPrivate(QFrameGraph *qq)
    : QComponentPrivate(qq)
    , m_activeFrameGraph(Q_NULLPTR)
{

}

void QFrameGraphPrivate::copy(const QNodePrivate *ref)
{
    QComponentPrivate::copy(ref);
    const QFrameGraphPrivate *other = static_cast<const QFrameGraphPrivate *>(ref);
    q_func()->setActiveFrameGraph(qobject_cast<QFrameGraphItem *>(QNodePrivate::get(other->m_activeFrameGraph)->clone()));
}

/*!
 * \class FrameGraph
 *
 * \brief Component that has an activeFrameGraph property that should
 * reference the root FrameGraphItem of a frame graph tree. The Entity
 * that contains a FrameGraph property defines the rendering method to
 * be used by the renderer.
 *
 * Note that only a single FrameGraph can be active at any moment.
 *
 * \since 5.3
 * \namespace Qt3D
 */

QFrameGraph::QFrameGraph(QNode *parent)
    : QComponent(*new QFrameGraphPrivate(this), parent)
{
}

QFrameGraph::QFrameGraph(QFrameGraphPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
 * Returns the current activeFrameGraph root node.
 */
QFrameGraphItem *QFrameGraph::activeFrameGraph() const
{
    Q_D(const QFrameGraph);
    return d->m_activeFrameGraph;
}

/*!
 * Sets the root node \a activeFrameGraph of the FrameGraph.
 */
void QFrameGraph::setActiveFrameGraph(QFrameGraphItem *activeFrameGraph)
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
