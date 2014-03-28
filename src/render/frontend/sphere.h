/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SPHERE_H
#define SPHERE_H

#include "qt3drenderer_global.h"

#include <QMatrix4x4>
#include <QVector3D>

namespace Qt3D {

class Sphere
{
public:
    inline Sphere()
        : m_center(),
          m_radius(0.0f)
    {}

    inline Sphere(const QVector3D &c, float r)
        : m_center(c),
          m_radius(r)
    {}

    void setCenter(const QVector3D &center);
    QVector3D center() const;

    void setRadius(float radius);
    float radius() const;

    void clear();
    void initializeFromPoints(const QVector<QVector3D> &points);
    void expandToContain(const QVector3D &point);
    inline void expandToContain(const QVector<QVector3D> &points)
    {
        Q_FOREACH (const QVector3D &p, points)
            expandToContain(p);
    }

    void expandToContain(const Sphere &sphere);

    Sphere transformed(const QMatrix4x4 &mat);
    inline Sphere &transform(const QMatrix4x4 &mat)
    {
        *this = transformed(mat);
        return *this;
    }

    static Sphere fromPoints(const QVector<QVector3D> &points);

private:
    QVector3D m_center;
    float m_radius;

    static const float ms_epsilon;
};

inline void Sphere::setCenter(const QVector3D &center)
{
    m_center = center;
}

inline QVector3D Sphere::center() const
{
    return m_center;
}

inline void Sphere::setRadius(float radius)
{
    m_radius = radius;
}

inline float Sphere::radius() const
{
    return m_radius;
}

inline void Sphere::clear()
{
    m_center = QVector3D();
    m_radius = 0.0f;
}

inline bool intersects(const Sphere &a, const Sphere &b)
{
    // Calculate squared distance between sphere centers
    const QVector3D d = a.center() - b.center();
    const float distSq = QVector3D::dotProduct(d, d);

    // Spheres intersect if squared distance is less than squared
    // sum of radii
    const float sumRadii = a.radius() + b.radius();
    return distSq <= sumRadii * sumRadii;
}

}

#endif // SPHERE_H
