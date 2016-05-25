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

#include "qframegraphnode.h"
#include "qframegraphnode_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QFrameGraphNodePrivate::QFrameGraphNodePrivate()
    : QNodePrivate()
{
}

/*!
  \class Qt3DRender::QFrameGraphNode
  \inmodule Qt3DRender
  \since 5.5

  \brief Base class of all FrameGraph configuration nodes.

  This is an abstract class so it cannot be instanced directly
  but rather through one of its subclasses.

 */

/*!
    \qmltype FrameGraphNode
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QFrameGraphNode
    \inherits Node
    \since 5.5
    \qmlabstract
*/

/*!
  The constructor creates an instance with the specified \a parent.
 */
QFrameGraphNode::QFrameGraphNode(QNode *parent)
    : QNode(*new QFrameGraphNodePrivate, parent)
{
}

/*! \internal */
QFrameGraphNode::~QFrameGraphNode()
{
}

/*!
  Returns a pointer to the parent.
 */
QFrameGraphNode *QFrameGraphNode::parentFrameGraphNode() const
{
    QFrameGraphNode *parentFGNode = nullptr;
    QNode *parentN = parentNode();

    while (parentN) {
        if ((parentFGNode = qobject_cast<QFrameGraphNode *>(parentN)) != nullptr)
            break;
        parentN = parentN->parentNode();
    }
    return parentFGNode;
}

/*! \internal */
QFrameGraphNode::QFrameGraphNode(QFrameGraphNodePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE
