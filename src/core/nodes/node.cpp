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

#include "node.h"

#include "entity.h"

#include <QEvent>
#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

Node::Node( Node* parent )
    : QObject( parent )
{
    if (parent)
        parent->m_children.append(this);
}

Node::~Node()
{
}

QQmlListProperty<QObject> Node::data()
{
    return QQmlListProperty<QObject>(this, 0,
                                     Node::appendData,
                                     Node::dataCount,
                                     Node::dataAt,
                                     Node::clearData);
}

QQmlListProperty<Node> Node::childNodes()
{
    return QQmlListProperty<Node>(this, 0,
                                  Node::appendChild,
                                  Node::childCount,
                                  Node::childAt,
                                  Node::clearChildren);
}

void Node::dump()
{
    const QMetaObject *meta = metaObject();
    QStringList result;
    for (int i = 0; i < meta->propertyCount(); ++i) {
        const QMetaProperty metaProperty = meta->property(i);
        const QVariant value = property(metaProperty.name());
        result += QString(QStringLiteral("%1 %2 = %3;"))
            .arg(QString::fromLatin1(metaProperty.typeName()))
            .arg(QString::fromLatin1(metaProperty.name()))
            .arg(value.toString());
    }

    qDebug() << result.join(QStringLiteral("\n"));

    foreach (QObject *child, children()) {
        Node *node = qobject_cast<Node *>(child);
        if (!node)
            continue;
        node->dump();
    }
}

NodeList Node::children() const
{
    return m_children;
}

void Node::addChild(Node *childNode)
{
    Q_CHECK_PTR( childNode );
    if (childNode->parent() == this)
        return;

    m_children.append(childNode);
    childNode->setParent(this);

    if (childNode->asEntity())
        childNode->asEntity()->update();
}

void Node::removeChild(Node *childNode)
{
    Q_CHECK_PTR( childNode );
    if (childNode->parent() != this)
        qWarning() << Q_FUNC_INFO << "not a child";

    m_children.removeOne(childNode);
    childNode->setParent(NULL);
}

void Node::removeAllChildren()
{
    foreach (QObject *child, children()) {
        child->deleteLater();
    }

    m_children.clear();
}

Entity *Node::asEntity()
{
    return NULL;
}

Node *Node::parentNode() const
{
    return qobject_cast<Node*>(parent());
}

bool Node::event(QEvent *e)
{
    if (e->type() == QEvent::DynamicPropertyChange) {
        qDebug() << "*** Dynamic Property Change ***";
    }
    return QObject::event(e);
}

void Node::appendData(QQmlListProperty<QObject> *list, QObject *obj)
{
    if (!obj)
        return;

    QObject *self = static_cast<QObject *>(list->object);
    if (obj->parent() == self)
        obj->setParent(0);
    obj->setParent(self);

    Node *parentNode = qobject_cast<Qt3D::Node *>(self);
    Node *node = qobject_cast<Qt3D::Node *>(obj);
    if (node && parentNode)
        parentNode->m_children.append(node);
}

QObject *Node::dataAt(QQmlListProperty<QObject> *list, int index)
{
    QObject *self = static_cast<QObject *>(list->object);
    return self->children().at(index);
}

int Node::dataCount(QQmlListProperty<QObject> *list)
{
    QObject *self = static_cast<QObject *>(list->object);
    return self->children().count();
}

void Node::clearData(QQmlListProperty<QObject> *list)
{
    QObject *self = static_cast<QObject *>(list->object);
    Q_FOREACH (QObject *const child, self->children())
        child->setParent(0);
}

void Node::appendChild(QQmlListProperty<Qt3D::Node> *list, Qt3D::Node *obj)
{
    if (!obj)
        return;

    Node *self = static_cast<Node *>(list->object);
    Q_ASSERT(!self->m_children.contains(obj));
    if (obj->parent() == self)
        obj->setParent(0);
    self->m_children.append(obj);
    obj->setParent(self);
}

Qt3D::Node *Node::childAt(QQmlListProperty<Qt3D::Node> *list, int index)
{
    Node *self = static_cast<Node *>(list->object);
    return self->m_children.at(index);
}

int Node::childCount(QQmlListProperty<Qt3D::Node> *list)
{
    Node *self = static_cast<Node *>(list->object);
    return self->m_children.count();
}

void Node::clearChildren(QQmlListProperty<Qt3D::Node> *list)
{
    Node *self = static_cast<Node *>(list->object);
    Q_FOREACH (Node *const child, self->m_children)
        child->setParent(0);
    self->m_children.clear();
}

} // namespace Qt3D

QT_END_NAMESPACE
