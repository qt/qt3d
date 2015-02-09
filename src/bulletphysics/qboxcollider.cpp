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

#include "qboxcollider.h"
#include "qboxcollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace BulletPhysics {

QBoxColliderPrivate::QBoxColliderPrivate(QBoxCollider *qq)
    : QAbstractColliderPrivate(qq)
    , m_center()
    , m_halfExtents()
{
}

void QBoxCollider::copy(const QNode *ref)
{
    QAbstractCollider::copy(ref);
    const QBoxCollider *other = static_cast<const QBoxCollider*>(ref);
    d_func()->m_center = other->d_func()->m_center;
    d_func()->m_halfExtents = other->d_func()->m_halfExtents;
}

QBoxCollider::QBoxCollider(Qt3D::QNode *parent)
    : QAbstractCollider(*new QBoxColliderPrivate(this), parent)
{
}

QBoxCollider::QBoxCollider(QBoxColliderPrivate &dd, QNode *parent)
    : QAbstractCollider(dd, parent)
{
}

void QBoxCollider::setCenter(const QVector3D &center)
{
    Q_D(QBoxCollider);
    if (!qFuzzyCompare(d->m_center, center)) {
        setCenterX(center.x());
        setCenterY(center.y());
        setCenterZ(center.z());
        emit centerChanged();
    }
}

QVector3D QBoxCollider::center() const
{
    Q_D(const QBoxCollider);
    return d->m_center;
}

void QBoxCollider::setHalfExtents(const QVector3D &halfExtents)
{
    Q_D(QBoxCollider);
    if (d->m_halfExtents != halfExtents) {
        d->m_halfExtents = halfExtents;
        emit halfExtentsChanged();
    }
}

QVector3D QBoxCollider::halfExtents() const
{
    Q_D(const QBoxCollider);
    return d->m_halfExtents;
}

void QBoxCollider::setCenterX(float x)
{
    Q_D(QBoxCollider);
    if (!qFuzzyCompare(d->m_center.x(), x)) {
        d->m_center.setX(x);
        emit centerXChanged();
    }
}

float QBoxCollider::centerX() const
{
    Q_D(const QBoxCollider);
    return d->m_center.x();
}

void QBoxCollider::setCenterY(float y)
{
    Q_D(QBoxCollider);
    if (!qFuzzyCompare(d->m_center.y(), y)) {
        d->m_center.setY(y);
        emit centerYChanged();
    }
}

float QBoxCollider::centerY() const
{
    Q_D(const QBoxCollider);
    return d->m_center.y();
}

void QBoxCollider::setCenterZ(float z)
{
    Q_D(QBoxCollider);
    if (!qFuzzyCompare(d->m_center.z(), z)) {
        d->m_center.setZ(z);
        emit centerZChanged();
    }
}

float QBoxCollider::centerZ() const
{
    Q_D(const QBoxCollider);
    return d->m_center.z();
}

} // namespace BulletPhysics
} // namespace Qt3D

QT_END_NAMESPACE
