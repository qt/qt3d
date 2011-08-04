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

#ifndef QSPHERE3D_H
#define QSPHERE3D_H

#include "qt3dglobal.h"
#include <QtGui/qvector3d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QMatrix4x4;
class QRay3D;
class QBox3D;
class QPlane3D;

class Q_QT3D_EXPORT QSphere3D
{
public:
    QSphere3D();
    QSphere3D(const QVector3D &center, qreal radius);

    QVector3D center() const;
    void setCenter(const QVector3D &center);

    qreal radius() const;
    void setRadius(qreal radius);

    bool contains(const QVector3D &point) const;

    bool intersects(const QRay3D &ray) const;
    bool intersects(const QSphere3D &sphere) const;
    bool intersects(const QBox3D &box) const;
    bool intersects(const QPlane3D &plane) const;

    bool intersection(const QRay3D &ray, qreal *minimum_t, qreal *maximum_t) const;
    qreal intersection(const QRay3D &ray) const;

    void transform(const QMatrix4x4 &matrix);
    QSphere3D transformed(const QMatrix4x4 &matrix) const;

    bool operator==(const QSphere3D &sphere) const;
    bool operator!=(const QSphere3D &sphere) const;

private:
    QVector3D m_center;
    qreal m_radius;
};

inline QSphere3D::QSphere3D() : m_radius(1.0f) {}

inline QSphere3D::QSphere3D(const QVector3D &center, qreal radius)
    : m_center(center), m_radius(radius) {}

inline QVector3D QSphere3D::center() const
{
    return m_center;
}

inline void QSphere3D::setCenter(const QVector3D &center)
{
    m_center = center;
}

inline qreal QSphere3D::radius() const
{
    return m_radius;
}

inline void QSphere3D::setRadius(qreal radius)
{
    m_radius = radius;
}

inline bool QSphere3D::contains(const QVector3D &point) const
{
    return (point - m_center).lengthSquared() <= (m_radius * m_radius);
}

inline bool QSphere3D::intersects(const QSphere3D &sphere) const
{
    qreal radsum = sphere.radius() + m_radius;
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

QT_END_HEADER

#endif
