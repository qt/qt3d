// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qboundingvolume.h"
#include "qboundingvolume_p.h"
#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/calcboundingvolumejob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

bool operator<(const QVector3D &a, const QVector3D &b) {
    return a.x() < b.x() && a.y() < b.y() && a.z() < b.z();
}

QBoundingVolumePrivate::QBoundingVolumePrivate()
    : QComponentPrivate()
    , m_view(nullptr)
    , m_implicitRadius(0.0f)
    , m_implicitPointsValid(false)
    , m_explicitPointsValid(false)
    , m_primaryProvider(true)
{
}

void QBoundingVolumePrivate::setImplicitBounds(const QVector3D &minPoint, const QVector3D &maxPoint,
                                               const QVector3D &center, float radius)
{
    Q_Q(QBoundingVolume);

    if (!minPoint.isNull() && !maxPoint.isNull() && minPoint < maxPoint) {
        if (m_implicitMinPoint != minPoint) {
            m_implicitMinPoint = minPoint;
            emit q->implicitMinPointChanged(m_implicitMinPoint);
        }
        if (m_implicitMaxPoint != maxPoint) {
            m_implicitMaxPoint = maxPoint;
            emit q->implicitMaxPointChanged(m_implicitMaxPoint);
        }
        m_implicitCenter = center;
        m_implicitRadius = radius;
        if (!m_implicitPointsValid) {
            m_implicitPointsValid = true;
            emit q->implicitPointsValidChanged(m_implicitPointsValid);
        }
    } else {
        if (m_implicitPointsValid) {
            m_implicitPointsValid = false;
            emit q->implicitPointsValidChanged(m_implicitPointsValid);
        }
    }
}

void QBoundingVolumePrivate::setView(QGeometryView *view)
{
    Q_Q(QBoundingVolume);

    if (m_view == view)
        return;

    if (m_view)
        unregisterDestructionHelper(m_view);

    if (view && !view->parent())
        view->setParent(q);

    m_view = view;

    // Ensures proper bookkeeping
    if (m_view)
        registerDestructionHelper(m_view, &QBoundingVolume::setView, m_view);

    emit q->viewChanged(view);
}

QBoundingVolumePrivate *QBoundingVolumePrivate::get(QBoundingVolume *q)
{
    return q->d_func();
}

/*!
 * \qmltype BoundingVolume
 * \nativetype Qt3DCore::QBoundingVolume
 * \inqmlmodule Qt3D.Core
 * \since 2.16
 *
 * \brief can be used to override an entity's bounding volume.
 *
 * An entity's bounding volume is used for many operations such as
 * picking or view frustum culling. It is normally computed by
 * traversing the rendered geometry.
 *
 * BoundingVolume can be used when the extent of the geometry is
 * known to the application so that Qt 3D does not have to
 * compute it.
 *
 * A bounding volume can be provided either as minimum and maximum
 * extent coordinates, or a separate, usually simpler, geometry
 * that approximates the rendered mesh.
 *
 * When using minimum and maximum extents, these are considered
 * to be the opposite corners of an axis aligned bounding box,
 * in the geometry's local coordinate system.
 *
 * BoundingVolume can also be used to query the computed bounding
 * volume of a GeometryView. The implicitMinPoint and implicitMaxPoint
 * properties will be updated if the geometry changes. Note that this
 * is done asynchronously on a background thread so you should check
 * the value of implicitPointsValid before reading them.
 *
 * You can force the implicit extents to be updated by calling
 * updateImplicitBounds. This will block on the calling thread until
 * the results are available.
 *
 * \note GeometryRenderer inherits BoundingVolume and thus supports
 * reading implicit bounds or setting explicit bounds also.
 */

/*!
    \class Qt3DCore::QBoundingVolume
    \inheaderfile Qt3DCore/QBoundingVolume
    \inmodule Qt3DCore
    \inherits Qt3DCore::QComponent
    \since 6.0
    \brief can be used to override the bounding volume of an entity.

    An entity's bounding volume is used for many operations such as
    picking or view frustum culling. It is normally computed by
    traversing the rendered geometry.

    QBoundingVolume can be used when the extent of the geometry is
    known to the application so that Qt 3D does not have to
    compute it.

    A bounding volume can be provided either as minimum and maximum
    extent coordinates, or a separate, usually simpler, geometry
    that approximates the rendered mesh.

    When using minimum and maximum extents, these are considered
    to be the opposite corners of an axis aligned bounding box,
    in the geometry's local coordinate system.

    QBoundingVolume can also be used to query the computed bounding
    volume of a GeometryView. The implicitMinPoint and implicitMaxPoint
    properties will be updated if the geometry changes. Note that this
    is done asynchronously on a background thread so you should check
    the value of implicitPointsValid before reading them.

    You can force the implicit extents to be updated by calling
    updateImplicitBounds. This will block on the calling thread until
    the results are available.

    \note GeometryRenderer inherits BoundingVolume and thus supports
    reading implicit bounds or setting explicit bounds also.
*/

/*!
 * Constructs a new QBoundingVolume with \a parent.
 */
QBoundingVolume::QBoundingVolume(QNode *parent)
    : QComponent(*new QBoundingVolumePrivate(), parent)
{
}

/*!
 * \internal
 */
QBoundingVolume::QBoundingVolume(QBoundingVolumePrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
 * \internal
 */
QBoundingVolume::~QBoundingVolume()
{
}

/*!
* \qmlproperty GeometryView BoundingVolume::view
*
* Holds a pointer to the instance of QGeometryView which will be used, if set,
* to compute the bounding volume.
*/
/*!
* \property Qt3DCore::QBoundingVolume::view
*
* Holds a pointer to the instance of QGeometryView which will be used, if set,
* to compute the bounding volume.
*/
QGeometryView *QBoundingVolume::view() const
{
    Q_D(const QBoundingVolume);
    return d->m_view;
}

/*!
* \qmlproperty vector3d BoundingVolume::implicitMinPoint
*
* Holds minimum extent of the bounding volume computed from the specified view.
*/
/*!
* \property Qt3DCore::QBoundingVolume::implicitMinPoint
*
* Holds minimum extent of the bounding volume computed from the specified view.
*/
QVector3D QBoundingVolume::implicitMinPoint() const
{
    Q_D(const QBoundingVolume);
    return d->m_implicitMinPoint;
}

/*!
* \qmlproperty vector3d BoundingVolume::implicitMaxPoint
*
* Holds maximum extent of the bounding volume computed from the specified view.
*/
/*!
* \property Qt3DCore::QBoundingVolume::implicitMaxPoint
*
* Holds maximum extent of the bounding volume computed from the specified view.
*/
QVector3D QBoundingVolume::implicitMaxPoint() const
{
    Q_D(const QBoundingVolume);
    return d->m_implicitMaxPoint;
}

/*!
* \qmlproperty bool BoundingVolume::implicitPointsValid
*
* True if a view has been assigned and the implicit extent properties are up to date.
*
* \sa updateImplicitBounds
*/
/*!
* \property Qt3DCore::QBoundingVolume::implicitPointsValid
*
* True if a view has been assigned and the implicit extent properties are up to date.
*
* \sa updateImplicitBounds
*/
bool QBoundingVolume::areImplicitPointsValid() const
{
    Q_D(const QBoundingVolume);
    return d->m_implicitPointsValid;
}

/*!
* \qmlproperty vector3d BoundingVolume::minPoint
*
* User specified minimum extent of the bounding volume. When set (along with maximum
* extent), this will be used internally to avoid computing the volume from the
* geometry.
*/
/*!
* \property Qt3DCore::QBoundingVolume::minPoint
*
* User specified minimum extent of the bounding volume. When set (along with maximum
* extent), this will be used internally to avoid computing the volume from the
* geometry.
*/
QVector3D QBoundingVolume::minPoint() const
{
    Q_D(const QBoundingVolume);
    return d->m_minPoint;
}

/*!
* \qmlproperty vector3d BoundingVolume::maxPoint
*
* User specified maximum extent of the bounding volume. When set (along with minimum
* extent), this will be used internally to avoid computing the volume from the
* geometry.
*/
/*!
* \property Qt3DCore::QBoundingVolume::maxPoint
*
* User specified maximum extent of the bounding volume. When set (along with minimum
* extent), this will be used internally to avoid computing the volume from the
* geometry.
*/
QVector3D QBoundingVolume::maxPoint() const
{
    Q_D(const QBoundingVolume);
    return d->m_maxPoint;
}

void QBoundingVolume::setView(QGeometryView *view)
{
    Q_D(QBoundingVolume);
    d->setView(view);
}

void QBoundingVolume::setMinPoint(const QVector3D &minPoint)
{
    Q_D(QBoundingVolume);
    if (d->m_minPoint != minPoint) {
        d->m_minPoint = minPoint;
        d->m_explicitPointsValid = true;
        d->markDirty(QScene::GeometryDirty);
        emit minPointChanged(d->m_minPoint);
    }
}

void QBoundingVolume::setMaxPoint(const QVector3D &maxPoint)
{
    Q_D(QBoundingVolume);
    if (d->m_maxPoint != maxPoint) {
        d->m_maxPoint = maxPoint;
        d->m_explicitPointsValid = true;
        d->markDirty(QScene::GeometryDirty);
        emit maxPointChanged(d->m_maxPoint);
    }
}

/*!
* \qmlmethod bool BoundingVolume::updateImplicitBounds()
*
* Updates the implicit bounds of the specified view. Returns
* \c true if the computation succeeded and false otherwise.
*
* \note The calculations are done in the call thread, this
* could take significant time for large meshes.
*/

/*!
* Updates the implicit bounds of the specified view. Returns
* \c true if the computation succeeded and false otherwise.
*
* \note The calculations are done in the call thread, this
* could take significant time for large meshes.
*/
bool QBoundingVolume::updateImplicitBounds()
{
    Q_D(QBoundingVolume);
    if (!d->m_view)
        return false;

    auto data = BoundingVolumeComputeData::fromView(d->m_view);
    if (!data.valid())
        return false;

    auto res = data.compute();
    if (!res.valid())
        return false;

    d->setImplicitBounds(res.m_min, res.m_max, res.m_center, res.m_radius);
    return true;
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qboundingvolume.cpp"
