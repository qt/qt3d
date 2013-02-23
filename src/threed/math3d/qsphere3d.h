/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QSPHERE3D_H
#define QSPHERE3D_H

#include <Qt3D/qt3dglobal.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;
class QRay3D;
class QBox3D;
class QPlane3D;

class Q_QT3D_EXPORT QSphere3D
{
public:
    QSphere3D();
    QSphere3D(const QVector3D &center, float radius);

    QVector3D center() const;
    void setCenter(const QVector3D &center);

    float radius() const;
    void setRadius(float radius);

    bool contains(const QVector3D &point) const;

    bool intersects(const QRay3D &ray) const;
    bool intersects(const QSphere3D &sphere) const;
    bool intersects(const QBox3D &box) const;
    bool intersects(const QPlane3D &plane) const;

    bool intersection(const QRay3D &ray, float *minimum_t, float *maximum_t) const;
    float intersection(const QRay3D &ray) const;

    void transform(const QMatrix4x4 &matrix);
    QSphere3D transformed(const QMatrix4x4 &matrix) const;

    bool operator==(const QSphere3D &sphere) const;
    bool operator!=(const QSphere3D &sphere) const;

private:
    QVector3D m_center;
    float m_radius;
};

inline QSphere3D::QSphere3D() : m_radius(1.0f) {}

inline QSphere3D::QSphere3D(const QVector3D &center_, float radius_)
    : m_center(center_), m_radius(radius_) {}

inline QVector3D QSphere3D::center() const
{
    return m_center;
}

inline void QSphere3D::setCenter(const QVector3D &center_)
{
    m_center = center_;
}

inline float QSphere3D::radius() const
{
    return m_radius;
}

inline void QSphere3D::setRadius(float radius_)
{
    m_radius = radius_;
}

inline bool QSphere3D::contains(const QVector3D &point) const
{
    return (point - m_center).lengthSquared() <= (m_radius * m_radius);
}

inline bool QSphere3D::intersects(const QSphere3D &sphere) const
{
    float radsum = sphere.radius() + m_radius;
    return (sphere.center() - m_center).lengthSquared() <= (radsum * radsum);
}

inline void QSphere3D::transform(const QMatrix4x4 &matrix)
{
    *this = transformed(matrix);
}

inline bool QSphere3D::operator==(const QSphere3D &sphere) const
{
    return m_center == sphere.m_center && m_radius == sphere.m_radius;
}

inline bool QSphere3D::operator!=(const QSphere3D &sphere) const
{
    return m_center != sphere.m_center || m_radius != sphere.m_radius;
}

inline bool qFuzzyCompare(const QSphere3D &sphere1, const QSphere3D &sphere2)
{
    return qFuzzyCompare(sphere1.center(), sphere2.center()) &&
           qFuzzyCompare(sphere1.radius(), sphere2.radius());
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QSphere3D &sphere);
#endif

#ifndef QT_NO_DATASTREAM
Q_QT3D_EXPORT QDataStream &operator<<(QDataStream &stream, const QSphere3D &sphere);
Q_QT3D_EXPORT QDataStream &operator>>(QDataStream &stream, QSphere3D &sphere);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QSphere3D)

#endif
