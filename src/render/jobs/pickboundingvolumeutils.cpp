/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "pickboundingvolumeutils_p.h"
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/rendersurfaceselector_p.h>
#include <Qt3DRender/private/triangleboundingvolume_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/segmentsvisitor_p.h>
#include <Qt3DRender/private/pointsvisitor_p.h>

#include <vector>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
using namespace Qt3DRender::RayCasting;

namespace Render {

namespace PickingUtils {

void ViewportCameraAreaGatherer::visit(FrameGraphNode *node)
{
    const auto children = node->children();
    for (Render::FrameGraphNode *n : children)
        visit(n);
    if (node->childrenIds().empty())
        m_leaves.push_back(node);
}

ViewportCameraAreaTriplet ViewportCameraAreaGatherer::gatherUpViewportCameraAreas(Render::FrameGraphNode *node) const
{
    ViewportCameraAreaTriplet vca;
    vca.viewport = QRectF(0.0f, 0.0f, 1.0f, 1.0f);

    while (node) {
        if (node->isEnabled()) {
            switch (node->nodeType()) {
            case FrameGraphNode::CameraSelector:
                vca.cameraId = static_cast<const CameraSelector *>(node)->cameraUuid();
                break;
            case FrameGraphNode::Viewport:
                vca.viewport = computeViewport(vca.viewport, static_cast<const ViewportNode *>(node));
                break;
            case FrameGraphNode::Surface:
                vca.area = static_cast<const RenderSurfaceSelector *>(node)->renderTargetSize();
                break;
            default:
                break;
            }
        }
        node = node->parent();
    }
    return vca;
}

QVector<ViewportCameraAreaTriplet> ViewportCameraAreaGatherer::gather(FrameGraphNode *root)
{
    // Retrieve all leaves
    visit(root);
    QVector<ViewportCameraAreaTriplet> vcaTriplets;
    vcaTriplets.reserve(m_leaves.count());

    // Find all viewport/camera pairs by traversing from leaf to root
    for (Render::FrameGraphNode *leaf : qAsConst(m_leaves)) {
        ViewportCameraAreaTriplet vcaTriplet = gatherUpViewportCameraAreas(leaf);
        if (!m_targetCamera.isNull() && vcaTriplet.cameraId != m_targetCamera)
            continue;
        if (!vcaTriplet.cameraId.isNull() && isUnique(vcaTriplets, vcaTriplet))
            vcaTriplets.push_back(vcaTriplet);
    }
    return vcaTriplets;
}

bool ViewportCameraAreaGatherer::isUnique(const QVector<ViewportCameraAreaTriplet> &vcaTriplets,
                                          const ViewportCameraAreaTriplet &vca) const
{
    for (const ViewportCameraAreaTriplet &triplet : vcaTriplets) {
        if (vca.cameraId == triplet.cameraId && vca.viewport == triplet.viewport && vca.area == triplet.area)
            return false;
    }
    return true;
}

QVector<Entity *> gatherEntities(Entity *entity, QVector<Entity *> entities)
{
    if (entity != nullptr && entity->isEnabled()) {
        entities.push_back(entity);
        // Traverse children
        const auto children = entity->children();
        for (Entity *child : children)
            entities = gatherEntities(child, std::move(entities));
    }
    return entities;
}

EntityGatherer::EntityGatherer(Entity *root)
    : m_root(root)
    , m_needsRefresh(true)
{
}

QVector<Entity *> EntityGatherer::entities() const
{
    if (m_needsRefresh) {
        m_entities.clear();
        m_entities = gatherEntities(m_root, std::move(m_entities));
        m_needsRefresh = false;
    }
    return m_entities;
}


class TriangleCollisionVisitor : public TrianglesVisitor
{
public:
    HitList hits;

    TriangleCollisionVisitor(NodeManagers* manager, const Entity *root, const RayCasting::QRay3D& ray,
                     bool frontFaceRequested, bool backFaceRequested)
        : TrianglesVisitor(manager), m_root(root), m_ray(ray), m_triangleIndex(0)
        , m_frontFaceRequested(frontFaceRequested), m_backFaceRequested(backFaceRequested)
    {
    }

private:
    const Entity *m_root;
    RayCasting::QRay3D m_ray;
    uint m_triangleIndex;
    bool m_frontFaceRequested;
    bool m_backFaceRequested;

    void visit(uint andx, const QVector3D &a,
               uint bndx, const QVector3D &b,
               uint cndx, const QVector3D &c) Q_DECL_OVERRIDE;
    bool intersectsSegmentTriangle(uint andx, const QVector3D &a,
                                   uint bndx, const QVector3D &b,
                                   uint cndx, const QVector3D &c);
};

void TriangleCollisionVisitor::visit(uint andx, const QVector3D &a, uint bndx, const QVector3D &b, uint cndx, const QVector3D &c)
{
    const QMatrix4x4 &mat = *m_root->worldTransform();
    const QVector3D tA = mat * a;
    const QVector3D tB = mat * b;
    const QVector3D tC = mat * c;

    bool intersected = m_frontFaceRequested &&
            intersectsSegmentTriangle(cndx, tC, bndx, tB, andx, tA);    // front facing
    if (!intersected && m_backFaceRequested) {
        intersected = intersectsSegmentTriangle(andx, tA, bndx, tB, cndx, tC);    // back facing
    }

    m_triangleIndex++;
}

bool TriangleCollisionVisitor::intersectsSegmentTriangle(uint andx, const QVector3D &a, uint bndx, const QVector3D &b, uint cndx, const QVector3D &c)
{
    float t = 0.0f;
    QVector3D uvw;
    bool intersected = Render::intersectsSegmentTriangle(m_ray, a, b, c, uvw, t);
    if (intersected) {
        QCollisionQueryResult::Hit queryResult;
        queryResult.m_type = QCollisionQueryResult::Hit::Triangle;
        queryResult.m_entityId = m_root->peerId();
        queryResult.m_primitiveIndex = m_triangleIndex;
        queryResult.m_vertexIndex[0] = andx;
        queryResult.m_vertexIndex[1] = bndx;
        queryResult.m_vertexIndex[2] = cndx;
        queryResult.m_uvw = uvw;
        queryResult.m_intersection = m_ray.point(t * m_ray.distance());
        queryResult.m_distance = m_ray.projectedDistance(queryResult.m_intersection);
        hits.push_back(queryResult);
    }
    return intersected;
}

class LineCollisionVisitor : public SegmentsVisitor
{
public:
    HitList hits;

    LineCollisionVisitor(NodeManagers* manager, const Entity *root, const RayCasting::QRay3D& ray,
                         float pickWorldSpaceTolerance)
        : SegmentsVisitor(manager), m_root(root), m_ray(ray)
        , m_segmentIndex(0), m_pickWorldSpaceTolerance(pickWorldSpaceTolerance)
    {
    }

private:
    const Entity *m_root;
    RayCasting::QRay3D m_ray;
    uint m_segmentIndex;
    float m_pickWorldSpaceTolerance;

    void visit(uint andx, const QVector3D &a,
               uint bndx, const QVector3D &b) Q_DECL_OVERRIDE;
    bool intersectsSegmentSegment(uint andx, const QVector3D &a,
                                  uint bndx, const QVector3D &b);
    bool rayToLineSegment(const QVector3D& lineStart,const QVector3D& lineEnd,
                          float &distance, QVector3D &intersection) const;
};

void LineCollisionVisitor::visit(uint andx, const QVector3D &a, uint bndx, const QVector3D &b)
{
    const QMatrix4x4 &mat = *m_root->worldTransform();
    const QVector3D tA = mat * a;
    const QVector3D tB = mat * b;

    intersectsSegmentSegment(andx, tA, bndx, tB);

    m_segmentIndex++;
}

bool LineCollisionVisitor::intersectsSegmentSegment(uint andx, const QVector3D &a,
                                                    uint bndx, const QVector3D &b)
{
    float distance = 0.f;
    QVector3D intersection;
    bool res = rayToLineSegment(a, b, distance, intersection);
    if (res) {
        QCollisionQueryResult::Hit queryResult;
        queryResult.m_type = QCollisionQueryResult::Hit::Edge;
        queryResult.m_entityId = m_root->peerId();
        queryResult.m_primitiveIndex = m_segmentIndex;
        queryResult.m_vertexIndex[0] = andx;
        queryResult.m_vertexIndex[1] = bndx;
        queryResult.m_intersection = intersection;
        queryResult.m_distance = m_ray.projectedDistance(queryResult.m_intersection);
        hits.push_back(queryResult);
        return true;
    }
    return false;
}

bool LineCollisionVisitor::rayToLineSegment(const QVector3D& lineStart,const QVector3D& lineEnd,
                                            float &distance, QVector3D &intersection) const
{
    const float epsilon = 0.00000001f;

    const QVector3D u = m_ray.direction() * m_ray.distance();
    const QVector3D v = lineEnd - lineStart;
    const QVector3D w = m_ray.origin() - lineStart;
    const float a = QVector3D::dotProduct(u, u);
    const float b = QVector3D::dotProduct(u, v);
    const float c = QVector3D::dotProduct(v, v);
    const float d = QVector3D::dotProduct(u, w);
    const float e = QVector3D::dotProduct(v, w);
    const float D = a * c - b * b;
    float sc, sN, sD = D;
    float tc, tN, tD = D;

    if (D < epsilon) {
        sN = 0.0;
        sD = 1.0;
        tN = e;
        tD = c;
    } else {
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (sN < 0.0) {
            sN = 0.0;
            tN = e;
            tD = c;
        }
    }

    if (tN < 0.0) {
        tN = 0.0;
        if (-d < 0.0)
            sN = 0.0;
        else {
            sN = -d;
            sD = a;
        }
    } else if (tN > tD) {
        tN = tD;
        if ((-d + b) < 0.0)
            sN = 0;
        else {
            sN = (-d + b);
            sD = a;
        }
    }

    sc = (qAbs(sN) < epsilon ? 0.0f : sN / sD);
    tc = (qAbs(tN) < epsilon ? 0.0f : tN / tD);

    const QVector3D dP = w + (sc * u) - (tc * v);
    const float f = dP.length();
    if (f < m_pickWorldSpaceTolerance) {
        distance = sc * u.length();
        intersection = lineStart + v * tc;
        return true;
    }
    return false;
}

class PointCollisionVisitor : public PointsVisitor
{
public:
    HitList hits;

    PointCollisionVisitor(NodeManagers* manager, const Entity *root, const RayCasting::QRay3D& ray,
                          float pickWorldSpaceTolerance)
        : PointsVisitor(manager), m_root(root), m_ray(ray)
        , m_pointIndex(0), m_pickWorldSpaceTolerance(pickWorldSpaceTolerance)
    {
    }

private:
    const Entity *m_root;
    RayCasting::QRay3D m_ray;
    uint m_pointIndex;
    float m_pickWorldSpaceTolerance;

    void visit(uint ndx, const QVector3D &p) Q_DECL_OVERRIDE;

    double pointToRayDistance(const QVector3D &a, QVector3D &p)
    {
        const QVector3D v = a - m_ray.origin();
        const double t = QVector3D::dotProduct(v, m_ray.direction());
        p = m_ray.origin() + t * m_ray.direction();
        return (p - a).length();
    }
};


void PointCollisionVisitor::visit(uint ndx, const QVector3D &p)
{
    const QMatrix4x4 &mat = *m_root->worldTransform();
    const QVector3D tP = mat * p;
    QVector3D intersection;

    float d = pointToRayDistance(tP, intersection);
    if (d < m_pickWorldSpaceTolerance) {
        QCollisionQueryResult::Hit queryResult;
        queryResult.m_type = QCollisionQueryResult::Hit::Point;
        queryResult.m_entityId = m_root->peerId();
        queryResult.m_primitiveIndex = m_pointIndex;
        queryResult.m_vertexIndex[0] = ndx;
        queryResult.m_intersection = intersection;
        queryResult.m_distance = d;
        hits.push_back(queryResult);
    }

    m_pointIndex++;
}

HitList reduceToFirstHit(HitList &result, const HitList &intermediate)
{
    if (!intermediate.empty()) {
        if (result.empty())
            result.push_back(intermediate.front());
        float closest = result.front().m_distance;
        for (const auto &v : intermediate) {
            if (v.m_distance < closest) {
                result.push_front(v);
                closest = v.m_distance;
            }
        }

        while (result.size() > 1)
            result.pop_back();
    }
    return result;
}

HitList reduceToAllHits(HitList &results, const HitList &intermediate)
{
    if (!intermediate.empty())
        results << intermediate;
    return results;
}

AbstractCollisionGathererFunctor::AbstractCollisionGathererFunctor()
    : m_manager(nullptr)
{ }

AbstractCollisionGathererFunctor::~AbstractCollisionGathererFunctor()
{ }

HitList AbstractCollisionGathererFunctor::operator ()(const Entity *entity) const
{
    HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();

    // If the Entity which actually received the hit doesn't have
    // an object picker component, we need to check the parent if it has one ...
    auto parentEntity = entity;
    while (objectPickerHandle.isNull() && parentEntity != nullptr) {
        parentEntity = parentEntity->parent();
        if (parentEntity != nullptr)
            objectPickerHandle = parentEntity->componentHandle<ObjectPicker, 16>();
    }

    ObjectPicker *objectPicker = m_manager->objectPickerManager()->data(objectPickerHandle);
    if (objectPicker == nullptr || !objectPicker->isEnabled())
        return {};   // don't bother picking entities that don't
                     // have an object picker, or if it's disabled

    return pick(entity);
}

bool AbstractCollisionGathererFunctor::rayHitsEntity(const Entity *entity) const
{
    QRayCastingService rayCasting;
    const QCollisionQueryResult::Hit queryResult = rayCasting.query(m_ray, entity->worldBoundingVolume());
    return queryResult.m_distance >= 0.f;
}

void AbstractCollisionGathererFunctor::sortHits(HitList &results)
{
    auto compareHitsDistance = [](const HitList::value_type &a,
                                  const HitList::value_type &b) {
        return a.m_distance < b.m_distance;
    };
    std::sort(results.begin(), results.end(), compareHitsDistance);
}

HitList EntityCollisionGathererFunctor::computeHits(const QVector<Entity *> &entities, bool allHitsRequested)
{
    const auto reducerOp = allHitsRequested ? PickingUtils::reduceToAllHits : PickingUtils::reduceToFirstHit;
    return QtConcurrent::blockingMappedReduced<HitList>(entities, *this, reducerOp);
}

HitList EntityCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    QRayCastingService rayCasting;
    const QCollisionQueryResult::Hit queryResult = rayCasting.query(m_ray, entity->worldBoundingVolume());
    if (queryResult.m_distance >= 0.f)
        result.push_back(queryResult);

    return result;
}

HitList TriangleCollisionGathererFunctor::computeHits(const QVector<Entity *> &entities, bool allHitsRequested)
{
    const auto reducerOp = allHitsRequested ? PickingUtils::reduceToAllHits : PickingUtils::reduceToFirstHit;
    return QtConcurrent::blockingMappedReduced<HitList>(entities, *this, reducerOp);
}

HitList TriangleCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
    if (!gRenderer)
        return result;

    if (rayHitsEntity(entity)) {
        TriangleCollisionVisitor visitor(m_manager, entity, m_ray, m_frontFaceRequested, m_backFaceRequested);
        visitor.apply(gRenderer, entity->peerId());
        result = visitor.hits;

        sortHits(result);
    }

    return result;
}

HitList LineCollisionGathererFunctor::computeHits(const QVector<Entity *> &entities, bool allHitsRequested)
{
    const auto reducerOp = allHitsRequested ? PickingUtils::reduceToAllHits : PickingUtils::reduceToFirstHit;
    return QtConcurrent::blockingMappedReduced<HitList>(entities, *this, reducerOp);
}

HitList LineCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
    if (!gRenderer)
        return result;

    if (rayHitsEntity(entity)) {
        LineCollisionVisitor visitor(m_manager, entity, m_ray, m_pickWorldSpaceTolerance);
        visitor.apply(gRenderer, entity->peerId());
        result = visitor.hits;
        sortHits(result);
    }

    return result;
}

HitList PointCollisionGathererFunctor::computeHits(const QVector<Entity *> &entities, bool allHitsRequested)
{
    const auto reducerOp = allHitsRequested ? PickingUtils::reduceToAllHits : PickingUtils::reduceToFirstHit;
    return QtConcurrent::blockingMappedReduced<HitList>(entities, *this, reducerOp);
}

HitList PointCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
    if (!gRenderer)
        return result;

    if (gRenderer->primitiveType() != Qt3DRender::QGeometryRenderer::Points)
        return result;

    if (rayHitsEntity(entity)) {
        PointCollisionVisitor visitor(m_manager, entity, m_ray, m_pickWorldSpaceTolerance);
        visitor.apply(gRenderer, entity->peerId());
        result = visitor.hits;
        sortHits(result);
    }

    return result;
}

HierarchicalEntityPicker::HierarchicalEntityPicker(const QRay3D &ray)
    : m_ray(ray)
{

}

bool HierarchicalEntityPicker::collectHits(Entity *root)
{
    m_hits.clear();
    m_entities.clear();

    QRayCastingService rayCasting;
    std::vector<std::pair<Entity *, bool>> worklist;
    worklist.push_back({root, !root->componentHandle<ObjectPicker, 16>().isNull()});

    while (!worklist.empty()) {
        auto current = worklist.back();
        worklist.pop_back();

        // first pick entry sub-scene-graph
        QCollisionQueryResult::Hit queryResult =
                rayCasting.query(m_ray, current.first->worldBoundingVolumeWithChildren());
        if (queryResult.m_distance < 0.f)
            continue;

        // if we get a hit, we check again for this specific entity
        queryResult = rayCasting.query(m_ray, current.first->worldBoundingVolume());
        if (queryResult.m_distance >= 0.f && current.second) {
            m_entities.push_back(current.first);
            m_hits.push_back(queryResult);
        }

        // and pick children
        for (auto child: current.first->children())
            worklist.push_back({child, current.second || !child->componentHandle<ObjectPicker, 16>().isNull()});
    }

    return !m_hits.empty();
}

} // PickingUtils

} // Render

} // Qt3DRender

QT_END_NAMESPACE
