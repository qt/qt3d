/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsphere3d.h"
#include "qray3d.h"
#include "qbox3d.h"
#include "qplane3d.h"
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QSphere3D
    \brief The QSphere3D class represents a mathematical sphere in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::math

    QSphere3D can be used to represent the bounding regions of objects
    in a 3D scene so that they can be easily culled if they are out of view.
    It can also be used to assist with collision testing.

    \sa QBox3D
*/

/*!
    \fn QSphere3D::QSphere3D()

    Constructs a default sphere with a center() of (0, 0, 0)
    and radius() of 1.
*/

/*!
    \fn QSphere3D::QSphere3D(const QVector3D &center, qreal radius)

    Constructs a sphere with the specified \a center and \a radius.
*/

/*!
    \fn QVector3D QSphere3D::center() const

    Returns the center of this sphere.

    \sa setCenter(), radius()
*/

/*!
    \fn void QSphere3D::setCenter(const QVector3D &center)

    Sets the \a center of this sphere.

    \sa center(), setRadius()
*/

/*!
    \fn qreal QSphere3D::radius() const

    Returns the radius of this sphere.

    \sa setRadius(), center()
*/

/*!
    \fn void QSphere3D::setRadius(qreal radius)

    Sets the \a radius of this sphere.

    \sa radius(), setCenter()
*/

/*!
    \fn bool QSphere3D::contains(const QVector3D &point) const

    Returns true if \a point is contained within the bounds of
    this sphere; false otherwise.
*/

/*!
    Returns true if this sphere intersects \a ray; false otherwise.

    \sa intersection()
*/
bool QSphere3D::intersects(const QRay3D &ray) const
{
    QVector3D centerToOrigin = ray.origin() - m_center;
    qreal term1 = ray.direction().lengthSquared();
    qreal term2 = 2.0f * QVector3D::dotProduct(centerToOrigin, ray.direction());
    qreal term3 = centerToOrigin.lengthSquared() - m_radius * m_radius;
    qreal det = term2 * term2 - (4.0f * term1 * term3);
    return term1 != 0.0f && det >= 0.0f;
}

/*!
    \fn bool QSphere3D::intersects(const QSphere3D &sphere) const

    Returns true if this sphere intersects \a sphere; false otherwise.

    \sa contains()
*/

/*!
    Returns true if this sphere intersects \a box; false otherwise.
*/
bool QSphere3D::intersects(const QBox3D &box) const
{
    if (box.isFinite()) {
        // Use Arvo's Algorithm to determine if we have an intersection.
        qreal dist = 0.0f;
        qreal center = m_center.x();
        qreal minval = box.minimum().x();
        qreal maxval = box.maximum().x();
        if (center < minval)
            dist += (center - minval) * (center - minval);
        else if (center > maxval)
            dist += (center - maxval) * (center - maxval);
        center = m_center.y();
        minval = box.minimum().y();
        maxval = box.maximum().y();
        if (center < minval)
            dist += (center - minval) * (center - minval);
        else if (center > maxval)
            dist += (center - maxval) * (center - maxval);
        center = m_center.z();
        minval = box.minimum().z();
        maxval = box.maximum().z();
        if (center < minval)
            dist += (center - minval) * (center - minval);
        else if (center > maxval)
            dist += (center - maxval) * (center - maxval);
        return dist <= (m_radius * m_radius);
    } else {
        return box.isInfinite();
    }
}

/*!
    Returns true if this sphere intersects \a plane; false otherwise.
*/
bool QSphere3D::intersects(const QPlane3D &plane) const
{
    return qAbs(plane.distanceTo(m_center)) <= m_radius;
}

/*!
    Finds the \a minimum_t and \a maximum_t values where \a ray intersects
    this sphere.  Returns true if intersections were found; or false if there
    is no intersection.

    If \a minimum_t and \a maximum_t are set to the same value, then \a ray
    touches the surface of the sphere at a single point.  If the t values are
    negative, then the intersection occurs before the ray's origin point
    in the reverse direction of the ray.

    The \a minimum_t and \a maximum_t values can be passed to QRay3D::point()
    to determine the actual intersection points, as shown in the following
    example:

    \code
    qreal minimum_t, maximum_t;
    if (sphere.intersection(ray, &minimum_t, &maximum_t)) {
        qDebug() << "intersections at"
                 << ray.point(minimum_t) << "and"
                 << ray.point(maximum_t);
    }
    \endcode

    \sa intersects(), QRay3D::point()
*/
bool QSphere3D::intersection(const QRay3D &ray, qreal *minimum_t, qreal *maximum_t) const
{
    QVector3D centerToOrigin = ray.origin() - m_center;
    qreal term1 = ray.direction().lengthSquared();
    qreal term2 = 2.0f * QVector3D::dotProduct(centerToOrigin, ray.direction());
    qreal term3 = centerToOrigin.lengthSquared() - m_radius * m_radius;
    qreal det = term2 * term2 - (4.0f * term1 * term3);
    if (term1 == 0.0f || det < 0.0f) {
        *minimum_t = qSNaN();
        *maximum_t = qSNaN();
        return false;
    } else if (det == 0.0f) {
        *minimum_t = *maximum_t = -term2 / (2.0f * term1);
    } else {
        qreal sqrtDet = qSqrt(det);
        qreal t1 = (-term2 - sqrtDet) / (2.0f * term1);
        qreal t2 = (-term2 + sqrtDet) / (2.0f * term1);
        if (t1 < t2) {
            *minimum_t = t1;
            *maximum_t = t2;
        } else {
            *minimum_t = t2;
            *maximum_t = t1;
        }
    }
    return true;
}

/*!
    Returns the t value at which \a ray first intersects the surface of
    this sphere, or not-a-number if there is no intersection.

    When the \a ray intersects this sphere, the return value is a
    parametric value that can be passed to QRay3D::point() to determine
    the actual intersection point, as shown in the following example:

    \code
    qreal t = sphere.intersection(ray);
    QVector3D pt;
    if (qIsNaN(t)) {
        qWarning("no intersection occurred");
    else
        pt = ray.point(t);
    \endcode

    The \a ray might intersect at two points - as the ray passes through
    the sphere - one on the near side, one on the far side; where near and far
    are relative to the origin point of the ray.  This function only
    returns the near intersection point.

    Only positive values on the ray are considered.  This means that if
    the origin point of the ray is inside the sphere, there is only one
    solution, not two.  To get the other solution, simply change
    the sign of the ray's direction vector.  If the origin point of
    the ray is outside the sphere, and the direction points away from
    the sphere, then there will be no intersection.

    When the ray does not intersect the sphere in the positive direction,
    then the return value is not-a-number.

    \sa intersects(), QRay3D::point()
*/
qreal QSphere3D::intersection(const QRay3D &ray) const
{
    qreal minimum_t, maximum_t;
    if (intersection(ray, &minimum_t, &maximum_t)) {
        if (minimum_t >= 0.0f)
            return minimum_t;
        else if (maximum_t >= 0.0f)
            return maximum_t;
        else
            return qSNaN();
    } else {
        return qSNaN();
    }
}

/*!
    \fn void QSphere3D::transform(const QMatrix4x4 &matrix)

    Transforms this sphere's center() and radius() according to \a matrix.

    It is assumed that \a matrix contains a uniform scale factor in the
    x, y, and z directions.  Otherwise the radius() in the result is undefined.

    \sa transformed()
*/

/*!
    Returns the result of transforming this sphere's center() and radius()
    according to \a matrix.

    It is assumed that \a matrix contains a uniform scale factor in the
    x, y, and z directions.  Otherwise the radius() in the result is undefined.

    \sa transform()
*/
QSphere3D QSphere3D::transformed(const QMatrix4x4 &matrix) const
{
    return QSphere3D(matrix * m_center,
                     matrix.mapVector(QVector3D(m_radius, 0, 0)).length());
}

/*!
    \fn bool QSphere3D::operator==(const QSphere3D &sphere) const

    Returns true if this sphere is the same as \a sphere; false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QSphere3D::operator!=(const QSphere3D &sphere) const

    Returns true if this sphere is not the same as \a sphere; false otherwise.

    \sa operator==()
*/

/*!
    \fn bool qFuzzyCompare(const QSphere3D &sphere1, const QSphere3D &sphere2)
    \relates QSphere3D

    Returns true if \a sphere1 and \a sphere2 are almost equal;
    false otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QSphere3D &sphere)
{
    dbg.nospace() << "QSphere3D(center=("
        << sphere.center().x() << ", " << sphere.center().y() << ", "
        << sphere.center().z() << "), radius=" << sphere.radius() << ')';
    return dbg.space();
}

#endif

#ifndef QT_NO_DATASTREAM

/*!
    \fn QDataStream &operator<<(QDataStream &stream, const QSphere3D &sphere)
    \relates QSphere3D

    Writes the given \a sphere to the given \a stream and returns a
    reference to the stream.
*/

QDataStream &operator<<(QDataStream &stream, const QSphere3D &sphere)
{
    stream << sphere.center();
    stream << double(sphere.radius());
    return stream;
}

/*!
    \fn QDataStream &operator>>(QDataStream &stream, QSphere3D &sphere)
    \relates QSphere3D

    Reads a 3D sphere from the given \a stream into the given \a sphere
    and returns a reference to the stream.
*/

QDataStream &operator>>(QDataStream &stream, QSphere3D &sphere)
{
    QVector3D center;
    double radius;
    stream >> center;
    stream >> radius;
    sphere.setCenter(center);
    sphere.setRadius(qreal(radius));
    return stream;
}

#endif

QT_END_NAMESPACE
