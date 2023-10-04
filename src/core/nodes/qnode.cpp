// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qnode.h"
#include "qnode_p.h"
#include "qscene_p.h"

#include <Qt3DCore/QComponent>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qentity.h>
#include <QtCore/QChildEvent>
#include <QtCore/QEvent>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QThread>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qdestructionidandtypecollector_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <QtCore/private/qmetaobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QNodePrivate::QNodePrivate()
    : QObjectPrivate()
    , m_changeArbiter(nullptr)
    , m_typeInfo(nullptr)
    , m_scene(nullptr)
    , m_id(QNodeId::createId())
    , m_blockNotifications(false)
    , m_hasBackendNode(false)
    , m_enabled(true)
    , m_notifiedParent(false)
    , m_propertyChangesSetup(false)
    , m_signals(this)
{
}

QNodePrivate::~QNodePrivate()
{
}

void QNodePrivate::init(QNode *parent)
{
    if (!parent)
        return;

    // If we have a QNode parent that has a scene (and hence change arbiter),
    // copy these to this QNode. If valid, then also notify the backend
    // in a deferred way when the object is fully constructed. This is delayed
    // until the object is fully constructed as it involves calling a virtual
    // function of QNode.
    m_parentId = parent->id();
    const auto parentPrivate = get(parent);
    m_scene = parentPrivate->m_scene;
    Q_Q(QNode);
    if (m_scene) {
        // schedule the backend notification and scene registering -> set observers through scene
        m_scene->postConstructorInit()->addNode(q);
    }
}

/*!
 * \internal
 *
 * Sends QNodeCreatedChange events to the aspects.
 */
void QNodePrivate::createBackendNode()
{
    // Do nothing if we already have already sent a node creation change
    // and not a subsequent node destroyed change.
    if (m_hasBackendNode || !m_scene || !m_scene->engine())
        return;

    Q_Q(QNode);
    QAspectEnginePrivate::get(m_scene->engine())->addNode(q);
}

/*!
 * \internal
 *
 * Notify the backend that the parent lost this node as a child and
 * that this node is being destroyed. We only send the node removed
 * change for the parent's children property iff we have an id for
 * a parent node. This is set/unset in the _q_addChild()/_q_removeChild()
 * functions (and initialized in init() if there is a parent at
 * construction time).
 *
 * Likewise, we only send the node destroyed change, iff we have
 * previously sent a node created change. This is tracked via the
 * m_hasBackendNode member.
 */
void QNodePrivate::notifyDestructionChangesAndRemoveFromScene()
{
    Q_Q(QNode);

    // Ensure this node is not queued up for post-construction init
    // to avoid crashing when the event loop spins.
    if (m_scene && m_scene->postConstructorInit())
        m_scene->postConstructorInit()->removeNode(q);

    // Tell the backend we are about to be destroyed
    if (m_hasBackendNode && m_scene && m_scene->engine())
        QAspectEnginePrivate::get(m_scene->engine())->removeNode(q);

    // We unset the scene from the node as its backend node was/is about to be destroyed
    QNodeVisitor visitor;
    visitor.traverse(q, this, &QNodePrivate::unsetSceneHelper);
}

/*!
 * \internal
 *
 * Sends a QNodeCreatedChange event to the aspects and then also notifies the
 * parent backend node of its new child. This is called in a deferred manner
 * by NodePostConstructorInit::processNodes to notify the backend of newly created
 * nodes with a parent that is already part of the scene.
 *
 * Also notify the scene of this node, so it may set it's change arbiter.
 */
void QNodePrivate::_q_postConstructorInit()
{
    Q_Q(QNode);

    // If we've already done the work then bail out. This can happen if the
    // user creates a QNode subclass with an explicit parent, then immediately
    // sets the new QNode as a property on another node. In this case, the
    // property setter will call this function directly, but as we can't
    // un-schedule a deferred invocation, this function will be called again
    // the next time the event loop spins. So, catch this case and abort.
    if (m_hasBackendNode)
        return;

    // Check that the parent hasn't been unset since this call was enqueued
    auto parentNode = q->parentNode();
    if (!parentNode)
        return;

    // Set the scene on this node and all children it references so that all
    // children have a scene set since createBackendNode will set
    // m_hasBackendNode to true for all children, which would prevent them from
    // ever having their scene set
    if (m_scene) {
        QNodeVisitor visitor;
        visitor.traverse(q, parentNode->d_func(), &QNodePrivate::setSceneHelper);
    }

    // Let the backend know we have been added to the scene
    createBackendNode();

    // Let the backend parent know that they have a new child
    Q_ASSERT(parentNode);
    QNodePrivate::get(parentNode)->_q_addChild(q);
}

/*!
 * \internal
 *
 * Called by _q_setParentHelper() or _q_postConstructorInit()
 * on the main thread.
 */
void QNodePrivate::_q_addChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    Q_ASSERT_X(childNode->parent() == q_func(), Q_FUNC_INFO,  "not a child of this node");

    // Have we already notified the parent about its new child? If so, bail out
    // early so that we do not send more than one new child event to the backend
    QNodePrivate *childD = QNodePrivate::get(childNode);
    if (childD->m_notifiedParent == true)
        return;

    // Store our id as the parentId in the child so that even if the child gets
    // removed from the scene as part of the destruction of the parent, when the
    // parent's children are deleted in the QObject dtor, we still have access to
    // the parentId. If we didn't store this, we wouldn't have access at that time
    // because the parent would then only be a QObject, the QNode part would have
    // been destroyed already.
    childD->m_parentId = m_id;

    if (!m_scene)
        return;

    // We need to send a QPropertyNodeAddedChange to the backend
    // to notify the backend that we have a new child
    if (m_changeArbiter != nullptr) {
        // Flag that we have notified the parent. We do this immediately before
        // creating the change because that recurses back into this function and
        // we need to catch that to avoid sending more than one new child event
        // to the backend.
        childD->m_notifiedParent = true;
        update();
    }

    // Update the scene
    // TODO: Fold this into the QAspectEnginePrivate::addNode so we don't have to
    // traverse the sub tree three times!
    QNodeVisitor visitor;
    visitor.traverse(childNode, this, &QNodePrivate::addEntityComponentToScene);
}

/*!
 * \internal
 *
 * Called by _q_setParentHelper on the main thread.
 */
void QNodePrivate::_q_removeChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    Q_ASSERT_X(childNode->parent() == q_func(), Q_FUNC_INFO, "not a child of this node");

    QNodePrivate::get(childNode)->m_parentId = QNodeId();
    update();
}

/*!
 * \internal
 *
 * Reparents the public QNode to \a parent. If the new parent is nullptr then this
 * QNode is no longer part of the scene and so we notify the backend of its removal
 * from its parent's list of children, and then send a QNodeDestroyedChange to the
 * aspects so that the corresponding backend node is destroyed.
 *
 * If \a parent is not null, then we must tell its new parent about this QNode now
 * being a child of it on the backend. If this QNode did not have a parent upon
 * entry to this function, then we must first send a QNodeCreatedChange to the backend
 * prior to sending the QPropertyNodeAddedChange to its parent.
 *
 * Note: This function should never be called from the ctor directly as the type may
 * not be fully created yet and creating creation changes involves calling a virtual
 * function on QNode. The function _q_notifyCreationAndChildChanges() is used
 * for sending initial notification when a parent is passed to the QNode ctor.
 * That function does a subset of this function with the assumption that the new object
 * had no parent before (must be true as it is newly constructed).
 */
void QNodePrivate::_q_setParentHelper(QNode *parent)
{
    Q_Q(QNode);
    QNode *oldParentNode = q->parentNode();

    // If we had a parent, we let him know that we are about to change
    // parent
    if (oldParentNode && m_hasBackendNode) {
        QNodePrivate::get(oldParentNode)->_q_removeChild(q);

        // If we have an old parent but the new parent is null or if the new
        // parent hasn't yet been added to the backend the backend node needs
        // to be destroyed
        // e.g:
        // QEntity *child = new QEntity(some_parent);
        // After some time, in a later event loop
        // QEntity *newSubTreeRoot = new QEntity(someGlobalExisitingRoot)
        // child->setParent(newSubTreeRoot)
        if (!parent || !QNodePrivate::get(parent)->m_hasBackendNode)
            notifyDestructionChangesAndRemoveFromScene();
    }

    // Flag that we need to notify any new parent
    m_notifiedParent = false;

    // Basically QObject::setParent but for QObjectPrivate
    QObjectPrivate::setParent_helper(parent);

    if (parent) {
        // If we had no parent but are about to set one,
        // we need to send a QNodeCreatedChange
        QNodePrivate *newParentPrivate = QNodePrivate::get(parent);

        // Set the scene helper / arbiter
        if (newParentPrivate->m_scene) {
            QNodeVisitor visitor;
            visitor.traverse(q, parent->d_func(), &QNodePrivate::setSceneHelper);
        }

        // We want to make sure that subTreeRoot is always created before
        // child.
        // Given a case such as below
        // QEntity *subTreeRoot = new QEntity(someGlobalExisitingRoot)
        // QEntity *child = new QEntity();
        // child->setParent(subTreeRoot)
        // We need to take into account that subTreeRoot needs to be
        // created in the backend before the child.
        // Therefore we only call createBackendNode if the parent
        // hasn't been created yet as we know that when the parent will be
        // fully created, it will also send the changes for all of its
        // children

        if (newParentPrivate->m_hasBackendNode)
            createBackendNode();

        // If we have a valid new parent, we let him know that we are its child
        QNodePrivate::get(parent)->_q_addChild(q);
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
    Q_UNUSED(propertyIndex);

    // Bail out early if we can to avoid the cost below
    if (m_blockNotifications)
        return;

    update();
}

/*!
    \internal
    Recursively sets and adds the nodes in the subtree of base node \a root to the scene.
    Also takes care of connecting Components and Entities together in the scene.
 */
void QNodePrivate::setSceneHelper(QNode *root)
{
    // Sets the scene
    root->d_func()->setScene(m_scene);
    // addObservable sets the QChangeArbiter
    m_scene->addObservable(root);

    // We also need to handle QEntity <-> QComponent relationships
    if (QComponent *c = qobject_cast<QComponent *>(root)) {
        const QList<QEntity *> entities = c->entities();
        for (QEntity *entity : entities) {
            if (!m_scene->hasEntityForComponent(c->id(), entity->id())) {
                if (!c->isShareable() && !m_scene->entitiesForComponent(c->id()).isEmpty())
                    qWarning() << "Trying to assign a non shareable component to more than one Entity";
                m_scene->addEntityForComponent(c->id(), entity->id());
            }
        }
    }
}

/*!
    \internal

    Recursively unsets and remove nodes in the subtree of base node \a root from
    the scene. Also takes care of removing Components and Entities connections.
 */
void QNodePrivate::unsetSceneHelper(QNode *node)
{
    QNodePrivate *nodePrivate = QNodePrivate::get(node);

    // We also need to handle QEntity <-> QComponent relationships removal
    if (QComponent *c = qobject_cast<QComponent *>(node)) {
        const QList<QEntity *> entities = c->entities();
        for (QEntity *entity : entities) {
            if (nodePrivate->m_scene)
                nodePrivate->m_scene->removeEntityForComponent(c->id(), entity->id());
        }
    }

    if (nodePrivate->m_scene != nullptr)
        nodePrivate->m_scene->removeObservable(node);
    nodePrivate->setScene(nullptr);
}

/*!
    \internal
 */
void QNodePrivate::addEntityComponentToScene(QNode *root)
{
    if (QEntity *e = qobject_cast<QEntity *>(root)) {
        const auto components = e->components();
        for (QComponent *c : components) {
            if (!m_scene->hasEntityForComponent(c->id(), e->id()))
                m_scene->addEntityForComponent(c->id(), e->id());
        }
    }
}

/*!
    \internal
 */
// Called in the main thread by QScene -> following QEvent::childAdded / addChild
void QNodePrivate::setArbiter(QChangeArbiter *arbiter)
{
    if (m_changeArbiter && m_changeArbiter != arbiter) {
        unregisterNotifiedProperties();

        // Remove node from dirtyFrontendNodeList on old arbiter
        Q_Q(QNode);
        m_changeArbiter->removeDirtyFrontEndNode(q);
    }
    m_changeArbiter = arbiter;
    if (m_changeArbiter)
        registerNotifiedProperties();
}

/*!
 * \internal
 * Makes sure this node has a backend by traversing the tree up to the most distant ancestor
 * without a backend node and initializing that node. This is done to make sure the parent nodes
 * are always created before the child nodes, since child nodes reference parent nodes at creation
 * time.
 */
void QNodePrivate::_q_ensureBackendNodeCreated()
{
    if (m_hasBackendNode)
        return;

    Q_Q(QNode);

    QNode *nextNode = q;
    QNode *topNodeWithoutBackend = nullptr;
    while (nextNode != nullptr && !QNodePrivate::get(nextNode)->m_hasBackendNode) {
        topNodeWithoutBackend = nextNode;
        nextNode = nextNode->parentNode();
    }
    QNodePrivate::get(topNodeWithoutBackend)->_q_postConstructorInit();
}

/*!
    \class Qt3DCore::QNode
    \inherits QObject

    \inmodule Qt3DCore
    \since 5.5

    \brief QNode is the base class of all Qt3D node classes used to build a
    Qt3D scene.

    The owernship of QNode is determined by the QObject parent/child
    relationship between nodes. By itself, a QNode has no visual appearance
    and no particular meaning, it is there as a way of building a node based tree
    structure.

    The parent of a QNode instance can only be another QNode instance.

    Each QNode instance has a unique id that allows it to be recognizable
    from other instances.

    When properties are defined on a QNode subclass, their NOTIFY signal
    will automatically generate notifications that the Qt3D backend aspects will
    receive.

    \sa QEntity, QComponent
*/

/*!
    \internal
 */
void QNodePrivate::setScene(QScene *scene)
{
    if (m_scene != scene) {
        m_scene = scene;
    }
}

/*!
    \internal
 */
QScene *QNodePrivate::scene() const
{
    return m_scene;
}

/*!
    \internal
 */
void QNodePrivate::notifyPropertyChange(const char *name, const QVariant &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);

    // Bail out early if we can to avoid operator new
    if (m_blockNotifications)
        return;

    update();
}

void QNodePrivate::notifyDynamicPropertyChange(const QByteArray &name, const QVariant &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);

    // Bail out early if we can to avoid operator new
    if (m_blockNotifications)
        return;

    update();
}

// Inserts this tree into the main Scene tree.
// Needed when SceneLoaders provide a cloned tree from the backend
// and need to insert it in the main scene tree
// QNode *root;
// QNode *subtree;
// QNodePrivate::get(root)->insertTree(subtree);

/*!
    \internal
 */
void QNodePrivate::insertTree(QNode *treeRoot, int depth)
{
    if (m_scene != nullptr) {
        treeRoot->d_func()->setScene(m_scene);
        m_scene->addObservable(treeRoot);
    }

    for (QObject *c : treeRoot->children()) {
        QNode *n = nullptr;
        if ((n = qobject_cast<QNode *>(c)) != nullptr)
            insertTree(n, depth + 1);
    }

    if (depth == 0)
        treeRoot->setParent(q_func());
}

void QNodePrivate::update()
{
    if (m_changeArbiter) {
        Q_Q(QNode);
        m_changeArbiter->addDirtyFrontEndNode(q);
    }
}

void QNodePrivate::markDirty(QScene::DirtyNodeSet changes)
{
    if (m_scene)
        m_scene->markDirty(changes);
}

/*!
    \internal
 */
QNodePrivate *QNodePrivate::get(QNode *q)
{
    return q->d_func();
}

/*!
    \internal
 */
const QNodePrivate *QNodePrivate::get(const QNode *q)
{
    return q->d_func();
}

/*!
    \internal
 */
void QNodePrivate::nodePtrDeleter(QNode *q)
{
    QObject *p = q->parent();
    if (p == nullptr)
        p = q;
    p->deleteLater();
}

/*!
    \fn Qt3DCore::QNodeId Qt3DCore::qIdForNode(Qt3DCore::QNode *node)
    \relates Qt3DCore::QNode
    \return node id for \a node.
*/

/*!
    \fn template<typename T> Qt3DCore::QNodeIdVector Qt3DCore::qIdsForNodes(const T &nodes)
    \relates Qt3DCore::QNode
    \return vector of node ids for \a nodes.
*/

/*!
     Creates a new QNode instance with parent \a parent.

     \note The backend aspects will be notified that a QNode instance is
     part of the scene only if it has a parent; unless this is the root node of
     the Qt3D scene.

     \sa setParent()
*/
QNode::QNode(QNode *parent)
    : QNode(*new QNodePrivate, parent) {}

/*! \internal */
QNode::QNode(QNodePrivate &dd, QNode *parent)
    : QObject(dd, parent)
{
    Q_D(QNode);
    d->init(parent);
}

/*!
    \fn Qt3DCore::QNode::nodeDestroyed()
    Emitted when the node is destroyed.
*/

/*! \internal */
QNode::~QNode()
{
    Q_D(QNode);
    // Disconnect each connection that was stored
    for (const auto &nodeConnectionPair : std::as_const(d->m_destructionConnections))
        QObject::disconnect(nodeConnectionPair.second);
    d->m_destructionConnections.clear();
    Q_EMIT nodeDestroyed();

    // Notify the backend that the parent lost this node as a child and
    // that this node is being destroyed.
    d->notifyDestructionChangesAndRemoveFromScene();
}

/*!
    Returns the id that uniquely identifies the QNode instance.
*/
QNodeId QNode::id() const
{
    Q_D(const QNode);
    return d->m_id;
}

/*!
    \property Qt3DCore::QNode::parent

    Holds the immediate QNode parent, or null if the node has no parent.

    Setting the parent will notify the backend aspects about current QNode
    instance's parent change.

    \note if \a parent happens to be null, this will actually notify that the
    current QNode instance was removed from the scene.
*/
QNode *QNode::parentNode() const
{
    return qobject_cast<QNode*>(parent());
}

/*!
    Returns \c true if aspect notifications are blocked; otherwise returns \c false.
    By default, notifications are \e not blocked.

    \sa blockNotifications()
*/
bool QNode::notificationsBlocked() const
{
    Q_D(const QNode);
    return d->m_blockNotifications;
}

/*!
    If \a block is \c true, property change notifications sent by this object
    to aspects are blocked. If \a block is \c false, no such blocking will occur.

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

// Note: should never be called from the ctor directly as the type may not be fully
// created yet
void QNode::setParent(QNode *parent)
{
    Q_D(QNode);

    // If we already have a parent don't do anything. Be careful to ensure
    // that QNode knows about the parent, not just QObject (by checking the ids)
    if (parentNode() == parent &&
            ((parent != nullptr && d->m_parentId == parentNode()->id()) || parent == nullptr))
        return;

    // remove ourself from postConstructorInit queue. The call to _q_setParentHelper
    // will take care of creating the backend node if necessary depending on new parent.
    if (d->m_scene)
        d->m_scene->postConstructorInit()->removeNode(this);

    d->_q_setParentHelper(parent);

    // Block notifications as we want to let the _q_setParentHelper
    // manually handle them
    const bool blocked = blockNotifications(true);
    emit parentChanged(parent);
    blockNotifications(blocked);
}

/*!
    \typedef Qt3DCore::QNodePtr
    \relates Qt3DCore::QNode

    A shared pointer for QNode.
*/
/*!
    \typedef Qt3DCore::QNodeVector
    \relates Qt3DCore::QNode

    List of QNode pointers.
*/

/*!
 * Returns a list filled with the QNode children of the current
 * QNode instance.
 */
QNodeVector QNode::childNodes() const
{
    QNodeVector nodeChildrenList;
    const QObjectList &objectChildrenList = QObject::children();
    nodeChildrenList.reserve(objectChildrenList.size());

    for (QObject *c : objectChildrenList) {
        if (QNode *n = qobject_cast<QNode *>(c))
            nodeChildrenList.push_back(n);
    }

    return nodeChildrenList;
}
void QNode::setEnabled(bool isEnabled)
{
    Q_D(QNode);

    if (d->m_enabled == isEnabled)
        return;

    d->m_enabled = isEnabled;
    emit enabledChanged(isEnabled);
}

/*!
    \property Qt3DCore::QNode::enabled

    Holds the QNode enabled flag.
    By default a QNode is always enabled.

    \note the interpretation of what enabled means is aspect-dependent. Even if
    enabled is set to \c false, some aspects may still consider the node in
    some manner. This is documented on a class by class basis.
*/
bool QNode::isEnabled() const
{
    Q_D(const QNode);
    return d->m_enabled;
}

namespace {

/*! \internal */
inline const QMetaObjectPrivate *priv(const uint* data)
{
    return reinterpret_cast<const QMetaObjectPrivate*>(data);
}

/*! \internal */
inline bool isDynamicMetaObject(const QMetaObject *mo)
{
    return (priv(mo->d.data)->flags & DynamicMetaObject);
}

} // anonymous

/*!
 * \internal
 *
 * Find the most derived metaobject that doesn't have a dynamic
 * metaobject farther up the chain.
 * TODO: Add support to QMetaObject to explicitly say if it's a dynamic
 * or static metaobject so we don't need this logic
 */
const QMetaObject *QNodePrivate::findStaticMetaObject(const QMetaObject *metaObject)
{
    const QMetaObject *lastStaticMetaobject = nullptr;
    auto mo = metaObject;
    while (mo) {
        const bool dynamicMetaObject = isDynamicMetaObject(mo);
        if (dynamicMetaObject)
            lastStaticMetaobject = nullptr;

        if (!dynamicMetaObject && !lastStaticMetaobject)
            lastStaticMetaobject = mo;

        mo = mo->superClass();
    }
    Q_ASSERT(lastStaticMetaobject);
    return lastStaticMetaobject;
}

/*!
 * \internal
 *
 * NodePostConstructorInit handles calling QNode::_q_postConstructorInit for
 * all nodes. By keeping track of nodes that need initialization we can
 * create them all together ensuring they get sent to the backend in a single
 * batch.
 */
NodePostConstructorInit::NodePostConstructorInit(QObject *parent)
    : QObject(parent)
    , m_requestedProcessing(false)
{
}

NodePostConstructorInit::~NodePostConstructorInit() {}

/*!
 * \internal
 *
 * Add a node to the list of nodes needing a call to _q_postConstructorInit
 * We only add the node if it does not have an ancestor already in the queue
 * because initializing the ancestor will initialize all it's children.
 * This ensures that all backend nodes are created from the top-down, with
 * all parents created before their children
 *
 */
void NodePostConstructorInit::addNode(QNode *node)
{
    Q_ASSERT(node);
    QNode *nextNode = node;
    while (nextNode != nullptr && !m_nodesToConstruct.contains(QNodePrivate::get(nextNode)))
        nextNode = nextNode->parentNode();

    if (!nextNode) {
        m_nodesToConstruct.append(QNodePrivate::get(node));
        if (!m_requestedProcessing){
            QMetaObject::invokeMethod(this, "processNodes", Qt::QueuedConnection);
            m_requestedProcessing = true;
        }
    }
}

/*!
 * \internal
 *
 * Remove a node from the queue. This will ensure none of its
 * children get initialized
 */
void NodePostConstructorInit::removeNode(QNode *node)
{
    Q_ASSERT(node);
    m_nodesToConstruct.removeAll(QNodePrivate::get(node));
}

/*!
 * \internal
 *
 * call _q_postConstructorInit for all nodes in the queue
 * and clear the queue
 */
void NodePostConstructorInit::processNodes()
{
    Q_ASSERT(thread() == QThread::currentThread());
    m_requestedProcessing = false;
    while (!m_nodesToConstruct.empty()) {
        auto node = m_nodesToConstruct.takeFirst();
        node->_q_postConstructorInit();
    }
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qnode_p.cpp"

#include "moc_qnode.cpp"
