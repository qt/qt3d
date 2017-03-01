/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qabstractclipblendnode.h"
#include "qabstractclipblendnode_p.h"
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DAnimation/qabstractanimationclip.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QAbstractClipBlendNodePrivate::QAbstractClipBlendNodePrivate()
    : Qt3DCore::QNodePrivate()
{
}

QAbstractClipBlendNode::QAbstractClipBlendNode(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QAbstractClipBlendNodePrivate(), parent)
{
}

QAbstractClipBlendNode::QAbstractClipBlendNode(QAbstractClipBlendNodePrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QAbstractClipBlendNode::~QAbstractClipBlendNode()
{
}

void QAbstractClipBlendNode::addClip(QAbstractAnimationClip *clip)
{
    Q_D(QAbstractClipBlendNode);
    if (!d->m_clips.contains(clip)) {
        d->m_clips.push_back(clip);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(clip, &QAbstractClipBlendNode::removeClip, d->m_clips);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!clip->parent())
            clip->setParent(this);

        if (d->m_changeArbiter != nullptr) {
            const auto change = Qt3DCore::QPropertyNodeAddedChangePtr::create(id(), clip);
            change->setPropertyName("clip");
            d->notifyObservers(change);
        }
    }
}

void QAbstractClipBlendNode::removeClip(QAbstractAnimationClip *clip)
{
    Q_D(QAbstractClipBlendNode);
    if (d->m_changeArbiter != nullptr) {
        const auto change = Qt3DCore::QPropertyNodeRemovedChangePtr::create(id(), clip);
        change->setPropertyName("clip");
        d->notifyObservers(change);
    }
    d->m_clips.removeOne(clip);
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(clip);
}

QVector<QAbstractAnimationClip *> QAbstractClipBlendNode::clips() const
{
    Q_D(const QAbstractClipBlendNode);
    return d->m_clips;
}

QAbstractClipBlendNode *QAbstractClipBlendNode::parentClipBlendNode() const
{
    QAbstractClipBlendNode *parentBlendClipNode = nullptr;
    QNode *parentN = parentNode();
    while (parentN != nullptr) {
        parentBlendClipNode = qobject_cast<QAbstractClipBlendNode *>(parentN);
        if (parentBlendClipNode != nullptr)
            break;
        parentN = parentN->parentNode();
    }
    return parentBlendClipNode;
}

} // Qt3DAnimation

QT_END_NAMESPACE
