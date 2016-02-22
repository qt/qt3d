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

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAbstractAspectPrivate::QAbstractAspectPrivate()
    : QObjectPrivate()
    , m_root(Q_NULLPTR)
    , m_aspectManager(Q_NULLPTR)
    , m_jobManager(Q_NULLPTR)
    , m_arbiter(Q_NULLPTR)
{
}

QAbstractAspectPrivate *QAbstractAspectPrivate::get(QAbstractAspect *aspect)
{
    return aspect->d_func();
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
/*!
    Registers backend.
*/
void QAbstractAspect::registerBackendType(const QMetaObject &obj, const QBackendNodeMapperPtr &functor)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.insert(&obj, functor);
}

void QAbstractAspectPrivate::sceneNodeAdded(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
    QNode *n = nodePtr.data();
    QNodeVisitor visitor;
    visitor.traverse(n, this, &QAbstractAspectPrivate::createBackendNode);
}

void QAbstractAspectPrivate::sceneNodeRemoved(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
    QNode *n = nodePtr.data();
    clearBackendNode(n);
}

QVariant QAbstractAspect::executeCommand(const QStringList &args)
{
    Q_UNUSED(args);
    return QVariant();
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
        // TO DO: Find a way to specify the changes to observe
        // Register backendNode with QChangeArbiter
        if (m_arbiter != Q_NULLPTR) { // Unit tests may not have the arbiter registered
            m_arbiter->registerObserver(backendPriv, backend->peerUuid(), AllChanges);
            if (backend->mode() == QBackendNode::ReadWrite)
                m_arbiter->scene()->addObservable(backendPriv, backend->peerUuid());
        }
        return backend;
    }
    return Q_NULLPTR;
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
            m_arbiter->unregisterObserver(backendPriv, backend->peerUuid());
            if (backend->mode() == QBackendNode::ReadWrite)
                m_arbiter->scene()->removeObservable(backendPriv, backend->peerUuid());
            functor->destroy(frontend->id());
        }
    }
}

void QAbstractAspectPrivate::registerAspect(QEntity *rootObject)
{
    Q_Q(QAbstractAspect);
    if (rootObject == m_root)
        return;

    m_root = rootObject;

    QNodeVisitor visitor;
    visitor.traverse(rootObject, this, &QAbstractAspectPrivate::createBackendNode);
    q->onRootEntityChanged(rootObject);
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
 *
 * Called in the QAspectThread context
 */
void QAbstractAspect::onStartup()
{
}

/*!
 *
 * Called in the QAspectThread context
 */
void QAbstractAspect::onShutdown()
{
}

void QAbstractAspect::onRootEntityChanged(QEntity *rootEntity)
{
    Q_UNUSED(rootEntity);
}

} // of namespace Qt3DCore

QT_END_NAMESPACE
