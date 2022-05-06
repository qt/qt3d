/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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
