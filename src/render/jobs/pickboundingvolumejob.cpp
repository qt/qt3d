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
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/qray3d.h>
#include <QSurface>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {

struct ViewportCameraPair
{
    Qt3DCore::QNodeId cameraId;
    QRectF viewport;
};

class ViewportCameraGatherer
{
private:
    QVector<FrameGraphNode *> m_leaves;

    void visit(FrameGraphNode *node)
    {
        Q_FOREACH (Render::FrameGraphNode *n, node->children())
            visit(n);
        if (node->childrenIds().empty())
            m_leaves.push_back(node);
    }

    ViewportCameraPair gatherUpViewportCameras(Render::FrameGraphNode *node) const
    {
        ViewportCameraPair vc;
        vc.viewport = QRectF(0.0f, 0.0f, 1.0f, 1.0f);

        while (node) {
            if (node->isEnabled()) {
                switch (node->nodeType()) {
                case FrameGraphNode::CameraSelector:
                    vc.cameraId = static_cast<const CameraSelector *>(node)->cameraUuid();
                    break;
                case FrameGraphNode::Viewport:
                    vc.viewport = computeViewport(vc.viewport, static_cast<const ViewportNode *>(node));
                    break;
                default:
                    break;
                }
            }
            node = node->parent();
        }
        return vc;
    }

public:
    QVector<ViewportCameraPair> gather(FrameGraphNode *root)
    {
        // Retrieve all leaves
        visit(root);
        QVector<ViewportCameraPair> vcPairs;
        vcPairs.reserve(m_leaves.count());

        // Find all viewport/camera pairs by traversing from leaf to root
        Q_FOREACH (Render::FrameGraphNode *leaf, m_leaves) {
            ViewportCameraPair vcPair = gatherUpViewportCameras(leaf);
            if (!vcPair.cameraId.isNull())
                vcPairs.push_back(vcPair);
        }
        return vcPairs;
    }

};

QVector<Entity *> gatherEntities(Entity *entity)
{
    QVector<Entity *> entities;

    if (entity != Q_NULLPTR) {
        entities.push_back(entity);
        // Traverse children
        Q_FOREACH (Entity *child, entity->children())
            entities += gatherEntities(child);
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
            m_entities = gatherEntities(m_root);
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

    CollisionVisitor(NodeManagers* manager, const Entity *root, const Qt3DCore::QRay3D& ray) : TrianglesVisitor(manager), m_root(root), m_ray(ray), m_triangleIndex(0) { }
private:
    const Entity *m_root;
    Qt3DCore::QRay3D m_ray;
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

struct CollisionGathererFunctor
{
    CollisionGathererFunctor() : m_renderer(0) { }
    Renderer *m_renderer;
    Qt3DCore::QRay3D m_ray;

    typedef CollisionVisitor::HitList result_type;

    result_type operator ()(const Entity *entity) const
    {
        result_type result;

        HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();

        // If the Entity which actually received the hit doesn't have
        // an object picker component, we need to check the parent if it has one ...
        while (objectPickerHandle.isNull() && entity != Q_NULLPTR) {
            entity = entity->parent();
            if (entity != Q_NULLPTR)
                objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();
        }

        ObjectPicker *objectPicker = m_renderer->nodeManagers()->objectPickerManager()->data(objectPickerHandle);
        if (objectPicker == Q_NULLPTR)
            return result;  // don't bother picking entities that don't have an object picker

        GeometryRenderer *gRenderer = entity->renderComponent<GeometryRenderer>();
        if (!gRenderer)
            return result;

        Qt3DRender::QRayCastingService rayCasting;

        if (rayHitsEntity(&rayCasting, entity)) {
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
        Q_FOREACH (const CollisionVisitor::HitList::value_type& v, intermediate) {
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
    , m_node(Q_NULLPTR)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::PickBoundingVolume, 0);
}

void PickBoundingVolumeJob::setRoot(Entity *root)
{
    m_node = root;
}

Qt3DCore::QRay3D PickBoundingVolumeJob::intersectionRay(const QPoint &pos, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix, const QRect &viewport)
{
    QVector3D nearPos = QVector3D(pos.x(), pos.y(), 0.0f);
    nearPos = nearPos.unproject(viewMatrix, projectionMatrix, viewport);
    QVector3D farPos = QVector3D(pos.x(), pos.y(), 1.0f);
    farPos = farPos.unproject(viewMatrix, projectionMatrix, viewport);

    return Qt3DCore::QRay3D(nearPos,
                            (farPos - nearPos).normalized(),
                            (farPos - nearPos).length());
}

void PickBoundingVolumeJob::run()
{
    m_mouseEvents = m_renderer->pendingPickingEvents();
    if (m_mouseEvents.empty())
        return;

    ViewportCameraGatherer vcGatherer;
    const QVector<ViewportCameraPair> vcPairs = vcGatherer.gather(m_renderer->frameGraphRoot());

    EntityGatherer entitiesGatherer(m_node);

    if (!vcPairs.empty()) {
        Q_FOREACH (const QMouseEvent &event, m_mouseEvents) {
            m_hoveredPickersToClear = m_hoveredPickers;
            ObjectPicker *lastCurrentPicker = m_manager->objectPickerManager()->data(m_currentPicker);

            Q_FOREACH (const ViewportCameraPair &vc, vcPairs) {
                typedef CollisionGathererFunctor::result_type HitList;
                CollisionGathererFunctor gathererFunctor;
                gathererFunctor.m_renderer = m_renderer;
                gathererFunctor.m_ray = rayForViewportAndCamera(event.pos(), vc.viewport, vc.cameraId);
                HitList sphereHits = QtConcurrent::blockingMappedReduced<HitList>(entitiesGatherer.entities(), gathererFunctor, reduceToAllHits);

                // If we have hits
                if (!sphereHits.isEmpty()) {
                    // Note: how can we control that we want the first/last/all elements along the ray to be picked

                    // How do we differentiate betwnee an Entity with no GeometryRenderer and one with one, both having
                    // an ObjectPicker component when it comes

                    // We want to gather hits against triangles
                    // build a triangle based bounding volume

                    Q_FOREACH (const QCollisionQueryResult::Hit &hit, sphereHits) {
                        Entity *entity = m_manager->renderNodesManager()->lookupResource(hit.m_entityId);
                        HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();

                        // If the Entity which actually received the hit doesn't have
                        // an object picker component, we need to check the parent if it has one ...
                        while (objectPickerHandle.isNull() && entity != Q_NULLPTR) {
                            entity = entity->parent();
                            if (entity != Q_NULLPTR)
                                objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();
                        }

                        ObjectPicker *objectPicker = m_manager->objectPickerManager()->data(objectPickerHandle);

                        if (objectPicker != Q_NULLPTR) {
                            // Send the corresponding event
                            QVector3D localIntersection = hit.m_intersection;
                            if (entity && entity->worldTransform())
                                localIntersection = hit.m_intersection * entity->worldTransform()->inverted();
                            QPickEventPtr pickEvent(new QPickEvent(event.localPos(), hit.m_intersection, localIntersection, hit.m_distance,
                                                                   hit.m_triangleIndex, hit.m_vertexIndex[0], hit.m_vertexIndex[1], hit.m_vertexIndex[2]));

                            switch (event.type()) {
                            case QEvent::MouseButtonPress: {
                                // Store pressed object handle
                                m_currentPicker = objectPickerHandle;
                                // Send pressed event to m_currentPicker
                                objectPicker->onPressed(pickEvent);
                            }
                                break;

                            case QEvent::MouseButtonRelease: {
                                // Send release event to m_currentPicker
                                if (lastCurrentPicker != Q_NULLPTR) {
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
                        if (lastCurrentPicker != Q_NULLPTR) {
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

    // Clear mouse events so that they aren't processed again for the next frame
    m_mouseEvents.clear();
}

void PickBoundingVolumeJob::viewMatrixForCamera(Qt3DCore::QNodeId cameraId,
                                                QMatrix4x4 &viewMatrix,
                                                QMatrix4x4 &projectionMatrix) const
{
    Render::CameraLens *lens = Q_NULLPTR;
    Entity *camNode = m_manager->renderNodesManager()->lookupResource(cameraId);
    if (camNode != Q_NULLPTR &&
            (lens = camNode->renderComponent<CameraLens>()) != Q_NULLPTR &&
            lens->isEnabled()) {
        viewMatrix = *camNode->worldTransform();
        projectionMatrix = lens->projection();
    }
}

QRect PickBoundingVolumeJob::windowViewport(const QRectF &relativeViewport) const
{
    // TO DO: find another way to retrieve the size since this won't work with Scene3D
    // const QSize s = m_renderer->surfaceSize();
    // if (s.isValid()) {
    //     const int surfaceWidth = s.width();
    //     const int surfaceHeight = s.height();
    //     return QRect(relativeViewport.x() * surfaceWidth,
    //                  (1.0 - relativeViewport.y() - relativeViewport.height()) * surfaceHeight,
    //                  relativeViewport.width() * surfaceWidth,
    //                  relativeViewport.height() * surfaceHeight);
    // }
    // return relativeViewport.toRect();
    return QRect();
}

Qt3DCore::QRay3D PickBoundingVolumeJob::rayForViewportAndCamera(const QPoint &pos,
                                                                const QRectF &relativeViewport,
                                                                Qt3DCore::QNodeId cameraId) const
{
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    viewMatrixForCamera(cameraId, viewMatrix, projectionMatrix);
    const QRect viewport = windowViewport(relativeViewport);

    // TO DO: find another way to retrieve the size since this won't work with Scene3D
    // const QSize s = m_renderer->surfaceSize();
    // // In GL the y is inverted compared to Qt
    // const QPoint glCorrectPos = s.isValid() ? QPoint(pos.x(), s.height() - pos.y()) : pos;
    const QPoint glCorrectPos = pos;
    const Qt3DCore::QRay3D ray = intersectionRay(glCorrectPos, viewMatrix, projectionMatrix, viewport);
    return ray;
}

void PickBoundingVolumeJob::clearPreviouslyHoveredPickers()
{
    Q_FOREACH (const HObjectPicker &pickHandle, m_hoveredPickersToClear) {
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
