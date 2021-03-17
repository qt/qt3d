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

#include "qabstractaspect.h"
#include "qabstractaspect_p.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertyvalueaddedchange.h>
#include <Qt3DCore/qpropertyvalueremovedchange.h>
#include <Qt3DCore/qcomponentaddedchange.h>
#include <Qt3DCore/qcomponentremovedchange.h>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAbstractAspectPrivate::QAbstractAspectPrivate()
    : QObjectPrivate()
    , m_root(nullptr)
    , m_rootId()
    , m_aspectManager(nullptr)
    , m_jobManager(nullptr)
    , m_arbiter(nullptr)
{
}

QAbstractAspectPrivate::~QAbstractAspectPrivate()
{
}

QAbstractAspectPrivate *QAbstractAspectPrivate::get(QAbstractAspect *aspect)
{
    return aspect->d_func();
}

/*!
 * \internal
 * Called in the context of the main thread
 */
void QAbstractAspectPrivate::onEngineAboutToShutdown()
{
}

/*! \internal */
void QAbstractAspectPrivate::unregisterBackendType(const QMetaObject &mo)
{
    m_backendCreatorFunctors.remove(&mo);
}

/*!
 * \class Qt3DCore::QAbstractAspect
 * \inheaderfile Qt3DCore/QAbstractAspect
 * \inherits QObject
 * \inmodule Qt3DCore
 * \brief QAbstractAspect is the base class for aspects that provide a vertical slice of behavior.
 */

/*!
 * \fn void Qt3DCore::QAbstractAspect::registerBackendType(const Qt3DCore::QBackendNodeMapperPtr &functor)
 * Registers backend with \a functor.
 */

/*!
 * \macro QT3D_REGISTER_ASPECT(name, AspectType)
 * \relates Qt3DCore::QAbstractAspect
 *
 * Convenience macro for registering \a AspectType for instantiation by the
 * currently set Qt3DCore::QAspectFactory. This makes it possible to create an
 * instance of \a AspectType in the aspect thread by later passing \a name to
 * Qt3DCore::QAspectEngine::registerAspect(const QString &name).
 *
 * \note It is also possible to register a new aspect without using this macro
 * by instead using Qt3DCore::QAspectEngine::registerAspect(QAbstractAspect *aspect)
 * which will handle moving a previously created aspect instance to the aspect
 * thread context.
 *
 * KDAB has published a few articles about writing custom Qt3D aspects
 * \l {https://www.kdab.com/writing-custom-qt-3d-aspect/}{on their blog}. These
 * provide an excellent starting point if you wish to learn more about it.
 */


/*!
 * Constructs a new QAbstractAspect with \a parent
 */
QAbstractAspect::QAbstractAspect(QObject *parent)
    : QAbstractAspect(*new QAbstractAspectPrivate, parent) {}

/*!
 * \typedef Qt3DCore::QAspectJobPtr
 * \relates Qt3DCore::QAbstractAspect
 *
 * A shared pointer for QAspectJob.
 */

/*!
 * \typedef Qt3DCore::QBackendNodeMapperPtr
 * \relates Qt3DCore::QAbstractAspect
 *
 * A shared pointer for QBackendNodeMapper.
 */

/*!
 * \internal
 */
QAbstractAspect::QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    \internal
*/
QAbstractAspect::~QAbstractAspect()
{
}

/*!
 * \return root entity node id.
 */
QNodeId QAbstractAspect::rootEntityId() const Q_DECL_NOEXCEPT
{
    Q_D(const QAbstractAspect);
    return d->m_rootId;
}

/*!
 * Registers backend with \a obj and \a functor.
 */
void QAbstractAspect::registerBackendType(const QMetaObject &obj, const QBackendNodeMapperPtr &functor)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.insert(&obj, {functor, QAbstractAspectPrivate::DefaultMapper});
}

void QAbstractAspect::registerBackendType(const QMetaObject &obj, const QBackendNodeMapperPtr &functor, bool supportsSyncing)
{
    Q_D(QAbstractAspect);
    const auto f = supportsSyncing ? QAbstractAspectPrivate::SupportsSyncing : QAbstractAspectPrivate::DefaultMapper;
    d->m_backendCreatorFunctors.insert(&obj, {functor, f});
}

void QAbstractAspect::unregisterBackendType(const QMetaObject &obj)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.remove(&obj);
}

QVariant QAbstractAspect::executeCommand(const QStringList &args)
{
    Q_UNUSED(args)
    return QVariant();
}

QVector<QAspectJobPtr> QAbstractAspect::jobsToExecute(qint64 time)
{
    Q_UNUSED(time)
    return QVector<QAspectJobPtr>();
}

QAbstractAspectPrivate::BackendNodeMapperAndInfo QAbstractAspectPrivate::mapperForNode(const QMetaObject *metaObj) const
{
    Q_ASSERT(metaObj);
    BackendNodeMapperAndInfo info;

    while (metaObj != nullptr && info.first.isNull()) {
        info = m_backendCreatorFunctors.value(metaObj);
        metaObj = metaObj->superClass();
    }
    return info;
}

void QAbstractAspectPrivate::syncDirtyFrontEndNodes(const QVector<QNode *> &nodes)
{
    for (auto node: qAsConst(nodes)) {
        const QMetaObject *metaObj = QNodePrivate::get(node)->m_typeInfo;
        const BackendNodeMapperAndInfo backendNodeMapperInfo = mapperForNode(metaObj);
        const QBackendNodeMapperPtr backendNodeMapper = backendNodeMapperInfo.first;

        if (!backendNodeMapper)
            continue;

        QBackendNode *backend = backendNodeMapper->get(node->id());
        if (!backend)
            continue;

        const bool supportsSyncing = backendNodeMapperInfo.second & SupportsSyncing;
        if (supportsSyncing)
            syncDirtyFrontEndNode(node, backend, false);
        else
            sendPropertyMessages(node, backend);
    }
}

void QAbstractAspectPrivate::syncDirtyFrontEndSubNodes(const QVector<NodeRelationshipChange> &nodes)
{
    for (const auto &nodeChange: qAsConst(nodes)) {
        auto getBackend = [this](QNode *node) -> std::tuple<QBackendNode *, bool> {
            const QMetaObject *metaObj = QNodePrivate::get(node)->m_typeInfo;
            if (!metaObj)
                return {};
            const BackendNodeMapperAndInfo backendNodeMapperInfo = mapperForNode(metaObj);
            const QBackendNodeMapperPtr backendNodeMapper = backendNodeMapperInfo.first;

            if (!backendNodeMapper)
                return {};

            QBackendNode *backend = backendNodeMapper->get(node->id());
            if (!backend)
                return {};

            const bool supportsSyncing = backendNodeMapperInfo.second & SupportsSyncing;

            return std::tuple<QBackendNode *, bool>(backend, supportsSyncing);
        };

        auto nodeInfo = getBackend(nodeChange.node);
        if (!std::get<0>(nodeInfo))
            continue;

        auto subNodeInfo = getBackend(nodeChange.subNode);
        if (!std::get<0>(subNodeInfo))
            continue;

        switch (nodeChange.change) {
        case PropertyValueAdded: {
            if (std::get<1>(nodeInfo))
                break; // do nothing as the node will be dirty anyway

            QPropertyValueAddedChange change(nodeChange.node->id());
            change.setPropertyName(nodeChange.property);
            change.setAddedValue(QVariant::fromValue(nodeChange.subNode->id()));
            QPropertyValueAddedChangePtr pChange(&change, [](QPropertyValueAddedChange *) { });
            std::get<0>(nodeInfo)->sceneChangeEvent(pChange);
        }
        break;
        case PropertyValueRemoved: {
            if (std::get<1>(nodeInfo))
                break; // do nothing as the node will be dirty anyway

            QPropertyValueRemovedChange change(nodeChange.node->id());
            change.setPropertyName(nodeChange.property);
            change.setRemovedValue(QVariant::fromValue(nodeChange.subNode->id()));
            QPropertyValueRemovedChangePtr pChange(&change, [](QPropertyValueRemovedChange *) { });
            std::get<0>(nodeInfo)->sceneChangeEvent(pChange);
        }
        break;
        case ComponentAdded: {
            // let the entity know it has a new component
            if (std::get<1>(nodeInfo)) {
                QBackendNodePrivate::get(std::get<0>(nodeInfo))->componentAdded(nodeChange.subNode);
            } else {
                QComponentAddedChange change(qobject_cast<Qt3DCore::QComponent *>(nodeChange.subNode), qobject_cast<Qt3DCore::QEntity *>(nodeChange.node));
                QComponentAddedChangePtr pChange(&change, [](QComponentAddedChange *) { });
                std::get<0>(nodeInfo)->sceneChangeEvent(pChange);
            }

            // let the component know it was added to an entity
            if (std::get<1>(subNodeInfo)) {
                QBackendNodePrivate::get(std::get<0>(subNodeInfo))->addedToEntity(nodeChange.node);
            } else {
                QComponentAddedChange change(qobject_cast<Qt3DCore::QComponent *>(nodeChange.subNode), qobject_cast<Qt3DCore::QEntity *>(nodeChange.node));
                QComponentAddedChangePtr pChange(&change, [](QComponentAddedChange *) { });
                std::get<0>(subNodeInfo)->sceneChangeEvent(pChange);
            }
        }
        break;
        case ComponentRemoved: {
            // let the entity know a component was removed
            if (std::get<1>(nodeInfo)) {
                QBackendNodePrivate::get(std::get<0>(nodeInfo))->componentRemoved(nodeChange.subNode);
            } else {
                QComponentRemovedChange change(qobject_cast<Qt3DCore::QComponent *>(nodeChange.subNode), qobject_cast<Qt3DCore::QEntity *>(nodeChange.node));
                QComponentRemovedChangePtr pChange(&change, [](QComponentRemovedChange *) { });
                std::get<0>(nodeInfo)->sceneChangeEvent(pChange);
            }

            // let the component know it was removed from an entity
            if (std::get<1>(subNodeInfo)) {
                QBackendNodePrivate::get(std::get<0>(subNodeInfo))->removedFromEntity(nodeChange.node);
            } else {
                QComponentRemovedChange change(qobject_cast<Qt3DCore::QEntity *>(nodeChange.node), qobject_cast<Qt3DCore::QComponent *>(nodeChange.subNode));
                QComponentRemovedChangePtr pChange(&change, [](QComponentRemovedChange *) { });
                std::get<0>(nodeInfo)->sceneChangeEvent(pChange);
            }
        }
        break;
        default:
            break;
        }
    }
}

void QAbstractAspectPrivate::syncDirtyFrontEndNode(QNode *node, QBackendNode *backend, bool firstTime) const
{
    Q_ASSERT(false); // overload in derived class
    if (!firstTime)
        sendPropertyMessages(node, backend);
}

void QAbstractAspectPrivate::sendPropertyMessages(QNode *node, QBackendNode *backend) const
{
    const int offset = QNode::staticMetaObject.propertyOffset();
    const auto metaObj = node->metaObject();
    const int count = metaObj->propertyCount();

    const auto toBackendValue = [](const QVariant &data) -> QVariant
    {
        if (data.canConvert<QNode*>()) {
            QNode *node = data.value<QNode*>();

            // Ensure the node and all ancestors have issued their node creation changes.
            // We can end up here if a newly created node with a parent is immediately set
            // as a property on another node. In this case the deferred call to
            // _q_postConstructorInit() will not have happened yet as the event
            // loop will still be blocked. We need to do this for all ancestors,
            // since the subtree of this node otherwise can end up on the backend
            // with a reference to a non-existent parent.
            if (node)
                QNodePrivate::get(node)->_q_ensureBackendNodeCreated();

            const QNodeId id = node ? node->id() : QNodeId();
            return QVariant::fromValue(id);
        }

        return data;
    };

    QPropertyUpdatedChange change(node->id());
    QPropertyUpdatedChangePtr pchange(&change, [](QPropertyUpdatedChange *) { });
    for (int index = offset; index < count; index++) {
        const QMetaProperty pro = metaObj->property(index);
        change.setPropertyName(pro.name());
        change.setValue(toBackendValue(pro.read(node)));
        backend->sceneChangeEvent(pchange);
    }

    auto const dynamicProperties = node->dynamicPropertyNames();
    for (const QByteArray &name: dynamicProperties) {
        change.setPropertyName(name.data());
        change.setValue(toBackendValue(node->property(name.data())));
        backend->sceneChangeEvent(pchange);
    }
}

QBackendNode *QAbstractAspectPrivate::createBackendNode(const NodeTreeChange &change) const
{
    const QMetaObject *metaObj = change.metaObj;
    const BackendNodeMapperAndInfo backendNodeMapperInfo = mapperForNode(metaObj);
    const QBackendNodeMapperPtr backendNodeMapper = backendNodeMapperInfo.first;

    if (!backendNodeMapper)
        return nullptr;

    QBackendNode *backend = backendNodeMapper->get(change.id);
    if (backend != nullptr)
        return backend;

    QNode *node = change.node;
    QNodeCreatedChangeBasePtr creationChange;
    const bool supportsSyncing = backendNodeMapperInfo.second & SupportsSyncing;
    if (supportsSyncing) {
        // All objects modified to use syncing should only use the id in the creation functor
        QNodeCreatedChangeBase changeObj(node);
        creationChange = QNodeCreatedChangeBasePtr(&changeObj, [](QNodeCreatedChangeBase *) {});
        backend = backendNodeMapper->create(creationChange);
    } else {
        creationChange = node->createNodeCreationChange();
        backend = backendNodeMapper->create(creationChange);
    }

    if (!backend)
        return nullptr;

    // TODO: Find some place else to do all of this function from the arbiter
    backend->setPeerId(change.id);

    // Backend could be null if the user decides that his functor should only
    // perform some action when encountering a given type of item but doesn't need to
    // return a QBackendNode pointer.

    QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
    backendPriv->setEnabled(node->isEnabled());

    // TO DO: Find a way to specify the changes to observe
    // Register backendNode with QChangeArbiter
    if (m_arbiter != nullptr) { // Unit tests may not have the arbiter registered
        qCDebug(Nodes) << q_func()->objectName() << "Creating backend node for node id"
                       << node->id() << "of type" << QNodePrivate::get(node)->m_typeInfo->className();
        m_arbiter->registerObserver(backendPriv, backend->peerId(), AllChanges);
        if (backend->mode() == QBackendNode::ReadWrite)
            m_arbiter->scene()->addObservable(backendPriv, backend->peerId());
    }

    if (supportsSyncing)
        syncDirtyFrontEndNode(node, backend, true);
    else
        backend->initializeFromPeer(creationChange);

    return backend;
}

void QAbstractAspectPrivate::clearBackendNode(const NodeTreeChange &change) const
{
    const QMetaObject *metaObj = change.metaObj;
    const BackendNodeMapperAndInfo backendNodeMapperInfo = mapperForNode(metaObj);
    const QBackendNodeMapperPtr backendNodeMapper = backendNodeMapperInfo.first;

    if (!backendNodeMapper)
        return;

    // Request the mapper to destroy the corresponding backend node
    QBackendNode *backend = backendNodeMapper->get(change.id);
    if (backend) {
        qCDebug(Nodes) << "Deleting backend node for node id"
                       << change.id << "of type" << metaObj->className();
        QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
        m_arbiter->unregisterObserver(backendPriv, backend->peerId());
        if (backend->mode() == QBackendNode::ReadWrite)
            m_arbiter->scene()->removeObservable(backendPriv, backend->peerId());
        backendNodeMapper->destroy(change.id);
    }
}

void QAbstractAspectPrivate::setRootAndCreateNodes(QEntity *rootObject, const QVector<NodeTreeChange> &nodesChanges)
{
    qCDebug(Aspects) << Q_FUNC_INFO << "rootObject =" << rootObject;
    if (rootObject == m_root)
        return;

    m_root = rootObject;
    m_rootId = rootObject->id();

    for (const NodeTreeChange &change : nodesChanges)
        createBackendNode(change);
}


QServiceLocator *QAbstractAspectPrivate::services() const
{
    return m_aspectManager ? m_aspectManager->serviceLocator() : nullptr;
}

QAbstractAspectJobManager *QAbstractAspectPrivate::jobManager() const
{
    return m_jobManager;
}

QVector<QAspectJobPtr> QAbstractAspectPrivate::jobsToExecute(qint64 time)
{
    Q_Q(QAbstractAspect);
    auto res = q->jobsToExecute(time);

    {
        QMutexLocker lock(&m_singleShotMutex);
        res << m_singleShotJobs;
        m_singleShotJobs.clear();
    }

    return res;
}

void QAbstractAspectPrivate::jobsDone()
{
}

void QAbstractAspectPrivate::frameDone()
{
}

/*!
 * Called in the context of the aspect thread once the aspect has been registered.
 * This provides an opportunity for the aspect to do any initialization tasks that
 * require to be in the aspect thread context such as creating QObject subclasses that
 * must have affinity with this thread.
 *
 * \sa onUnregistered
 */
void QAbstractAspect::onRegistered()
{
}

/*!
 * Called in the context of the aspect thread during unregistration
 * of the aspect. This gives the aspect a chance to do any final pieces of
 * cleanup that it would not do when just changing to a new scene.
 *
 * \sa onRegistered
 */
void QAbstractAspect::onUnregistered()
{
}

/*!
 *
 * Called in the QAspectThread context
 */
void QAbstractAspect::onEngineStartup()
{
}

/*!
 *
 * Called in the QAspectThread context
 */
void QAbstractAspect::onEngineShutdown()
{
}

void QAbstractAspect::scheduleSingleShotJob(const Qt3DCore::QAspectJobPtr &job)
{
    Q_D(QAbstractAspect);
    QMutexLocker lock(&d->m_singleShotMutex);
    d->m_singleShotJobs.push_back(job);
}

namespace Debug {

AsynchronousCommandReply::AsynchronousCommandReply(const QString &commandName, QObject *parent)
    : QObject(parent)
    , m_commandName(commandName)
    , m_finished(false)
{
}

void AsynchronousCommandReply::setFinished(bool replyFinished)
{
    m_finished = replyFinished;
    if (m_finished)
        emit finished(this);
}

void AsynchronousCommandReply::setData(const QByteArray &data)
{
    m_data = data;
}

} // Debug


} // of namespace Qt3DCore

QT_END_NAMESPACE
