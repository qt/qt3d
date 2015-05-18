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

#include "qnode.h"
#include "qnode_p.h"

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qsceneinterface.h>
#include <QEvent>
#include <QChildEvent>
#include <QMetaObject>
#include <QMetaProperty>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QHash<QNodeId, QNode *> QNodePrivate::m_clonesLookupTable = QHash<QNodeId, QNode *>();

/*!
    \class Qt3D::QNodePrivate
    \internal
*/
QNodePrivate::QNodePrivate()
    : QObjectPrivate()
    , m_changeArbiter(Q_NULLPTR)
    , m_scene(Q_NULLPTR)
    , m_id(QNodeId::createId())
    , m_blockNotifications(false)
    , m_propertyChangesSetup(false)
    , m_signals(this)
{
}

// Called by QEvent::childAdded (main thread)
void QNodePrivate::addChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    if (childNode == q_func())
        return ;

    // Set the scene
    childNode->d_func()->setScene(m_scene);

    // addObservable set the QChangeArbiter
    if (m_scene != Q_NULLPTR)
        m_scene->addObservable(childNode);

    // We notify only if we have a QChangeArbiter
    if (m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeCreated, QSceneChange::Node, m_id));
        e->setPropertyName("node");
        // We need to clone the parent of the childNode we send
        QNode *parentClone = QNode::clone(q_func());
        QNode *childClone = Q_NULLPTR;
        Q_FOREACH (QObject *c, parentClone->children()) {
            QNode *clone = qobject_cast<QNode *>(c);
            if (clone != Q_NULLPTR && clone->id() == childNode->id()) {
                childClone = clone;
                break;
            }
        }
        e->setValue(QVariant::fromValue(QNodePtr(childClone, &QNodePrivate::nodePtrDeleter)));
        notifyObservers(e);
    }
}

// Called by QEvent::childRemoved (main thread)
void QNodePrivate::removeChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    if (childNode->parent() != q_func())
        qCWarning(Nodes) << Q_FUNC_INFO << "not a child of " << this;

    // Notify only if child isn't a clone
    if (m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeAboutToBeDeleted, QSceneChange::Node, m_id));
        e->setPropertyName("node");
        // We need to clone the parent of the childNode we send
        //        QNode *parentClone = QNode::clone(childNode->parentNode());
        //        QNode *childClone = Q_NULLPTR;
        //        Q_FOREACH (QObject *c, parentClone->children()) {
        //            QNode *clone = qobject_cast<QNode *>(c);
        //            if (clone != Q_NULLPTR && clone->id() == childNode->id()) {
        //                childClone = clone;
        //                break;
        //            }
        //        }

        // We cannot clone the parent as it seems that the childNode is already removed
        // from the parent when the ChildRemoved event is triggered
        // and that would therefore return us a childNode NULL (because not found in the parent's children list)
        // and crash the backend

        QNode *childClone = QNode::clone(childNode);
        e->setValue(QVariant::fromValue(QNodePtr(childClone, &QNodePrivate::nodePtrDeleter)));
        notifyObservers(e);
    }

    if (m_scene != Q_NULLPTR)
        m_scene->removeObservable(childNode);
    childNode->d_func()->setScene(Q_NULLPTR);
}

void QNodePrivate::removeAllChildren()
{
    Q_FOREACH (QObject *child, q_func()->children()) {
        QNode *childNode = qobject_cast<QNode *>(child);
        if (childNode != Q_NULLPTR)
            removeChild(childNode);
    }
}

void QNodePrivate::registerNotifiedProperties()
{
    Q_Q(QNode);
    if (m_propertyChangesSetup)
        return;

    const int offset = QNode::staticMetaObject.propertyOffset();
    const int count = q->metaObject()->propertyCount();

    for (int index = offset; index < count; index++)
        m_signals.connectToPropertyChange(q, index);

    m_propertyChangesSetup = true;
}

void QNodePrivate::unregisterNotifiedProperties()
{
    Q_Q(QNode);
    if (!m_propertyChangesSetup)
        return;

    const int offset = QNode::staticMetaObject.propertyOffset();
    const int count = q->metaObject()->propertyCount();

    for (int index = offset; index < count; index++)
        m_signals.disconnectFromPropertyChange(q, index);

    m_propertyChangesSetup = false;
}

void QNodePrivate::propertyChanged(int propertyIndex)
{
    // Bail out early if we can to avoid the cost below
    if (m_blockNotifications)
        return;

    Q_Q(QNode);

    const QMetaProperty property = q->metaObject()->property(propertyIndex);

    const QVariant data = property.read(q);
    if (data.canConvert<QNode*>()) {
        const QNode * const node = data.value<QNode*>();
        const QNodeId id = node ? node->id() : QNodeId();
        notifyPropertyChange(property.name(), QVariant::fromValue(id));
    } else {
        notifyPropertyChange(property.name(), data);
    }
}

// Called in the main thread by QScene -> following QEvent::childAdded / addChild
void QNodePrivate::setArbiter(QLockableObserverInterface *arbiter)
{
    if (m_changeArbiter && m_changeArbiter != arbiter)
        unregisterNotifiedProperties();
    m_changeArbiter = arbiter;
    if (m_changeArbiter)
        registerNotifiedProperties();
}

/*!
    Called when one or more backend aspects sends a notification \a change to the
    current Qt3D::QNode instance.

    \note This method should be reimplemented in your subclasses to properly
    handle the \a change.
*/
void QNode::sceneChangeEvent(const QSceneChangePtr &change)
{
    Q_UNUSED(change);
    qWarning() << Q_FUNC_INFO << "sceneChangeEvent should have been subclassed";
}

void QNodePrivate::setScene(QSceneInterface *scene)
{
    if (m_scene != scene)
        m_scene = scene;
}

QSceneInterface *QNodePrivate::scene() const
{
    return m_scene;
}

void QNodePrivate::notifyPropertyChange(const char *name, const QVariant &value)
{
    // Bail out early if we can to avoid operator new
    if (m_blockNotifications)
        return;

    QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, m_id));
    e->setPropertyName(name);
    e->setValue(value);
    notifyObservers(e);
}

// Called by the main thread
void QNodePrivate::notifyObservers(const QSceneChangePtr &change)
{
    Q_ASSERT(change);

    // Don't send notifications if we are blocking
    if (m_blockNotifications && change->type() == NodeUpdated)
        return;

    if (m_changeArbiter != Q_NULLPTR)
        m_changeArbiter->sceneChangeEventWithLock(change);
}

// Inserts this tree into the main Scene tree.
// Needed when SceneLoaders provide a cloned tree from the backend
// and need to insert it in the main scene tree
// QNode *root;
// QNode *subtree;
// QNodePrivate::get(root)->insertTree(subtree);

void QNodePrivate::insertTree(QNode *treeRoot, int depth)
{
    if (m_scene != Q_NULLPTR) {
        treeRoot->d_func()->setScene(m_scene);
        m_scene->addObservable(treeRoot);
    }

    Q_FOREACH (QObject *c, treeRoot->children()) {
        QNode *n = Q_NULLPTR;
        if ((n = qobject_cast<QNode *>(c)) != Q_NULLPTR)
            insertTree(n, depth + 1);
    }

    if (depth == 0)
        treeRoot->setParent(q_func());
}

QNodePrivate *QNodePrivate::get(QNode *q)
{
    return q->d_func();
}

void QNodePrivate::nodePtrDeleter(QNode *q)
{
    QObject *p = q->parent();
    if (p == Q_NULLPTR)
        p = q;
    p->deleteLater();
}


/*!
    \class Qt3D::QNode
    \inherits QObject

    \inmodule Qt3DCore
    \since 5.5

    \brief Qt3D::QNode is the base class of all Qt3D node classes used to build a
    Qt3D scene.

    The owernship of Qt3D::QNode is determined by the QObject parent/child
    relationship between nodes. By itself a Qt3D::QNode has no visual appearance
    and no particular meaning, it is there as a way of building a node based tree
    structure.

    Each Qt3D::QNode instance has a unique id that allows it to be recognizable
    from other instances.

    When properties are defined on a Qt3D::QNode subclass, their NOTIFY signal
    will automatically generate notifications that the Qt3D backend aspects will
    receive.

    \sa Qt3D::QEntity, Qt3D::QComponent
*/

/*!
     Creates a new Qt3D::QNode instance with parent \a parent.
*/
QNode::QNode(QNode *parent)
    : QObject(*new QNodePrivate, parent)
{
    // We rely on QEvent::childAdded to be triggered on the parent
    // So we don't actually need to invoke a method or anything
    // to add ourselve with the parent
}

/*! \internal */
QNode::QNode(QNodePrivate &dd, QNode *parent)
    : QObject(dd, parent)
{
}

/*!
    Copies all the attributes from \a ref to the current Qt3D::QNode instance.

    \note When subclassing Qt3D::QNode you should reimplement this method and
    always call the copy method on the base class. This will ensure that when cloned,
    the Qt3D::QNode is properly initialized.
*/
void QNode::copy(const QNode *ref)
{
    if (ref)
        d_func()->m_id = ref->d_func()->m_id;
}

QNode::~QNode()
{
}

/*!
    Returns the id that uniquely identifies the Qt3D::QNode instance.
*/
const QNodeId QNode::id() const
{
    Q_D(const QNode);
    return d->m_id;
}

/*!
    Returns the immediate Qt3D::QNode parent, null if the node has no parent.
*/
QNode *QNode::parentNode() const
{
    return qobject_cast<QNode*>(parent());
}

/*!
    Returns \c true if aspect notifications are blocked; otherwise returns \c false.
    Notifications are not blocked by default.

    \sa blockNotifications()
*/
bool QNode::notificationsBlocked() const
{
    Q_D(const QNode);
    return d->m_blockNotifications;
}

/*!
    If \a block is true, property change notifications sent by this object
    to aspects are blocked. If \a block is false, no such blocking will occur.

    The return value is the previous value of notificationsBlocked().

    Note that the other notification types will be sent even if the
    notifications for this object have been blocked.

    \sa notificationsBlocked()
*/
bool QNode::blockNotifications(bool block)
{
    Q_D(QNode);
    bool previous = d->m_blockNotifications;
    d->m_blockNotifications = block;
    return previous;
}

/*!
    Returns a clone of \a node. All the children of \a node are cloned as well.

    \note This is the only way to create two nodes with the same id.
*/
QNode *QNode::clone(QNode *node)
{
    static int clearLock = 0;
    clearLock++;

    // We keep a reference of clones for the current subtree
    // In order to preserve relationships when multiple entities
    // reference the same component
    QNode *clonedNode = QNodePrivate::m_clonesLookupTable.value(node->id());
    if (clonedNode == Q_NULLPTR) {
        clonedNode = node->doClone();
        // doClone, returns new instance with content copied
        // and relationships added
        QNodePrivate::m_clonesLookupTable.insert(clonedNode->id(), clonedNode);
    }
    Q_FOREACH (QObject *c, node->children()) {
        QNode *childNode = qobject_cast<QNode *>(c);
        if (childNode != Q_NULLPTR) {
            QNode *cclone = QNode::clone(childNode);
            if (cclone != Q_NULLPTR)
                cclone->setParent(clonedNode);
        }
    }

    if (--clearLock == 0)
        QNodePrivate::m_clonesLookupTable.clear();

    return clonedNode;
}

bool QNode::event(QEvent *e)
{
    Q_D(QNode);

    switch (e->type()) {

    case QEvent::ChildAdded: {
        QNode *childNode = qobject_cast<QNode *>(static_cast<QChildEvent *>(e)->child());
        if (childNode != Q_NULLPTR) {
            d->addChild(childNode);
        }
        break;
    }

    case QEvent::ChildRemoved: {
        QNode *childNode = qobject_cast<QNode *>(static_cast<QChildEvent *>(e)->child());
        if (childNode != Q_NULLPTR) {
            d->removeChild(childNode);
        }
        break;
    }

    default:
        break;

    } // switch

    return QObject::event(e);
}

/*!
    Returns a pointer to the Qt3D::QNode instance's scene.
*/
QSceneInterface *QNode::scene() const
{
    Q_D(const QNode);
    return d->m_scene;
}

} // namespace Qt3D


QT_END_NAMESPACE

#include "moc_qnode.cpp"
