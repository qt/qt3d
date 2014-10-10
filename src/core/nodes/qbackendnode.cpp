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

#include "qbackendnode.h"
#include "qbackendnode_p.h"
#include "qaspectengine.h"
#include "qchangearbiter_p.h"
#include "qnode.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QBackendNodePrivate::QBackendNodePrivate(QBackendNode *qq, QBackendNode::Mode mode)
    : QObservable()
    , q_ptr(qq)
    , m_mode(mode)
    , m_arbiter(Q_NULLPTR)
{
}

void QBackendNodePrivate::registerObserver(QObserverInterface *observer)
{
    Q_ASSERT(m_mode == QBackendNode::ReadWrite);
    m_arbiter = dynamic_cast<QChangeArbiter *>(observer);
}

void QBackendNodePrivate::unregisterObserver(QObserverInterface *observer)
{
    Q_ASSERT(m_mode == QBackendNode::ReadWrite);
    if (dynamic_cast<QChangeArbiter *>(observer) == m_arbiter)
        m_arbiter = Q_NULLPTR;
}

void QBackendNodePrivate::notifyObservers(const QSceneChangePtr &e)
{
    Q_ASSERT(m_mode == QBackendNode::ReadWrite);
    if (m_arbiter != Q_NULLPTR)
        m_arbiter->sceneChangeEventWithLock(e);
}

void QBackendNodePrivate::sceneChangeEvent(const QSceneChangePtr &e)
{
    q_ptr->sceneChangeEvent(e);
}

QBackendNodePrivate *QBackendNodePrivate::get(QBackendNode *n)
{
    return n->d_func();
}

QBackendNode::QBackendNode(QBackendNode::Mode mode)
    : d_ptr(new QBackendNodePrivate(this, mode))
{
}

QBackendNode::~QBackendNode()
{
    delete d_ptr;
}

void QBackendNode::setPeer(QNode *peer)
{
    Q_D(QBackendNode);
    QUuid peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    d->m_peerUuid = peerUuid;
    updateFromPeer(peer);
}

QUuid QBackendNode::peerUuid() const
{
    Q_D(const QBackendNode);
    return d->m_peerUuid;
}

QBackendNode::Mode QBackendNode::mode() const
{
    Q_D(const QBackendNode);
    return d->m_mode;
}

QBackendNode::QBackendNode(QBackendNodePrivate &dd)
    : d_ptr(&dd)
{
}

void QBackendNode::notifyObservers(const QSceneChangePtr &e)
{
    Q_D(QBackendNode);
    d->notifyObservers(e);
}

} // Qt3D

QT_END_NAMESPACE
