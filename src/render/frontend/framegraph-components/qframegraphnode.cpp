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

#include "qframegraphnode.h"
#include "qframegraphnode_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QFrameGraphNodePrivate
    \internal
*/
QFrameGraphNodePrivate::QFrameGraphNodePrivate()
    : QNodePrivate()
    , m_enabled(true)
{
}

/*!
  \class QFrameGraphNode
  \inmodule Qt3DRenderer
  \since 5.5

  \brief Base class of all FrameGraph configuration nodes.

  This is an abstract class so it cannot be instanced directly
  but rather through one of its subclasses.

 */

/*!
    \qmltype FrameGraphNode
    \inqmlmodule Qt3D.Renderer
    \instantiates QFrameGraphNode
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

QFrameGraphNode::~QFrameGraphNode()
{
    Q_ASSERT_X(QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3D::QFrameGraphNode subclass didn't call QNode::cleanup in its destructor");
}

/*!
  Returns a pointer to the parent.
 */
QFrameGraphNode *QFrameGraphNode::parentFrameGraphNode() const
{
    QFrameGraphNode *parentFGNode = Q_NULLPTR;
    QNode *parentN = parentNode();

    while (parentN) {
        if ((parentFGNode = qobject_cast<QFrameGraphNode *>(parentN)) != Q_NULLPTR)
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

/*!
    \qmlproperty bool Qt3D.Renderer::FrameGraphNode::enabled

    Holds whether the frame graph node is enabled or disabled.
*/

/*!
    \property Qt3D::QFrameGraphNode::enabled

    Holds whether the frame graph node is enabled or disabled.
*/

bool QFrameGraphNode::isEnabled() const
{
    Q_D(const QFrameGraphNode);
    return d->m_enabled;
}

void QFrameGraphNode::setEnabled(bool enabled)
{
    Q_D(QFrameGraphNode);
    if (d->m_enabled != enabled) {
        d->m_enabled = enabled;
        emit enabledChanged();
    }
}


} // Qt3D

QT_END_NAMESPACE
