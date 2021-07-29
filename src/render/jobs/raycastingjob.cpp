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
****************************************************************************/

#include "raycastingjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>
#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/entityvisitor_p.h>
#include <Qt3DRender/private/qabstractraycaster_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;
using namespace Qt3DRender::RayCasting;
using namespace Render;

namespace {

class EntityCasterGatherer : public EntityVisitor
{
public:
    using EntityCasterList = QVector<QPair<Entity *, RayCaster*>>;
    EntityCasterList m_result;

    explicit EntityCasterGatherer(NodeManagers *manager) : EntityVisitor(manager) { setPruneDisabled(true); }

    Operation visit(Entity *entity) override {
        QVector<RayCaster *> components = entity->renderComponents<RayCaster>();
        for (const auto c: qAsConst(components)) {
            if (c->isEnabled())
                m_result.push_back(qMakePair(entity, c));
        }

        return Continue;
    }
};

} // anonymous


class Qt3DRender::Render::RayCastingJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    RayCastingJobPrivate(RayCastingJob *q) : q_ptr(q) { }
    ~RayCastingJobPrivate() override { Q_ASSERT(dispatches.isEmpty()); }

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QVector<QPair<RayCaster *, QAbstractRayCaster::Hits>> dispatches;

    RayCastingJob *q_ptr;
    Q_DECLARE_PUBLIC(RayCastingJob)
};


bool RayCastingJobPrivate::isRequired() const
{
    Q_Q(const RayCastingJob);
    return q->m_castersDirty || q->m_oneEnabledAtLeast;
}

void RayCastingJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    for (auto res: qAsConst(dispatches)) {
        QAbstractRayCaster *node = qobject_cast<QAbstractRayCaster *>(manager->lookupNode(res.first->peerId()));
        if (!node)
            continue;

        QAbstractRayCasterPrivate *d = QAbstractRayCasterPrivate::get(node);
        d->dispatchHits(res.second);

        if (node->runMode() == QAbstractRayCaster::SingleShot) {
            node->setEnabled(false);
            res.first->setEnabled(false);
        }
    }

    dispatches.clear();
}


RayCastingJob::RayCastingJob()
    : AbstractPickingJob(*new RayCastingJobPrivate(this))
    , m_castersDirty(true)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::RayCasting, 0)
}

void RayCastingJob::markCastersDirty()
{
    m_castersDirty = true;
}

bool RayCastingJob::runHelper()
{
    // Quickly look which caster settings we've got
    // NOTE: should not really cached, we're tracking the state of
    //       RayCaster components but not of the parent entities
    if (m_castersDirty) {
        m_castersDirty = false;
        m_oneEnabledAtLeast = false;

        const auto activeHandles = m_manager->rayCasterManager()->activeHandles();
        for (const auto &handle : activeHandles) {
            const auto caster = m_manager->rayCasterManager()->data(handle);
            m_oneEnabledAtLeast |= caster->isEnabled();
            if (m_oneEnabledAtLeast)
                break;
        }
    }

    // bail out early if no caster is enabled
    if (!m_oneEnabledAtLeast)
        return false;

    const bool trianglePickingRequested = (m_renderSettings->pickMethod() & QPickingSettings::TrianglePicking);
    const bool edgePickingRequested = (m_renderSettings->pickMethod() & QPickingSettings::LinePicking);
    const bool pointPickingRequested = (m_renderSettings->pickMethod() & QPickingSettings::PointPicking);
    const bool primitivePickingRequested = pointPickingRequested | edgePickingRequested | trianglePickingRequested;
    const bool frontFaceRequested =
            m_renderSettings->faceOrientationPickingMode() != QPickingSettings::BackFace;
    const bool backFaceRequested =
            m_renderSettings->faceOrientationPickingMode() != QPickingSettings::FrontFace;
    const float pickWorldSpaceTolerance = m_renderSettings->pickWorldSpaceTolerance();

    EntityCasterGatherer gatherer(m_manager);
    gatherer.apply(m_node);
    const EntityCasterGatherer::EntityCasterList &entities = gatherer.m_result;

    PickingUtils::ViewportCameraAreaGatherer vcaGatherer;
    const QVector<PickingUtils::ViewportCameraAreaDetails> vcaDetails = vcaGatherer.gather(m_frameGraphRoot);

    const float sceneRayLength = m_node->worldBoundingVolumeWithChildren()->radius() * 3.f;

    for (const EntityCasterGatherer::EntityCasterList::value_type &pair: entities) {
        QVector<QRay3D> rays;

        switch (pair.second->type()) {
        case QAbstractRayCasterPrivate::WorldSpaceRayCaster:
            rays << QRay3D(Vector3D(pair.second->origin()),
                           Vector3D(pair.second->direction()),
                           pair.second->length() > 0.f ? pair.second->length() : sceneRayLength);
            rays.back().transform(*pair.first->worldTransform());
            break;
        case QAbstractRayCasterPrivate::ScreenScapeRayCaster:
            for (const PickingUtils::ViewportCameraAreaDetails &vca : vcaDetails) {
                auto ray = rayForViewportAndCamera(vca, nullptr, pair.second->position());
                if (ray.isValid())
                    rays << ray;
            }
            break;
        default:
            Q_UNREACHABLE();
        }

        for (const QRay3D &ray: qAsConst(rays)) {
            PickingUtils::HitList sphereHits;
            PickingUtils::HierarchicalEntityPicker entityPicker(ray, false);
            entityPicker.setLayerIds(pair.second->layerIds(), pair.second->filterMode());
            if (entityPicker.collectHits(m_manager, m_node)) {
                if (trianglePickingRequested) {
                    PickingUtils::TriangleCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_frontFaceRequested = frontFaceRequested;
                    gathererFunctor.m_backFaceRequested = backFaceRequested;
                    gathererFunctor.m_manager = m_manager;
                    gathererFunctor.m_ray = ray;
                    gathererFunctor.m_objectPickersRequired = false;
                    sphereHits << gathererFunctor.computeHits(entityPicker.entities(), QPickingSettings::AllPicks);
                }
                if (edgePickingRequested) {
                    PickingUtils::LineCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_manager = m_manager;
                    gathererFunctor.m_ray = ray;
                    gathererFunctor.m_pickWorldSpaceTolerance = pickWorldSpaceTolerance;
                    gathererFunctor.m_objectPickersRequired = false;
                    sphereHits << gathererFunctor.computeHits(entityPicker.entities(), QPickingSettings::AllPicks);
                    PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                }
                if (pointPickingRequested) {
                    PickingUtils::PointCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_manager = m_manager;
                    gathererFunctor.m_ray = ray;
                    gathererFunctor.m_pickWorldSpaceTolerance = pickWorldSpaceTolerance;
                    gathererFunctor.m_objectPickersRequired = false;
                    sphereHits << gathererFunctor.computeHits(entityPicker.entities(), QPickingSettings::AllPicks);
                    PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                }
                if (!primitivePickingRequested) {
                    sphereHits << entityPicker.hits();
                    PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                }
            }

            dispatchHits(pair.second, sphereHits);
        }
    }

    return true;
}

void RayCastingJob::dispatchHits(RayCaster *rayCaster, const PickingUtils::HitList &sphereHits)
{
    QAbstractRayCaster::Hits hits;
    for (const PickingUtils::HitList::value_type &sphereHit: sphereHits) {
        Entity *entity = m_manager->renderNodesManager()->lookupResource(sphereHit.m_entityId);
        Vector3D localIntersection = sphereHit.m_intersection;
        if (entity && entity->worldTransform())
            localIntersection = entity->worldTransform()->inverted() * localIntersection;

        QRayCasterHit::HitType hitType = QRayCasterHit::EntityHit;
        switch (sphereHit.m_type) {
        case RayCasting::QCollisionQueryResult::Hit::Entity:
            break;
        case RayCasting::QCollisionQueryResult::Hit::Triangle:
            hitType = QRayCasterHit::TriangleHit;
            break;
        case RayCasting::QCollisionQueryResult::Hit::Edge:
            hitType = QRayCasterHit::LineHit;
            break;
        case RayCasting::QCollisionQueryResult::Hit::Point:
            hitType = QRayCasterHit::PointHit;
            break;
        default: Q_UNREACHABLE();
        }

        hits << QRayCasterHit{
                hitType,
                sphereHit.m_entityId,
                sphereHit.m_distance,
                convertToQVector3D(localIntersection),
                convertToQVector3D(sphereHit.m_intersection),
                sphereHit.m_primitiveIndex,
                sphereHit.m_vertexIndex[0],
                sphereHit.m_vertexIndex[1],
                sphereHit.m_vertexIndex[2]
        };
    }

    Q_D(RayCastingJob);
    d->dispatches.push_back({rayCaster, hits});
}

QT_END_NAMESPACE
