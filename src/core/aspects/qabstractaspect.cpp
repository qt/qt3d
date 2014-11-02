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

#include "qabstractaspect.h"
#include "qentity.h"
#include <private/qabstractaspect_p.h>
#include <Qt3DCore/qaspectjobmanager.h>
#include <private/qchangearbiter_p.h>
#include <Qt3DCore/qsceneinterface.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAbstractAspectPrivate::QAbstractAspectPrivate(QAbstractAspect *qq)
    : QObjectPrivate()
    , m_root(Q_NULLPTR)
    , m_jobManager(Q_NULLPTR)
    , m_arbiter(Q_NULLPTR)
{
    q_ptr = qq;
}

QAbstractAspectPrivate *QAbstractAspectPrivate::get(QAbstractAspect *aspect)
{
    return aspect->d_func();
}

QAbstractAspect::QAbstractAspect(AspectType aspectType, QObject *parent)
    : QObject(*new QAbstractAspectPrivate(this), parent)
{
    Q_D(QAbstractAspect);
    d->m_aspectType = aspectType;
}

QAbstractAspect::QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

void QAbstractAspect::registerBackendType(const QMetaObject &obj, const QBackendNodeFunctorPtr &functor)
{
    Q_D(QAbstractAspect);
    d->m_backendCreatorFunctors.insert(QString::fromLatin1(obj.className()), functor);
}

QBackendNode *QAbstractAspect::createBackendNode(QNode *frontend) const
{
    Q_D(const QAbstractAspect);
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeFunctorPtr functor;
    while (metaObj != Q_NULLPTR && functor.isNull()) {
                functor = d->m_backendCreatorFunctors.value(QString::fromLatin1(metaObj->className()));
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull()) {
        QBackendNode *backend = functor->get(frontend);
        if (backend != Q_NULLPTR)
            return backend;
        backend = functor->create(frontend);
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
    }
    return Q_NULLPTR;
}

QBackendNode *QAbstractAspect::getBackendNode(QNode *frontend) const
{
    Q_D(const QAbstractAspect);
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeFunctorPtr functor;

    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = d->m_backendCreatorFunctors.value(QString::fromLatin1(metaObj->className()));
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull())
        return functor->get(frontend);
    return Q_NULLPTR;
}

void QAbstractAspect::clearBackendNode(QNode *frontend) const
{
    Q_D(const QAbstractAspect);
    const QMetaObject *metaObj = frontend->metaObject();
    QBackendNodeFunctorPtr functor;

    while (metaObj != Q_NULLPTR && functor.isNull()) {
        functor = d->m_backendCreatorFunctors.value(QString::fromLatin1(metaObj->className()));
        metaObj = metaObj->superClass();
    }
    if (!functor.isNull()) {
        QBackendNode *backend = functor->get(frontend);
        if (backend != Q_NULLPTR) {
            QBackendNodePrivate *backendPriv = QBackendNodePrivate::get(backend);
            d->m_arbiter->unregisterObserver(backendPriv, backend->peerUuid());
            if (backend->mode() == QBackendNode::ReadWrite)
                d->m_arbiter->scene()->removeObservable(backendPriv, backend->peerUuid());
            functor->destroy(frontend);
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

QAbstractAspectJobManager *QAbstractAspect::jobManager() const
{
    Q_D(const QAbstractAspect);
    return d->m_jobManager;
}

} // of namespace Qt3D

QT_END_NAMESPACE
