// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_CAMERALENS_H
#define QT3DRENDER_RENDER_CAMERALENS_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DCore/private/matrix4x4_p.h>
#include <Qt3DRender/private/qcameralens_p.h>
#include <QRectF>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class EntityManager;
class CameraManager;
class Sphere;

class CameraLensFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit CameraLensFunctor(AbstractRenderer *renderer, QRenderAspect *renderAspect);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    CameraManager *m_manager;
    AbstractRenderer *m_renderer;
    QRenderAspect *m_renderAspect;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT CameraLens : public BackendNode
{
public:
    CameraLens();
    ~CameraLens();
    void cleanup();

    void setRenderAspect(QRenderAspect* renderAspect);

    Matrix4x4 viewMatrix(const Matrix4x4 &worldTransform);

    void setProjection(const Matrix4x4 &projection);
    inline Matrix4x4 projection() const { return m_projection; }

    void setExposure(float exposure);
    inline float exposure() const { return m_exposure; }
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void processViewAllResult(Qt3DCore::QAspectManager *aspectManager, const Sphere &sphere, Qt3DCore::QNodeId commandId);

    static bool viewMatrixForCamera(EntityManager *manager, Qt3DCore::QNodeId cameraId,
                                    Matrix4x4 &viewMatrix, Matrix4x4 &projectionMatrix);

private:
    void computeSceneBoundingVolume(Qt3DCore::QNodeId entityId,
                                    Qt3DCore::QNodeId cameraId,
                                    Qt3DCore::QNodeId requestId);

    QRenderAspect *m_renderAspect;
    CameraLensRequest m_pendingViewAllRequest;
    Matrix4x4 m_projection;
    float m_exposure;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_CAMERALENS_H
