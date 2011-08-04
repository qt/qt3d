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

#ifndef QRAY3D_H
#define QRAY3D_H

#include "qt3dglobal.h"
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class Q_QT3D_EXPORT QRay3D
{
public:
    QRay3D();
    QRay3D(const QVector3D &origin, const QVector3D &direction);

    QVector3D origin() const;
    void setOrigin(const QVector3D & value);

    QVector3D direction() const;
    void setDirection(const QVector3D & value);

    bool contains(const QVector3D &point) const;
    bool contains(const QRay3D &ray) const;

    QVector3D point(qreal t) const;
    qreal fromPoint(const QVector3D &point) const;

    QVector3D project(const QVector3D &vector) const;

    qreal distanceTo(const QVector3D &point) const;

    void transform(const QMatrix4x4 &matrix);
    QRay3D transformed(const QMatrix4x4 &matrix) const;

    bool operator==(const QRay3D &other);
    bool operator!=(const QRay3D &other);

private:
    QVector3D m_origin;
    QVector3D m_direction;

};

inline QRay3D::QRay3D() : m_direction(1.0f, 0.0f, 0.0f) {}

inline QRay3D::QRay3D(const QVector3D &origin, const QVector3D &direction)
    : m_origin(origin)
    , m_direction(direction)
{
}

inline QVector3D QRay3D::origin() const
{
    return m_origin;
}

inline void QRay3D::setOrigin(const QVector3D &value)
{
    m_origin = value;
}

inline QVector3D QRay3D::direction() const
{
    return m_direction;
}

inline void QRay3D::setDirection(const QVector3D & value)
{
    m_direction = value;
}

inline QVector3D QRay3D::point(qreal t) const
{
    return m_origin + t * m_direction;
}

inline void QRay3D::transform(const QMatrix4x4 &matrix)
{
    m_origin = matrix * m_origin;
    m_direction = matrix.mapVector(m_direction);
}

inline QRay3D QRay3D::transformed(const QMatrix4x4 &matrix) const
{
    return QRay3D(matrix * m_origin, matrix.mapVector(m_direction));
}

inline bool QRay3D::operator==(const QRay3D &other)
{
    return m_origin == other.origin() && m_direction == other.direction();
}

inline bool QRay3D::operator!=(const QRay3D &other)
{
    return m_origin != other.origin() || m_direction != other.direction();
}

inline bool qFuzzyCompare(const QRay3D &ray1, const QRay3D &ray2)
{
    return qFuzzyCompare(ray1.origin(), ray2.origin()) &&
           qFuzzyCompare(ray1.direction(), ray2.direction());
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QRay3D &ray);
#endif

#ifndef QT_NO_DATASTREAM
Q_QT3D_EXPORT QDataStream &operator<<(QDataStream &stream, const QRay3D &ray);
Q_QT3D_EXPORT QDataStream &operator>>(QDataStream &stream, QRay3D &ray);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QRay3D)

QT_END_HEADER

#endif
