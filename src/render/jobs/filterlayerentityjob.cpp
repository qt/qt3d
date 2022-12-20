// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "filterlayerentityjob_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {
int layerFilterJobCounter = 0;
} // anonymous

FilterLayerEntityJob::FilterLayerEntityJob()
    : Qt3DCore::QAspectJob()
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LayerFiltering, layerFilterJobCounter++)
}


void FilterLayerEntityJob::run()
{

    m_filteredEntities.clear();
    if (hasLayerFilter()) // LayerFilter set -> filter
        filterLayerAndEntity();
    else // No LayerFilter set -> retrieve all
        selectAllEntities();

    // sort needed for set_intersection in RenderViewBuilder
    std::sort(m_filteredEntities.begin(), m_filteredEntities.end());
}

void FilterLayerEntityJob::filterEntityAgainstLayers(Entity *entity,
                                                     const Qt3DCore::QNodeIdVector &layerIds,
                                                     const QLayerFilter::FilterMode filterMode)
{
    // Perform filtering
    switch (filterMode) {
    case QLayerFilter::AcceptAnyMatchingLayers: {
        filterAcceptAnyMatchingLayers(entity, layerIds);
        break;
    }
    case QLayerFilter::AcceptAllMatchingLayers: {
        filterAcceptAllMatchingLayers(entity, layerIds);
        break;
    }
    case QLayerFilter::DiscardAnyMatchingLayers: {
        filterDiscardAnyMatchingLayers(entity, layerIds);
        break;
    }
    case QLayerFilter::DiscardAllMatchingLayers: {
        filterDiscardAllMatchingLayers(entity, layerIds);
        break;
    }
    default:
        Q_UNREACHABLE();
    }
}

// We accept the entity if it contains any of the layers that are in the layer filter
void FilterLayerEntityJob::filterAcceptAnyMatchingLayers(Entity *entity,
                                                         const Qt3DCore::QNodeIdVector &layerIds)
{
    const Qt3DCore::QNodeIdVector entityLayers = entity->layerIds();

    for (const Qt3DCore::QNodeId &id : entityLayers) {
        const bool layerAccepted = layerIds.contains(id);

        if (layerAccepted) {
            m_filteredEntities.push_back(entity);
            break;
        }
    }
}

// We accept the entity if it contains all the layers that are in the layer
// filter
void FilterLayerEntityJob::filterAcceptAllMatchingLayers(Entity *entity,
                                                         const Qt3DCore::QNodeIdVector &layerIds)
{
    const Qt3DCore::QNodeIdVector entityLayers = entity->layerIds();
    int layersAccepted = 0;

    for (const Qt3DCore::QNodeId &id : entityLayers) {
        if (layerIds.contains(id))
            ++layersAccepted;
    }

    if (layersAccepted == layerIds.size())
        m_filteredEntities.push_back(entity);
}

// We discard the entity if it contains any of the layers that are in the layer
// filter
// In other words that means we select an entity if one of its layers is not on
// the layer filter
void FilterLayerEntityJob::filterDiscardAnyMatchingLayers(Entity *entity,
                                                          const Qt3DCore::QNodeIdVector &layerIds)
{
    const Qt3DCore::QNodeIdVector entityLayers = entity->layerIds();
    bool entityCanBeDiscarded = false;

    for (const Qt3DCore::QNodeId &id : entityLayers) {
        if (layerIds.contains(id)) {
            entityCanBeDiscarded =  true;
            break;
        }
    }

    if (!entityCanBeDiscarded)
        m_filteredEntities.push_back(entity);
}

// We discard the entity if it contains all of the layers that are in the layer
// filter
// In other words that means we select an entity if none of its layers are on
// the layer filter
void FilterLayerEntityJob::filterDiscardAllMatchingLayers(Entity *entity,
                                                          const Qt3DCore::QNodeIdVector &layerIds)
{
    const Qt3DCore::QNodeIdVector entityLayers = entity->layerIds();

    int containedLayers = 0;

    for (const Qt3DCore::QNodeId &id : layerIds) {
        if (entityLayers.contains(id))
            ++containedLayers;
    }

    if (containedLayers != layerIds.size())
        m_filteredEntities.push_back(entity);
}

void FilterLayerEntityJob::filterLayerAndEntity()
{
    EntityManager *entityManager = m_manager->renderNodesManager();
    const std::vector<HEntity> &handles = entityManager->activeHandles();

    std::vector<Entity *> entitiesToFilter;
    entitiesToFilter.reserve(handles.size());

    for (const HEntity &handle : handles) {
        Entity *entity = entityManager->data(handle);

        if (entity->isTreeEnabled())
            entitiesToFilter.push_back(entity);
    }

    FrameGraphManager *frameGraphManager = m_manager->frameGraphManager();
    LayerManager *layerManager = m_manager->layerManager();

    for (const Qt3DCore::QNodeId &layerFilterId : std::as_const(m_layerFilterIds)) {
        LayerFilterNode *layerFilter = static_cast<LayerFilterNode *>(frameGraphManager->lookupNode(layerFilterId));
        Qt3DCore::QNodeIdVector layerIds = layerFilter->layerIds();

        // Remove layerIds which are not active/enabled
        for (qsizetype i = layerIds.size() - 1; i >= 0; --i) {
            Layer *backendLayer = layerManager->lookupResource(layerIds.at(i));
            if (backendLayer == nullptr || !backendLayer->isEnabled())
                layerIds.removeAt(i);
        }

        const QLayerFilter::FilterMode filterMode = layerFilter->filterMode();

        // Perform filtering
        for (Entity *entity : entitiesToFilter)
            filterEntityAgainstLayers(entity, layerIds, filterMode);

        // Entities to filter for the next frame are the filtered result of the
        // current LayerFilter
        entitiesToFilter = std::move(m_filteredEntities);
    }
    m_filteredEntities = std::move(entitiesToFilter);
}

// No layer filter -> retrieve all entities
void FilterLayerEntityJob::selectAllEntities()
{
    EntityManager *entityManager = m_manager->renderNodesManager();
    const std::vector<HEntity> &handles = entityManager->activeHandles();

    m_filteredEntities.reserve(handles.size());
    for (const HEntity &handle : handles) {
        Entity *e = entityManager->data(handle);
        if (e->isTreeEnabled())
            m_filteredEntities.push_back(e);
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
