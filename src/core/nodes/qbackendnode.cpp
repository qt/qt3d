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

#include "qbackendnode.h"
#include "qbackendnode_p.h"
#include "qaspectengine.h"
#include "qnode.h"
#include "qscenepropertychange.h"
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QBackendNodePrivate::QBackendNodePrivate(QBackendNode::Mode mode)
    : q_ptr(Q_NULLPTR)
    , m_mode(mode)
    , m_arbiter(Q_NULLPTR)
    , m_enabled(false)
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

void QBackendNodePrivate::setEnabled(bool enabled)
{
    m_enabled = enabled;
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

void QBackendNode::setPeer(QNode *peer)
{
    Q_D(QBackendNode);
    QNodeId peerId;
    if (peer != Q_NULLPTR)
        peerId = peer->id();
    d->m_peerId = peerId;
    d->m_enabled = peer->isEnabled();
    updateFromPeer(peer);
}

void QBackendNode::setPeerId(QNodeId id) Q_DECL_NOEXCEPT
{
    Q_D(QBackendNode);
    d->m_peerId = id;
}

QNodeId QBackendNode::peerId() const Q_DECL_NOEXCEPT
{
    Q_D(const QBackendNode);
    return d->m_peerId;
}

bool QBackendNode::isEnabled() const Q_DECL_NOEXCEPT
{
    Q_D(const QBackendNode);
    return d->m_enabled;
}

QBackendNode::Mode QBackendNode::mode() const Q_DECL_NOEXCEPT
{
    Q_D(const QBackendNode);
    return d->m_mode;
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

void QBackendNode::initializeFromPeer(const QNodeCreatedChangeBasePtr &change)
{
    Q_UNUSED(change);
    qCDebug(Nodes) << Q_FUNC_INFO << change->metaObject()->className() << "does not override";
}

void QBackendNode::setEnabled(bool enabled)
{
    Q_D(QBackendNode);
    d->m_enabled = enabled;
}

void QBackendNode::sceneChangeEvent(const QSceneChangePtr &e)
{
    Q_D(QBackendNode);
    auto propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->type()) {
        case NodeUpdated: {
            if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
                d->m_enabled = propertyChange->value().value<bool>();
            break;
        }
        default:
            break;
    }
}

} // Qt3D

QT_END_NAMESPACE
