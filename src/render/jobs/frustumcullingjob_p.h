// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FRUSTUMCULLINGJOB_P_H
#define QT3DRENDER_RENDER_FRUSTUMCULLINGJOB_P_H

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/private/matrix4x4_p.h>
#include <Qt3DCore/private/vector3d_p.h>
#include <Qt3DCore/private/vector4d_p.h>
#include <Qt3DCore/private/aligned_malloc_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

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

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Entity;
class EntityManager;
class NodeManagers;
struct Plane;

class Q_3DRENDERSHARED_PRIVATE_EXPORT FrustumCullingJob : public Qt3DCore::QAspectJob
{
public:
    FrustumCullingJob();
    ~FrustumCullingJob();

    QT3D_ALIGNED_MALLOC_AND_FREE()

    inline void setRoot(Entity *root) noexcept { m_root = root; }
    inline void setManagers(NodeManagers *manager) noexcept { m_manager = manager; }
    inline void setActive(bool active) noexcept { m_active = active; }
    inline bool isActive() const noexcept { return m_active; }
    inline void setViewProjection(const Matrix4x4 &viewProjection) noexcept { m_viewProjection = viewProjection; }
    inline Matrix4x4 viewProjection() const noexcept { return m_viewProjection; }
    bool isRequired() override;

    const std::vector<Entity *> &visibleEntities() const noexcept { return m_visibleEntities; }

    void run() final;

private:
    struct Q_AUTOTEST_EXPORT Plane
    {
        explicit Plane(const Vector4D &planeEquation)
            : planeEquation(planeEquation)
            , normal(Vector3D(planeEquation).normalized())
            , d(planeEquation.w() / Vector3D(planeEquation).length())
        {}

        const Vector4D planeEquation;
        const Vector3D normal;
        const float d;
    };

    void cullScene(Entity *e, const Plane *planes);
    Matrix4x4 m_viewProjection;
    Entity *m_root;
    NodeManagers *m_manager;
    std::vector<Entity *> m_visibleEntities;
    bool m_active;
};

typedef QSharedPointer<FrustumCullingJob> FrustumCullingJobPtr;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FRUSTUMCULLINGJOB_P_H
