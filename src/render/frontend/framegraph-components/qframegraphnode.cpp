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

/*!
 * \class FrameGraphNode
 *
 * \brief Base class of all FrameGraph configuration nodes.
 *
 * This is an abstract class so it cannot be instanced directly
 * but rather through one of its subclasses.
 *
 * \since 5.3
 * \namespace Qt3D
 */

QT_BEGIN_NAMESPACE

namespace Qt3D {

QFrameGraphNodePrivate::QFrameGraphNodePrivate(QFrameGraphNode *qq)
    : QNodePrivate(qq)
    , m_enabled(true)
{
}

void QFrameGraphNode::copy(const QNode *ref)
{
    QNode::copy(ref);
}

QFrameGraphNode::QFrameGraphNode(QNode *parent)
    : QNode(*new QFrameGraphNodePrivate(this), parent)
{
}

QFrameGraphNode::QFrameGraphNode(QFrameGraphNodePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QFrameGraphNode::appendFrameGraphNode(QFrameGraphNode *item)
{
    Q_D(QFrameGraphNode);
    if (!d->m_fgChildren.contains(item)) {
        if (!item->parent())
            item->setParent(this);
        d->m_fgChildren.append(item);
    }
}

void QFrameGraphNode::removeFrameGraphNode(QFrameGraphNode *item)
{
    Q_D(QFrameGraphNode);
    if (!d->m_fgChildren.contains(item)) {
        d->m_fgChildren.removeOne(item);
    }
}

QList<QFrameGraphNode *> QFrameGraphNode::frameGraphChildren() const
{
    Q_D(const QFrameGraphNode);
    return d->m_fgChildren;
}

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
