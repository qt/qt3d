// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qraycasterhit.h"
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRayCasterHitData : public QSharedData
{
public:
    QRayCasterHitData() { }
    QRayCasterHitData(QRayCasterHit::HitType type, Qt3DCore::QNodeId id, float distance,
                      const QVector3D &localIntersect, const QVector3D &worldIntersect,
                      uint primitiveIndex, uint v1 = 0, uint v2 = 0, uint v3 = 0);
    QRayCasterHitData(const QRayCasterHitData& other) : QSharedData(), m_type(other.m_type), m_entityId(other.m_entityId), m_entity(other.m_entity),
                                                        m_distance(other.m_distance), m_localIntersection(other.m_localIntersection),
                                                        m_worldIntersection(other.m_worldIntersection), m_primitiveIndex(other.m_primitiveIndex),
                                                        m_vertex1Index(other.m_vertex1Index), m_vertex2Index(other.m_vertex2Index), m_vertex3Index(other.m_vertex3Index) { }

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

/*!
    \class Qt3DRender::QRayCasterHit
    \brief Details of a hit when casting a ray through a model.
    \inmodule Qt3DRender
    \since 5.11

    Qt3DRender::QRayCasterHit contains the details of a successful hit when casting a ray through
    a model using a Qt3DRender::QRayCaster or Qt3DRender::QScreenRayCaster component.

    \sa Qt3DRender::QRayCaster, Qt3DRender::QScreenRayCaster, Qt3DRender::QPickingSettings
*/

/*!
    \enum QRayCasterHit::HitType

    Specifies type of hit that was returned. This is controlled using QPickingSettings.

    \value TriangleHit The picked primitive was a triangle and the vertex indices refer to the
    three points making up the triangle

    \value LineHit The picked primitive was a line segment, and the first two vertices refer to
    the two points making up the line

    \value PointHit The picked primitive was a single point; all 3 vertex indices
    will be undefined

    \value EntityHit Only the bounding volume was considered; the primitive and vertex indices
    will be undefined
*/

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

/*!
 * \brief Returns the type of the hit
 */
QRayCasterHit::HitType Qt3DRender::QRayCasterHit::type() const
{
    return d->m_type;
}

/*!
 * \brief Returns the id of the entity that was hit
 */
Qt3DCore::QNodeId QRayCasterHit::entityId() const
{
    return d->m_entityId;
}

/*!
 * \brief Returns a pointer to the entity that was hit
 */
Qt3DCore::QEntity *QRayCasterHit::entity() const
{
    return d->m_entity;
}

/*!
 * \brief Returns the distance between the origin of the ray and the intersection point
 */
float QRayCasterHit::distance() const
{
    return d->m_distance;
}

/*!
 * \brief Returns the coordinates of the intersection point in the entity's coordinate system
 */
QVector3D QRayCasterHit::localIntersection() const
{
    return d->m_localIntersection;
}

/*!
 * \brief Returns the coordinates of the intersection point in the model's coordinate system
 */
QVector3D QRayCasterHit::worldIntersection() const
{
    return d->m_worldIntersection;
}

/*!
 * \brief Returns the index of the picked primitive
 */
uint QRayCasterHit::primitiveIndex() const
{
    return d->m_primitiveIndex;
}

/*!
 * \brief Returns the index of the first vertex of the picked primitive
 */
uint QRayCasterHit::vertex1Index() const
{
    return d->m_vertex1Index;
}

/*!
 * \brief Returns the index of the second vertex of the picked primitive
 */
uint QRayCasterHit::vertex2Index() const
{
    return d->m_vertex2Index;
}

/*!
 * \brief Returns the index of the third vertex of the picked primitive
 */
uint QRayCasterHit::vertex3Index() const
{
    return d->m_vertex3Index;
}

QString QRayCasterHit::toString()
{
    QString res;
    if (!d->m_entity)
        return QLatin1String("{}");
    if (d->m_entity->objectName().size())
        res = d->m_entity->objectName();
    else
        res = QLatin1String("Entity");

    res += QString(QLatin1String(" (%1)  Distance: %2  Local: (%3, %4, %5)  World: (%6, %7, %8)"))
               .arg(d->m_entity->id().id()).arg(double(d->m_distance))
               .arg(double(d->m_localIntersection.x())).arg(double(d->m_localIntersection.y())).arg(double(d->m_localIntersection.z()))
               .arg(double(d->m_worldIntersection.x())).arg(double(d->m_worldIntersection.y())).arg(double(d->m_worldIntersection.z()));

    switch (d->m_type) {
    case TriangleHit:
        res += QString(QLatin1String("  Type: Triangle  Index: %1  Vertices: %2 / %3 / %4")).arg(d->m_primitiveIndex).arg(d->m_vertex1Index).arg(d->m_vertex2Index).arg(d->m_vertex3Index);
        break;
    case LineHit:
        res += QString(QLatin1String("  Type: Line  Index: %1  Vertices: %2 / %3")).arg(d->m_primitiveIndex).arg(d->m_vertex1Index).arg(d->m_vertex2Index);
        break;
    case PointHit:
        res += QString(QLatin1String("  Type: Point  Index: %1")).arg(d->m_primitiveIndex);
        break;
    case EntityHit:
        res += QLatin1String("  Type: Entity");
        break;
    }
    return res;
}

/*! \internal */
void QRayCasterHit::setEntity(Qt3DCore::QEntity *entity) const
{
    // don't detach
    const_cast<QRayCasterHitData *>(d.constData())->m_entity = entity;
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qraycasterhit.cpp"
