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

QAspectEnginePrivate::QAspectEnginePrivate(QAspectEngine *qq)
    : QObjectPrivate()
    , m_postman(new QPostman())
    , m_scene(new QScene(qq))
{
    q_ptr = qq;
    m_postman->setScene(m_scene);
    qRegisterMetaType<Qt3D::QAbstractAspect *>();
    qRegisterMetaType<Qt3D::QObserverInterface *>();
    qRegisterMetaType<Qt3D::QEntity *>();
    qRegisterMetaType<Qt3D::QSceneInterface *>();
}

QAspectEngine::QAspectEngine(QObject *parent)
    : QObject(*new QAspectEnginePrivate(this), parent)
{
    // Don't show any debug output from Qt3D. If you need to enable additional logging
    // for debugging use a rules file as explained in the QLoggingCategory documentation.
    QLoggingCategory::setFilterRules(QString::fromUtf8("Qt3D.*.debug=false\n"));

    qCDebug(Aspects) << Q_FUNC_INFO;
    Q_D(QAspectEngine);
    d->m_aspectThread = new QAspectThread(this);
    d->m_aspectThread->waitForStart(QThread::HighestPriority);
}

QAspectEngine::QAspectEngine(QAspectEnginePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QAspectEngine);
    d->m_aspectThread = new QAspectThread(this);
    d->m_aspectThread->waitForStart(QThread::HighestPriority);
}

QAspectEngine::~QAspectEngine()
{
    shutdown();
}

void QAspectEngine::initNodeTree(QNode *node) const
{
    Q_D(const QAspectEngine);
    node->d_func()->setScene(d->m_scene);
    d->m_scene->addObservable(node);
    QEntity *entity = qobject_cast<QEntity *>(node);
    if (entity != Q_NULLPTR)
        Q_FOREACH (QComponent *comp, entity->components())
            d->m_scene->addEntityForComponent(comp->id(), entity->id());

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
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "registerAspect",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(Qt3D::QAbstractAspect *, aspect));
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

} // namespace Qt3D

QT_END_NAMESPACE
