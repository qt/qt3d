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

#ifndef QBOX3D_H
#define QBOX3D_H

#include "qt3dglobal.h"
#include "qray3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QMatrix4x4;

class Q_QT3D_EXPORT QBox3D
{
public:
    QBox3D();
    QBox3D(const QVector3D& corner1, const QVector3D& corner2);

    bool isNull() const;
    bool isFinite() const;
    bool isInfinite() const;

    QVector3D minimum() const;
    QVector3D maximum() const;
    void setExtents(const QVector3D& corner1, const QVector3D& corner2);

    void setToNull();
    void setToInfinite();

    QVector3D size() const;
    QVector3D center() const;

    bool contains(const QVector3D& point) const;
    bool contains(const QBox3D& box) const;

    bool intersects(const QRay3D& ray) const;
    bool intersection(const QRay3D &ray, qreal *minimum_t, qreal *maximum_t) const;
    qreal intersection(const QRay3D& ray) const;

    bool intersects(const QBox3D& box) const;
    void intersect(const QBox3D& box);
    QBox3D intersected(const QBox3D& box) const;

    void unite(const QVector3D& point);
    void unite(const QBox3D& box);

    QBox3D united(const QVector3D& point) const;
    QBox3D united(const QBox3D& box) const;

    void transform(const QMatrix4x4& matrix);
    QBox3D transformed(const QMatrix4x4& matrix) const;

    bool operator==(const QBox3D& box) const;
    bool operator!=(const QBox3D& box) const;

    friend bool qFuzzyCompare(const QBox3D& box1, const QBox3D& box2);

private:
    enum Type
    {
        Null,
        Finite,
        Infinite
    };

    QBox3D::Type boxtype;
    QVector3D mincorner, maxcorner;
};

inline QBox3D::QBox3D() : boxtype(Null), mincorner(0, 0, 0), maxcorner(0, 0, 0) {}

inline QBox3D::QBox3D(const QVector3D& corner1, const QVector3D& corner2)
    : boxtype(Finite),
      mincorner(qMin(corner1.x(), corner2.x()),
                qMin(corner1.y(), corner2.y()),
                qMin(corner1.z(), corner2.z())),
      maxcorner(qMax(corner1.x(), corner2.x()),
                qMax(corner1.y(), corner2.y()),
                qMax(corner1.z(), corner2.z())) {}

inline bool QBox3D::isNull() const { return (boxtype == Null); }
inline bool QBox3D::isFinite() const { return (boxtype == Finite); }
inline bool QBox3D::isInfinite() const { return (boxtype == Infinite); }

inline QVector3D QBox3D::minimum() const { return mincorner; }
inline QVector3D QBox3D::maximum() const { return maxcorner; }

inline void QBox3D::setExtents(const QVector3D& corner1, const QVector3D& corner2)
{
    boxtype = Finite;
    mincorner = QVector3D(qMin(corner1.x(), corner2.x()),
                          qMin(corner1.y(), corner2.y()),
                          qMin(corner1.z(), corner2.z()));
    maxcorner = QVector3D(qMax(corner1.x(), corner2.x()),
                          qMax(corner1.y(), corner2.y()),
                          qMax(corner1.z(), corner2.z()));
}

inline void QBox3D::setToNull()
{
    boxtype = Null;
    mincorner = QVector3D(0, 0, 0);
    maxcorner = QVector3D(0, 0, 0);
}

inline void QBox3D::setToInfinite()
{
    boxtype = Infinite;
    mincorner = QVector3D(0, 0, 0);
    maxcorner = QVector3D(0, 0, 0);
}

inline QVector3D QBox3D::size() const { return maxcorner - mincorner; }
inline QVector3D QBox3D::center() const { return (mincorner + maxcorner) * 0.5f; }

inline bool QBox3D::contains(const QVector3D& point) const
{
    if (boxtype == Finite) {
        return (point.x() >= mincorner.x() && point.x() <= maxcorner.x() &&
                point.y() >= mincorner.y() && point.y() <= maxcorner.y() &&
                point.z() >= mincorner.z() && point.z() <= maxcorner.z());
    } else if (boxtype == Infinite) {
        return true;
    } else {
        return false;
    }
}

inline bool QBox3D::contains(const QBox3D& box) const
{
    if (box.boxtype == Finite)
        return contains(box.mincorner) && contains(box.maxcorner);
    else if (box.boxtype == Infinite)
        return (boxtype == Infinite);
    else
        return false;
}

inline bool QBox3D::operator==(const QBox3D& box) const
{
    return (boxtype == box.boxtype &&
            mincorner == box.mincorner &&
            maxcorner == box.maxcorner);
}

inline bool QBox3D::operator!=(const QBox3D& box) const
{
    return (boxtype != box.boxtype ||
            mincorner != box.mincorner ||
            maxcorner != box.maxcorner);
}

inline bool qFuzzyCompare(const QBox3D& box1, const QBox3D& box2)
{
    return box1.boxtype == box2.boxtype &&
           qFuzzyCompare(box1.mincorner, box2.mincorner) &&
           qFuzzyCompare(box1.maxcorner, box2.maxcorner);
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QBox3D &box);
#endif

#ifndef QT_NO_DATASTREAM
Q_QT3D_EXPORT QDataStream &operator<<(QDataStream &stream, const QBox3D &box);
Q_QT3D_EXPORT QDataStream &operator>>(QDataStream &stream, QBox3D &box);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QBox3D)

QT_END_HEADER

#endif
