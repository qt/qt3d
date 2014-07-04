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

Quick3DNode::Quick3DNode(QObject *parent)
    : QObject(parent)
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

QQmlListProperty<QNode> Quick3DNode::childNodes()
{
    return QQmlListProperty<QNode>(this, 0,
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

    QNode *parentNode = qobject_cast<Quick3DNode *>(self)->parentNode();
    QNode *node = qobject_cast<Qt3D::QNode *>(obj);
    if (node && parentNode)
        parentNode->addChild(node);
    // Set after otherwise addChild might not work
    obj->setParent(parentNode);
}

QObject *Quick3DNode::dataAt(QQmlListProperty<QObject> *list, int index)
{
    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    return self->parentNode()->children().at(index);
}

int Quick3DNode::dataCount(QQmlListProperty<QObject> *list)
{
    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    return self->parentNode()->children().count();
}

void Quick3DNode::clearData(QQmlListProperty<QObject> *list)
{
    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    Q_FOREACH (QObject *const child, self->parentNode()->children())
        child->setParent(0);
}

void Quick3DNode::appendChild(QQmlListProperty<Qt3D::QNode> *list, Qt3D::QNode *obj)
{
    if (!obj)
        return;

    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    Q_ASSERT(!self->parentNode()->children().contains(obj));
    if (obj->parent() == self->parentNode())
        obj->setParent(0);
    // Set parent is set in appendChild
    self->parentNode()->addChild(obj);
}

Qt3D::QNode *Quick3DNode::childAt(QQmlListProperty<Qt3D::QNode> *list, int index)
{
    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    return self->parentNode()->children().at(index);
}

int Quick3DNode::childCount(QQmlListProperty<Qt3D::QNode> *list)
{
    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    return self->parentNode()->children().count();
}

void Quick3DNode::clearChildren(QQmlListProperty<Qt3D::QNode> *list)
{
    Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
    Q_FOREACH (QNode *const child, self->parentNode()->children())
        self->parentNode()->removeChild(child);
}

} // Quick

} // Qt3D

QT_END_NAMESPACE
