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

#include "quick3dnode.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

Quick3DNode::Quick3DNode(Node *parent) : Node(parent)
{
}

QQmlListProperty<QObject> Quick3DNode::data()
{
    return QQmlListProperty<QObject>(this, 0,
                                     Quick3DNode::appendData,
                                     Quick3DNode::dataCount,
                                     Quick3DNode::dataAt,
                                     Quick3DNode::clearData);
}

QQmlListProperty<Node> Quick3DNode::childNodes()
{
    return QQmlListProperty<Node>(this, 0,
                                  Quick3DNode::appendChild,
                                  Quick3DNode::childCount,
                                  Quick3DNode::childAt,
                                  Quick3DNode::clearChildren);
}

void Quick3DNode::appendData(QQmlListProperty<QObject> *list, QObject *obj)
{
    if (!obj)
        return;

    QObject *self = static_cast<QObject *>(list->object);
    if (obj->parent() == self)
        obj->setParent(0);

    Node *parentNode = qobject_cast<Qt3D::Node *>(self);
    Node *node = qobject_cast<Qt3D::Node *>(obj);
    if (node && parentNode)
        parentNode->addChild(node);
    // Set after otherwise addChild might not work
    obj->setParent(self);
}

QObject *Quick3DNode::dataAt(QQmlListProperty<QObject> *list, int index)
{
    QObject *self = static_cast<QObject *>(list->object);
    return self->children().at(index);
}

int Quick3DNode::dataCount(QQmlListProperty<QObject> *list)
{
    QObject *self = static_cast<QObject *>(list->object);
    return self->children().count();
}

void Quick3DNode::clearData(QQmlListProperty<QObject> *list)
{
    QObject *self = static_cast<QObject *>(list->object);
    Q_FOREACH (QObject *const child, self->children())
        child->setParent(0);
}

void Quick3DNode::appendChild(QQmlListProperty<Qt3D::Node> *list, Qt3D::Node *obj)
{
    if (!obj)
        return;

    Node *self = static_cast<Node *>(list->object);
    Q_ASSERT(!self->children().contains(obj));
    if (obj->parent() == self)
        obj->setParent(0);
    // Set parent if set in appendChild
    self->addChild(obj);
}

Qt3D::Node *Quick3DNode::childAt(QQmlListProperty<Qt3D::Node> *list, int index)
{
    Node *self = static_cast<Node *>(list->object);
    return self->children().at(index);
}

int Quick3DNode::childCount(QQmlListProperty<Qt3D::Node> *list)
{
    Node *self = static_cast<Node *>(list->object);
    return self->children().count();
}

void Quick3DNode::clearChildren(QQmlListProperty<Qt3D::Node> *list)
{
    Node *self = static_cast<Node *>(list->object);
    Q_FOREACH (Node *const child, self->children())
        self->removeChild(child);
}

} // Quick

} // Qt3D

QT_END_NAMESPACE
