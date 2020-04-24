/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    const QVector<QPair<Qt3DCore::QNodeId, int>> &updatedIndices() const { return m_updatedIndices; }

    Operation visit(Qt3DRender::Render::Entity *entity = nullptr) override {
        using namespace Qt3DRender;
        using namespace Qt3DRender::Render;

        if (!entity->isEnabled())
            return Prune; // skip disabled sub-trees, since their bounding box is probably not valid anyway

        QVector<LevelOfDetail *> lods = entity->renderComponents<LevelOfDetail>();
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
    QVector<QPair<Qt3DCore::QNodeId, int>> m_updatedIndices;

    void updateEntityLodByDistance(Qt3DRender::Render::Entity *entity, Qt3DRender::Render::LevelOfDetail *lod)
    {
        using namespace Qt3DRender;
        using namespace Qt3DRender::Render;

        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        if (!Render::CameraLens::viewMatrixForCamera(m_manager->renderNodesManager(), lod->camera(), viewMatrix, projectionMatrix))
            return;

        const QVector<qreal> thresholds = lod->thresholds();
        Vector3D center(lod->center());
        if (lod->hasBoundingVolumeOverride() || entity->worldBoundingVolume() == nullptr) {
            center = *entity->worldTransform() * center;
        } else {
            center = entity->worldBoundingVolume()->center();
        }

        const Vector3D tcenter = viewMatrix * center;
        const float dist = tcenter.length();
        const int n = thresholds.size();
        for (int i=0; i<n; ++i) {
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
        const QVector<PickingUtils::ViewportCameraAreaDetails> vcaTriplets = vcaGatherer.gather(m_frameGraphRoot);
        if (vcaTriplets.isEmpty())
            return;

        const PickingUtils::ViewportCameraAreaDetails &vca = vcaTriplets.front();

        const QVector<qreal> thresholds = lod->thresholds();
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
        area =  std::sqrt(area * r.width() * r.height());

        const int n = thresholds.size();
        for (int i = 0; i < n; ++i) {
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

    QVector<QPair<Qt3DCore::QNodeId, int>> m_updatedIndices;

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
    for (const auto &updatedNode: qAsConst(m_updatedIndices)) {
        QLevelOfDetail *node = qobject_cast<QLevelOfDetail *>(manager->lookupNode(updatedNode.first));
        if (!node)
            continue;

        node->setCurrentIndex(updatedNode.second);
    }
}

} // Render
} // Qt3DRender

QT_END_NAMESPACE
