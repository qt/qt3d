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

#include "qboxcollider.h"
#include "qboxcollider_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QBoxColliderPrivate
    \internal
*/
QBoxColliderPrivate::QBoxColliderPrivate()
    : QAbstractColliderPrivate()
    , m_center()
    , m_radii(0.5f, 0.5f, 0.5f) // Unit cube
{
}

/*!
    \class Qt3D::QBoxCollider
    \inmodule Qt3DCollision
    \since 5.5
    \brief Represents a box used for collision detection
*/

/*!
    \qmltype BoxCollider
    \inqmlmodule Qt3D.Collision
    \instantiates Qt3D::QBoxCollider
    \inherits Component3D
    \since 5.5
*/

/*!
    Constructs a new QBoxCollider instance with parent \a parent.
 */
QBoxCollider::QBoxCollider(QNode *parent)
    : QAbstractCollider(*new QBoxColliderPrivate, parent)
{
}

/*! \internal */
QBoxCollider::QBoxCollider(QBoxColliderPrivate &dd, QNode *parent)
    : QAbstractCollider(dd, parent)
{
}

QBoxCollider::~QBoxCollider()
{
    QNode::cleanup();
}

QVector3D QBoxCollider::center() const
{
    Q_D(const QBoxCollider);
    return d->m_center;
}

QVector3D QBoxCollider::radii() const
{
    Q_D(const QBoxCollider);
    return d->m_radii;
}

void QBoxCollider::setCenter(const QVector3D &center)
{
    Q_D(QBoxCollider);
    if (d->m_center == center)
        return;

    d->m_center = center;
    emit centerChanged(center);
}

void QBoxCollider::setRadii(const QVector3D &radii)
{
    Q_D(QBoxCollider);
    if (d->m_radii == radii)
        return;

    d->m_radii = radii;
    emit radiiChanged(radii);
}

QT_END_NAMESPACE

} // namespace Qt3D

