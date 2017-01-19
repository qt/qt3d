/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qboundingsphere.h"
#include "qboundingsphere_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QBoundingSpherePrivate::QBoundingSpherePrivate()
    : QObjectPrivate()
    , m_radius(1.f)
{

}

/*!
    \class Qt3DRender::QBoundingSphere
    \inmodule Qt3DRender
    \since 5.9
    \brief The QBoundingSphere class provides a simple spherical volume, defined by it's center and radius.
*/

/*!
    \qmltype LevelOfDetail
    \instantiates Qt3DRender::QLevelOfDetail
    \inherits Component3D
    \inqmlmodule Qt3D.Render
    \brief The BoundingSphere class provides a simple spherical volume, defined by it's center and radius.
*/

/*!
 * \qmlproperty QVector3D BoundingSphere::center
 *
 * Specifies the center of the bounding sphere
 */

/*!
 * \property QBoundingSphere::center
 *
 * Specifies the center of the bounding sphere
 */

/*!
 * \qmlproperty qreal BoundingSphere::radius
 *
 * Specifies the radius of the bounding sphere
 */

/*!
 * \property QBoundingSphere::radius
 *
 * Specifies the radius of the bounding sphere
 */

/*! \fn Qt3DRender::QBoundingSphere::QBoundingSphere(QObject *parent)
  Constructs a new QBoundingSphere with the specified \a parent.
 */
QBoundingSphere::QBoundingSphere(QObject *parent)
    : QObject(*new QBoundingSpherePrivate, parent)
{

}

QBoundingSphere::QBoundingSphere(const QVector3D &center, float radius, QObject *parent)
    : QBoundingSphere(parent)
{
    Q_D(QBoundingSphere);
    d->m_center = center;
    d->m_radius = radius;
}

QVector3D QBoundingSphere::center() const
{
    Q_D(const QBoundingSphere);
    return d->m_center;
}

float QBoundingSphere::radius() const
{
    Q_D(const QBoundingSphere);
    return d->m_radius;
}

/*!
 * Sets the radius of the bounding sphere.
 */
void QBoundingSphere::setRadius(float radius)
{
    Q_D(QBoundingSphere);
    if (d->m_radius != radius) {
        d->m_radius = radius;
        emit radiusChanged(radius);
    }
}

/*!
 * Sets the center of the bounding sphere.
 */
void QBoundingSphere::setCenter(const QVector3D &center)
{
    Q_D(QBoundingSphere);
    if (d->m_center != center) {
        d->m_center = center;
        emit centerChanged(center);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
