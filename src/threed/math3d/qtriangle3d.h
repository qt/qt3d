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

#ifndef QTRIANGLE3D_H
#define QTRIANGLE3D_H

#include <Qt3D/qplane3d.h>
#include <Qt3D/qray3d.h>

#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

class Q_QT3D_EXPORT QTriangle3D
{
public:
    QTriangle3D();
    QTriangle3D(const QVector3D &p, const QVector3D &q, const QVector3D &r);

    QVector3D p() const;
    void setP(const QVector3D & point);

    QVector3D q() const;
    void setQ(const QVector3D & point);

    QVector3D r() const;
    void setR(const QVector3D & point);

    QPlane3D plane() const;
    QVector3D center() const;
    QVector3D faceNormal() const;

    bool contains(const QVector3D &point) const;

    bool intersects(const QRay3D &ray) const;
    float intersection(const QRay3D &ray) const;

    void transform(const QMatrix4x4 &matrix);
    QTriangle3D transformed(const QMatrix4x4 &matrix) const;

    QVector2D uv(const QVector3D &point) const;

    bool operator==(const QTriangle3D &other);
    bool operator!=(const QTriangle3D &other);

private:
    QVector3D m_p, m_q, m_r;
};

inline QTriangle3D::QTriangle3D()
    : m_p(0.0f, 0.0f, 0.0f)
    , m_q(1.0f, 0.0f, 0.0f)
    , m_r(0.0f, 1.0f, 0.0f) {}

inline QTriangle3D::QTriangle3D(const QVector3D &p_, const QVector3D &q_, const QVector3D &r_)
    : m_p(p_)
    , m_q(q_)
    , m_r(r_) {}

inline QVector3D QTriangle3D::p() const
{
    return m_p;
}

inline void QTriangle3D::setP(const QVector3D &point)
{
    m_p = point;
}

inline QVector3D QTriangle3D::q() const
{
    return m_q;
}

inline void QTriangle3D::setQ(const QVector3D &point)
{
    m_q = point;
}

inline QVector3D QTriangle3D::r() const
{
    return m_r;
}

inline void QTriangle3D::setR(const QVector3D &point)
{
    m_r = point;
}

inline QPlane3D QTriangle3D::plane() const
{
    return QPlane3D(m_p, m_q, m_r);
}

inline QVector3D QTriangle3D::center() const
{
    return (m_p + m_q + m_r) / 3.0f;
}

inline QVector3D QTriangle3D::faceNormal() const
{
    return QVector3D::crossProduct(m_q - m_p, m_r - m_q);
}

inline bool QTriangle3D::operator==(const QTriangle3D &other)
{
    return m_p == other.m_p && m_q == other.m_q && m_r == other.m_r;
}

inline bool QTriangle3D::operator!=(const QTriangle3D &other)
{
    return m_p != other.m_p || m_q != other.m_q || m_r != other.m_r;
}

inline bool qFuzzyCompare
    (const QTriangle3D &triangle1, const QTriangle3D &triangle2)
{
    return qFuzzyCompare(triangle1.p(), triangle2.p()) &&
           qFuzzyCompare(triangle1.q(), triangle2.q()) &&
           qFuzzyCompare(triangle1.r(), triangle2.r());
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QTriangle3D &triangle);
#endif

#ifndef QT_NO_DATASTREAM
Q_QT3D_EXPORT QDataStream &operator<<(QDataStream &stream, const QTriangle3D &triangle);
Q_QT3D_EXPORT QDataStream &operator>>(QDataStream &stream, QTriangle3D &triangle);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QTriangle3D)

#endif // QTRIANGLE3D_H
