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

#include "qabstractaspect.h"
#include "qentity.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <private/qabstractaspect_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <private/qchangearbiter_p.h>
#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

static QByteArray className(const QMetaObject &obj)
{
    // note: class names are stored in static meta objects, thus the usage of fromRawData here is fine
    return QByteArray::fromRawData(obj.className(), int(strlen(obj.className())));
}

namespace Qt3D {

/*!
    \class Qt3D::QAbstractAspectPrivate
    \internal
*/
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
    \class Qt3D::QAbstractAspect
    \inmodule Qt3DCore
    \brief QAbstractAspect is the base class for aspects that provide a vertical slice of behavior.
*/
QAbstractAspect::QAbstractAspect(AspectType aspectType, QObject *parent)
    : QObject(*new QAbstractAspectPrivate, parent)
{
    Q_D(QAbstractAspect);
    d->m_aspectType = aspectType;
}

/*! \internal */
QAbstractAspect::QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

void QAbstractAspect::registerBackendType(const QMetaObject &obj, const QBackendNodeFunctorPtr &functor)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.insert(className(obj), functor);
}

QBackendNode *QAbstractAspect::createBackendNode(QNode *frontend) const
{
    Q_D(const QAbstractAspect);
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeFunctorPtr functor;
    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = d->m_backendCreatorFunctors.value(className(*metaObj));
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull()) {
        QBackendNode *backend = functor->get(frontend->id());
        if (backend != Q_NULLPTR)
            return backend;
        backend = functor->create(frontend, this);
        // backend could be null if the user decides that his functor should only
        // perform some action when encountering a given type of item but doesn't need to
        // return a QBackendNode pointer.
        if (backend == Q_NULLPTR)
            return Q_NULLPTR;
        // Register backendNode with QChangeArbiter
        QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
        // TO DO: Find a way to specify the changes to observe
        d->m_arbiter->registerObserver(backendPriv, backend->peerUuid(), AllChanges);
        if (backend->mode() == QBackendNode::ReadWrite)
            d->m_arbiter->scene()->addObservable(backendPriv, backend->peerUuid());
        return backend;
    }
    return Q_NULLPTR;
}

QBackendNode *QAbstractAspect::getBackendNode(QNode *frontend) const
{
    Q_D(const QAbstractAspect);
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeFunctorPtr functor;

    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = d->m_backendCreatorFunctors.value(className(*metaObj));
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull())
        return functor->get(frontend->id());
    return Q_NULLPTR;
}

void QAbstractAspect::clearBackendNode(QNode *frontend) const
{
    Q_D(const QAbstractAspect);
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeFunctorPtr functor;

    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = d->m_backendCreatorFunctors.value(className(*metaObj));
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull()) {
        QBackendNode *backend = functor->get(frontend->id());
        if (backend != Q_NULLPTR) {
            QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
            d->m_arbiter->unregisterObserver(backendPriv, backend->peerUuid());
            if (backend->mode() == QBackendNode::ReadWrite)
                d->m_arbiter->scene()->removeObservable(backendPriv, backend->peerUuid());
            functor->destroy(frontend->id());
        }
    }
}

QAbstractAspect::AspectType QAbstractAspect::aspectType() const
{
    Q_D(const QAbstractAspect);
    return d->m_aspectType;
}

void QAbstractAspect::registerAspect(QEntity *rootObject)
{
    Q_D(QAbstractAspect);
    if (rootObject == d->m_root)
        return;

    d->m_root = rootObject;
    setRootEntity(rootObject);
}

QServiceLocator *QAbstractAspect::services() const
{
    Q_D(const QAbstractAspect);
    return d->m_aspectManager->serviceLocator();
}

QAbstractAspectJobManager *QAbstractAspect::jobManager() const
{
    Q_D(const QAbstractAspect);
    return d->m_jobManager;
}

bool QAbstractAspect::isShuttingDown() const
{
    Q_D(const QAbstractAspect);
    return d->m_aspectManager->isShuttingDown();
}

void QAbstractAspect::onStartup()
{
}

void QAbstractAspect::onShutdown()
{
}

} // of namespace Qt3D

QT_END_NAMESPACE
