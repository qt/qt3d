// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updateworldboundingvolumejob_p.h"
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/sphere_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

UpdateWorldBoundingVolumeJob::UpdateWorldBoundingVolumeJob()
    : Qt3DCore::QAspectJob()
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateWorldBoundingVolume, 0)
}

void UpdateWorldBoundingVolumeJob::run()
{
    const std::vector<HEntity> &handles = m_manager->activeHandles();

    for (const HEntity &handle : handles) {
        Entity *node = m_manager->data(handle);
        if (!node->isEnabled())
            continue;
        *(node->worldBoundingVolume()) = node->localBoundingVolume()->transformed(*(node->worldTransform()));
        *(node->worldBoundingVolumeWithChildren()) = *(node->worldBoundingVolume()); // expanded in UpdateBoundingVolumeJob
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
