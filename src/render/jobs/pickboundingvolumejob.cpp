/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "pickboundingvolumejob_p.h"
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/qraycastingservice.h>
#include <Qt3DCore/qservicelocator.h>
#include <Qt3DCore/qray3d.h>
#include <Qt3DCore/qabstractcollisionqueryservice.h>
#include <QSurface>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {

struct ViewportCameraPair
{
    Qt3DCore::QNodeId cameraId;
    QRectF viewport;
};

class ViewportCameraGatherer {

private:
    QVector<FrameGraphNode *> m_leaves;

    void visit(FrameGraphNode *node)
    {
        Q_FOREACH (Render::FrameGraphNode *n, node->children())
            visit(n);
        if (node->childrenHandles().empty())
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

} // anonymous

PickBoundingVolumeJob::PickBoundingVolumeJob(Renderer *renderer)
    : m_renderer(renderer)
    , m_node(Q_NULLPTR)
{
}

void PickBoundingVolumeJob::setRoot(Entity *root)
{
    m_node = root;
}

QVector<Qt3DCore::QBoundingVolume *> PickBoundingVolumeJob::boundingVolumes() const
{
    return gatherBoundingVolumes(m_node);
}

Qt3DCore::QRay3D PickBoundingVolumeJob::intersectionRay(const QPoint &pos, const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix, const QRect &viewport)
{
    // TO DO: We may need to invert the y pos (Qt vs GL coordinates)
    QVector3D nearPos = QVector3D(pos.x(), pos.y(), 0.0f);
    nearPos = nearPos.unproject(viewMatrix, projectionMatrix, viewport);
    QVector3D farPos = QVector3D(pos.x(), pos.y(), 1.0f);
    farPos = farPos.unproject(viewMatrix, projectionMatrix, viewport);

    Qt3DCore::QRay3D ray;
    ray.setOrigin(nearPos);
    ray.setDirection((farPos - nearPos).normalized());
    return ray;
}

void PickBoundingVolumeJob::run()
{
    m_mouseEvents = m_renderer->pendingPickingEvents();
    if (m_mouseEvents.empty())
        return;

    Qt3DCore::QAbstractCollisionQueryService *rayCasting = m_renderer->rendererAspect()->services()->service<Qt3DCore::QAbstractCollisionQueryService>
            (Qt3DCore::QServiceLocator::CollisionService);

    if (rayCasting == Q_NULLPTR) {
        Qt3DRender::QRayCastingService *rayCastingService = new QRayCastingService(this);
        m_renderer->rendererAspect()->services()->registerServiceProvider(Qt3DCore::QServiceLocator::CollisionService, rayCastingService);
        rayCasting = rayCastingService;
    }

    if (rayCasting != Q_NULLPTR) {
        ViewportCameraGatherer vcGatherer;
        const QVector<ViewportCameraPair> vcPairs = vcGatherer.gather(m_renderer->frameGraphRoot());

        if (!vcPairs.empty()) {
            Q_FOREACH (const QMouseEvent &event, m_mouseEvents) {
                Q_FOREACH (const ViewportCameraPair &vc, vcPairs) {
                    const QVector<Qt3DCore::QNodeId> hits = hitsForViewportAndCamera(event.pos(),
                                                                                     vc.viewport,
                                                                                     vc.cameraId,
                                                                                     rayCasting);
                    ObjectPicker *lastCurrentPicker = m_renderer->objectPickerManager()->data(m_currentPicker);

                    // If we have hits
                    if (!hits.isEmpty()) {
                        Q_FOREACH (const Qt3DCore::QNodeId &entityId, hits) {
                            Entity *entity = m_renderer->renderNodesManager()->lookupResource(entityId);
                            HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();
                            ObjectPicker *objectPicker = m_renderer->objectPickerManager()->data(objectPickerHandle);

                            if (objectPicker != Q_NULLPTR) {
                                // Send the corresponding event
                                switch (event.type()) {
                                case QEvent::MouseButtonPress: {
                                    // Store pressed object handle
                                    m_currentPicker = objectPickerHandle;
                                    // Send pressed event to m_currentPicker
                                    objectPicker->onPressed();
                                }
                                    break;

                                case QEvent::MouseButtonRelease: {
                                    // Send release event to m_currentPicker
                                    if (lastCurrentPicker != Q_NULLPTR)
                                        lastCurrentPicker->onReleased();
                                    break;
                                }

                                case Qt::TapGesture: {
                                    objectPicker->onClicked();
                                    break;
                                }

                                case QEvent::MouseMove: {
                                    if (!m_hoveredPickers.contains(objectPickerHandle)) {
                                        // Send exited event to object pickers on which we
                                        // had set the hovered flag
                                        clearPreviouslyHoveredPickers();

                                        if (objectPicker->hoverEnabled()) {
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
                            lastCurrentPicker = m_renderer->objectPickerManager()->data(m_currentPicker);
                        }

                        // Otherwise
                    } else {
                        switch (event.type()) {
                        case QEvent::MouseButtonRelease: {
                            // Send release event to m_currentPicker
                            if (lastCurrentPicker != Q_NULLPTR)
                                lastCurrentPicker->onReleased();
                            break;
                        }
                        case QEvent::MouseMove: {
                            // Send exited event to object pickers on which we
                            // had set the hovered flag
                            clearPreviouslyHoveredPickers();
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }

    // Clear mouse events so that they aren't processed again for the next frame
    m_mouseEvents.clear();
}

QVector<Qt3DCore::QBoundingVolume *> PickBoundingVolumeJob::gatherBoundingVolumes(Entity *entity) const
{
    QVector<Qt3DCore::QBoundingVolume *> volumes;

    if (entity != Q_NULLPTR) {
        volumes.push_back(entity->worldBoundingVolume());
        // Traverse children
        Q_FOREACH (Entity *child, entity->children())
            volumes += gatherBoundingVolumes(child);
    }
    return volumes;
}

void PickBoundingVolumeJob::viewMatrixForCamera(const Qt3DCore::QNodeId &cameraId,
                                                QMatrix4x4 &viewMatrix,
                                                QMatrix4x4 &projectionMatrix) const
{
    Render::CameraLens *lens = Q_NULLPTR;
    Entity *camNode = m_renderer->renderNodesManager()->lookupResource(cameraId);
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
    const QSurface *s = m_renderer->surface();
    if (s) {
        const int surfaceWidth = s->size().width();
        const int surfaceHeight = s->size().height();
        return QRect(relativeViewport.x() * surfaceWidth,
                     (1.0 - relativeViewport.y() - relativeViewport.height()) * surfaceHeight,
                     relativeViewport.width() * surfaceWidth,
                     relativeViewport.height() * surfaceHeight);
    }
    return relativeViewport.toRect();
}


QVector<Qt3DCore::QNodeId> PickBoundingVolumeJob::hitsForViewportAndCamera(const QPoint &pos,
                                                                           const QRectF &relativeViewport,
                                                                           const Qt3DCore::QNodeId &cameraId,
                                                                           Qt3DCore::QAbstractCollisionQueryService *rayCasting) const
{
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    viewMatrixForCamera(cameraId, viewMatrix, projectionMatrix);
    const QRect viewport = windowViewport(relativeViewport);

    const QSurface *s = m_renderer->surface();
    // TO DO: find another way to retrieve the size since this won't work with Scene3D
    // In GL the y is inverted compared to Qt
    const QPoint glCorrectPos = s ? QPoint(pos.x(), s->size().height() - pos.y()) : pos;
    const Qt3DCore::QRay3D ray = intersectionRay(glCorrectPos, viewMatrix, projectionMatrix, viewport);
    const Qt3DCore::QQueryHandle rayCastingHandle = rayCasting->query(ray, Qt3DCore::QAbstractCollisionQueryService::FirstHit);
    const Qt3DCore::QCollisionQueryResult queryResult = rayCasting->fetchResult(rayCastingHandle);
    return queryResult.entitiesHit();
}

void PickBoundingVolumeJob::clearPreviouslyHoveredPickers()
{
    Q_FOREACH (const HObjectPicker &pickHandle, m_hoveredPickers) {
        ObjectPicker *pick = m_renderer->objectPickerManager()->data(pickHandle);
        if (pick)
            pick->onExited();
    }
    m_hoveredPickers.clear();
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

