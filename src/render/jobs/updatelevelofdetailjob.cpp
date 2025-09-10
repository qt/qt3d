// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "updatelevelofdetailjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DRender/QLevelOfDetail>
#include <Qt3DRender/private/entityvisitor_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>

QT_BEGIN_NAMESPACE

namespace
{

template <unsigned N>
double approxRollingAverage(double avg, double input) {
    avg -= avg / N;
    avg += input / N;
    return avg;
}

class LODUpdateVisitor : public Qt3DRender::Render::EntityVisitor
{
public:
    LODUpdateVisitor(double filterValue, Qt3DRender::Render::FrameGraphNode *frameGraphRoot, Qt3DRender::Render::NodeManagers *manager)
        : Qt3DRender::Render::EntityVisitor(manager)
        , m_filterValue(filterValue)
        , m_frameGraphRoot(frameGraphRoot)
    {
        m_updatedIndices.reserve(manager->levelOfDetailManager()->count());
    }

    double filterValue() const { return m_filterValue; }
    const QList<QPair<Qt3DCore::QNodeId, int>> &updatedIndices() const { return m_updatedIndices; }

    Operation visit(Qt3DRender::Render::Entity *entity = nullptr) override {
        using namespace Qt3DRender;
        using namespace Qt3DRender::Render;

        if (!entity->isEnabled())
            return Prune; // skip disabled sub-trees, since their bounding box is probably not valid anyway

        const std::vector<LevelOfDetail *> &lods = entity->renderComponents<LevelOfDetail>();
        if (!lods.empty()) {
            LevelOfDetail* lod = lods.front();  // other lods are ignored

            if (lod->isEnabled() && !lod->thresholds().isEmpty()) {
                switch (lod->thresholdType()) {
                case QLevelOfDetail::DistanceToCameraThreshold:
                    updateEntityLodByDistance(entity, lod);
                    break;
                case QLevelOfDetail::ProjectedScreenPixelSizeThreshold:
                    updateEntityLodByScreenArea(entity, lod);
                    break;
                default:
                    Q_ASSERT(false);
                    break;
                }
            }
        }

        return Continue;
    }

private:
    double m_filterValue = 0.;
    Qt3DRender::Render::FrameGraphNode *m_frameGraphRoot;
    QList<QPair<Qt3DCore::QNodeId, int>> m_updatedIndices;

    void updateEntityLodByDistance(Qt3DRender::Render::Entity *entity, Qt3DRender::Render::LevelOfDetail *lod)
    {
        using namespace Qt3DRender;
        using namespace Qt3DRender::Render;

        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        if (!Render::CameraLens::viewMatrixForCamera(m_manager->renderNodesManager(), lod->camera(), viewMatrix, projectionMatrix))
            return;

        const QList<qreal> thresholds = lod->thresholds();
        Vector3D center(lod->center());
        if (lod->hasBoundingVolumeOverride() || entity->worldBoundingVolume() == nullptr) {
            center = entity->worldTransform()->map(center);
        } else {
            center = entity->worldBoundingVolume()->center();
        }

        const Vector3D tcenter = viewMatrix.map(center);
        const double dist = double(tcenter.length());
        const qsizetype n = thresholds.size();
        for (qsizetype i = 0; i < n; ++i) {
            if (dist <= thresholds[i] || i == n -1) {
                m_filterValue = approxRollingAverage<30>(m_filterValue, i);
                i = qBound(0, static_cast<int>(qRound(m_filterValue)), n - 1);
                if (lod->currentIndex() != i) {
                    lod->setCurrentIndex(i);
                    m_updatedIndices.push_back({lod->peerId(), i});
                }
                break;
            }
        }
    }

    void updateEntityLodByScreenArea(Qt3DRender::Render::Entity *entity, Qt3DRender::Render::LevelOfDetail *lod)
    {
        using namespace Qt3DRender;
        using namespace Qt3DRender::Render;

        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        if (!Render::CameraLens::viewMatrixForCamera(m_manager->renderNodesManager(), lod->camera(), viewMatrix, projectionMatrix))
            return;

        PickingUtils::ViewportCameraAreaGatherer vcaGatherer(lod->camera());
        const std::vector<PickingUtils::ViewportCameraAreaDetails> &vcaTriplets = vcaGatherer.gather(m_frameGraphRoot);
        if (vcaTriplets.empty())
            return;

        const PickingUtils::ViewportCameraAreaDetails &vca = vcaTriplets.front();

        const QList<qreal> thresholds = lod->thresholds();
        Sphere bv(Vector3D(lod->center()), lod->radius());
        if (!lod->hasBoundingVolumeOverride() && entity->worldBoundingVolume() != nullptr) {
            bv = *(entity->worldBoundingVolume());
        } else {
            bv.transform(*entity->worldTransform());
        }

        bv.transform(projectionMatrix * viewMatrix);
        const float sideLength = bv.radius() * 2.f;
        float area = vca.viewport.width() * sideLength * vca.viewport.height() * sideLength;

        const QRect r = windowViewport(vca.area, vca.viewport);
        area =  std::sqrt(area * float(r.width()) * float(r.height()));

        const qsizetype n = thresholds.size();
        for (qsizetype i = 0; i < n; ++i) {
            if (thresholds[i] < area || i == n -1) {
                m_filterValue = approxRollingAverage<30>(m_filterValue, i);
                i = qBound(0, static_cast<int>(qRound(m_filterValue)), n - 1);
                if (lod->currentIndex() != i) {
                    lod->setCurrentIndex(i);
                    m_updatedIndices.push_back({lod->peerId(), i});
                }
                break;
            }
        }
    }

    QRect windowViewport(const QSize &area, const QRectF &relativeViewport) const
    {
        if (area.isValid()) {
            const int areaWidth = area.width();
            const int areaHeight = area.height();
            return QRect(relativeViewport.x() * areaWidth,
                         (1.0 - relativeViewport.y() - relativeViewport.height()) * areaHeight,
                         relativeViewport.width() * areaWidth,
                         relativeViewport.height() * areaHeight);
        }
        return relativeViewport.toRect();
    }
};

}


namespace Qt3DRender {
namespace Render {

class UpdateLevelOfDetailJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    UpdateLevelOfDetailJobPrivate(UpdateLevelOfDetailJob *q) : q_ptr(q) { }

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    QList<QPair<Qt3DCore::QNodeId, int>> m_updatedIndices;

    UpdateLevelOfDetailJob *q_ptr;
    Q_DECLARE_PUBLIC(UpdateLevelOfDetailJob)
};

UpdateLevelOfDetailJob::UpdateLevelOfDetailJob()
    : Qt3DCore::QAspectJob(*new UpdateLevelOfDetailJobPrivate(this))
    , m_manager(nullptr)
    , m_frameGraphRoot(nullptr)
    , m_root(nullptr)
    , m_filterValue(0.)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::UpdateLevelOfDetail, 0)
}

UpdateLevelOfDetailJob::~UpdateLevelOfDetailJob()
{
}

void UpdateLevelOfDetailJob::setRoot(Entity *root)
{
    m_root = root;
}

void UpdateLevelOfDetailJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void UpdateLevelOfDetailJob::setFrameGraphRoot(FrameGraphNode *frameGraphRoot)
{
    m_frameGraphRoot = frameGraphRoot;
}

void UpdateLevelOfDetailJob::run()
{
    Q_D(UpdateLevelOfDetailJob);

    Q_ASSERT(m_frameGraphRoot && m_root && m_manager);

    // short-circuit if no LoDs exist
    if (m_manager->levelOfDetailManager()->count() == 0)
        return;

    LODUpdateVisitor visitor(m_filterValue, m_frameGraphRoot, m_manager);
    visitor.apply(m_root);
    m_filterValue = visitor.filterValue();
    d->m_updatedIndices = visitor.updatedIndices();
}

bool UpdateLevelOfDetailJobPrivate::isRequired() const
{
    Q_Q(const UpdateLevelOfDetailJob);
    return q->m_manager->levelOfDetailManager()->count() > 0;
}

void UpdateLevelOfDetailJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    for (const auto &updatedNode: std::as_const(m_updatedIndices)) {
        QLevelOfDetail *node = qobject_cast<QLevelOfDetail *>(manager->lookupNode(updatedNode.first));
        if (!node)
            continue;

        node->setCurrentIndex(updatedNode.second);
    }
}

} // Render
} // Qt3DRender

QT_END_NAMESPACE
