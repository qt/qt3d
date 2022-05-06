/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "filterproximitydistancejob_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/proximityfilter_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/sphere_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {
int instanceCounter = 0;
} // anonymous


FilterProximityDistanceJob::FilterProximityDistanceJob()
    : m_manager(nullptr)
    , m_targetEntity(nullptr)
    , m_distanceThresholdSquared(0.)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::ProximityFiltering, instanceCounter++)
}

FilterProximityDistanceJob::~FilterProximityDistanceJob()
{
    --instanceCounter;
}

bool FilterProximityDistanceJob::isRequired()
{
    return hasProximityFilter();
}

void FilterProximityDistanceJob::run()
{
    Q_ASSERT(m_manager != nullptr);
    m_filteredEntities.clear();

    // Fill m_filteredEntities
    // If no filtering needs to be done, this will be the output value
    // otherwise it will be used as the base list of entities to filter

    if (hasProximityFilter()) {
        selectAllEntities();
        std::vector<Entity *> entitiesToFilter = std::move(m_filteredEntities);
        FrameGraphManager *frameGraphManager = m_manager->frameGraphManager();
        EntityManager *entityManager = m_manager->renderNodesManager();

        for (const Qt3DCore::QNodeId &proximityFilterId : qAsConst(m_proximityFilterIds)) {
            ProximityFilter *proximityFilter = static_cast<ProximityFilter *>(frameGraphManager->lookupNode(proximityFilterId));
            m_targetEntity = entityManager->lookupResource(proximityFilter->entityId());
            m_distanceThresholdSquared = proximityFilter->distanceThreshold();
            m_distanceThresholdSquared *= m_distanceThresholdSquared;

            // We can't filter, select nothings
            if (m_targetEntity == nullptr || m_distanceThresholdSquared <= 0.0f) {
                m_filteredEntities.clear();
                return;
            }
            // Otherwise we filter
            filterEntities(entitiesToFilter);

            // And we make the filtered subset be the list of entities to filter
            // for the next loop
            entitiesToFilter = std::move(m_filteredEntities);
        }
        m_filteredEntities = std::move(entitiesToFilter);
    }

    // sort needed for set_intersection in RenderViewBuilder
    std::sort(m_filteredEntities.begin(), m_filteredEntities.end());
}

void FilterProximityDistanceJob::selectAllEntities()
{
    EntityManager *entityManager = m_manager->renderNodesManager();
    const std::vector<HEntity> &handles = entityManager->activeHandles();

    m_filteredEntities.reserve(handles.size());
    for (const HEntity &handle : handles) {
        Entity *e = entityManager->data(handle);
        m_filteredEntities.push_back(e);
    }
}

void FilterProximityDistanceJob::filterEntities(const std::vector<Entity *> &entitiesToFilter)
{
    const Sphere *target = m_targetEntity->worldBoundingVolumeWithChildren();

    for (Entity *entity : entitiesToFilter) {
        // Note: The target entity is always selected as distance will be 0

        // Retrieve center of bounding volume for entity
        const Sphere *s = entity->worldBoundingVolumeWithChildren();

        // If distance between entity and target is less than threshold, we keep the entity
        if ((s->center() - target->center()).lengthSquared() <= m_distanceThresholdSquared)
            m_filteredEntities.push_back(entity);
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
