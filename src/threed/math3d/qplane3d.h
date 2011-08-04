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

#ifndef QPLANE3D_H
#define QPLANE3D_H

#include <QtGui/qvector3d.h>
#include <QtCore/qnumeric.h>
#include "qray3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class Q_QT3D_EXPORT QPlane3D
{
public:
    QPlane3D();
    QPlane3D(const QVector3D &point, const QVector3D &normal);
    QPlane3D(const QVector3D &p, const QVector3D &q, const QVector3D &r);

    QVector3D origin() const;
    void setOrigin(const QVector3D& value);

    QVector3D normal() const;
    void setNormal(const QVector3D& value);

    bool contains(const QVector3D &point) const;
    bool contains(const QRay3D &ray) const;

    bool intersects(const QRay3D &ray) const;
    qreal intersection(const QRay3D &ray) const;

    qreal distanceTo(const QVector3D &point) const;

    void transform(const QMatrix4x4 &matrix);
    QPlane3D transformed(const QMatrix4x4 &matrix) const;

    bool operator==(const QPlane3D &other);
    bool operator!=(const QPlane3D &other);

private:
    QVector3D m_origin;
    QVector3D m_normal;
};

inline QPlane3D::QPlane3D() : m_normal(1.0f, 0.0f, 0.0f) {}

inline QPlane3D::QPlane3D(const QVector3D &point, const QVector3D &normal)
    : m_origin(point), m_normal(normal)
{
}

inline QPlane3D::QPlane3D(const QVector3D &p, const QVector3D &q, const QVector3D &r)
    : m_origin(p), m_normal(QVector3D::crossProduct(q - p, r - q))
{
}

inline QVector3D QPlane3D::origin() const
{
    return m_origin;
}

inline void QPlane3D::setOrigin(const QVector3D &value)
{
    m_origin = value;
}

inline QVector3D QPlane3D::normal() const
{
    return m_normal;
}

inline void QPlane3D::setNormal(const QVector3D& value)
{
    m_normal = value;
}

inline void QPlane3D::transform(const QMatrix4x4 &matrix)
{
    m_origin = matrix * m_origin;
    m_normal = matrix.mapVector(m_normal);
}

inline QPlane3D QPlane3D::transformed(const QMatrix4x4 &matrix) const
{
    return QPlane3D(matrix * m_origin, matrix.mapVector(m_normal));
}

inline bool QPlane3D::operator==(const QPlane3D &other)
{
    return m_origin == other.origin() && m_normal == other.normal();
}

inline bool QPlane3D::operator!=(const QPlane3D &other)
{
    return m_origin != other.origin() || m_normal != other.normal();
}

inline bool qFuzzyCompare(const QPlane3D &plane1, const QPlane3D &plane2)
{
    return qFuzzyCompare(plane1.origin(), plane2.origin()) &&
           qFuzzyCompare(plane1.normal(), plane2.normal());
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QPlane3D &plane);
#endif

#ifndef QT_NO_DATASTREAM
Q_QT3D_EXPORT QDataStream &operator<<(QDataStream &stream, const QPlane3D &plane);
Q_QT3D_EXPORT QDataStream &operator>>(QDataStream &stream, QPlane3D &plane);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlane3D)

QT_END_HEADER

#endif // QPLANE3D_H
