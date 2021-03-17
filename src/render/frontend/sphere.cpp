/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "sphere_p.h"

#include <Qt3DRender/private/qray3d_p.h>

#include <QPair>

#include <math.h>
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace {

// Algorithms taken from Real-time collision detection, p178-179

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns true and intersection point q; false otherwise
bool intersectRaySphere(const Qt3DRender::RayCasting::QRay3D &ray, const Qt3DRender::Render::Sphere &s, Vector3D *q = nullptr)
{
    if (s.isNull())
        return false;

    const Vector3D p = ray.origin();
    const Vector3D d = ray.direction();
    const Vector3D m = p - s.center();
    const float c = Vector3D::dotProduct(m, m) - s.radius() * s.radius();

    // If there is definitely at least one real root, there must be an intersection
    if (q == nullptr && c <= 0.0f)
        return true;

    const float b = Vector3D::dotProduct(m, d);
    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return false;

    const float discr = b*b - c;
    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return false;

    // If we don't need the intersection point, return early
    if (q == nullptr)
        return true;

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -b - sqrt(discr);

    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.0f)
        t = 0.0f;

    *q = p + t * d;
    return true;
}

inline void constructRitterSphere(Qt3DRender::Render::Sphere &s, const QVector<Vector3D> &points)
{
    //def bounding_sphere(points):
    //  dist = lambda a,b: ((a[0] - b[0])**2 + (a[1] - b[1])**2 + (a[2] - b[2])**2)**0.5
    //  x = points[0]
    //  y = max(points,key= lambda p: dist(p,x) )
    //  z = max(points,key= lambda p: dist(p,y) )
    //  bounding_sphere = (((y[0]+z[0])/2,(y[1]+z[1])/2,(y[2]+z[2])/2), dist(y,z)/2)
    //
    //  exterior_points = [p for p in points if dist(p,bounding_sphere[0]) > bounding_sphere[1] ]
    //  while ( len(exterior_points) > 0 ):
    //    pt = exterior_points.pop()
    //    if (dist(pt, bounding_sphere[0]) > bounding_sphere[1]):
    //      bounding_sphere = (bounding_sphere[0],dist(pt,bounding_sphere[0]))
    //
    //  return bounding_sphere

    const Vector3D x = points[0];
    const Vector3D y = *std::max_element(points.begin(), points.end(), [&x](const Vector3D& lhs, const Vector3D& rhs){ return (lhs - x).lengthSquared() < (rhs - x).lengthSquared(); });
    const Vector3D z = *std::max_element(points.begin(), points.end(), [&y](const Vector3D& lhs, const Vector3D& rhs){ return (lhs - y).lengthSquared() < (rhs - y).lengthSquared(); });

    const Vector3D center = (y + z) * 0.5f;
    const Vector3D maxDistPt = *std::max_element(points.begin(), points.end(), [&center](const Vector3D& lhs, const Vector3D& rhs){ return (lhs - center).lengthSquared() < (rhs - center).lengthSquared(); });
    const float radius = (maxDistPt - center).length();

    s.setCenter(center);
    s.setRadius(radius);
}

} // anonymous namespace

namespace Qt3DRender {

namespace Render {

const float Sphere::ms_epsilon = 1.0e-7f;

Sphere Sphere::fromPoints(const QVector<Vector3D> &points)
{
    Sphere s;
    s.initializeFromPoints(points);
    return s;
}

void Sphere::initializeFromPoints(const QVector<Vector3D> &points)
{
    if (!points.isEmpty())
        constructRitterSphere(*this, points);
}

void Sphere::expandToContain(const Vector3D &p)
{
    if (isNull()) {
        m_center = p;
        m_radius = 0.0f;
        return;
    }

    const Vector3D d = p - m_center;
    const float dist2 = d.lengthSquared();

    if (dist2 > m_radius * m_radius) {
        // Expand radius so sphere also contains p
        const float dist = sqrt(dist2);
        const float newRadius = 0.5f * (m_radius + dist);
        const float k = (newRadius - m_radius) / dist;
        m_radius = newRadius;
        m_center += k * d;
    }
}

void Sphere::expandToContain(const Sphere &sphere)
{
    if (isNull()) {
        *this = sphere;
        return;
    } else if (sphere.isNull()) {
        return;
    }

    const Vector3D d(sphere.m_center - m_center);
    const float dist2 = d.lengthSquared();

    const float dr = sphere.m_radius - m_radius;
    if (dr * dr >= dist2) {
        // Larger sphere encloses the smaller. Set our size to the larger
        if (m_radius > sphere.m_radius)
            return;
        else
            *this = sphere;
    } else {
        // The spheres are overlapping or disjoint
        const float dist = sqrt(dist2);
        const float newRadius = 0.5f * (dist + m_radius + sphere.m_radius);
        if (dist > ms_epsilon)
            m_center += d * (newRadius - m_radius) / dist;
        m_radius = newRadius;
    }
}

Sphere Sphere::transformed(const Matrix4x4 &mat) const
{
    if (isNull())
        return *this;

    // Transform extremities in x, y, and z directions to find extremities
    // of the resulting ellipsoid
    Vector3D x = mat.map(m_center + Vector3D(m_radius, 0.0f, 0.0f));
    Vector3D y = mat.map(m_center + Vector3D(0.0f, m_radius, 0.0f));
    Vector3D z = mat.map(m_center + Vector3D(0.0f, 0.0f, m_radius));

    // Transform center and find maximum radius of ellipsoid
    Vector3D c = mat.map(m_center);
    float rSquared = qMax(qMax((x - c).lengthSquared(), (y - c).lengthSquared()), (z - c).lengthSquared());
    return Sphere(c, sqrt(rSquared), id());
}

Qt3DCore::QNodeId Sphere::id() const
{
    return m_id;
}

bool Sphere::intersects(const RayCasting::QRay3D &ray, Vector3D *q, Vector3D *uvw) const
{
    Q_UNUSED(uvw);
    return intersectRaySphere(ray, *this, q);
}

Sphere::Type Sphere::type() const
{
    return RayCasting::QBoundingVolume::Sphere;
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const Sphere &sphere)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "Sphere(center("
        << sphere.center().x() << ", " << sphere.center().y() << ", "
        << sphere.center().z() << ") - radius(" << sphere.radius() << "))";
    return dbg;
}

#endif

} // Render

} // Qt3DRender

QT_END_NAMESPACE
