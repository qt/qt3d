/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qraycasterhit.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRayCasterHitData : public QSharedData
{
public:
    QRayCasterHitData() { }
    QRayCasterHitData(QRayCasterHit::HitType type, Qt3DCore::QNodeId id, float distance,
                      const QVector3D &localIntersect, const QVector3D &worldIntersect,
                      uint primitiveIndex, uint v1 = 0, uint v2 = 0, uint v3 = 0);

    QRayCasterHit::HitType m_type = QRayCasterHit::EntityHit;
    Qt3DCore::QNodeId m_entityId;
    Qt3DCore::QEntity *m_entity = nullptr;
    float m_distance = 0.f;
    QVector3D m_localIntersection;
    QVector3D m_worldIntersection;
    uint m_primitiveIndex = 0;
    uint m_vertex1Index = 0;
    uint m_vertex2Index = 0;
    uint m_vertex3Index = 0;
};

QRayCasterHitData::QRayCasterHitData(QRayCasterHit::HitType type, Qt3DCore::QNodeId id, float distance,
                                     const QVector3D &localIntersect, const QVector3D &worldIntersect,
                                     uint primitiveIndex, uint v1, uint v2, uint v3)
    : m_type(type)
    , m_entityId(id)
    , m_entity(nullptr)
    , m_distance(distance)
    , m_localIntersection(localIntersect)
    , m_worldIntersection(worldIntersect)
    , m_primitiveIndex(primitiveIndex)
    , m_vertex1Index(v1)
    , m_vertex2Index(v2)
    , m_vertex3Index(v3)
{

}


QRayCasterHit::QRayCasterHit()
    : d(new QRayCasterHitData)
{

}

QRayCasterHit::QRayCasterHit(QRayCasterHit::HitType type, Qt3DCore::QNodeId id, float distance,
                             const QVector3D &localIntersect, const QVector3D &worldIntersect,
                             uint primitiveIndex, uint v1, uint v2, uint v3)
    : d(new QRayCasterHitData(type, id, distance, localIntersect, worldIntersect, primitiveIndex, v1, v2, v3))
{

}

QRayCasterHit::QRayCasterHit(const QRayCasterHit &other)
    : d(other.d)
{

}

QRayCasterHit::~QRayCasterHit()
{

}

QRayCasterHit &QRayCasterHit::operator =(const QRayCasterHit &other)
{
    d = other.d;
    return *this;
}

QRayCasterHit::HitType Qt3DRender::QRayCasterHit::type() const
{
    return d->m_type;
}

Qt3DCore::QNodeId QRayCasterHit::entityId() const
{
    return d->m_entityId;
}

Qt3DCore::QEntity *QRayCasterHit::entity() const
{
    return d->m_entity;
}

float QRayCasterHit::distance() const
{
    return d->m_distance;
}

QVector3D QRayCasterHit::localIntersection() const
{
    return d->m_localIntersection;
}

QVector3D QRayCasterHit::worldIntersection() const
{
    return d->m_worldIntersection;
}

uint QRayCasterHit::primitiveIndex() const
{
    return d->m_primitiveIndex;
}

uint QRayCasterHit::vertex1Index() const
{
    return d->m_vertex1Index;
}

uint QRayCasterHit::vertex2Index() const
{
    return d->m_vertex2Index;
}

uint QRayCasterHit::vertex3Index() const
{
    return d->m_vertex3Index;
}

void QRayCasterHit::setEntity(Qt3DCore::QEntity *entity) const
{
    // don't detach
    const_cast<QRayCasterHitData *>(d.constData())->m_entity = entity;
}

} // Qt3DRender

QT_END_NAMESPACE
