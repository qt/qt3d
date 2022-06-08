// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updateentitylayersjob_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

UpdateEntityLayersJob::UpdateEntityLayersJob()
    : m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateLayerEntity, 0)

}

void UpdateEntityLayersJob::run()
{
    Q_ASSERT(m_manager);
    EntityManager *entityManager = m_manager->renderNodesManager();

    const std::vector<HEntity> &handles = entityManager->activeHandles();

    // Clear list of recursive layerIds
    for (const HEntity &handle : handles) {
        Entity *entity = entityManager->data(handle);
        entity->clearRecursiveLayerIds();
    }

    LayerManager *layerManager = m_manager->layerManager();

    // Set recursive layerIds on children
    for (const HEntity &handle : handles) {
        Entity *entity = entityManager->data(handle);
        const Qt3DCore::QNodeIdVector entityLayers = entity->componentsUuid<Layer>();

        for (const Qt3DCore::QNodeId &layerId : entityLayers) {
            Layer *layer = layerManager->lookupResource(layerId);
            if (layer->recursive()) {
                // Find all children of the entity and add the layers to them
                entity->traverse([layerId](Entity *e) {
                    e->addRecursiveLayerId(layerId);
                });
            }
        }
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
