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

#include "qscene_p.h"
#include <QHash>
#include <QReadLocker>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qlockableobserverinterface_p.h>
#include <Qt3DCore/private/qobservableinterface_p.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QScenePrivate
{
public:
    QScenePrivate(QAspectEngine *engine)
        : m_engine(engine)
        , m_arbiter(Q_NULLPTR)
    {
    }

    QAspectEngine *m_engine;
    QHash<QNodeId, QNode *> m_nodeLookupTable;
    QMultiHash<QNodeId, QNodeId> m_componentToEntities;
    QMultiHash<QNodeId, QObservableInterface *> m_observablesLookupTable;
    QHash<QObservableInterface *, QNodeId> m_observableToUuid;
    QLockableObserverInterface *m_arbiter;
    QHash<QNodeId, QNode *> m_clonesLookupTable;
    mutable QReadWriteLock m_lock;
};


QScene::QScene(QAspectEngine *engine)
    : d_ptr(new QScenePrivate(engine))
{
}

QScene::~QScene()
{
}

QAspectEngine *QScene::engine() const
{
    Q_D(const QScene);
    return d->m_engine;
}

// Called by any thread
void QScene::addObservable(QObservableInterface *observable, const QNodeId &id)
{
    Q_D(QScene);
    QWriteLocker lock(&d->m_lock);
    d->m_observablesLookupTable.insert(id, observable);
    d->m_observableToUuid.insert(observable, id);
    if (d->m_arbiter != Q_NULLPTR)
        observable->setArbiter(d->m_arbiter);
}

// Called by main thread only
void QScene::addObservable(QNode *observable)
{
    Q_D(QScene);
    if (observable != Q_NULLPTR) {
        QWriteLocker lock(&d->m_lock);
        d->m_nodeLookupTable.insert(observable->id(), observable);
        if (d->m_arbiter != Q_NULLPTR)
            observable->d_func()->setArbiter(d->m_arbiter);
    }
}

// Called by any thread
void QScene::removeObservable(QObservableInterface *observable, const QNodeId &id)
{
    Q_D(QScene);
    QWriteLocker lock(&d->m_lock);
    d->m_observablesLookupTable.remove(id, observable);
    d->m_observableToUuid.remove(observable);
    observable->setArbiter(Q_NULLPTR);
}

// Called by main thread
void QScene::removeObservable(QNode *observable)
{
    Q_D(QScene);
    if (observable != Q_NULLPTR) {
        QWriteLocker lock(&d->m_lock);
        QNodeId nodeUuid = observable->id();
        QObservableList observables = d->m_observablesLookupTable.values(nodeUuid);
        Q_FOREACH (QObservableInterface *o, observables) {
            o->setArbiter(Q_NULLPTR);
            d->m_observableToUuid.remove(o);
        }
        d->m_observablesLookupTable.remove(nodeUuid);
        d->m_nodeLookupTable.remove(nodeUuid);
        observable->d_func()->setArbiter(Q_NULLPTR);
    }
}

// Called by any thread
QObservableList QScene::lookupObservables(const QNodeId &id) const
{
    Q_D(const QScene);
    QReadLocker lock(&d->m_lock);
    return d->m_observablesLookupTable.values(id);
}

// Called by any thread
QNode *QScene::lookupNode(const QNodeId &id) const
{
    Q_D(const QScene);
    QReadLocker lock(&d->m_lock);
    return d->m_nodeLookupTable.value(id);
}

QVector<QNode *> QScene::lookupNodes(const QVector<QNodeId> &ids) const
{
    Q_D(const QScene);
    QReadLocker lock(&d->m_lock);
    QVector<QNode *> nodes(ids.size());
    int index = 0;
    Q_FOREACH (const QNodeId &id, ids)
        nodes[index++] = d->m_nodeLookupTable.value(id);
    return nodes;
}

QNodeId QScene::nodeIdFromObservable(QObservableInterface *observable) const
{
    Q_D(const QScene);
    QReadLocker lock(&d->m_lock);
    return d->m_observableToUuid.value(observable);
}

void QScene::setArbiter(QLockableObserverInterface *arbiter)
{
    Q_D(QScene);
    d->m_arbiter = arbiter;
}

QLockableObserverInterface *QScene::arbiter() const
{
    Q_D(const QScene);
    return d->m_arbiter;
}

QList<QNodeId> QScene::entitiesForComponent(const QNodeId &id) const
{
    Q_D(const QScene);
    QReadLocker lock(&d->m_lock);
    return d->m_componentToEntities.values(id);
}

void QScene::addEntityForComponent(const QNodeId &componentUuid, const QNodeId &entityUuid)
{
    Q_D(QScene);
    QWriteLocker lock(&d->m_lock);
    d->m_componentToEntities.insert(componentUuid, entityUuid);
}

void QScene::removeEntityForComponent(const QNodeId &componentUuid, const QNodeId &entityUuid)
{
    Q_D(QScene);
    QWriteLocker lock(&d->m_lock);
    d->m_componentToEntities.remove(componentUuid, entityUuid);
}

bool QScene::hasEntityForComponent(const QNodeId &componentUuid, const QNodeId &entityUuid)
{
    Q_D(QScene);
    QReadLocker lock(&d->m_lock);
    return d->m_componentToEntities.values(componentUuid).contains(entityUuid);
}

} // Qt3D

QT_END_NAMESPACE
