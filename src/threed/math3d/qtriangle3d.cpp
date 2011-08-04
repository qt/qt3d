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

#include "qtriangle3d.h"
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qnumeric.h>

QT_BEGIN_NAMESPACE

/*!
    \class QTriangle3D
    \brief The QTriangle3D class represents a triangle as three points in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::math

    A triangle is defined by 3 points in 3D space.  Since any 3 points define
    a plane, the triangle can be thought of as defining a plane, and forming a
    geometric region in that plane.

    If you need a simple plane, with no particular geometry, then
    QPlane3D is a more compact and mathematically sufficient class.

    The three points are labelled p(), q() and r() for consistency with
    textbook treatments.  It is recommended that the points be supplied
    in counter-clockwise order for correct orientation of the
    triangle's plane().

    \sa QPlane3D
*/

/*!
    \fn QTriangle3D::QTriangle3D()

    Constructs a default triangle which lies in the x-z plane,
    with the three vertices (0, 0, 0), (1, 0, 0), and (0, 1, 0).
*/

/*!
    \fn QTriangle3D::QTriangle3D(const QVector3D &p, const QVector3D &q, const QVector3D &r)

    Constructs a triangle with the supplied \a p, \a q and \a r vertices.
*/

/*!
    \fn QVector3D QTriangle3D::p() const

    Returns the value of the P vertex on the triangle.

    \sa q(), r(), setP()
*/

/*!
    \fn void QTriangle3D::setP(const QVector3D &point)

    Sets the value of the P vertex on the triangle to \a point.

    \sa setQ(), setR(), p()
*/

/*!
    \fn QVector3D QTriangle3D::q() const

    Returns the value of the Q vertex on the triangle.

    \sa p(), r(), setQ()
*/

/*!
    \fn void QTriangle3D::setQ(const QVector3D &point)

    Sets the value of the Q vertex on the triangle \a point.

    \sa setP(), setR(), q()
*/


/*!
    \fn QVector3D QTriangle3D::r() const

    Returns the value of the R vertex on the triangle.

    \sa p(), q(), setR()
*/

/*!
    \fn void QTriangle3D::setR(const QVector3D &point)

    Sets the value of the R vertex on the triangle \a point.

    \sa setP(), setQ(), r()
*/

/*!
    \fn QPlane3D QTriangle3D::plane() const

    Returns the plane in which the triangle lies.

    \sa QPlane3D
*/

/*!
    \fn QVector3D QTriangle3D::center() const

    Returns the center of the triangle, which is the geometric average of the
    three vertices.
*/

/*!
    \fn QVector3D QTriangle3D::faceNormal() const

    Returns the vector normal to this triangle, computed from the
    cross-product of P-Q and Q-R.  The result is not normalized.
*/

/*!
    Returns true if this triangle contains \a point; false otherwise.
    To contain the \a point means that:
    \list
    \o the point lies on the same plane as the triangle, and
    \o the point
        \list
        \o lies either wholly within the triangle, or
        \o lies on one of the sides, or
        \o coincides with one of the 3 vertices
        \endlist
    \endlist

    \sa intersects()
*/
bool QTriangle3D::contains(const QVector3D &point) const
{
    // Check if the point is on the triangle's plane first.
    QVector3D normal = QVector3D::crossProduct(m_q - m_p, m_r - m_q);
    if (!qFuzzyIsNull(float(QVector3D::dotProduct(normal, m_p - point))))
        return false;

    // Compute the barycentric co-ordinates and use them to determine
    // if the point is within the triangle.
    QVector2D c = uv(point);
    if (c.x() < 0.0f || c.x() > 1.0f)
        return false;
    if (c.y() < 0.0f || c.y() > 1.0f)
        return false;
    if ((c.x() + c.y()) > 1.0f)
        return false;
    return true;
}

/*!
    Returns true if the \a ray intersects this triangle; false otherwise.

    This function will return false if the triangle is degenerate.

    \sa contains(), intersection()
*/
bool QTriangle3D::intersects(const QRay3D &ray) const
{
    qreal t = plane().intersection(ray);
    if (qIsNaN(t))
        return false;
    return contains(ray.point(t));
}

/*!
    Returns the t value at which \a ray intersects this triangle, or
    not-a-number if there is no intersection.

    When the \a ray intersects this triangle, the return value is a
    parametric value that can be passed to QRay3D::point() to determine
    the actual intersection point, as shown in the following example:

    \code
    qreal t = triangle.intersection(ray);
    QVector3D pt;
    if (qIsNaN(t)) {
        qWarning("no intersection occurred");
    else
        pt = ray.point(t);
    \endcode

    \sa intersects(), contains(), QRay3D::point()
 */
qreal QTriangle3D::intersection(const QRay3D &ray) const
{
    qreal t = plane().intersection(ray);
    if (qIsNaN(t) || contains(ray.point(t)))
        return t;
    return qSNaN();
}

/*!
    Transforms the points of this triangle according to \a matrix.

    \sa transformed()
*/
void QTriangle3D::transform(const QMatrix4x4 &matrix)
{
    m_p = matrix * m_p;
    m_q = matrix * m_q;
    m_r = matrix * m_r;
}

/*!
    Returns a new triangle that results from transforming this
    one using \a matrix.

    \sa transform()
*/
QTriangle3D QTriangle3D::transformed(const QMatrix4x4 &matrix) const
{
    return QTriangle3D(matrix * m_p, matrix * m_q, matrix * m_r);
}

/*!
    Returns the (u, v) barycentric co-ordinates of \a point within
    this triangle.

    The returned barycentric co-ordinates will be (1, 0) at p(),
    (0, 1) at q(), and (0, 0) at r().  Technically, barycentric
    co-ordinates have three components with the corners at
    (1, 0, 0), (0, 1, 0), and (0, 0, 1).  However, the third
    component is always equal to (1 - u - v) so we do not return it.

    The typical use case for this function is to convert an intersection
    point on a triangle into the texture co-ordinate corresponding to
    that point.  If \c p, \c q, and \c r are the points on the triangle,
    with corresponding texture co-ordinates \c tp, \c tq, and \c tr,
    then the texture co-ordinate \c tc of \a point can be determined
    by the following code:

    \code
    QTriangle3D triangle(p, q, r);
    QVector2D uv = triangle.uv(point);
    QVector2D tc = uv.x() * tp + uv.y() * tq + (1 - uv.x() - uv.y()) * tr;
    \endcode

    \sa contains(), intersection()
*/
QVector2D QTriangle3D::uv(const QVector3D &point) const
{
    // Algorithm from: http://www.blackpawn.com/texts/pointinpoly/default.html
    // More: http://en.wikipedia.org/wiki/Barycentric_coordinates_(mathematics)
    QVector3D rq = m_q - m_r;
    QVector3D rp = m_p - m_r;
    QVector3D pp = point - m_r;
    qreal dot_rq_rq = QVector3D::dotProduct(rq, rq);
    qreal dot_rq_rp = QVector3D::dotProduct(rq, rp);
    qreal dot_rq_pp = QVector3D::dotProduct(rq, pp);
    qreal dot_rp_rp = QVector3D::dotProduct(rp, rp);
    qreal dot_rp_pp = QVector3D::dotProduct(rp, pp);
    qreal det = dot_rq_rq * dot_rp_rp - dot_rq_rp * dot_rq_rp;
    if (qFuzzyIsNull(float(det))) {
        // The point is probably not in the triangle, or the triangle
        // is degenerate.  Return an out of range value for (u, v) so
        // that contains() will fail when this case happens.
        return QVector2D(-1.0f, -1.0f);
    }
    return QVector2D((dot_rq_rq * dot_rp_pp - dot_rq_rp * dot_rq_pp) / det,
                     (dot_rp_rp * dot_rq_pp - dot_rq_rp * dot_rp_pp) / det);
}

/*!
    \fn bool QTriangle3D::operator==(const QTriangle3D &other)

    Returns true if this triangle is the same as \a other; false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QTriangle3D::operator!=(const QTriangle3D &other)

    Returns true if this triangle is not the same as \a other; false otherwise.

    \sa operator==()
*/

/*!
    \fn bool qFuzzyCompare(const QTriangle3D &triangle1, const QTriangle3D &triangle2)
    \relates QTriangle3D

    Returns true if \a triangle1 and \a triangle2 are almost equal;
    false otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QTriangle3D &triangle)
{
    dbg.nospace() << "QTriangle3D(("
        << triangle.p().x() << ", " << triangle.p().y() << ", "
        << triangle.p().z() << "), ("
        << triangle.q().x() << ", " << triangle.q().y() << ", "
        << triangle.q().z() << "), ("
        << triangle.r().x() << ", " << triangle.r().y() << ", "
        << triangle.r().z() << "))";
    return dbg.space();
}

#endif

#ifndef QT_NO_DATASTREAM

/*!
    \fn QDataStream &operator<<(QDataStream &stream, const QTriangle3D &triangle)
    \relates QTriangle3D

    Writes the given \a triangle to the given \a stream and returns a
    reference to the stream.
*/

QDataStream &operator<<(QDataStream &stream, const QTriangle3D &triangle)
{
    stream << triangle.p();
    stream << triangle.q();
    stream << triangle.r();
    return stream;
}

/*!
    \fn QDataStream &operator>>(QDataStream &stream, QTriangle3D &triangle)
    \relates QTriangle3D

    Reads a 3D triangle from the given \a stream into the given \a triangle
    and returns a reference to the stream.
*/

QDataStream &operator>>(QDataStream &stream, QTriangle3D &triangle)
{
    QVector3D p, q, r;
    stream >> p;
    stream >> q;
    stream >> r;
    triangle = QTriangle3D(p, q, r);
    return stream;
}

#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE
