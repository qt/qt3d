// Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/qray3d_p.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

/*!
    \namespace Qt3DRender::RayCasting
    \internal
*/

/*!
    \internal
    \class Qt3DRender::RayCasting::QRay3D
    \inmodule Qt3DRender
    \brief The QRay3D class defines a directional line in 3D space extending through an origin point.
    \since 5.5
    \ingroup qt3d
    \ingroup qt3d::math

    A ray is defined by the origin() point and the direction() vector.
    Rays are infinite in length, extending out from origin() in
    both directions.  If the direction() is zero length, then the
    behavior of the class is undefined.

    A ray can be thought of as a one-dimensional co-ordinate system.
    If the co-ordinate is \b t then the origin() point is at
    \b t = 0, the point origin() + direction() is at \b t = 1,
    and the point origin() - direction() is at \b t = -1.
    The point() method can be used to obtain the position of a point
    within this one-dimensional co-ordinate system. The projectedDistance()
    method can be used to convert a point into a value in this
    one-dimensional co-ordinate system.
*/

/*!
    \fn Qt3DRender::RayCasting::QRay3D::QRay3D()

    Construct a default ray with an origin() of (0, 0, 0), a
    direction() of (0, 0, 1) and a distance of 1.
*/
QRay3D::QRay3D()
    : m_direction(0.0f, 0.0f, 1.0f)
    , m_distance(1.0f)
{
}

/*!
    \fn Qt3DRender::RayCasting::QRay3D::QRay3D(const Vector3D &origin, const Vector3D &direction, float distance)

    Construct a ray given its defining \a origin, \a direction and \a distance.
    The \a direction does not need to be normalized.

    To construct a ray that passes through two points, use the following:

    \code
    QRay3D thruAB(pointA, pointB - pointA);
    \endcode
*/
QRay3D::QRay3D(const Vector3D &origin, const Vector3D &direction, float distance)
    : m_origin(origin)
    , m_direction(direction.normalized())
    , m_distance(distance)
{}

QRay3D::~QRay3D()
{
}

/*!
    \fn QVector3D Qt3DRender::RayCasting::QRay3D::origin() const

    Returns the origin of this ray.  The default value is (0, 0, 0).

    \sa setOrigin(), direction()
*/
Vector3D QRay3D::origin() const
{
    return m_origin;
}

/*!
    \fn void Qt3DRender::RayCasting::QRay3D::setOrigin(const Vector3D &value)

    Sets the origin point of this ray to \a value.

    \sa origin(), setDirection()
 */
void QRay3D::setOrigin(const Vector3D &value)
{
    m_origin = value;
}

/*!
    \fn QVector3D Qt3DRender::RayCasting::QRay3D::direction() const

    Returns the direction vector of this ray.  The default value is (0, 0, 1).

    \sa setDirection(), origin()
*/
Vector3D QRay3D::direction() const
{
    return m_direction;
}

/*!
    \fn void Qt3DRender::RayCasting::QRay3D::setDirection(const Vector3D &direction)

    Sets the direction vector of this ray to \a direction.

    \sa direction(), setOrigin()
*/
void QRay3D::setDirection(const Vector3D &value)
{
    if (value.isNull())
        return;

    m_direction = value.normalized();
}

float QRay3D::distance() const
{
    return m_distance;
}

void QRay3D::setDistance(float distance)
{
    m_distance = distance;
}

Vector3D QRay3D::point(float t) const
{
    return m_origin + t * m_direction;
}

QRay3D &QRay3D::transform(const Matrix4x4 &matrix)
{
    Vector3D pointAtDistance = point(distance());
    m_origin = matrix.map(m_origin);
    pointAtDistance = matrix.map(pointAtDistance);
    m_direction = (pointAtDistance - m_origin).normalized();
    m_distance = (pointAtDistance - m_origin).length();

    return *this;
}

QRay3D QRay3D::transformed(const Matrix4x4 &matrix) const
{
    QRay3D cpy(m_origin, m_direction, m_distance);
    return cpy.transform(matrix);
}

bool QRay3D::operator==(const QRay3D &other) const
{
    return m_origin == other.origin() && m_direction == other.direction();
}

bool QRay3D::operator!=(const QRay3D &other) const
{
    return !(*this == other);
}

/*!
    Returns \c true if \a point lies on this ray; \c false otherwise.
*/
bool QRay3D::contains(const Vector3D &point) const
{
    Vector3D  ppVec(point - m_origin);
    if (ppVec.isNull()) // point coincides with origin
        return true;
    const float dot = Vector3D ::dotProduct(ppVec, m_direction);
    if (qFuzzyIsNull(dot))
        return false;
    return qFuzzyCompare(dot*dot, ppVec.lengthSquared() * m_direction.lengthSquared());
}

/*!
    Returns \c true if \a ray lies on this ray; \c false otherwise. If true,
    this implies that the two rays are actually the same, but with
    different origin() points or an inverted direction().
*/
bool QRay3D::contains(const QRay3D &ray) const
{
    const float dot = Vector3D ::dotProduct(m_direction, ray.direction());
    if (!qFuzzyCompare(dot*dot, m_direction.lengthSquared() * ray.direction().lengthSquared()))
        return false;
    return contains(ray.origin());
}

/*!
    \fn QVector3D Qt3DRender::RayCasting::QRay3D::point(float t) const

    Returns the point on the ray defined by moving \a t units
    along the ray in the direction of the direction() vector.
    Note that \a t may be negative in which case the point returned
    will lie behind the origin() point with respect to the
    direction() vector.

    The units for \a t are defined by direction().  The return value
    is precisely origin() + t * direction().

    \sa projectedDistance(), distance()
*/

/*!
    Returns the number of direction() units along the ray from origin()
    to \a point.  Essentially, this function computes the value t, where
    \a point = origin() + t * direction().  If \a point is not on the ray,
    then the closest point that is on the ray will be used instead.

    If the return value is positive, then \a point lies in front of
    the origin() with respect to the direction() vector.  If the return
    value is negative, then \a point lies behind the origin() with
    respect to the direction() vector.

    \sa point(), project()
*/
float QRay3D::projectedDistance(const Vector3D  &point) const
{
    Q_ASSERT(!m_direction.isNull());

    return Vector3D ::dotProduct(point - m_origin, m_direction) /
                m_direction.lengthSquared();
}

/*!
    Returns the projection of \a vector onto this ray.  In the
    following diagram, the dotted line is the ray, and V is the
    \a vector.  The return value will be the vector V':

    \image qray3d-project.png

    \sa projectedDistance()
*/
Vector3D  QRay3D::project(const Vector3D &vector) const
{
    Vector3D  norm = m_direction.normalized();
    return Vector3D ::dotProduct(vector, norm) * norm;
}

/*!
    Returns the minimum distance from this ray to \a point, or equivalently
    the length of a line perpendicular to this ray which passes through
    \a point.  If \a point is on the ray, then this function will return zero.

    \sa point()
*/
float QRay3D::distance(const Vector3D  &point) const
{
    float t = projectedDistance(point);
    return (point - (m_origin + t * m_direction)).length();
}

/*!
    \fn Qt3DRender::RayCasting::QRay3D &Qt3DRender::RayCasting::QRay3D::transform(const Matrix4x4 &matrix)

    Transforms this ray using \a matrix, replacing origin() and
    direction() with the transformed versions.

    \sa transformed()
*/

/*!
    \fn Qt3DRender::RayCasting::QRay3D Qt3DRender::RayCasting::QRay3D::transformed(const Matrix4x4 &matrix) const

    Returns a new ray that is formed by transforming origin()
    and direction() using \a matrix.

    \sa transform()
*/

/*!
    \fn bool Qt3DRender::RayCasting::QRay3D::operator==(const QRay3D &other) const

    Returns \c true if this ray is the same as \a other; \c false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool Qt3DRender::RayCasting::QRay3D::operator!=(const QRay3D &other) const

    Returns \c true if this ray is not the same as \a other; \c false otherwise.

    \sa operator==()
*/

/*!
    \fn bool qFuzzyCompare(const Qt3DRender::RayCasting::QRay3D &ray1, const Qt3DRender::RayCasting::QRay3D &ray2)
    \relates  Qt3DRender::RayCasting::QRay3D

    Returns \c true if \a ray1 and \a ray2 are almost equal; \c false
    otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QRay3D &ray)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QRay3D(origin("
        << ray.origin().x() << ", " << ray.origin().y() << ", "
        << ray.origin().z() << ") - direction("
        << ray.direction().x() << ", " << ray.direction().y() << ", "
        << ray.direction().z() << ") - distance(" << ray.distance() << "))";
    return dbg;
}

#endif

#ifndef QT_NO_DATASTREAM

/*!
    \relates Qt3DRender::RayCasting::QRay3D

    Writes the given \a ray to the given \a stream and returns a
    reference to the stream.
*/
QDataStream &operator<<(QDataStream &stream, const QRay3D &ray)
{
    stream << convertToQVector3D(ray.origin());
    stream << convertToQVector3D(ray.direction());
    if (stream.version() >= QDataStream::Qt_5_11)
        stream << ray.distance();
    return stream;
}

/*!
    \relates Qt3DRender::RayCasting::QRay3D

    Reads a 3D ray from the given \a stream into the given \a ray
    and returns a reference to the stream.
*/
QDataStream &operator>>(QDataStream &stream, QRay3D &ray)
{
    QVector3D origin, direction;
    float distance = 1.f;

    stream >> origin;
    stream >> direction;
    if (stream.version() >= QDataStream::Qt_5_11)
        stream >> distance;
    ray = QRay3D(Vector3D(origin), Vector3D(direction), distance);
    return stream;
}

#endif // QT_NO_DATASTREAM

}  // namespace RayCasting
}  // namespace Qt3DRender

QT_END_NAMESPACE
