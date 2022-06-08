// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_ABSTRACTPICKINGJOB_P_H
#define QT3DRENDER_RENDER_ABSTRACTPICKINGJOB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qboundingvolumeprovider_p.h>
#include <Qt3DRender/private/qcollisionqueryresult_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNodeId;
}

namespace Qt3DRender {
namespace Render {

class Entity;
class Renderer;
class NodeManagers;
class RenderSettings;

class Q_3DRENDERSHARED_PRIVATE_EXPORT AbstractPickingJob : public Qt3DCore::QAspectJob
{
public:
    AbstractPickingJob();

    void setRoot(Entity *root);
    void setFrameGraphRoot(FrameGraphNode *frameGraphRoot);
    void setRenderSettings(RenderSettings *settings);
    void setManagers(NodeManagers *manager);

    // public for unit tests
    virtual bool runHelper() = 0;
    static RayCasting::QRay3D intersectionRay(const QPoint &pos,
                                              const Matrix4x4 &viewMatrix,
                                              const Matrix4x4 &projectionMatrix,
                                              const QRect &viewport);

protected:
    AbstractPickingJob(Qt3DCore::QAspectJobPrivate &dd);

    void run() final;

    NodeManagers *m_manager;
    Entity *m_node;
    FrameGraphNode *m_frameGraphRoot;
    RenderSettings *m_renderSettings;

    bool m_oneEnabledAtLeast;

    QRect windowViewport(const QSize &area, const QRectF &relativeViewport) const;
    RayCasting::QRay3D rayForViewportAndCamera(const PickingUtils::ViewportCameraAreaDetails &vca,
                                               QObject *eventSource,
                                               const QPoint &pos) const;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ABSTRACTPICKINGJOB_P_H
