/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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

#include "pickboundingvolumejob_p.h"
#include "qpickevent.h"
#include "qpicktriangleevent.h"
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/triangleboundingvolume_p.h>
#include <Qt3DRender/private/qraycastingservice_p.h>
#include <Qt3DRender/private/rendersurfaceselector_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/rendersurfaceselector_p.h>
#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <QSurface>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

struct ViewportCameraAreaTriplet
{
    Qt3DCore::QNodeId cameraId;
    QRectF viewport;
    QSize area;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, Render, ViewportCameraAreaTriplet, Q_PRIMITIVE_TYPE)

namespace {

class ViewportCameraAreaGatherer
{
private:
    QVector<FrameGraphNode *> m_leaves;

    void visit(FrameGraphNode *node)
    {
        const auto children = node->children();
        for (Render::FrameGraphNode *n : children)
            visit(n);
        if (node->childrenIds().empty())
            m_leaves.push_back(node);
    }

    ViewportCameraAreaTriplet gatherUpViewportCameraAreas(Render::FrameGraphNode *node) const
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

public:
    QVector<ViewportCameraAreaTriplet> gather(FrameGraphNode *root)
    {
        // Retrieve all leaves
        visit(root);
        QVector<ViewportCameraAreaTriplet> vcaTriplets;
        vcaTriplets.reserve(m_leaves.count());

        // Find all viewport/camera pairs by traversing from leaf to root
        for (Render::FrameGraphNode *leaf : qAsConst(m_leaves)) {
            ViewportCameraAreaTriplet vcaTriplet = gatherUpViewportCameraAreas(leaf);
            if (!vcaTriplet.cameraId.isNull())
                vcaTriplets.push_back(vcaTriplet);
        }
        return vcaTriplets;
    }

};

static QVector<Entity *> gatherEntities(Entity *entity, QVector<Entity *> entities)
{
    if (entity != nullptr) {
        entities.push_back(entity);
        // Traverse children
        const auto children = entity->children();
        for (Entity *child : children)
            entities = gatherEntities(child, std::move(entities));
    }
    return entities;
}

class EntityGatherer
{
public:
    explicit EntityGatherer(Entity *root)
        : m_root(root)
        , m_needsRefresh(true)
    {
    }

    QVector<Entity *> entities() const
    {
        if (m_needsRefresh) {
            m_entities.clear();
            m_entities = gatherEntities(m_root, std::move(m_entities));
            m_needsRefresh = false;
        }
        return m_entities;
    }

private:
    Entity *m_root;
    mutable QVector<Entity *> m_entities;
    mutable bool m_needsRefresh;
};

class CollisionVisitor : public TrianglesVisitor
{
public:
    typedef QVector<QCollisionQueryResult::Hit> HitList;
    HitList hits;

    CollisionVisitor(NodeManagers* manager, const Entity *root, const QRay3D& ray) : TrianglesVisitor(manager), m_root(root), m_ray(ray), m_triangleIndex(0) { }
private:
    const Entity *m_root;
    QRay3D m_ray;
    Qt3DRender::QRayCastingService rayCasting;
    uint m_triangleIndex;

    void visit(uint andx, const QVector3D &a,
               uint bndx, const QVector3D &b,
               uint cndx, const QVector3D &c)
    {
        TriangleBoundingVolume volume(m_root->peerId(), a, b, c);
        volume = volume.transform(*m_root->worldTransform());

        QCollisionQueryResult::Hit queryResult = rayCasting.query(m_ray, &volume);
        if (queryResult.m_distance > 0.) {
            queryResult.m_triangleIndex = m_triangleIndex;
            queryResult.m_vertexIndex[0] = andx;
            queryResult.m_vertexIndex[1] = bndx;
            queryResult.m_vertexIndex[2] = cndx;
            hits.push_back(queryResult);
        }

        m_triangleIndex++;
    }
};

struct AbstractCollisionGathererFunctor
{
    AbstractCollisionGathererFunctor() : m_renderer(0) { }
    virtual ~AbstractCollisionGathererFunctor() { }

    Renderer *m_renderer;
    QRay3D m_ray;

    typedef CollisionVisitor::HitList result_type;

    result_type operator ()(const Entity *entity) const
    {
        result_type result;

        HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();

        // If the Entity which actually received the hit doesn't have
        // an object picker component, we need to check the parent if it has one ...
        while (objectPickerHandle.isNull() && entity != nullptr) {
            entity = entity->parent();
            if (entity != nullptr)
                objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();
        }

        ObjectPicker *objectPicker = m_renderer->nodeManagers()->objectPickerManager()->data(objectPickerHandle);
        if (objectPicker == nullptr)
            return result_type();  // don't bother picking entities that don't have an object picker

        Qt3DRender::QRayCastingService rayCasting;

        return pick(&rayCasting, entity);
    }

    virtual result_type pick(QAbstractCollisionQueryService *rayCasting, const Entity *entity) const = 0;
};

struct EntityCollisionGathererFunctor : public AbstractCollisionGathererFunctor
{
    result_type pick(QAbstractCollisionQueryService *rayCasting, const Entity *entity) const Q_DECL_OVERRIDE
    {
        result_type result;

        const QCollisionQueryResult::Hit queryResult = rayCasting->query(m_ray, entity->worldBoundingVolume());
        if (queryResult.m_distance >= 0.f)
            result.push_back(queryResult);

        return result;
    }
};

struct TriangleCollisionGathererFunctor : public AbstractCollisionGathererFunctor
{
    result_type pick(QAbstractCollisionQueryService *rayCasting, const Entity *entity) const Q_DECL_OVERRIDE
    {
        result_type result;

        GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
        if (!gRenderer)
            return result;

        if (rayHitsEntity(rayCasting, entity)) {
            CollisionVisitor visitor(m_renderer->nodeManagers(), entity, m_ray);
            visitor.apply(gRenderer, entity->peerId());
            result = visitor.hits;

            struct
            {
                bool operator()(const result_type::value_type &a, const result_type::value_type &b)
                {
                    return a.m_distance < b.m_distance;
                }
            } compareHitsDistance;
            std::sort(result.begin(), result.end(), compareHitsDistance);
        }

        return result;
    }

    bool rayHitsEntity(QAbstractCollisionQueryService *rayCasting, const Entity *entity) const
    {
        const QCollisionQueryResult::Hit queryResult = rayCasting->query(m_ray, entity->worldBoundingVolume());
        return queryResult.m_distance >= 0.f;
    }
};

CollisionVisitor::HitList reduceToFirstHit(CollisionVisitor::HitList &result, const CollisionVisitor::HitList &intermediate)
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

// Unordered
CollisionVisitor::HitList reduceToAllHits(CollisionVisitor::HitList &results, const CollisionVisitor::HitList &intermediate)
{
    if (!intermediate.empty())
        results << intermediate;
    return results;
}

} // anonymous

PickBoundingVolumeJob::PickBoundingVolumeJob(Renderer *renderer)
    : m_renderer(renderer)
    , m_manager(renderer->nodeManagers())
    , m_node(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::PickBoundingVolume, 0);
}

void PickBoundingVolumeJob::setRoot(Entity *root)
{
    m_node = root;
}

QRay3D PickBoundingVolumeJob::intersectionRay(const QPoint &pos, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix, const QRect &viewport)
{
    QVector3D nearPos = QVector3D(pos.x(), pos.y(), 0.0f);
    nearPos = nearPos.unproject(viewMatrix, projectionMatrix, viewport);
    QVector3D farPos = QVector3D(pos.x(), pos.y(), 1.0f);
    farPos = farPos.unproject(viewMatrix, projectionMatrix, viewport);

    return QRay3D(nearPos,
                  (farPos - nearPos).normalized(),
                  (farPos - nearPos).length());
}

void PickBoundingVolumeJob::run()
{
    const auto mouseEvents = m_renderer->pendingPickingEvents();
    if (mouseEvents.empty())
        return;

    ViewportCameraAreaGatherer vcaGatherer;
    const QVector<ViewportCameraAreaTriplet> vcaTriplets = vcaGatherer.gather(m_renderer->frameGraphRoot());

    EntityGatherer entitiesGatherer(m_node);

    if (!vcaTriplets.empty()) {
        for (const QMouseEvent &event : mouseEvents) {
            m_hoveredPickersToClear = m_hoveredPickers;
            ObjectPicker *lastCurrentPicker = m_manager->objectPickerManager()->data(m_currentPicker);

            for (const ViewportCameraAreaTriplet &vca : vcaTriplets) {
                typedef AbstractCollisionGathererFunctor::result_type HitList;
                HitList sphereHits;
                QRay3D ray = rayForViewportAndCamera(vca.area, event.pos(), vca.viewport, vca.cameraId);
                auto reducerOp = m_renderer->settings() && m_renderer->settings()->pickResultMode() == QPickingSettings::AllPicks ? reduceToAllHits : reduceToFirstHit;
                if (m_renderer->settings() && m_renderer->settings()->pickMethod() == QPickingSettings::TrianglePicking) {
                    TriangleCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_renderer = m_renderer;
                    gathererFunctor.m_ray = ray;
                    sphereHits = QtConcurrent::blockingMappedReduced<HitList>(entitiesGatherer.entities(), gathererFunctor, reducerOp);
                } else {
                    EntityCollisionGathererFunctor gathererFunctor;
                    gathererFunctor.m_renderer = m_renderer;
                    gathererFunctor.m_ray = ray;
                    sphereHits = QtConcurrent::blockingMappedReduced<HitList>(entitiesGatherer.entities(), gathererFunctor, reducerOp);
                }

                // If we have hits
                if (!sphereHits.isEmpty()) {
                    // Note: how can we control that we want the first/last/all elements along the ray to be picked

                    // How do we differentiate betwnee an Entity with no GeometryRenderer and one with one, both having
                    // an ObjectPicker component when it comes

                    // We want to gather hits against triangles
                    // build a triangle based bounding volume

                    for (const QCollisionQueryResult::Hit &hit : qAsConst(sphereHits)) {
                        Entity *entity = m_manager->renderNodesManager()->lookupResource(hit.m_entityId);
                        HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();

                        // If the Entity which actually received the hit doesn't have
                        // an object picker component, we need to check the parent if it has one ...
                        while (objectPickerHandle.isNull() && entity != nullptr) {
                            entity = entity->parent();
                            if (entity != nullptr)
                                objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();
                        }

                        ObjectPicker *objectPicker = m_manager->objectPickerManager()->data(objectPickerHandle);

                        if (objectPicker != nullptr) {
                            // Send the corresponding event
                            QVector3D localIntersection = hit.m_intersection;
                            if (entity && entity->worldTransform())
                                localIntersection = hit.m_intersection * entity->worldTransform()->inverted();
                            QPickEventPtr pickEvent;
                            if (m_renderer->settings() && m_renderer->settings()->pickMethod() == QPickingSettings::TrianglePicking) {
                                pickEvent.reset(new QPickTriangleEvent(event.localPos(), hit.m_intersection, localIntersection, hit.m_distance,
                                                                       hit.m_triangleIndex, hit.m_vertexIndex[0], hit.m_vertexIndex[1], hit.m_vertexIndex[2]));
                            } else {
                                pickEvent.reset(new QPickEvent(event.localPos(), hit.m_intersection, localIntersection, hit.m_distance));
                            }

                            switch (event.type()) {
                            case QEvent::MouseButtonPress: {
                                // Store pressed object handle
                                m_currentPicker = objectPickerHandle;
                                // Send pressed event to m_currentPicker
                                objectPicker->onPressed(pickEvent);
                                break;
                            }

                            case QEvent::MouseButtonRelease: {
                                // Send release event to m_currentPicker
                                if (lastCurrentPicker != nullptr) {
                                    m_currentPicker = HObjectPicker();
                                    lastCurrentPicker->onClicked(pickEvent);
                                    lastCurrentPicker->onReleased(pickEvent);
                                }
                                break;
                            }

                            case Qt::TapGesture: {
                                objectPicker->onClicked(pickEvent);
                                break;
                            }

                            case QEvent::MouseMove: {
                                if (objectPicker->isPressed() && objectPicker->isDragEnabled()) {
                                    objectPicker->onMoved(pickEvent);
                                }
                                // fallthrough
                            }
                            case QEvent::HoverMove: {
                                if (!m_hoveredPickers.contains(objectPickerHandle)) {
                                    if (objectPicker->isHoverEnabled()) {
                                        // Send entered event to objectPicker
                                        objectPicker->onEntered();
                                        // and save it in the hoveredPickers
                                        m_hoveredPickers.push_back(objectPickerHandle);
                                    }
                                }
                                break;
                            }

                            default:
                                break;
                            }
                        }

                        // The ObjectPicker was hit -> it is still being hovered
                        m_hoveredPickersToClear.removeAll(objectPickerHandle);

                        lastCurrentPicker = m_manager->objectPickerManager()->data(m_currentPicker);
                    }

                    // Otherwise no hits
                } else {
                    switch (event.type()) {
                    case QEvent::MouseButtonRelease: {
                        // Send release event to m_currentPicker
                        if (lastCurrentPicker != nullptr) {
                            m_currentPicker = HObjectPicker();
                            QPickEventPtr pickEvent(new QPickEvent);
                            lastCurrentPicker->onReleased(pickEvent);
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }

        // Clear Hovered elements that needs to be cleared
        // Send exit event to object pickers on which we
        // had set the hovered flag for a previous frame
        // and that aren't being hovered any longer
        clearPreviouslyHoveredPickers();
    }
}

void PickBoundingVolumeJob::viewMatrixForCamera(Qt3DCore::QNodeId cameraId,
                                                QMatrix4x4 &viewMatrix,
                                                QMatrix4x4 &projectionMatrix) const
{
    Render::CameraLens *lens = nullptr;
    Entity *camNode = m_manager->renderNodesManager()->lookupResource(cameraId);
    if (camNode != nullptr &&
            (lens = camNode->renderComponent<CameraLens>()) != nullptr &&
            lens->isEnabled()) {
        viewMatrix = *camNode->worldTransform();
        projectionMatrix = lens->projection();
    }
}

QRect PickBoundingVolumeJob::windowViewport(const QSize &area, const QRectF &relativeViewport) const
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

QRay3D PickBoundingVolumeJob::rayForViewportAndCamera(const QSize &area,
                                                      const QPoint &pos,
                                                      const QRectF &relativeViewport,
                                                      Qt3DCore::QNodeId cameraId) const
{
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    viewMatrixForCamera(cameraId, viewMatrix, projectionMatrix);
    const QRect viewport = windowViewport(area, relativeViewport);

    // In GL the y is inverted compared to Qt
    const QPoint glCorrectPos = QPoint(pos.x(), area.isValid() ? area.height() - pos.y() : pos.y());
    const QRay3D ray = intersectionRay(glCorrectPos, viewMatrix, projectionMatrix, viewport);
    return ray;
}

void PickBoundingVolumeJob::clearPreviouslyHoveredPickers()
{
    for (const HObjectPicker pickHandle : qAsConst(m_hoveredPickersToClear)) {
        ObjectPicker *pick = m_manager->objectPickerManager()->data(pickHandle);
        if (pick)
            pick->onExited();
        m_hoveredPickers.removeAll(pickHandle);
    }
    m_hoveredPickersToClear.clear();
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
