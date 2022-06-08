// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "abstractpickingjob_p.h"
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <QtGui/qoffscreensurface.h>
#include <QtGui/qwindow.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

AbstractPickingJob::AbstractPickingJob()
    : Qt3DCore::QAspectJob()
    , m_manager(nullptr)
    , m_node(nullptr)
    , m_frameGraphRoot(nullptr)
    , m_renderSettings(nullptr)
    , m_oneEnabledAtLeast(false)
{
}

AbstractPickingJob::AbstractPickingJob(Qt3DCore::QAspectJobPrivate &dd)
    : Qt3DCore::QAspectJob(dd)
    , m_manager(nullptr)
    , m_node(nullptr)
    , m_frameGraphRoot(nullptr)
    , m_renderSettings(nullptr)
    , m_oneEnabledAtLeast(false)
{

}

void AbstractPickingJob::setRoot(Entity *root)
{
    m_node = root;
}

void AbstractPickingJob::setFrameGraphRoot(FrameGraphNode *frameGraphRoot)
{
    m_frameGraphRoot = frameGraphRoot;
}

void AbstractPickingJob::setRenderSettings(RenderSettings *settings)
{
    m_renderSettings = settings;
}

void AbstractPickingJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void AbstractPickingJob::run()
{
    Q_ASSERT(m_frameGraphRoot && m_renderSettings && m_node && m_manager);
    runHelper();
}

RayCasting::QRay3D AbstractPickingJob::intersectionRay(const QPoint &pos, const Matrix4x4 &viewMatrix,
                                                       const Matrix4x4 &projectionMatrix, const QRect &viewport)
{
    Vector3D nearPos = Vector3D(pos.x(), pos.y(), 0.0f);
    nearPos = nearPos.unproject(viewMatrix, projectionMatrix, viewport);
    Vector3D farPos = Vector3D(pos.x(), pos.y(), 1.0f);
    farPos = farPos.unproject(viewMatrix, projectionMatrix, viewport);

    return RayCasting::QRay3D(nearPos,
                              (farPos - nearPos).normalized(),
                              (farPos - nearPos).length());
}

QRect AbstractPickingJob::windowViewport(const QSize &area, const QRectF &relativeViewport) const
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

RayCasting::QRay3D AbstractPickingJob::rayForViewportAndCamera(const PickingUtils::ViewportCameraAreaDetails &vca,
                                                               QObject *eventSource,
                                                               const QPoint &pos) const
{
    static RayCasting::QRay3D invalidRay({}, {}, 0.f);

    if (!vca.area.isValid())
        return invalidRay;

    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    Render::CameraLens::viewMatrixForCamera(m_manager->renderNodesManager(),
                                            vca.cameraId,
                                            viewMatrix,
                                            projectionMatrix);
    // Returns viewport rect in GL coordinates (y inverted)
    const QRect viewport = windowViewport(vca.area, vca.viewport);
    // In GL the y is inverted compared to Qt
    const QPoint glCorrectPos = QPoint(pos.x(), vca.area.height() - pos.y());

    if (!viewport.contains(glCorrectPos))
        return invalidRay;
    if (vca.surface) {
        QSurface *surface = nullptr;
        if (eventSource) {
            QWindow *window = qobject_cast<QWindow *>(eventSource);
            if (window) {
                surface = static_cast<QSurface *>(window);
            } else {
                QOffscreenSurface *offscreen = qobject_cast<QOffscreenSurface *>(eventSource);
                if (offscreen)
                    surface = static_cast<QSurface *>(offscreen);
            }
        }
        if (surface && vca.surface != surface)
            return invalidRay;
    }

    const auto ray = intersectionRay(glCorrectPos, viewMatrix, projectionMatrix, viewport);
    return ray;
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
