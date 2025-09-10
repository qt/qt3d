// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qlevelofdetailboundingsphere.h"
#include <QSharedData>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLevelOfDetailBoundingSpherePrivate: public QSharedData
{
public:
    QLevelOfDetailBoundingSpherePrivate()
        : QSharedData()
        , m_radius(0.0f)
    {}

    QLevelOfDetailBoundingSpherePrivate(QVector3D center, float radius)
        : QSharedData()
        , m_center(center)
        , m_radius(radius)
    {}

    ~QLevelOfDetailBoundingSpherePrivate()
    {}

    QVector3D m_center;
    float m_radius;
};

/*!
    \class Qt3DRender::QLevelOfDetailBoundingSphere
    \inmodule Qt3DRender
    \since 5.9
    \brief The QLevelOfDetailBoundingSphere class provides a simple spherical volume, defined by its center and radius.
*/

/*!
    \qmltype levelOfDetailBoundingSphere
    \nativetype Qt3DRender::QLevelOfDetailBoundingSphere
    \inherits Component3D
    \inqmlmodule Qt3D.Render
    \brief The levelOfDetailBoundingSphere QML type provides a simple spherical volume, defined by its center and radius.
*/

/*!
 * \qmlproperty vector3d levelOfDetailBoundingSphere::center
 *
 * Specifies the center of the bounding sphere
 */

/*!
 * \property Qt3DRender::QLevelOfDetailBoundingSphere::center
 *
 * Specifies the center of the bounding sphere
 */

/*!
 * \qmlproperty real levelOfDetailBoundingSphere::radius
 *
 * Specifies the radius of the bounding sphere
 */

/*!
 * \property Qt3DRender::QLevelOfDetailBoundingSphere::radius
 *
 * Specifies the radius of the bounding sphere
 */

/*!
  Constructs a new QLevelOfDetailBoundingSphere with the specified \a center and \a radius.
 */
QLevelOfDetailBoundingSphere::QLevelOfDetailBoundingSphere(QVector3D center, float radius)
    : d_ptr(new QLevelOfDetailBoundingSpherePrivate(center, radius))
{
}

QLevelOfDetailBoundingSphere::QLevelOfDetailBoundingSphere(const QLevelOfDetailBoundingSphere &other)
    : d_ptr(other.d_ptr)
{
}

QLevelOfDetailBoundingSphere::~QLevelOfDetailBoundingSphere()
{
}

QLevelOfDetailBoundingSphere &QLevelOfDetailBoundingSphere::operator =(const QLevelOfDetailBoundingSphere &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

QVector3D QLevelOfDetailBoundingSphere::center() const
{
    return d_ptr->m_center;
}

float QLevelOfDetailBoundingSphere::radius() const
{
    return d_ptr->m_radius;
}

bool QLevelOfDetailBoundingSphere::isEmpty() const
{
    return d_ptr->m_radius <= 0.0f;
}

bool QLevelOfDetailBoundingSphere::operator ==(const QLevelOfDetailBoundingSphere &other) const
{
    return d_ptr->m_center == other.center() && other.d_ptr->m_radius == other.radius();
}

bool QLevelOfDetailBoundingSphere::operator !=(const QLevelOfDetailBoundingSphere &other) const
{
    return !(*this == other);
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qlevelofdetailboundingsphere.cpp"
