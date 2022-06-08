// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "expandboundingvolumejob_p.h"

#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

void expandWorldBoundingVolume(NodeManagers *manager, Entity *node)
{
    // Go to the nodes that have the most depth
    const auto &childrenHandles = node->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *c = manager->renderNodesManager()->data(handle);
        if (c && c->isEnabled())
            expandWorldBoundingVolume(manager, c);
    }

    // Then traverse back from leaf to root
    // Initialize parent bounding volume to be equal to that of the first child
    if (!childrenHandles.empty()) {
        Qt3DRender::Render::Sphere *parentBoundingVolume = node->worldBoundingVolumeWithChildren();
        for (const HEntity &handle : childrenHandles) {
            Entity *c = manager->renderNodesManager()->data(handle);
            if (c && c->isEnabled())
                parentBoundingVolume->expandToContain(*c->worldBoundingVolumeWithChildren());
        }
    }
}

}

ExpandBoundingVolumeJob::ExpandBoundingVolumeJob()
    : m_node(nullptr)
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::ExpandBoundingVolume, 0)
}

void ExpandBoundingVolumeJob::setRoot(Entity *root)
{
    m_node = root;
}

void ExpandBoundingVolumeJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void ExpandBoundingVolumeJob::run()
{
    // Expand worldBoundingVolumeWithChildren of each node that has children by the
    // bounding volumes of the children.

    // TODO: Implement this using a parallel_for
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();
    expandWorldBoundingVolume(m_manager, m_node);
    qCDebug(Jobs) << "Exiting" << Q_FUNC_INFO << QThread::currentThread();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
