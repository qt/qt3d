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

#include "qspherecollider.h"
#include "qspherecollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QSphereColliderPrivate
    \internal
*/
QSphereColliderPrivate::QSphereColliderPrivate()
    : QAbstractColliderPrivate()
    , m_center()
    , m_radius(1.0f)
{
}

/*!
    \class Qt3D::QSphereCollider
    \inmodule Qt3DCollision
    \since 5.5
    \brief Represents a sphere used for collision detection
*/

/*!
    \qmltype SphereCollider
    \inqmlmodule Qt3D.Collision
    \instantiates Qt3D::QSphereCollider
    \inherits Component3D
    \since 5.5
*/

/*!
    Constructs a new QSphereCollider instance with parent \a parent.
 */
QSphereCollider::QSphereCollider(QNode *parent)
    : QAbstractCollider(*new QSphereColliderPrivate, parent)
{
}

/*! \internal */
QSphereCollider::QSphereCollider(QSphereColliderPrivate &dd, QNode *parent)
    : QAbstractCollider(dd, parent)
{
}

QSphereCollider::~QSphereCollider()
{
    QNode::cleanup();
}

QVector3D QSphereCollider::center() const
{
    Q_D(const QSphereCollider);
    return d->m_center;
}

float QSphereCollider::radius() const
{
    Q_D(const QSphereCollider);
    return d->m_radius;
}

void QSphereCollider::setCenter(const QVector3D &center)
{
    Q_D(QSphereCollider);
    if (d->m_center == center)
        return;

    d->m_center = center;
    emit centerChanged(center);
}

void QSphereCollider::setRadius(float radius)
{
    Q_D(QSphereCollider);
    if (d->m_radius == radius)
        return;

    d->m_radius = radius;
    emit radiusChanged(radius);
}

}

QT_END_NAMESPACE
