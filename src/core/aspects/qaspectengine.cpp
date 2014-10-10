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
#include "qaspectmanager.h"
#include "qchangearbiter_p.h"
#include "qabstractaspect.h"
#include "qnode.h"
#include "corelogging.h"
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
    , m_scene(new QScene())
{
    q_ptr = qq;
    m_postman->setScene(m_scene);
    qRegisterMetaType<Qt3D::QAbstractAspect *>();
    qRegisterMetaType<Qt3D::QObserverInterface *>();
    qRegisterMetaType<Qt3D::QNode *>();
    qRegisterMetaType<Qt3D::QSceneInterface *>();
}

QAspectEngine::QAspectEngine(QObject *parent)
    : QObject(*new QAspectEnginePrivate(this), parent)
{
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

void QAspectEngine::initNodeTree(QNode *node) const
{
    Q_D(const QAspectEngine);
    node->d_func()->setScene(d->m_scene);
    d->m_scene->addObservable(node);
    QEntity *entity = qobject_cast<QEntity *>(node);
    if (entity != Q_NULLPTR)
        Q_FOREACH (QComponent *comp, entity->components())
            d->m_scene->addEntityForComponent(comp->uuid(), entity->uuid());

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
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "quit");
    d->m_aspectThread->wait();
}

void QAspectEngine::setWindow(QWindow *window)
{
    Q_D(QAspectEngine);
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "setWindow",
                              Q_ARG(QWindow *, window));
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

void QAspectEngine::setRoot(QNode *rootObject)
{
    Q_D(QAspectEngine);
    qCDebug(Aspects) << "Setting scene root on aspect manager";
    initNodeTree(rootObject);
    QMetaObject::invokeMethod(d->m_aspectThread->aspectManager(),
                              "setRoot",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(Qt3D::QNode *, rootObject));
    qCDebug(Aspects) << "Done setting scene root on aspect manager";
}

} // namespace Qt3D

QT_END_NAMESPACE
