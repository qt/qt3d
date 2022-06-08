// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_CAMERALENS_P_H
#define QT3DRENDER_CAMERALENS_P_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/private/qcomponent_p.h>

#include "qcameralens.h"

#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

struct CameraLensRequest
{
    Qt3DCore::QNodeId requestId;
    Qt3DCore::QNodeId cameraId;
    Qt3DCore::QNodeId entityId;

    inline operator bool() const { return !requestId.isNull(); }
};

inline bool operator ==(const CameraLensRequest &a, const CameraLensRequest &b) noexcept
{
    return a.cameraId == b.cameraId && a.entityId == b.entityId && a.requestId == b.requestId;
}

inline bool operator !=(const CameraLensRequest &a, const CameraLensRequest &b) noexcept
{
    return !(a == b);
}

class Q_3DRENDERSHARED_PRIVATE_EXPORT QCameraLensPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QCameraLensPrivate();

    inline void updateProjectionMatrix()
    {
        switch (m_projectionType) {
        case QCameraLens::OrthographicProjection:
            updateOrthographicProjection();
            break;
        case QCameraLens::PerspectiveProjection:
            updatePerpectiveProjection();
            break;
        case QCameraLens::FrustumProjection:
            updateFrustumProjection();
            break;
        case QCameraLens::CustomProjection:
            break;
        }
    }

    Q_DECLARE_PUBLIC(QCameraLens)

    QCameraLens::ProjectionType m_projectionType;

    float m_nearPlane;
    float m_farPlane;

    float m_fieldOfView;
    float m_aspectRatio;

    float m_left;
    float m_right;
    float m_bottom;
    float m_top;

    mutable QMatrix4x4 m_projectionMatrix;

    float m_exposure;

    CameraLensRequest m_pendingViewAllRequest;
    void processViewAllResult(Qt3DCore::QNodeId requestId, const QVector3D &center, float radius);

private:
    inline void updatePerpectiveProjection()
    {
        Q_Q(QCameraLens);
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.perspective(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
        Q_EMIT q->projectionMatrixChanged(m_projectionMatrix);
    }

    inline void updateOrthographicProjection()
    {
        Q_Q(QCameraLens);
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.ortho(m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
        Q_EMIT q->projectionMatrixChanged(m_projectionMatrix);
    }

    inline void updateFrustumProjection()
    {
        Q_Q(QCameraLens);
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.frustum(m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
        Q_EMIT q->projectionMatrixChanged(m_projectionMatrix);
    }
};

struct QCameraLensData
{
    QMatrix4x4 projectionMatrix;
    float exposure;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_CAMERALENS_P_H
