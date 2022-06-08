// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "frustumcullingjob_p.h"
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {
int instanceCounter = 0;
} // anonymous

FrustumCullingJob::FrustumCullingJob()
    : Qt3DCore::QAspectJob()
    , m_root(nullptr)
    , m_manager(nullptr)
    , m_active(false)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::FrustumCulling, instanceCounter++)
}

FrustumCullingJob::~FrustumCullingJob()
{
    --instanceCounter;
}

void FrustumCullingJob::run()
{
    // Early return if not activated
    if (!m_active)
        return;

    m_visibleEntities.clear();

    const Plane planes[6] = {
        Plane(m_viewProjection.row(3) + m_viewProjection.row(0)), // Left
        Plane(m_viewProjection.row(3) - m_viewProjection.row(0)), // Right
        Plane(m_viewProjection.row(3) + m_viewProjection.row(1)), // Top
        Plane(m_viewProjection.row(3) - m_viewProjection.row(1)), // Bottom
        Plane(m_viewProjection.row(3) + m_viewProjection.row(2)), // Front
        Plane(m_viewProjection.row(3) - m_viewProjection.row(2)), // Back
    };

    cullScene(m_root, planes);

    // sort needed for set_intersection in RenderViewBuilder
    std::sort(m_visibleEntities.begin(), m_visibleEntities.end());
}

void FrustumCullingJob::cullScene(Entity *e, const Plane *planes)
{
    e->traverse([planes, this](Entity *e) {
        const Sphere *s = e->worldBoundingVolumeWithChildren();

        // Unrolled loop
        if (Vector3D::dotProduct(s->center(), planes[0].normal) + planes[0].d < -s->radius())
            return;
        if (Vector3D::dotProduct(s->center(), planes[1].normal) + planes[1].d < -s->radius())
            return;
        if (Vector3D::dotProduct(s->center(), planes[2].normal) + planes[2].d < -s->radius())
            return;
        if (Vector3D::dotProduct(s->center(), planes[3].normal) + planes[3].d < -s->radius())
            return;
        if (Vector3D::dotProduct(s->center(), planes[4].normal) + planes[4].d < -s->radius())
            return;
        if (Vector3D::dotProduct(s->center(), planes[5].normal) + planes[5].d < -s->radius())
            return;

        m_visibleEntities.push_back(e);
    });
}

bool FrustumCullingJob::isRequired()
{
    return m_active;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
