// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qproximityfilter.h"
#include "qproximityfilter_p.h"
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QProximityFilterPrivate::QProximityFilterPrivate()
    : QFrameGraphNodePrivate()
    , m_entity(nullptr)
    , m_distanceThreshold(0.0f)
{
}

/*!
    \class Qt3DRender::QProximityFilter
    \inmodule Qt3DRender
    \since 5.10

    \brief Select entities which are within a distance threshold of a target
    entity.

    A \l Qt3DRender::QProximityFilter can be used to select entities to render
    when they are placed within a given distance threshold of another entity.
*/

/*!
    \property Qt3DRender::QProximityFilter::entity

    Holds the entity against which we should compare the distance to.
*/

/*!
    \property Qt3DRender::QProximityFilter::distanceThreshold

    Holds the distance to the target entity above which entities are filtered
    out.
*/

/*!
    \qmltype ProximityFilter
    \nativetype Qt3DRender::QProximityFilter
    \inherits FrameGraphNode
    \inqmlmodule Qt3D.Render
    \since 5.10

    \brief Select entities which are within a distance threshold of a target
    entity.

    A \l ProximityFilter can be used to select entities to render
    when they are placed within a given distance threshold of another entity.

    \badcode
    import Qt3DRender 2.10
    ...
    RenderSetting {
        Viewport {
            CameraSelector {
                camera: mainCamera
                ProximityFilter {
                    entity: mainCamera
                    distanceThreshold: 50 // select entities within 50m metre radius of mainCamera
                }
            }
        }
    }
    \endcode
*/

/*!
    \qmlproperty Entity Qt3D.Render::ProximityFilter::entity

    Holds the entity against which we should compare the distance to.
 */

/*!
    \qmlproperty real Qt3D.Render::ProximityFilter::distanceThreshold

    Holds the distance to the target entity above which entities are filtered
    out.
 */


QProximityFilter::QProximityFilter(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QProximityFilterPrivate, parent)
{

}

/*! \internal */
QProximityFilter::QProximityFilter(QProximityFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*! \internal */
QProximityFilter::~QProximityFilter()
{
}

Qt3DCore::QEntity *QProximityFilter::entity() const
{
    Q_D(const QProximityFilter);
    return d->m_entity;
}

float QProximityFilter::distanceThreshold() const
{
    Q_D(const QProximityFilter);
    return d->m_distanceThreshold;
}

void QProximityFilter::setEntity(Qt3DCore::QEntity *entity)
{
    Q_D(QProximityFilter);
    if (d->m_entity != entity) {

        if (d->m_entity)
            d->unregisterDestructionHelper(d->m_entity);

        if (entity && !entity->parent())
            entity->setParent(this);

        d->m_entity = entity;

        if (d->m_entity)
            d->registerDestructionHelper(d->m_entity, &QProximityFilter::setEntity, d->m_entity);

        emit entityChanged(entity);
    }
}

void QProximityFilter::setDistanceThreshold(float distanceThreshold)
{
    Q_D(QProximityFilter);
    if (d->m_distanceThreshold == distanceThreshold)
        return;

    d->m_distanceThreshold = distanceThreshold;
    emit distanceThresholdChanged(distanceThreshold);
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qproximityfilter.cpp"
