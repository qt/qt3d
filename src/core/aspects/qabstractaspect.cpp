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
#include "qentity.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <private/qabstractaspect_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <private/qchangearbiter_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAbstractAspectPrivate::QAbstractAspectPrivate()
    : QObjectPrivate()
    , m_root(Q_NULLPTR)
    , m_rootId()
    , m_aspectManager(Q_NULLPTR)
    , m_jobManager(Q_NULLPTR)
    , m_arbiter(Q_NULLPTR)
    , m_useCloning(!qEnvironmentVariableIsSet("QT3D_NO_CLONE"))
{
}

QAbstractAspectPrivate *QAbstractAspectPrivate::get(QAbstractAspect *aspect)
{
    return aspect->d_func();
}

/*!
 *
 * Called in the context of the main thread
 */
void QAbstractAspectPrivate::onEngineAboutToShutdown()
{
}

/*!
    \class Qt3DCore::QAbstractAspect
    \inmodule Qt3DCore
    \brief QAbstractAspect is the base class for aspects that provide a vertical slice of behavior.
*/
QAbstractAspect::QAbstractAspect(QObject *parent)
    : QObject(*new QAbstractAspectPrivate, parent)
{
}
/*!
    \typedef Qt3DCore::QAspectJobPtr
    \relates Qt3DCore::QAbstractAspect

    A shared pointer for QAspectJob.
*/

/*!
    \typedef Qt3DCore::QBackendNodeMapperPtr
    \relates Qt3DCore::QAbstractAspect

    A shared pointer for QBackendNodeMapper.
*/

/*!
    \internal
*/
QAbstractAspect::QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

QNodeId QAbstractAspect::rootEntityId() const Q_DECL_NOEXCEPT
{
    Q_D(const QAbstractAspect);
    return d->m_rootId;
}

/*!
    Registers backend.
*/
void QAbstractAspect::registerBackendType(const QMetaObject &obj, const QBackendNodeMapperPtr &functor)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.insert(&obj, functor);
}

void QAbstractAspectPrivate::sceneNodeAdded(QSceneChangePtr &change)
{
    if (m_useCloning) {
        QScenePropertyChangePtr propertyChange = change.staticCast<QScenePropertyChange>();
        QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
        QNode *n = nodePtr.data();
        QNodeVisitor visitor;
        visitor.traverse(n, this, &QAbstractAspectPrivate::createBackendNode);
    } else {
        QNodeCreatedChangeBasePtr creationChange = qSharedPointerCast<QNodeCreatedChangeBase>(change);
        createBackendNodeNoClone(creationChange);
    }
}

void QAbstractAspectPrivate::sceneNodeRemoved(QSceneChangePtr &change)
{
    if (m_useCloning) {
        QScenePropertyChangePtr propertyChange = change.staticCast<QScenePropertyChange>();
        QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
        QNode *n = nodePtr.data();
        clearBackendNode(n);
    } else {
        QNodeDestroyedChangePtr destructionChange = qSharedPointerCast<QNodeDestroyedChange>(change);
        clearBackendNodeNoClone(destructionChange);
    }
}

QVariant QAbstractAspect::executeCommand(const QStringList &args)
{
    Q_UNUSED(args);
    return QVariant();
}

QVector<QAspectJobPtr> QAbstractAspect::jobsToExecute(qint64 time)
{
    Q_UNUSED(time);
    return QVector<QAspectJobPtr>();
}

QBackendNode *QAbstractAspectPrivate::createBackendNode(QNode *frontend) const
{
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeMapperPtr functor;
    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = m_backendCreatorFunctors.value(metaObj);
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull()) {
        QBackendNode *backend = functor->get(frontend->id());
        if (backend != Q_NULLPTR)
            return backend;
        backend = functor->create(frontend);
        // backend could be null if the user decides that his functor should only
        // perform some action when encountering a given type of item but doesn't need to
        // return a QBackendNode pointer.
        if (backend == Q_NULLPTR)
            return Q_NULLPTR;
        QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
        backendPriv->setEnabled(frontend->isEnabled());
        // TO DO: Find a way to specify the changes to observe
        // Register backendNode with QChangeArbiter
        if (m_arbiter != Q_NULLPTR) { // Unit tests may not have the arbiter registered
            m_arbiter->registerObserver(backendPriv, backend->peerId(), AllChanges);
            if (backend->mode() == QBackendNode::ReadWrite)
                m_arbiter->scene()->addObservable(backendPriv, backend->peerId());
        }
        return backend;
    }
    return Q_NULLPTR;
}

QBackendNode *QAbstractAspectPrivate::createBackendNodeNoClone(const QNodeCreatedChangeBasePtr &change) const
{
    const QMetaObject *metaObj = change->metaObject();
    QBackendNodeMapperPtr backendNodeMapper;
    while (metaObj != Q_NULLPTR && backendNodeMapper.isNull()) {
        backendNodeMapper = m_backendCreatorFunctors.value(metaObj);
        metaObj = metaObj->superClass();
    }

    if (!backendNodeMapper)
        return nullptr;

    QBackendNode *backend = backendNodeMapper->get(change->subjectId());
    if (backend != Q_NULLPTR)
        return backend;
    backend = backendNodeMapper->create(change);

    if (!backend)
        return nullptr;

    // TODO: Find some place else to do all of this function from the arbiter
    backend->setPeerId(change->subjectId());
    backend->initializeFromPeer(change);

    // Backend could be null if the user decides that his functor should only
    // perform some action when encountering a given type of item but doesn't need to
    // return a QBackendNode pointer.

    QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
    backendPriv->setEnabled(change->isNodeEnabled());
    // TO DO: Find a way to specify the changes to observe
    // Register backendNode with QChangeArbiter
    if (m_arbiter != Q_NULLPTR) { // Unit tests may not have the arbiter registered
        m_arbiter->registerObserver(backendPriv, backend->peerId(), AllChanges);
        if (backend->mode() == QBackendNode::ReadWrite)
            m_arbiter->scene()->addObservable(backendPriv, backend->peerId());
    }
    return backend;
}

void QAbstractAspectPrivate::clearBackendNode(QNode *frontend) const
{
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeMapperPtr functor;

    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = m_backendCreatorFunctors.value(metaObj);
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull()) {
        QBackendNode *backend = functor->get(frontend->id());
        if (backend != Q_NULLPTR) {
            QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
            m_arbiter->unregisterObserver(backendPriv, backend->peerId());
            if (backend->mode() == QBackendNode::ReadWrite)
                m_arbiter->scene()->removeObservable(backendPriv, backend->peerId());
            functor->destroy(frontend->id());
        }
    }
}

void QAbstractAspectPrivate::clearBackendNodeNoClone(const QNodeDestroyedChangePtr &change) const
{
    // Each QNodeDestroyedChange may contain info about a whole sub-tree of nodes that
    // are being destroyed. Iterate over them and process each in turn
    for (const auto &idAndType : change->subtreeIdsAndTypes()) {
        const QMetaObject *metaObj = idAndType.type;
        QBackendNodeMapperPtr backendNodeMapper;

        // Find backend node mapper for this type
        while (metaObj != Q_NULLPTR && backendNodeMapper.isNull()) {
            backendNodeMapper = m_backendCreatorFunctors.value(metaObj);
            metaObj = metaObj->superClass();
        }

        if (!backendNodeMapper) {
            qWarning() << "Failed to find backend node mapper for node id"
                       << idAndType.id << "of type" << idAndType.type->className();
            continue;
        }

        // Request the mapper to destroy the corresponding backend node
        QBackendNode *backend = backendNodeMapper->get(idAndType.id);
        if (backend) {
            QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
            m_arbiter->unregisterObserver(backendPriv, backend->peerId());
            if (backend->mode() == QBackendNode::ReadWrite)
                m_arbiter->scene()->removeObservable(backendPriv, backend->peerId());
            backendNodeMapper->destroy(idAndType.id);
        }
    }
}

void QAbstractAspectPrivate::setRootAndCreateNodes(QEntity *rootObject, const QVector<QNodeCreatedChangeBasePtr> &changes)
{
    qCDebug(Aspects) << Q_FUNC_INFO << "rootObject =" << rootObject;
    if (rootObject == m_root)
        return;

    m_root = rootObject;
    m_rootId = rootObject->id();

    // Use old method for now, unless user explicitly requests new method
    if (m_useCloning) {
        QNodeVisitor visitor;
        visitor.traverse(rootObject, this, &QAbstractAspectPrivate::createBackendNode);
    } else {
        for (const auto &change : changes)
            createBackendNodeNoClone(change);
    }
}

QServiceLocator *QAbstractAspectPrivate::services() const
{
    return m_aspectManager->serviceLocator();
}

QAbstractAspectJobManager *QAbstractAspectPrivate::jobManager() const
{
    return m_jobManager;
}

QVector<QAspectJobPtr> QAbstractAspectPrivate::jobsToExecute(qint64 time)
{
    Q_Q(QAbstractAspect);
    return q->jobsToExecute(time);
}

/*!
    Called in the context of the aspect thread once the aspect has been registered.
    This provides an opportunity for the aspect to do any initialization tasks that
    require to be in the aspect thread context such as creating QObject subclasses that
    must have affinity with this thread.

    \sa onUnregistered
*/
void QAbstractAspect::onRegistered()
{
}

/*!
    Called in the context of the aspect thread during unregistration

    of the aspect. This gives the aspect a chance to do any final pieces of
    cleanup that it would not do when just changing to a new scene.

    \sa onRegistered
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


} // of namespace Qt3DCore

QT_END_NAMESPACE
