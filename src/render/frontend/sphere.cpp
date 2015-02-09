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

#include "sphere.h"

#include <QPair>

#include <math.h>

QT_BEGIN_NAMESPACE

namespace {

inline QPair<int, int> findExtremePoints(const QVector<QVector3D> &points)
{
    // Find indices of extreme points along x, y, and z axes
    int xMin = 0, xMax = 0, yMin = 0, yMax = 0, zMin = 0, zMax = 0;
    for (int i = 1; i < points.size(); ++i) {
        const QVector3D &p = points.at(i);
        if (p.x() < points[xMin].x())
            xMin = i;
        if (p.x() > points[xMax].x())
            xMax = i;
        if (p.y() < points[yMin].y())
            yMin = i;
        if (p.y() > points[yMax].y())
            yMax = i;
        if (p.z() < points[zMin].z())
            zMin = i;
        if (p.z() > points[zMax].z())
            zMax = i;
    }

    // Calculate squared distance for the pairs of points
    const float xDist2 = (points.at(xMax) - points.at(xMin)).lengthSquared();
    const float yDist2 = (points.at(yMax) - points.at(yMin)).lengthSquared();
    const float zDist2 = (points.at(zMax) - points.at(zMin)).lengthSquared();

    // Select most distant pair
    QPair<int, int> extremeIndices(xMin, xMax);
    if (yDist2 > xDist2 && yDist2 > zDist2)
        extremeIndices = qMakePair(yMin, yMax);
    if (zDist2 > xDist2 && zDist2 > yDist2)
        extremeIndices = qMakePair(zMin, zMax);

    return extremeIndices;
}

inline void sphereFromExtremePoints(Qt3D::Sphere &s, const QVector<QVector3D> &points)
{
    // Find two most separated points on any of the basis vectors
    QPair<int, int> extremeIndices = findExtremePoints(points);

    // Construct sphere to contain these two points
    const QVector3D &p = points.at(extremeIndices.first);
    const QVector3D &q = points.at(extremeIndices.second);
    const QVector3D c = 0.5f * (p + q);
    s.setCenter(c);
    s.setRadius((q - c).length());
}

inline void constructRitterSphere(Qt3D::Sphere &s, const QVector<QVector3D> &points)
{
    // Calculate the sphere encompassing two axially extreme points
    sphereFromExtremePoints(s, points);

    // Now make sure the sphere bounds all points by growing if needed
    s.expandToContain(points);
}

} // anonymous namespace

namespace Qt3D {

const float Sphere::ms_epsilon = 1.0e-7f;

Sphere Sphere::fromPoints(const QVector<QVector3D> &points)
{
    Sphere s;
    s.initializeFromPoints(points);
    return s;
}

void Sphere::initializeFromPoints(const QVector<QVector3D> &points)
{
    constructRitterSphere(*this, points);
}

void Sphere::expandToContain(const QVector3D &p)
{
    const QVector3D d = p - m_center;
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
    const QVector3D d(sphere.m_center - m_center);
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
    }
}

Sphere Sphere::transformed(const QMatrix4x4 &mat)
{
    // Transform extremities in x, y, and z directions to find extremities
    // of the resulting ellipsoid
    QVector3D x = mat.mapVector(m_center + QVector3D(m_radius, 0.0f, 0.0f));
    QVector3D y = mat.mapVector(m_center + QVector3D(0.0f, m_radius, 0.0f));
    QVector3D z = mat.mapVector(m_center + QVector3D(0.0f, 0.0f, m_radius));

    // Transform center and find maximum radius of ellipsoid
    QVector3D c = mat.mapVector(m_center);
    float rSquared = qMax(qMax((x - c).lengthSquared(), (y - c).lengthSquared()), (z - c).lengthSquared());
    return Sphere(c, sqrt(rSquared));
}

}

QT_END_NAMESPACE
