// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaxisalignedboundingbox_p.h"

#include <QDebug>
#include <QMatrix4x4>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

void QAxisAlignedBoundingBox::update(const std::vector<QVector3D> &points)
{
    if (points.empty()) {
        m_center = QVector3D();
        m_radii = QVector3D();
        return;
    }

    QVector3D minPoint = points.at( 0 );
    QVector3D maxPoint = points.at( 0 );

    for (size_t i = 1; i < points.size(); ++i) {
        const QVector3D &point = points.at(i);
        if (point.x() > maxPoint.x())
            maxPoint.setX(point.x());
        if (point.y() > maxPoint.y())
            maxPoint.setY(point.y());
        if (point.z() > maxPoint.z())
            maxPoint.setZ(point.z());
        if (point.x() < minPoint.x())
            minPoint.setX(point.x());
        if (point.y() < minPoint.y())
            minPoint.setY(point.y());
        if (point.z() < minPoint.z())
            minPoint.setZ(point.z());
    }

    m_center = 0.5 * (minPoint + maxPoint);
    m_radii = 0.5 * (maxPoint - minPoint);
#if 0
    qDebug() << "AABB:";
    qDebug() << "    min =" << minPoint;
    qDebug() << "    max =" << maxPoint;
    qDebug() << " center =" << m_center;
    qDebug() << "  radii =" << m_radii;
#endif
}

QDebug operator<<(QDebug dbg, const QAxisAlignedBoundingBox &c)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "AABB ( min:" << c.minPoint() << ", max:" << c.maxPoint() << ')';
    return dbg;
}

} //namespace Qt3DRender

QT_END_NAMESPACE
