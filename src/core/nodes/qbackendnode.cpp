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

#include "qbackendnode.h"
#include "qbackendnode_p.h"
#include "qbackendnodefactory.h"
#include "qaspectengine.h"
#include "qnode.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QBackendNodePrivate
    \internal
*/
QBackendNodePrivate::QBackendNodePrivate(QBackendNode::Mode mode)
    : q_ptr(Q_NULLPTR)
    , m_mode(mode)
    , m_factory(Q_NULLPTR)
    , m_arbiter(Q_NULLPTR)
{
}

// Called by backend thread (renderer or other) while we are locked to sync changes
void QBackendNodePrivate::setArbiter(QLockableObserverInterface *arbiter)
{
    Q_ASSERT(m_mode == QBackendNode::ReadWrite);
    m_arbiter = arbiter;
}

// Called by backend thread/worker threads. We don't need locking
// as setting/unsetting the arbiter cannot happen at that time
void QBackendNodePrivate::notifyObservers(const QSceneChangePtr &e)
{
    Q_ASSERT(m_mode == QBackendNode::ReadWrite);
    if (m_arbiter != Q_NULLPTR)
        m_arbiter->sceneChangeEvent(e);
}

void QBackendNodePrivate::sceneChangeEvent(const QSceneChangePtr &e)
{
    q_func()->sceneChangeEvent(e);
}

QBackendNodePrivate *QBackendNodePrivate::get(QBackendNode *n)
{
    return n->d_func();
}

QBackendNode::QBackendNode(QBackendNode::Mode mode)
    : d_ptr(new QBackendNodePrivate(mode))
{
    d_ptr->q_ptr = this;
}

QBackendNode::~QBackendNode()
{
    delete d_ptr;
}

void QBackendNode::setFactory(const QBackendNodeFactory *factory)
{
    Q_D(QBackendNode);
    d->m_factory = factory;
}

void QBackendNode::setPeer(QNode *peer)
{
    Q_D(QBackendNode);
    QNodeId peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->id();
    d->m_peerUuid = peerUuid;
    updateFromPeer(peer);
}

QNodeId QBackendNode::peerUuid() const
{
    Q_D(const QBackendNode);
    return d->m_peerUuid;
}

QBackendNode::Mode QBackendNode::mode() const
{
    Q_D(const QBackendNode);
    return d->m_mode;
}

QBackendNode *QBackendNode::createBackendNode(QNode *frontend) const
{
    Q_D(const QBackendNode);
    Q_ASSERT(d->m_factory);
    return d->m_factory->createBackendNode(frontend);
}

/*! \internal */
QBackendNode::QBackendNode(QBackendNodePrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

void QBackendNode::notifyObservers(const QSceneChangePtr &e)
{
    Q_D(QBackendNode);
    d->notifyObservers(e);
}

} // Qt3D

QT_END_NAMESPACE
