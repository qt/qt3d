// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

namespace Qt3DRender {
namespace Render {

using namespace Qt3DRender::RayCasting;

namespace {

class EntityCasterGatherer : public EntityVisitor
{
public:
    using EntityCasterList = QList<QPair<Entity *, RayCaster *>>;
    EntityCasterList m_result;
    RayCaster *m_trigger;

    explicit EntityCasterGatherer(NodeManagers *manager, RayCaster *trigger = nullptr)
        : EntityVisitor(manager), m_trigger(trigger) {
        setPruneDisabled(true);
    }

    Operation visit(Entity *entity) override {
        const std::vector<RayCaster *> &components = entity->renderComponents<RayCaster>();
        for (const auto c: components) {
            if ((m_trigger == nullptr && c->isEnabled()) || (m_trigger != nullptr && m_trigger == c))
                m_result.push_back(qMakePair(entity, c));
        }

        return Continue;
    }
};

} // anonymous

class RayCastingJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    RayCastingJobPrivate(RayCastingJob *q) : q_ptr(q) { }
    ~RayCastingJobPrivate() override { Q_ASSERT(dispatches.isEmpty()); }

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<QPair<RayCaster *, QAbstractRayCaster::Hits>> dispatches;

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
    for (auto res: std::as_const(dispatches)) {
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

    EntityCasterGatherer gatherer(m_manager);
    gatherer.apply(m_node);
    const EntityCasterGatherer::EntityCasterList &entities = gatherer.m_result;
    return pick(entities);
}

bool RayCastingJob::pick(const QList<QPair<Entity *, RayCaster *>> &entities)
{
    const PickingUtils::PickConfiguration pickConfiguration(m_frameGraphRoot, m_renderSettings);
    if (pickConfiguration.vcaDetails.empty())
        return false;

    const float sceneRayLength = m_node->worldBoundingVolumeWithChildren()->radius() * 3.f;

    for (const EntityCasterGatherer::EntityCasterList::value_type &pair: entities) {
        std::vector<QRay3D> rays;

        switch (pair.second->type()) {
        case QAbstractRayCasterPrivate::WorldSpaceRayCaster:
            rays.emplace_back(Vector3D(pair.second->origin()),
                              Vector3D(pair.second->direction()),
                              pair.second->length() > 0.f ? pair.second->length() : sceneRayLength);
            rays.back().transform(*pair.first->worldTransform());
            break;
        case QAbstractRayCasterPrivate::ScreenScapeRayCaster:
            for (const PickingUtils::ViewportCameraAreaDetails &vca : pickConfiguration.vcaDetails) {
                const auto ray = rayForViewportAndCamera(vca, nullptr, pair.second->position());
                if (ray.isValid())
                    rays.push_back(ray);
            }
            break;
        default:
            Q_UNREACHABLE();
        }

        for (const QRay3D &ray: std::as_const(rays)) {
            PickingUtils::HitList sphereHits;
            PickingUtils::HierarchicalEntityPicker entityPicker(ray, false);
            entityPicker.setLayerIds(pair.second->layerIds(), pair.second->filterMode());
            if (entityPicker.collectHits(m_manager, m_node)) {
                if (pickConfiguration.trianglePickingRequested) {
                    PickingUtils::TriangleCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_frontFaceRequested = pickConfiguration.frontFaceRequested;
                    gathererFunctor.m_backFaceRequested = pickConfiguration.backFaceRequested;
                    gathererFunctor.m_manager = m_manager;
                    gathererFunctor.m_ray = ray;
                    gathererFunctor.m_objectPickersRequired = false;
                    PickingUtils::HitList hits = gathererFunctor.computeHits(entityPicker.entities(), QPickingSettings::AllPicks);
                    Qt3DCore::moveAtEnd(sphereHits, std::move(hits));
                }
                if (pickConfiguration.edgePickingRequested) {
                    PickingUtils::LineCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_manager = m_manager;
                    gathererFunctor.m_ray = ray;
                    gathererFunctor.m_pickWorldSpaceTolerance = pickConfiguration.pickWorldSpaceTolerance;
                    gathererFunctor.m_objectPickersRequired = false;
                    PickingUtils::HitList hits = gathererFunctor.computeHits(entityPicker.entities(), QPickingSettings::AllPicks);
                    Qt3DCore::moveAtEnd(sphereHits, std::move(hits));
                    PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                }
                if (pickConfiguration.pointPickingRequested) {
                    PickingUtils::PointCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_manager = m_manager;
                    gathererFunctor.m_ray = ray;
                    gathererFunctor.m_pickWorldSpaceTolerance = pickConfiguration.pickWorldSpaceTolerance;
                    gathererFunctor.m_objectPickersRequired = false;
                    PickingUtils::HitList hits = gathererFunctor.computeHits(entityPicker.entities(), QPickingSettings::AllPicks);
                    Qt3DCore::moveAtEnd(sphereHits, std::move(hits));
                    PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                }
                if (!pickConfiguration.primitivePickingRequested) {
                    PickingUtils::HitList hits = entityPicker.hits();
                    Qt3DCore::moveAtEnd(sphereHits, std::move(hits));
                    PickingUtils::AbstractCollisionGathererFunctor::sortHits(sphereHits);
                }
            }

            dispatchHits(pair.second, sphereHits);
        }
    }

    return true;
}

QAbstractRayCaster::Hits RayCastingJob::pick(Qt3DRender::QAbstractRayCaster *rayCaster)
{
    const PickingUtils::PickConfiguration pickConfiguration(m_frameGraphRoot, m_renderSettings);
    if (pickConfiguration.vcaDetails.empty())
        return {};

    auto backendRayCaster = m_manager->rayCasterManager()->lookupResource(rayCaster->id());
    if (!backendRayCaster)
        return {};

    backendRayCaster->syncFromFrontEnd(rayCaster, false);

    EntityCasterGatherer gatherer(m_manager, backendRayCaster);
    gatherer.apply(m_node);
    const EntityCasterGatherer::EntityCasterList &entities = gatherer.m_result;

    if (!pick(entities))
        return {};

    Q_D(RayCastingJob);
    QAbstractRayCaster::Hits res;
    for (const auto &hit: d->dispatches) {
        if (hit.first->peerId() == rayCaster->id()) {
            res = hit.second;
            break;
        }
    }
    d->dispatches.clear();
    return res;
}

void RayCastingJob::dispatchHits(RayCaster *rayCaster, const PickingUtils::HitList &sphereHits)
{
    QAbstractRayCaster::Hits hits;
    for (const PickingUtils::HitList::value_type &sphereHit: sphereHits) {
        Entity *entity = m_manager->renderNodesManager()->lookupResource(sphereHit.m_entityId);
        Vector3D localIntersection = sphereHit.m_intersection;
        if (entity && entity->worldTransform())
            localIntersection = entity->worldTransform()->inverted().map(localIntersection);

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

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
