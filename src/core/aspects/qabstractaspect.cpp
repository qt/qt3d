// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractaspect.h"
#include "qabstractaspect_p.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

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
 * \fn template <class Frontend> void Qt3DCore::QAbstractAspect::registerBackendType(const Qt3DCore::QBackendNodeMapperPtr &functor)
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
QNodeId QAbstractAspect::rootEntityId() const noexcept
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
    d->m_backendCreatorFunctors.insert(&obj, functor);
}

void QAbstractAspect::unregisterBackendType(const QMetaObject &obj)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.remove(&obj);
}

QVariant QAbstractAspect::executeCommand(const QStringList &args)
{
    Q_UNUSED(args);
    return QVariant();
}

std::vector<QAspectJobPtr> QAbstractAspect::jobsToExecute(qint64 time)
{
    Q_UNUSED(time);
    return {};
}

QBackendNodeMapperPtr QAbstractAspectPrivate::mapperForNode(const QMetaObject *metaObj) const
{
    Q_ASSERT(metaObj);
    QBackendNodeMapperPtr mapper;

    while (metaObj != nullptr && mapper.isNull()) {
        mapper = m_backendCreatorFunctors.value(metaObj);
        metaObj = metaObj->superClass();
    }
    return mapper;
}

void QAbstractAspectPrivate::syncDirtyFrontEndNodes(const QList<QNode *> &nodes)
{
    for (auto node: std::as_const(nodes)) {
        const QMetaObject *metaObj = QNodePrivate::get(node)->m_typeInfo;
        const QBackendNodeMapperPtr backendNodeMapper = mapperForNode(metaObj);

        if (!backendNodeMapper)
            continue;

        QBackendNode *backend = backendNodeMapper->get(node->id());
        if (!backend)
            continue;

        syncDirtyFrontEndNode(node, backend, false);
    }
}

void QAbstractAspectPrivate::syncDirtyFrontEndNode(QNode *node, QBackendNode *backend, bool firstTime) const
{
    backend->syncFromFrontEnd(node, firstTime);
}

void QAbstractAspectPrivate::syncDirtyEntityComponentNodes(const QList<ComponentRelationshipChange> &changes)
{
    auto getBackend = [this] (QNode *node) -> QBackendNode* {
        const QMetaObject *metaObj = QNodePrivate::get(node)->m_typeInfo;
        const QBackendNodeMapperPtr backendNodeMapper = mapperForNode(metaObj);

        if (!backendNodeMapper)
            return nullptr;

        return backendNodeMapper->get(node->id());
    };

    for (const auto &change: std::as_const(changes)) {
        auto entityBackend = getBackend(change.node);
        if (!entityBackend)
            continue;

        auto componentBackend = getBackend(change.subNode);
        if (!componentBackend)
            continue;

        switch (change.change) {
        case ComponentRelationshipChange::Added:
            QBackendNodePrivate::get(entityBackend)->componentAdded(change.subNode);
            QBackendNodePrivate::get(componentBackend)->addedToEntity(change.node);
            break;
        case ComponentRelationshipChange::Removed:
            QBackendNodePrivate::get(entityBackend)->componentRemoved(change.subNode);
            QBackendNodePrivate::get(componentBackend)->removedFromEntity(change.node);
            break;
        }
    }
}

QBackendNode *QAbstractAspectPrivate::createBackendNode(const NodeTreeChange &change) const
{
    const QMetaObject *metaObj = change.metaObj;
    const QBackendNodeMapperPtr backendNodeMapper = mapperForNode(metaObj);

    if (!backendNodeMapper)
        return nullptr;

    QBackendNode *backend = backendNodeMapper->get(change.id);
    if (backend != nullptr)
        return backend;

    QNode *node = change.node;
    QNodeId nodeId = qIdForNode(node);
    backend = backendNodeMapper->create(nodeId);

    if (!backend)
        return nullptr;

    // TODO: Find some place else to do all of this function from the arbiter
    backend->setPeerId(nodeId);

    // Backend could be null if the user decides that his functor should only
    // perform some action when encountering a given type of item but doesn't need to
    // return a QBackendNode pointer.

    QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
    backendPriv->setEnabled(node->isEnabled());

    syncDirtyFrontEndNode(node, backend, true);

    return backend;
}

void QAbstractAspectPrivate::clearBackendNode(const NodeTreeChange &change) const
{
    const QMetaObject *metaObj = change.metaObj;
    const QBackendNodeMapperPtr backendNodeMapper = mapperForNode(metaObj);

    if (!backendNodeMapper)
        return;

    backendNodeMapper->destroy(change.id);
}

void QAbstractAspectPrivate::setRootAndCreateNodes(QEntity *rootObject, const QList<NodeTreeChange> &nodesChanges)
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

std::vector<QAspectJobPtr> QAbstractAspectPrivate::jobsToExecute(qint64 time)
{
    Q_Q(QAbstractAspect);
    auto res = q->jobsToExecute(time);

    {
        QMutexLocker lock(&m_singleShotMutex);
        Qt3DCore::moveAtEnd(res, std::move(m_singleShotJobs));
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

/*!
 * Called in the main thread once all the jobs have been executed.
 *
 * \note This is called after QAspectJob::postFrame is called on every jobs.
 *
 * \sa QAspectJob::postFrame
 */
void QAbstractAspect::jobsDone()
{
    Q_D(QAbstractAspect);
    d->jobsDone();
}

/*!
 * Called in the main thread when the frame processing is complete.
 */
void QAbstractAspect::frameDone()
{
    Q_D(QAbstractAspect);
    d->frameDone();
}

void QAbstractAspect::scheduleSingleShotJob(const Qt3DCore::QAspectJobPtr &job)
{
    Q_D(QAbstractAspect);
    QMutexLocker lock(&d->m_singleShotMutex);
    d->m_singleShotJobs.push_back(job);
}

QStringList QAbstractAspect::dependencies() const
{
    return {};
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

#include "moc_qabstractaspect_p.cpp"

#include "moc_qabstractaspect.cpp"
