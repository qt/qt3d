/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qcapsulecollider.h"
#include "qcapsulecollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QCapsuleColliderPrivate
    \internal
*/
QCapsuleColliderPrivate::QCapsuleColliderPrivate()
    : QAbstractColliderPrivate()
    , m_center()
    , m_radius(1.0f)
    , m_length(1.0f)
    , m_axisDirection(QCapsuleCollider::YAxis)
{
}

/*!
    \class Qt3D::QCapsuleCollider
    \inmodule Qt3DCollision
    \since 5.5
    \brief Represents a capsule used for collision detection
*/

/*!
    \qmltype CapsuleCollider
    \inqmlmodule Qt3D.Collision
    \instantiates Qt3D::QCapsuleCollider
    \inherits Component3D
    \since 5.5
*/

/*!
    Constructs a new QCapsuleCollider instance with parent \a parent.
 */
QCapsuleCollider::QCapsuleCollider(QNode *parent)
    : QAbstractCollider(*new QCapsuleColliderPrivate, parent)
{
}

/*! \internal */
QCapsuleCollider::QCapsuleCollider(QCapsuleColliderPrivate &dd, QNode *parent)
    : QAbstractCollider(dd, parent)
{
}

QCapsuleCollider::~QCapsuleCollider()
{
    QNode::cleanup();
}

QVector3D QCapsuleCollider::center() const
{
    Q_D(const QCapsuleCollider);
    return d->m_center;
}

float QCapsuleCollider::radius() const
{
    Q_D(const QCapsuleCollider);
    return d->m_radius;
}

float QCapsuleCollider::length() const
{
    Q_D(const QCapsuleCollider);
    return d->m_length;
}

QCapsuleCollider::Direction QCapsuleCollider::axisDirection() const
{
    Q_D(const QCapsuleCollider);
    return d->m_axisDirection;
}

void QCapsuleCollider::setCenter(const QVector3D &center)
{
    Q_D(QCapsuleCollider);
    if (d->m_center == center)
        return;

    d->m_center = center;
    emit centerChanged(center);
}

void QCapsuleCollider::setRadius(float radius)
{
    Q_D(QCapsuleCollider);
    if (d->m_radius == radius)
        return;

    d->m_radius = radius;
    emit radiusChanged(radius);
}

void QCapsuleCollider::setLength(float length)
{
    Q_D(QCapsuleCollider);
    if (d->m_length == length)
        return;

    d->m_length = length;
    emit lengthChanged(length);
}

void QCapsuleCollider::setAxisDirection(Qt3D::QCapsuleCollider::Direction axisDirection)
{
    Q_D(QCapsuleCollider);
    if (d->m_axisDirection == axisDirection)
        return;

    d->m_axisDirection = axisDirection;
    emit axisDirectionChanged(axisDirection);
}

QT_END_NAMESPACE

} // namespace Qt3D

