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

#include "qaspectengine.h"

#include "qabstractaspect.h"
#include "qaspectthread.h"
#include "qaspectmanager_p.h"
#include "qchangearbiter_p.h"
#include "qabstractaspect.h"
#include "qnode.h"
#include <Qt3DCore/private/corelogging_p.h>
#include <QMetaObject>
#include <private/qpostman_p.h>
#include "qscene.h"
#include <private/qaspectengine_p.h>
#include <private/qnode_p.h>
#include "qentity.h"
#include "qcomponent.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAspectEnginePrivate
    \internal
*/
QAspectEnginePrivate::QAspectEnginePrivate()
    : QObjectPrivate()
    , m_postman(Q_NULLPTR)
    , m_scene(Q_NULLPTR)
{
    qRegisterMetaType<Qt3D::QAbstractAspect *>();
    qRegisterMetaType<Qt3D::QObserverInterface *>();
    qRegisterMetaType<Qt3D::QEntity *>();
    qRegisterMetaType<Qt3D::QSceneInterface *>();
}

QAspectEngine::QAspectEngine(QObject *parent)
    : QObject(*new QAspectEnginePrivate, parent)
{
    // Don't show any debug output from Qt3D. If you need to enable additional logging
    // for debugging use a rules file as explained in the QLoggingCategory documentation.
    QLoggingCategory::setFilterRules(QString::fromUtf8("Qt3D.*.debug=false\n"));

    qCDebug(Aspects) << Q_FUNC_INFO;
    Q_D(QAspectEngine);
    d->m_scene = new QScene(this);
    d->m_postman = new QPostman(this);
    d->m_postman->setScene(d->m_scene);
    d->m_aspectThread = new QAspectThread(this);
    d->m_aspectThread->waitForStart(QThread::HighestPriority);
}

/*! \internal */
QAspectEngine::QAspectEngine(QAspectEnginePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QAspectEngine);
    d->m_scene = new QScene(this);
    d->m_postman = new QPostman(this);
    d->m_postman->setScene(d->m_scene);
    d->m_aspectThread = new QAspectThread(this);
    d->m_aspectThread->waitForStart(QThread::HighestPriority);
}

QAspectEngine::~QAspectEngine()
{
    Q_D(QAspectEngine);
    shutdown();
    delete d->m_aspectThread;
    delete d->m_postman;
    delete d->m_scene;
}

void QAspectEngine::initNodeTree(QNode *node) const
{
    Q_D(const QAspectEngine);
    node->d_func()->setScene(d->m_scene);
    d->m_scene->addObservable(node);
    QEntity *entity = qobject_cast<QEntity *>(node);
    if (entity != Q_NULLPTR)
        Q_FOREACH (QComponent *comp, entity->components()) {
            if (!comp->shareable() && !d->m_scene->entitiesForComponent(comp->id()).isEmpty())
                qWarning() << "Trying to assign a non shareable component to more than one Entity";
            d->m_scene->addEntityForComponent(comp->id(), entity->id());
        }

    Q_FOREACH (QObject *c, node->children()) {
        QNode *childNode = qobject_cast<QNode *>(c);
        if (childNode != Q_NULLPTR)
            initNodeTree(childNode);
    }
}

void QAspectEngine::initialize()
{
    Q_D(QAspectEngine);
    QChangeArbiter *arbiter = d->m_aspectThread->aspectManager()->changeArbiter();
    d->m_scene->setArbiter(arbiter);
    QChangeArbiter::createUnmanagedThreadLocalChangeQueue(arbiter);
    QMetaObject::invokeMethod(arbiter,
                              "setPostman",
                              Q_ARG(Qt3D::QObserverInterface *, d->m_postman));
    QMetaObject::invokeMethod(arbiter,
                              "setScene",
                              Q_ARG(Qt3D::QSceneInterface *, d->m_scene));
}

void QAspectEngine::shutdown()
{
    Q_D(QAspectEngine);
    qCDebug(Aspects) << Q_FUNC_INFO;

    // Cleanup the scene before quitting the backend
    setRootEntity(Q_NULLPTR);
    d->m_scene->setArbiter(Q_NULLPTR);
    QChangeArbiter *arbiter = d->m_aspectThread->aspectManager()->changeArbiter();
    QChangeArbiter::destroyUnmanagedThreadLocalChangeQueue(arbiter);
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "quit");
    d->m_aspectThread->wait();
}

void QAspectEngine::setData(const QVariantMap &data)
{
    Q_D(QAspectEngine);
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "setData",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(const QVariantMap &, data));
}

/*!
 * Registers a new \a aspect to the AspectManager.
 * Passing as a QObject* as abstracts like AbstractAspect
 * cannot be registered as a meta type.
 */
void QAspectEngine::registerAspect(QAbstractAspect *aspect)
{
    Q_D(QAspectEngine);
    aspect->moveToThread(d->m_aspectThread);
    d->m_aspects << aspect;
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "registerAspect",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(Qt3D::QAbstractAspect *, aspect));
}

/*!
 * Registers a new aspect to the AspectManager based on its \a name
 * Uses the currently set aspect factory to create the actual aspect
 * instance.
 */
void QAspectEngine::registerAspect(const QString &name)
{
    Q_D(QAspectEngine);
    QAbstractAspect *aspect = d->m_factory.createAspect(name);
    if (aspect)
        registerAspect(aspect);
}

QList<QAbstractAspect *> QAspectEngine::aspects() const
{
    Q_D(const QAspectEngine);
    return d->m_aspects;
}

void QAspectEngine::setRootEntity(QEntity *root)
{
    qCDebug(Aspects) << "Setting scene root on aspect manager";
    Q_D(QAspectEngine);
    if (d->m_root == root)
        return;

    // Set the new root object. This will cause the old tree to be deleted
    // and the deletion of the old frontend tree will cause the backends to
    // free any related resources
    d->m_root.reset(root);

    // Do we actually have a new scene?
    if (!d->m_root)
        return;

    // The aspect engine takes ownership of the scene root. We also set the
    // parent of the scene root to be the engine
    d->m_root->setParent(this);

    // Prepare the frontend tree for use by giving each node a pointer to the
    // scene object and adding each node to the scene
    // TODO: We probably need a call symmetric to this one above in order to
    // deregister the nodes from the scene
    initNodeTree(root);

    // Finally, tell the aspects about the new scene object tree. This is done
    // in a blocking manner to allow the backends to get synchronized before the
    // main thread starts triggering potentially more notifications
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "setRootEntity",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(Qt3D::QEntity *, root));
    qCDebug(Aspects) << "Done setting scene root on aspect manager";
}

QSharedPointer<QEntity> QAspectEngine::rootEntity() const
{
    Q_D(const QAspectEngine);
    return d->m_root;
}

QAspectFactory QAspectEngine::aspectFactory() const
{
    Q_D(const QAspectEngine);
    return d->m_factory;
}

void QAspectEngine::setAspectFactory(const QAspectFactory &factory)
{
    Q_D(QAspectEngine);
    d->m_factory = factory;
}

} // namespace Qt3D

QT_END_NAMESPACE
