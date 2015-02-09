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

#include "qrigidbody.h"
#include "qrigidbody_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace BulletPhysics {

QRigidBodyPrivate::QRigidBodyPrivate(QRigidBody *qq)
    : Qt3D::QComponentPrivate(qq)
    , m_mass(0.0f)
    , m_centerOfMass()
{
}


QRigidBody::QRigidBody(Qt3D::QNode *parent)
    : Qt3D::QComponent(*new QRigidBodyPrivate(this), parent)
{
}

QRigidBody::QRigidBody(QRigidBodyPrivate &dd, Qt3D::QNode *parent)
    : Qt3D::QComponent(dd, parent)
{
}

void QRigidBody::copy(const QNode *ref)
{
    QRigidBody::copy(ref);
    const QRigidBody *other = static_cast<const QRigidBody*>(ref);
    d_func()->m_mass = other->d_func()->m_mass;
    d_func()->m_centerOfMass = other->d_func()->m_centerOfMass;
}

void QRigidBody::setMass(float mass)
{
    Q_D(QRigidBody);
    if (!qFuzzyCompare(d->m_mass, mass)) {
        d->m_mass = mass;
        emit massChanged();
    }
}

float QRigidBody::mass() const
{
    Q_D(const QRigidBody);
    return d->m_mass;
}

void QRigidBody::setCenterOfMass(const QVector3D &centerOfMass)
{
    Q_D(QRigidBody);
    if (!qFuzzyCompare(d->m_centerOfMass,centerOfMass)) {
        setCenterOfMassX(centerOfMass.x());
        setCenterOfMassY(centerOfMass.y());
        setCenterOfMassZ(centerOfMass.z());
        emit centerOfMassChanged();
    }
}

QVector3D QRigidBody::centerOfMass() const
{
    Q_D(const QRigidBody);
    return d->m_centerOfMass;
}

void QRigidBody::setCenterOfMassX(float x)
{
    Q_D(QRigidBody);
    if (!qFuzzyCompare(d->m_centerOfMass.x(), x)) {
        d->m_centerOfMass.setX(x);
        emit centerOfMassXChanged();
    }
}

float QRigidBody::centerOfMassX() const
{
    Q_D(const QRigidBody);
    return d->m_centerOfMass.x();
}

void QRigidBody::setCenterOfMassY(float y)
{
    Q_D(QRigidBody);
    if (!qFuzzyCompare(d->m_centerOfMass.y(), y)) {
        d->m_centerOfMass.setY(y);
        emit centerOfMassYChanged();
    }
}

float QRigidBody::centerOfMassY() const
{
    Q_D(const QRigidBody);
    return d->m_centerOfMass.y();
}

void QRigidBody::setCenterOfMassZ(float z)
{
    Q_D(QRigidBody);
    if (!qFuzzyCompare(d->m_centerOfMass.z(), z)) {
        d->m_centerOfMass.setZ(z);
        emit centerOfMassZChanged();
    }
}

float QRigidBody::centerOfMassZ() const
{
    Q_D(const QRigidBody);
    return d->m_centerOfMass.z();
}

} // namespace BulletPhysics
} // namespace Qt3D

QT_END_NAMESPACE
