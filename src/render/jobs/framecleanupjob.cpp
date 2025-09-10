// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "framecleanupjob_p.h"
#include <private/nodemanagers_p.h>
#include <private/entity_p.h>
#include <private/shaderdata_p.h>
#include <private/managers_p.h>
#include <private/sphere_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

FrameCleanupJob::FrameCleanupJob()
    : m_managers(nullptr)
    , m_root(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::FrameCleanup, 0)
}

FrameCleanupJob::~FrameCleanupJob()
{
}

void FrameCleanupJob::setRoot(Entity *root)
{
    m_root = root;
}

void FrameCleanupJob::run()
{
    // Debug bounding volume debug
    updateBoundingVolumesDebug(m_root);
}

void FrameCleanupJob::updateBoundingVolumesDebug(Entity *node)
{
    Q_UNUSED(node);
#if 0
    node->traverse([](Entity *node) {
        BoundingVolumeDebug *debugBV = node->renderComponent<BoundingVolumeDebug>();
        if (debugBV) {
            Qt3DRender::Render::Sphere s;
            if (!debugBV->isRecursive()) {
                s = *node->worldBoundingVolume();
            } else {
                s = *node->worldBoundingVolumeWithChildren();
            }
            debugBV->setRadius(s.radius());
            debugBV->setCenter(s.center());
        }
    });

#endif
}

void FrameCleanupJob::setManagers(NodeManagers *managers)
{
    m_managers = managers;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
