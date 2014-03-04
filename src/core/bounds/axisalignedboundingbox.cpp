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

#include "axisalignedboundingbox.h"

#include <QDebug>
#include <QMatrix4x4>

namespace Qt3D {

void AxisAlignedBoundingBox::update(const QVector<QVector3D> &points)
{
    QVector3D minPoint, maxPoint;
    for (int i = 0; i < points.size(); ++i)
    {
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

QT3DCORESHARED_EXPORT QDebug operator<<(QDebug dbg, const AxisAlignedBoundingBox &c)
{
    dbg.nospace() << "AABB ( min:" << c.minPoint() << ", max:" << c.maxPoint() << ")";
    return dbg.space();
}

}
