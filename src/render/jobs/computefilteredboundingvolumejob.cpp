// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "computefilteredboundingvolumejob_p.h"

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

void expandWorldBoundingVolume(NodeManagers *manager,
                               Qt3DRender::Render::Sphere *sphere,
                               Qt3DRender::Render::Entity *node,
                               Qt3DRender::Render::Entity *excludeSubTree)
{
    Qt3DRender::Render::Sphere childSphere(*node->worldBoundingVolume());
    // Go to the nodes that have the most depth
    const auto &childrenHandles = node->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *c = manager->renderNodesManager()->data(handle);
        if (c && c != excludeSubTree)
            expandWorldBoundingVolume(manager, &childSphere, c, excludeSubTree);
    }
    sphere->expandToContain(childSphere);
}

} // namespace

class ComputeFilteredBoundingVolumeJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    ComputeFilteredBoundingVolumeJobPrivate(ComputeFilteredBoundingVolumeJob *job) : Qt3DCore::QAspectJobPrivate(), m_job(job) {}
    ~ComputeFilteredBoundingVolumeJobPrivate() {}

    void postFrame(Qt3DCore::QAspectManager *aspectManager) override;

    ComputeFilteredBoundingVolumeJob *m_job;
    Qt3DRender::Render::Sphere m_sphere;
};

ComputeFilteredBoundingVolumeJob::ComputeFilteredBoundingVolumeJob()
    : Qt3DCore::QAspectJob(*new ComputeFilteredBoundingVolumeJobPrivate(this))
    , m_root(nullptr)
    , m_ignoreSubTree(nullptr)
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::ExpandBoundingVolume, 0)
}

void ComputeFilteredBoundingVolumeJob::setRoot(Entity *root)
{
    m_root = root;
}

void ComputeFilteredBoundingVolumeJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void ComputeFilteredBoundingVolumeJob::ignoreSubTree(Entity *node)
{
    m_ignoreSubTree = node;
}

void ComputeFilteredBoundingVolumeJob::run()
{
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();
    Q_D(ComputeFilteredBoundingVolumeJob);
    d->m_sphere = {};

    if (!m_root)
        return;
    if (!m_ignoreSubTree) {
        d->m_sphere = *m_root->worldBoundingVolumeWithChildren();
        return;
    }

    bool isFilterChildOfRoot = false;
    Entity *parent = m_ignoreSubTree->parent();
    while (parent) {
        if (parent == m_root) {
            isFilterChildOfRoot = true;
            break;
        }
        parent = parent->parent();
    }
    if (!isFilterChildOfRoot) {
        d->m_sphere = *m_root->worldBoundingVolumeWithChildren();
        return;
    }

    expandWorldBoundingVolume(m_manager, &d->m_sphere, m_root, m_ignoreSubTree);

    qCDebug(Jobs) << "Exiting" << Q_FUNC_INFO << QThread::currentThread();
}

void ComputeFilteredBoundingVolumeJob::finished(Qt3DCore::QAspectManager *aspectManager, const Qt3DRender::Render::Sphere &sphere)
{
    Q_UNUSED(aspectManager);
    Q_UNUSED(sphere);
}

void ComputeFilteredBoundingVolumeJobPrivate::postFrame(Qt3DCore::QAspectManager *aspectManager)
{
    m_job->finished(aspectManager, m_sphere);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
