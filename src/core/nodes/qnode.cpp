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
#include "corelogging.h"
#include <Qt3DCore/QComponent>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QHash<QUuid, QNode *> QNodePrivate::m_clonesLookupTable = QHash<QUuid, QNode *>();

QNodePrivate::QNodePrivate(QNode *qq)
    : QObjectPrivate()
    , m_changeArbiter(Q_NULLPTR)
    , m_scene(Q_NULLPTR)
    , m_uuid(QUuid::createUuid())
    , m_blockNotifications(false)
{
    q_ptr = qq;
}

// Called by QEvent::childAdded
void QNodePrivate::addChild(QNode *childNode)
{

    Q_ASSERT(childNode);
    if (childNode == q_ptr)
        return ;

    // Set the scene
    childNode->d_func()->setScene(m_scene);

    // addObservable set the QChangeArbiter
    if (m_scene != Q_NULLPTR)
        m_scene->addObservable(childNode);

    // We notify only if we have a QChangeArbiter
    if (m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeCreated, qobject_cast<QNode *>(q_ptr)));
        e->setPropertyName(QByteArrayLiteral("node"));
        // We need to clone the parent of the childNode we send
        QNode *parentClone = clone();
        QNode *childClone = Q_NULLPTR;
        Q_FOREACH (QObject *c, parentClone->children()) {
            QNode *clone = qobject_cast<QNode *>(c);
            if (clone != Q_NULLPTR && clone->uuid() == childNode->uuid()) {
                childClone = clone;
                break;
            }
        }
        e->setValue(QVariant::fromValue(QNodePtr(childClone, &QNodePrivate::nodePtrDeleter)));
        notifyObservers(e);
    }
}

// Called by QEvent::childRemoved
void QNodePrivate::removeChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    if (childNode->parent() != q_ptr)
        qCWarning(Nodes) << Q_FUNC_INFO << "not a child of " << this;

    // Notify only if child isn't a clone
    if (m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr e(new QScenePropertyChange(NodeAboutToBeDeleted, qobject_cast<QNode *>(q_ptr)));
        e->setPropertyName(QByteArrayLiteral("node"));
        // We need to clone the parent of the childNode we send
        QNode *parentClone = clone();
        QNode *childClone = Q_NULLPTR;
        Q_FOREACH (QObject *c, parentClone->children()) {
            QNode *clone = qobject_cast<QNode *>(c);
            if (clone != Q_NULLPTR && clone->uuid() == childNode->uuid()) {
                childClone = clone;
                break;
            }
        }
        e->setValue(QVariant::fromValue(QNodePtr(childClone, &QNodePrivate::nodePtrDeleter)));
        notifyObservers(e);
    }

    if (m_scene != Q_NULLPTR)
        m_scene->removeObservable(childNode);
    childNode->d_func()->setScene(Q_NULLPTR);
}

// In most cases isClone is true so that the clone isn't handled like
// a real node. If there is a need for a real clone, set isClone to false
// eg When a subtree built in the backend needs to be cloned
// in the main thread to be added to the scene graph
QNode *QNodePrivate::clone()
{
    static int clearLock = 0;
    clearLock++;

    // We keep a reference of clones for the current subtree
    // In order to preserve relationships when multiple entities
    // reference the same component
    QNode *clonedNode = QNodePrivate::m_clonesLookupTable.value(m_uuid);
    if (clonedNode == Q_NULLPTR) {
        clonedNode = qobject_cast<QNode *>(q_ptr)->doClone();
        // doClone, returns new instance with content copied
        // and relationships added
        QNodePrivate::m_clonesLookupTable.insert(clonedNode->uuid(), clonedNode);
    }
    Q_FOREACH (QObject *c, q_ptr->children()) {
        QNode *childNode = qobject_cast<QNode *>(c);
        if (childNode != Q_NULLPTR) {
            QNode *cclone = childNode->d_func()->clone();
            if (cclone != Q_NULLPTR)
                cclone->setParent(clonedNode);
        }
    }

    if (--clearLock == 0)
        QNodePrivate::m_clonesLookupTable.clear();

    return clonedNode;
}

void QNodePrivate::removeAllChildren()
{
    Q_FOREACH (QObject *child, q_ptr->children()) {
        QNode *childNode = qobject_cast<QNode *>(child);
        if (childNode != Q_NULLPTR)
            removeChild(childNode);
    }
}

void QNodePrivate::registerNotifiedProperties()
{
    Q_Q(QNode);
    if (!m_notifiedProperties.isEmpty())
        return;

    const int offset = QNode::staticMetaObject.propertyOffset();
    const int count = q->metaObject()->propertyCount();

    for (int index = offset; index < count; index++) {
        const QMetaProperty property = q->metaObject()->property(index);
        if (!property.hasNotifySignal())
            continue;
        m_notifiedProperties[property.notifySignalIndex()] = property.name();
        const QByteArray signal = QByteArray::number(QSIGNAL_CODE)
                                + property.notifySignal().methodSignature();
        QObject::connect(q, signal,
                         q, SLOT(_q_onNodePropertyChanged()));
    }
}

void QNodePrivate::_q_onNodePropertyChanged()
{
    Q_Q(QNode);
    const int signalIndex = q->senderSignalIndex();
    if (!m_notifiedProperties.contains(signalIndex))
        return;

    const QByteArray name = m_notifiedProperties.value(signalIndex);
    const QVariant data = q->property(name);
    if (data.canConvert<QNode*>()) {
        const QNode * const node = data.value<QNode*>();
        const QUuid uuid = node ? node->uuid() : QUuid();
        notifyPropertyChange(name, uuid);
    } else {
        notifyPropertyChange(name, data);
    }
}

// Called in the QAspectThread context
void QNodePrivate::registerObserver(QObserverInterface *observer)
{
    Q_CHECK_PTR(observer);

    // For now we only care about the QChangeArbiter observing us
    QChangeArbiter *changeArbiter = dynamic_cast<QChangeArbiter *>(observer);
    if (changeArbiter) {
        QWriteLocker locker(&m_observerLock);
        m_changeArbiter = changeArbiter;
        registerNotifiedProperties();
    }
}

void QNodePrivate::unregisterObserver(QObserverInterface *observer)
{
    Q_CHECK_PTR(observer);

    // For now we only care about the QChangeArbiter observing us
    QChangeArbiter *changeArbiter = dynamic_cast<QChangeArbiter *>(observer);
    if (changeArbiter == m_changeArbiter) {
        QWriteLocker locker(&m_observerLock);
        m_changeArbiter = Q_NULLPTR;
    }
}

void QNode::sceneChangeEvent(const QSceneChangePtr &)
{
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

void QNodePrivate::notifyPropertyChange(const QByteArray &name, const QVariant &value)
{
    // TODO: Review change types. Is there any need to distinguish between NodeUpdated, ComponentUpdated?
    // They're both just property changes
    QScenePropertyChangePtr e(new QScenePropertyChange(NodeUpdated, qobject_cast<QNode *>(q_ptr)));
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

    QReadLocker locker(&m_observerLock);
    QChangeArbiter *changeArbiter = m_changeArbiter;
    locker.unlock();
    // There is a deadlock issue as sceneChangeEventWithLock locks the QChangeArbiter's mutex
    // while d->m_observerLock is locked by the locker right above.
    // In the case that a call the QChangeArbiter registerObserver which locks the QChangeArviter's mutex
    // and calls registerObserver on the same Node with locks d->m_observerLock

    if (changeArbiter != Q_NULLPTR)
        changeArbiter->sceneChangeEventWithLock(change);
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
        treeRoot->setParent(q_ptr);
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

QNode::QNode(QNode *parent)
    : QObject(*new QNodePrivate(this), parent)
{
    // We rely on QEvent::childAdded to be triggered on the parent
    // So we don't actually need to invoke a method or anything
    // to add ourselve with the parent
}

QNode::QNode(QNodePrivate &dd, QNode *parent)
    : QObject(dd, parent)
{
}

void QNode::copy(const QNode *ref)
{
    if (ref)
        d_func()->m_uuid = ref->d_func()->m_uuid;
}

QNode::~QNode()
{
}

const QUuid QNode::uuid() const
{
    Q_D(const QNode);
    return d->m_uuid;
}

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

} // namespace Qt3D


QT_END_NAMESPACE

#include "moc_qnode.cpp"
