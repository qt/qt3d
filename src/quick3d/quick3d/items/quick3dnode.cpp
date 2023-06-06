// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "quick3dnode_p.h"

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

/*!
    \qmltype Node
    \inqmlmodule Qt3D.Core
    \since 5.5

    \brief A base QML type that other types inherit. It cannot be directly
    created.
*/

Quick3DNode::Quick3DNode(QObject *parent)
    : QObject(parent)
{
}

/*!
    \qmlproperty list<QtQml::QtObject> Qt3D.Core::Node::data
    \qmldefault
*/

QQmlListProperty<QObject> Quick3DNode::data()
{
    using qt_size_type = qsizetype;
    using ListContentType = QObject;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *obj) {
        if (!obj)
            return;

        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        self->childAppended(0, obj);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        return self->parentNode()->children().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        return self->parentNode()->children().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        for (QObject *const child : self->parentNode()->children())
            self->childRemoved(0, child);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

/*!
    \qmlproperty list<Node> Qt3D.Core::Node::childNodes
    \readonly
*/

QQmlListProperty<QNode> Quick3DNode::childNodes()
{
    using qt_size_type = qsizetype;
    using ListContentType = QNode;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *obj) {
        if (!obj)
            return;

        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        Q_ASSERT(!self->parentNode()->children().contains(obj));

        self->childAppended(0, obj);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        return self->parentNode()->children().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        return qobject_cast<QNode *>(self->parentNode()->children().at(index));
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DNode *self = static_cast<Quick3DNode *>(list->object);
        for (QObject *const child : self->parentNode()->children())
            self->childRemoved(0, child);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

void Quick3DNode::childAppended(int, QObject *obj)
{
    QNode *parentNode = this->parentNode();
    if (obj->parent() == parentNode)
        obj->setParent(nullptr);
    // Set after otherwise addChild might not work
    if (QNode *n = qobject_cast<QNode *>(obj))
        n->setParent(parentNode);
    else
        obj->setParent(parentNode);
}

void Quick3DNode::childRemoved(int, QObject *obj)
{
    if (QNode *n = qobject_cast<QNode *>(obj))
        n->setParent(Q_NODE_NULLPTR);
    else
        obj->setParent(nullptr);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_quick3dnode_p.cpp"
#include "moc_qt3dquickforeign_p.cpp"
