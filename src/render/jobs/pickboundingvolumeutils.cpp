// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
#include <Qt3DRender/private/layer_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/filterlayerentityjob_p.h>

#include <vector>
#include <algorithm>
#include <functional>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
using namespace Qt3DRender::RayCasting;

namespace Render {

namespace PickingUtils {


PickConfiguration::PickConfiguration(FrameGraphNode *frameGraphRoot, RenderSettings *renderSettings)
{
    ViewportCameraAreaGatherer vcaGatherer;
    // TO DO: We could cache this and only gather when we know the FrameGraph tree has changed
    vcaDetails = vcaGatherer.gather(frameGraphRoot);

    // If we have no viewport / camera or area, return early
    if (vcaDetails.empty())
        return;

    // TO DO:
    // If we have move or hover move events that someone cares about, we try to avoid expensive computations
    // by compressing them into a single one

    trianglePickingRequested = (renderSettings->pickMethod() & QPickingSettings::TrianglePicking);
    edgePickingRequested = (renderSettings->pickMethod() & QPickingSettings::LinePicking);
    pointPickingRequested = (renderSettings->pickMethod() & QPickingSettings::PointPicking);
    primitivePickingRequested = pointPickingRequested | edgePickingRequested | trianglePickingRequested;
    frontFaceRequested = renderSettings->faceOrientationPickingMode() != QPickingSettings::BackFace;
    backFaceRequested = renderSettings->faceOrientationPickingMode() != QPickingSettings::FrontFace;
    pickWorldSpaceTolerance = renderSettings->pickWorldSpaceTolerance();
}


void ViewportCameraAreaGatherer::visit(FrameGraphNode *node)
{
    const auto children = node->children();
    for (Render::FrameGraphNode *n : children)
        visit(n);
    if (node->childrenIds().empty())
        m_leaves.push_back(node);
}

ViewportCameraAreaDetails ViewportCameraAreaGatherer::gatherUpViewportCameraAreas(Render::FrameGraphNode *node) const
{
    ViewportCameraAreaDetails vca;
    vca.viewport = QRectF(0., 0., 1., 1.);

    while (node) {
        if (node->isEnabled()) {
            switch (node->nodeType()) {
            case FrameGraphNode::CameraSelector:
                vca.cameraId = static_cast<const CameraSelector *>(node)->cameraUuid();
                break;
            case FrameGraphNode::Viewport: {
                auto vnode = static_cast<const ViewportNode *>(node);
                // we want the leaf viewport so if we have a viewport node already don't override it with its parent
                if (!vca.viewportNodeId)
                    vca.viewportNodeId = vnode->peerId();
                vca.viewport = ViewportNode::computeViewport(vca.viewport, vnode);
                break;
            }
            case FrameGraphNode::Surface: {
                auto selector = static_cast<const RenderSurfaceSelector *>(node);
                vca.area = selector->renderTargetSize();
                vca.surface = selector->surface();
                break;
            }
            case FrameGraphNode::NoPicking: {
                // Return an empty/invalid ViewportCameraAreaDetails which will
                // prevent picking in the presence of a NoPicking node
                return {};
            }
            case FrameGraphNode::LayerFilter: {
                auto fnode = static_cast<const LayerFilterNode *>(node);
                vca.layersFilters.push_back(fnode->peerId());
                break;
            }
            default:
                break;
            }
        }
        node = node->parent();
    }
    return vca;
}

std::vector<ViewportCameraAreaDetails> ViewportCameraAreaGatherer::gather(FrameGraphNode *root)
{
    // Retrieve all leaves
    visit(root);
    std::vector<ViewportCameraAreaDetails> vcaTriplets;
    vcaTriplets.reserve(m_leaves.size());

    // Find all viewport/camera pairs by traversing from leaf to root
    for (Render::FrameGraphNode *leaf : m_leaves) {
        ViewportCameraAreaDetails vcaDetails = gatherUpViewportCameraAreas(leaf);
        if (!m_targetCamera.isNull() && vcaDetails.cameraId != m_targetCamera)
            continue;
        if (!vcaDetails.cameraId.isNull() && isUnique(vcaTriplets, vcaDetails))
            vcaTriplets.push_back(vcaDetails);
    }
    return vcaTriplets;
}

bool ViewportCameraAreaGatherer::isUnique(const std::vector<ViewportCameraAreaDetails> &vcaList,
                                          const ViewportCameraAreaDetails &vca) const
{
    for (const ViewportCameraAreaDetails &listItem : vcaList) {
        if (vca.cameraId == listItem.cameraId &&
                vca.viewport == listItem.viewport &&
                vca.surface == listItem.surface &&
                vca.area == listItem.area &&
                vca.layersFilters == listItem.layersFilters)
            return false;
    }
    return true;
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
        m_worldMatrix = *m_root->worldTransform();
        m_localRay = m_ray;
        m_localRay.transform(m_worldMatrix.inverted());
    }

private:
    const Entity *m_root;
    RayCasting::QRay3D m_ray;
    RayCasting::QRay3D m_localRay;
    Matrix4x4 m_worldMatrix;
    uint m_triangleIndex;
    bool m_frontFaceRequested;
    bool m_backFaceRequested;

    void visit(uint andx, const Vector3D &a,
               uint bndx, const Vector3D &b,
               uint cndx, const Vector3D &c) override;
    bool intersectsSegmentTriangle(uint andx, const Vector3D &a,
                                   uint bndx, const Vector3D &b,
                                   uint cndx, const Vector3D &c);
};

void TriangleCollisionVisitor::visit(uint andx, const Vector3D &a, uint bndx, const Vector3D &b, uint cndx, const Vector3D &c)
{
    bool intersected = m_frontFaceRequested && intersectsSegmentTriangle(cndx, c, bndx, b, andx, a); // front facing
    if (!intersected && m_backFaceRequested) {
        intersected = intersectsSegmentTriangle(andx, a, bndx, b, cndx, c); // back facing
    }

    m_triangleIndex++;
}

bool TriangleCollisionVisitor::intersectsSegmentTriangle(uint andx, const Vector3D &a, uint bndx, const Vector3D &b, uint cndx, const Vector3D &c)
{
    float t = 0.0f;
    Vector3D uvw;
    bool intersected = Render::intersectsSegmentTriangle(m_localRay, a, b, c, uvw, t);
    if (intersected) {
        QCollisionQueryResult::Hit queryResult;
        queryResult.m_type = QCollisionQueryResult::Hit::Triangle;
        queryResult.m_entityId = m_root->peerId();
        queryResult.m_primitiveIndex = m_triangleIndex;
        queryResult.m_vertexIndex[0] = andx;
        queryResult.m_vertexIndex[1] = bndx;
        queryResult.m_vertexIndex[2] = cndx;
        queryResult.m_uvw = uvw;
        queryResult.m_intersection = m_worldMatrix.map(m_localRay.point(t * m_localRay.distance()));
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

    void visit(uint andx, const Vector3D &a,
               uint bndx, const Vector3D &b) override;
    bool intersectsSegmentSegment(uint andx, const Vector3D &a,
                                  uint bndx, const Vector3D &b);
    bool rayToLineSegment(const Vector3D& lineStart,const Vector3D& lineEnd,
                          float &distance, Vector3D &intersection) const;
};

void LineCollisionVisitor::visit(uint andx, const Vector3D &a, uint bndx, const Vector3D &b)
{
    const Matrix4x4 &mat = *m_root->worldTransform();
    const Vector3D tA = mat.map(a);
    const Vector3D tB = mat.map(b);

    intersectsSegmentSegment(andx, tA, bndx, tB);

    m_segmentIndex++;
}

bool LineCollisionVisitor::intersectsSegmentSegment(uint andx, const Vector3D &a,
                                                    uint bndx, const Vector3D &b)
{
    float distance = 0.f;
    Vector3D intersection;
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

bool LineCollisionVisitor::rayToLineSegment(const Vector3D& lineStart,const Vector3D& lineEnd,
                                            float &distance, Vector3D &intersection) const
{
    const float epsilon = 0.00000001f;

    const Vector3D u = m_ray.direction() * m_ray.distance();
    const Vector3D v = lineEnd - lineStart;
    const Vector3D w = m_ray.origin() - lineStart;
    const float a = Vector3D::dotProduct(u, u);
    const float b = Vector3D::dotProduct(u, v);
    const float c = Vector3D::dotProduct(v, v);
    const float d = Vector3D::dotProduct(u, w);
    const float e = Vector3D::dotProduct(v, w);
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

    const Vector3D dP = w + (sc * u) - (tc * v);
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

    void visit(uint ndx, const Vector3D &p) override;

    double pointToRayDistance(const Vector3D &a, Vector3D &p)
    {
        const Vector3D v = a - m_ray.origin();
        const double t = Vector3D::dotProduct(v, m_ray.direction());
        p = m_ray.origin() + t * m_ray.direction();
        return (p - a).length();
    }
};


void PointCollisionVisitor::visit(uint ndx, const Vector3D &p)
{
    const Matrix4x4 &mat = *m_root->worldTransform();
    const Vector3D tP = mat.map(p);
    Vector3D intersection;

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
                result.insert(result.begin(), v);
                closest = v.m_distance;
            }
        }

        while (result.size() > 1)
            result.pop_back();
    }
    return result;
}


struct HighestPriorityHitReducer
{
    // No need to protect this from concurrent access as the table
    // is read only
    const QHash<Qt3DCore::QNodeId, int> entityToPriorityTable;

    HitList operator()(HitList &result, const HitList &intermediate)
    {
        // Sort by priority first
        // If we have equal priorities, we then sort by distance

        if (!intermediate.empty()) {
            if (result.empty())
                result.push_back(intermediate.front());
            int currentPriority = entityToPriorityTable.value(result.front().m_entityId, 0);
            float closest = result.front().m_distance;

            for (const auto &v : intermediate) {
                const int newEntryPriority = entityToPriorityTable.value(v.m_entityId, 0);
                if (newEntryPriority > currentPriority) {
                    result.insert(result.begin(), v);
                    currentPriority = newEntryPriority;
                    closest = v.m_distance;
                } else if (newEntryPriority == currentPriority) {
                    if (v.m_distance < closest) {
                        result.insert(result.begin(), v);
                        closest = v.m_distance;
                        currentPriority = newEntryPriority;
                    }
                }
            }

            while (result.size() > 1)
                result.pop_back();
        }
        return result;
    }
};

HitList reduceToAllHits(HitList &results, const HitList &intermediate)
{
    if (!intermediate.empty())
        results.insert(results.end(), intermediate.begin(), intermediate.end());
    return results;
}

AbstractCollisionGathererFunctor::AbstractCollisionGathererFunctor()
    : m_manager(nullptr)
{ }

AbstractCollisionGathererFunctor::~AbstractCollisionGathererFunctor()
{ }

HitList AbstractCollisionGathererFunctor::operator ()(const Entity *entity) const
{
    if (m_objectPickersRequired) {
        HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker>();

        // If the Entity which actually received the hit doesn't have
        // an object picker component, we need to check the parent if it has one ...
        auto parentEntity = entity;
        while (objectPickerHandle.isNull() && parentEntity != nullptr) {
            parentEntity = parentEntity->parent();
            if (parentEntity != nullptr)
                objectPickerHandle = parentEntity->componentHandle<ObjectPicker>();
        }

        ObjectPicker *objectPicker = m_manager->objectPickerManager()->data(objectPickerHandle);
        if (objectPicker == nullptr || !objectPicker->isEnabled())
            return {};   // don't bother picking entities that don't
                         // have an object picker, or if it's disabled
    }

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

namespace {

// Workaround to avoid passing *this into the blockMappedReduce calls for the
// mapFunctor which would cause an SSE alignment error on Windows Also note
// that a lambda doesn't work since we need the typedef result_type defined to
// work with QtConcurrent
struct MapFunctorHolder
{
    MapFunctorHolder(const AbstractCollisionGathererFunctor *gatherer)
        : m_gatherer(gatherer)
    {}

    // This define is required to work with QtConcurrent
    typedef HitList result_type;
    HitList operator ()(const Entity *e) const { return m_gatherer->operator ()(e); }

    const AbstractCollisionGathererFunctor *m_gatherer;
};

} // anonymous

HitList EntityCollisionGathererFunctor::computeHits(const std::vector<Entity *> &entities,
                                                    Qt3DRender::QPickingSettings::PickResultMode mode)
{
    std::function<HitList (HitList &, const HitList &)> reducerOp;
    switch (mode) {
    case QPickingSettings::AllPicks:
        reducerOp = PickingUtils::reduceToAllHits;
        break;
    case QPickingSettings::NearestPriorityPick:
        reducerOp = HighestPriorityHitReducer{ m_entityToPriorityTable };
        break;
    case QPickingSettings::NearestPick:
        reducerOp = PickingUtils::reduceToFirstHit;
        break;
    }

    const MapFunctorHolder holder(this);
#if QT_CONFIG(concurrent)
    return QtConcurrent::blockingMappedReduced<HitList>(entities, holder, reducerOp);
#else
    HitList sphereHits;
    QList<PickingUtils::EntityCollisionGathererFunctor::result_type> results;
    for (const Entity *entity : entities)
        sphereHits = reducerOp(sphereHits, holder(entity));
    return sphereHits;
#endif
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

HitList TriangleCollisionGathererFunctor::computeHits(const std::vector<Entity *> &entities,
                                                      Qt3DRender::QPickingSettings::PickResultMode mode)
{
    std::function<HitList (HitList &, const HitList &)> reducerOp;
    switch (mode) {
    case QPickingSettings::AllPicks:
        reducerOp = PickingUtils::reduceToAllHits;
        break;
    case QPickingSettings::NearestPriorityPick:
        reducerOp = HighestPriorityHitReducer { m_entityToPriorityTable };
        break;
    case QPickingSettings::NearestPick:
        reducerOp = PickingUtils::reduceToFirstHit;
        break;
    }

    const MapFunctorHolder holder(this);
#if QT_CONFIG(concurrent)
    return QtConcurrent::blockingMappedReduced<HitList>(entities, holder, reducerOp);
#else
    HitList sphereHits;
    QList<PickingUtils::TriangleCollisionGathererFunctor::result_type> results;
    for (const Entity *entity : entities)
        sphereHits = reducerOp(sphereHits, holder(entity));
        return sphereHits;
#endif
}

HitList TriangleCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    PickingProxy *proxy = entity->renderComponent<PickingProxy>();
    if (proxy && proxy->isEnabled() && proxy->isValid()) {
        if (rayHitsEntity(entity)) {
            TriangleCollisionVisitor visitor(m_manager, entity, m_ray, m_frontFaceRequested, m_backFaceRequested);
            visitor.apply(proxy, entity->peerId());
            result = visitor.hits;

            sortHits(result);
        }
    } else {
        GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
        if (!gRenderer || !gRenderer->isEnabled())
            return result;

        if (rayHitsEntity(entity)) {
            TriangleCollisionVisitor visitor(m_manager, entity, m_ray, m_frontFaceRequested, m_backFaceRequested);
            visitor.apply(gRenderer, entity->peerId());
            result = visitor.hits;

            sortHits(result);
        }
    }

    return result;
}

HitList LineCollisionGathererFunctor::computeHits(const std::vector<Entity *> &entities,
                                                  Qt3DRender::QPickingSettings::PickResultMode mode)
{
    std::function<HitList (HitList &, const HitList &)> reducerOp;
    switch (mode) {
    case QPickingSettings::AllPicks:
        reducerOp = PickingUtils::reduceToAllHits;
        break;
    case QPickingSettings::NearestPriorityPick:
        reducerOp = HighestPriorityHitReducer { m_entityToPriorityTable };
        break;
    case QPickingSettings::NearestPick:
        reducerOp = PickingUtils::reduceToFirstHit;
        break;
    }

    const MapFunctorHolder holder(this);
#if QT_CONFIG(concurrent)
    return QtConcurrent::blockingMappedReduced<HitList>(entities, holder, reducerOp);
#else
    HitList sphereHits;
    QList<PickingUtils::LineCollisionGathererFunctor::result_type> results;
    for (const Entity *entity : entities)
        sphereHits = reducerOp(sphereHits, holder(entity));
    return sphereHits;
#endif
}

HitList LineCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    PickingProxy *proxy = entity->renderComponent<PickingProxy>();
    if (proxy && proxy->isEnabled() && proxy->isValid()) {
        if (rayHitsEntity(entity)) {
            LineCollisionVisitor visitor(m_manager, entity, m_ray, m_pickWorldSpaceTolerance);
            visitor.apply(proxy, entity->peerId());
            result = visitor.hits;

            sortHits(result);
        }
    } else {
        GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
        if (!gRenderer)
            return result;

        if (rayHitsEntity(entity)) {
            LineCollisionVisitor visitor(m_manager, entity, m_ray, m_pickWorldSpaceTolerance);
            visitor.apply(gRenderer, entity->peerId());
            result = visitor.hits;
            sortHits(result);
        }
    }

    return result;
}

HitList PointCollisionGathererFunctor::computeHits(const std::vector<Entity *> &entities,
                                                   Qt3DRender::QPickingSettings::PickResultMode mode)
{
    std::function<HitList (HitList &, const HitList &)> reducerOp;
    switch (mode) {
    case QPickingSettings::AllPicks:
        reducerOp = PickingUtils::reduceToAllHits;
        break;
    case QPickingSettings::NearestPriorityPick:
        reducerOp = HighestPriorityHitReducer { m_entityToPriorityTable };
        break;
    case QPickingSettings::NearestPick:
        reducerOp = PickingUtils::reduceToFirstHit;
        break;
    }

    const MapFunctorHolder holder(this);
#if QT_CONFIG(concurrent)
    return QtConcurrent::blockingMappedReduced<HitList>(entities, holder, reducerOp);
#else
    HitList sphereHits;
    QList<PickingUtils::PointCollisionGathererFunctor::result_type> results;
    for (const Entity *entity : entities)
        sphereHits = reducerOp(sphereHits, holder(entity));
    return sphereHits;
#endif
}

HitList PointCollisionGathererFunctor::pick(const Entity *entity) const
{
    HitList result;

    PickingProxy *proxy = entity->renderComponent<PickingProxy>();
    if (proxy && proxy->isEnabled() && proxy->isValid() && proxy->primitiveType() != Qt3DCore::QGeometryView::Points) {
        if (rayHitsEntity(entity)) {
            PointCollisionVisitor visitor(m_manager, entity, m_ray, m_pickWorldSpaceTolerance);
            visitor.apply(proxy, entity->peerId());
            result = visitor.hits;

            sortHits(result);
        }
    } else {
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
    }

    return result;
}

HierarchicalEntityPicker::HierarchicalEntityPicker(const QRay3D &ray, bool requireObjectPicker)
    : m_ray(ray)
    , m_objectPickersRequired(requireObjectPicker)
{
}

void HierarchicalEntityPicker::setLayerFilterIds(const Qt3DCore::QNodeIdVector &layerFilterIds)
{
    m_layerFilterIds = layerFilterIds;
}

void HierarchicalEntityPicker::setLayerIds(const Qt3DCore::QNodeIdVector &layerIds,
                                           QAbstractRayCaster::FilterMode mode)
{
    m_layerIds = layerIds;
    m_layerFilterMode = mode;
}

bool HierarchicalEntityPicker::collectHits(NodeManagers *manager, Entity *root)
{
    m_hits.clear();
    m_entities.clear();
    m_entityToPriorityTable.clear();

    QRayCastingService rayCasting;
    struct EntityData {
        Entity* entity;
        bool hasObjectPicker;
        int priority;
    };
    std::vector<EntityData> worklist;
    worklist.push_back({root, !root->componentHandle<ObjectPicker>().isNull(), 0});

    // Record all entities that satisfy layerFiltering. We can then check against
    // that to see if a picked Entity also satisfies the layer filtering

    // Note: PickBoundingVolumeJob filters against LayerFilter nodes (FG) whereas
    // the RayCastingJob filters only against a set of Layers and a filter Mode
    const bool hasLayerFilters = !m_layerFilterIds.empty();
    const bool hasLayers = !m_layerIds.empty();
    const bool hasLayerFiltering = hasLayerFilters || hasLayers;
    std::vector<Entity *> layerFilterEntities;
    FilterLayerEntityJob layerFilterJob;
    layerFilterJob.setManager(manager);

    if (hasLayerFilters) {
        // Note: we expect UpdateEntityLayersJob was called beforehand to handle layer recursivness
        // Filtering against LayerFilters (PickBoundingVolumeJob)
        if (!m_layerFilterIds.empty()) {
            layerFilterJob.setLayerFilters(m_layerFilterIds);
            layerFilterJob.run();
            layerFilterEntities = layerFilterJob.filteredEntities();
        }
    }

    while (!worklist.empty()) {
        EntityData current = worklist.back();
        worklist.pop_back();

        // first pick entry sub-scene-graph
        QCollisionQueryResult::Hit queryResult =
                rayCasting.query(m_ray, current.entity->worldBoundingVolumeWithChildren());
        if (queryResult.m_distance < 0.f)
            continue;

        // if we get a hit, we check again for this specific entity
        queryResult = rayCasting.query(m_ray, current.entity->worldBoundingVolume());

        // Check Entity is in selected Layers if we have LayerIds or LayerFilterIds
        // Note: it's not because a parent doesn't satisfy the layerFiltering that a child might not.
        // Therefore we need to keep traversing children in all cases

        // Are we filtering against layerIds (RayCastingJob)
        if (hasLayers) {
            // QLayerFilter::FilterMode and QAbstractRayCaster::FilterMode are the same
            layerFilterJob.filterEntityAgainstLayers(current.entity, m_layerIds, static_cast<QLayerFilter::FilterMode>(m_layerFilterMode));
            layerFilterEntities = layerFilterJob.filteredEntities();
        }

        const bool isInLayers = !hasLayerFiltering || Qt3DCore::contains(layerFilterEntities, current.entity);

        if (isInLayers && queryResult.m_distance >= 0.f && (current.hasObjectPicker || !m_objectPickersRequired)) {
            m_entities.push_back(current.entity);
            m_hits.push_back(queryResult);
            // Record entry for entity/priority
            m_entityToPriorityTable.insert(current.entity->peerId(), current.priority);
        }

        // and pick children
        const auto &childrenHandles = current.entity->childrenHandles();
        for (const HEntity &handle : childrenHandles) {
            Entity *child = manager->renderNodesManager()->data(handle);
            if (child) {
                ObjectPicker *childPicker = child->renderComponent<ObjectPicker>();
                worklist.push_back({child, current.hasObjectPicker || childPicker,
                                    (childPicker ? childPicker->priority() : current.priority)});
            }
        }
    }

    return !m_hits.empty();
}

} // PickingUtils

} // Render

} // Qt3DRender

QT_END_NAMESPACE
