// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_CAMERALENS_H
#define QT3DRENDER_CAMERALENS_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QCameraLensPrivate;

class Q_3DRENDERSHARED_EXPORT QCameraLens : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane NOTIFY farPlaneChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix WRITE setProjectionMatrix NOTIFY projectionMatrixChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged REVISION(2, 9))

public:
    explicit QCameraLens(QNode *parent = nullptr);
    ~QCameraLens();

    enum ProjectionType {
        OrthographicProjection,
        PerspectiveProjection,
        FrustumProjection,
        CustomProjection
    };
    Q_ENUM(ProjectionType) // LCOV_EXCL_LINE

    ProjectionType projectionType() const;
    float nearPlane() const;
    float farPlane() const;
    float fieldOfView() const;
    float aspectRatio() const;
    float left() const;
    float right() const;
    float bottom() const;
    float top() const;

    QMatrix4x4 projectionMatrix() const;

    void setOrthographicProjection(float left, float right,
                                   float bottom, float top,
                                   float nearPlane, float farPlane);

    void setFrustumProjection(float left, float right,
                              float bottom, float top,
                              float nearPlane, float farPlane);

    void setPerspectiveProjection(float fieldOfView, float aspect,
                                  float nearPlane, float farPlane);

    float exposure() const;

    void viewAll(Qt3DCore::QNodeId cameraId);
    void viewEntity(Qt3DCore::QNodeId entityId, Qt3DCore::QNodeId cameraId);

public Q_SLOTS:
    void setProjectionType(ProjectionType projectionType);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setFieldOfView(float fieldOfView);
    void setAspectRatio(float aspectRatio);
    void setLeft(float left);
    void setRight(float right);
    void setBottom(float bottom);
    void setTop(float top);
    void setProjectionMatrix(const QMatrix4x4 &projectionMatrix);
    void setExposure(float exposure);

Q_SIGNALS:
    void projectionTypeChanged(QCameraLens::ProjectionType projectionType);
    void nearPlaneChanged(float nearPlane);
    void farPlaneChanged(float farPlane);
    void fieldOfViewChanged(float fieldOfView);
    void aspectRatioChanged(float aspectRatio);
    void leftChanged(float left);
    void rightChanged(float right);
    void bottomChanged(float bottom);
    void topChanged(float top);
    void projectionMatrixChanged(const QMatrix4x4 &projectionMatrix);
    void exposureChanged(float exposure);
    void viewSphere(const QVector3D &center, float radius);

protected:
    explicit QCameraLens(QCameraLensPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QCameraLens)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_CAMERALENS_H
