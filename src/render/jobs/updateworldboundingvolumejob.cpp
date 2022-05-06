/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
