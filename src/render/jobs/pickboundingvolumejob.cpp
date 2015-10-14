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
#include <Qt3DRender/sphere.h>
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

void PickBoundingVolumeJob::setMouseEvents(const QList<QMouseEvent> &mouseEvents)
{
    m_mouseEvents = mouseEvents;
}

QVector<Qt3DCore::QBoundingVolume *> PickBoundingVolumeJob::boundingVolumes() const
{
    return gatherBoundingVolumes(m_node);
}

void PickBoundingVolumeJob::run()
{
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
            // Note we may want to keep all the pressed / hovered nodes
            // so that we know when to call the release / exited events

            Q_FOREACH (const QMouseEvent &event, m_mouseEvents) {
                Q_FOREACH (const ViewportCameraPair &vc, vcPairs) {
                    QMatrix4x4 viewMatrix;
                    QMatrix4x4 projectionMatrix;
                    viewMatrixForCamera(vc.cameraId, viewMatrix, projectionMatrix);
                    const QRect viewport = windowViewport(vc.viewport);

                    QVector3D nearPos = QVector3D(event.pos().x(), event.pos().y(), 0.0f);
                    nearPos.unproject(viewMatrix, projectionMatrix, viewport);
                    QVector3D farPos = QVector3D(event.pos().x(), event.pos().y(), 1.0f);
                    farPos.unproject(viewMatrix, projectionMatrix, viewport);

                    Qt3DCore::QRay3D ray;
                    ray.setOrigin(nearPos);
                    ray.setDirection(farPos);

                    const Qt3DCore::QQueryHandle rayCastingHandle = rayCasting->query(ray, Qt3DCore::QAbstractCollisionQueryService::FirstHit);
                    const Qt3DCore::QCollisionQueryResult queryResult = rayCasting->fetchResult(rayCastingHandle);

                    Q_FOREACH (const Qt3DCore::QNodeId &entityId, queryResult.entitiesHit()) {
                        Entity *entity = m_renderer->renderNodesManager()->lookupResource(entityId);
                        HObjectPicker objectPickerHandle = entity->componentHandle<ObjectPicker, 16>();
                        ObjectPicker *objectPicker = m_renderer->objectPickerManager()->data(objectPickerHandle);
                        if (objectPicker != Q_NULLPTR) {
                            // Send the correspond event
                            switch (event.type()) {
                            case QEvent::MouseButtonPress: {
                                // Store pressed object handle
                                if (objectPickerHandle != m_currentPicker) {
                                    // Send release event to m_currentPicker
                                    m_currentPicker = objectPickerHandle;
                                    // Send pressed event to m_currentPicker
                                }
                                break;
                            }

                            case QEvent::MouseButtonRelease: {
                                // Send release event to m_currentPicker
                                break;
                            }

                            case QEvent::MouseMove: {
                                if (objectPickerHandle != m_currentPicker) {
                                    // Send exited event to m_currentPicker
                                    // if m_currentPick hoverEnabled
                                    m_currentPicker = objectPickerHandle;
                                    // Send entered event to m_currentPicker
                                    // if m_currentPick hoverEnabled
                                } else {
                                    // Send mose event to m_currentPick
                                    // if m_currentPick hoverEnabled
                                }
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
    }

    // Clear mouse events so that they aren't processed again for the next frame
    m_mouseEvents.clear();
}

QVector<Qt3DCore::QBoundingVolume *> PickBoundingVolumeJob::gatherBoundingVolumes(Entity *entity) const
{
    QVector<Qt3DCore::QBoundingVolume *> volumes;

    volumes.push_back(entity->worldBoundingVolume());

    // Traverse children
    Q_FOREACH (Entity *child, m_node->children())
        volumes += gatherBoundingVolumes(child);

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

QRect PickBoundingVolumeJob::windowViewport(const QRectF &relativeViewport)
{
    // TO DO: find another way to retrieve the size since this won't work with Scene3D
    QSurface *s = m_renderer->surface();

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

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

