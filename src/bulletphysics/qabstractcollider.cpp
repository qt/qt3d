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

#include "qabstractcollider.h"
#include "qabstractcollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace BulletPhysics {

QAbstractColliderPrivate::QAbstractColliderPrivate(QAbstractCollider *qq)
    : Qt3D::QComponentPrivate(qq)
    , m_collisionBehavior(QAbstractCollider::NotifyOnly)
{
}

void QAbstractCollider::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QAbstractCollider *other = static_cast<const QAbstractCollider *>(ref);
    d_func()->m_collisionBehavior = other->d_func()->m_collisionBehavior;
}

QAbstractCollider::QAbstractCollider(QNode *parent)
    : Qt3D::QComponent(*new QAbstractColliderPrivate(this), parent)
{
}

QAbstractCollider::QAbstractCollider(QAbstractColliderPrivate &dd, Qt3D::QNode *parent)
    : Qt3D::QComponent(dd, parent)
{
}


void QAbstractCollider::setCollisionBehavior(CollisionBehavior collisionBehavior)
{
    Q_D(QAbstractCollider);
    if (d->m_collisionBehavior != collisionBehavior) {
        d->m_collisionBehavior = collisionBehavior;
        emit collisionBehaviorChanged();
    }
}

QAbstractCollider::CollisionBehavior QAbstractCollider::collisionBehavior() const
{
    Q_D(const QAbstractCollider);
    return d->m_collisionBehavior;
}

} // namespace BulletPhysics
} // namespace Qt3D

QT_END_NAMESPACE
