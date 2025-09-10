// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_CAMERA_H
#define QT3DRENDER_CAMERA_H

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qcameralens.h>
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QCameraPrivate;

class Q_3DRENDERSHARED_EXPORT QCamera : public Qt3DCore::QEntity
{
    Q_OBJECT
    // CameraLens
    Q_PROPERTY(Qt3DRender::QCameraLens::ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
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
    // LookAt
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(QVector3D viewCenter READ viewCenter WRITE setViewCenter NOTIFY viewCenterChanged)
    Q_PROPERTY(QVector3D viewVector READ viewVector NOTIFY viewVectorChanged)
    Q_PROPERTY(QMatrix4x4 viewMatrix READ viewMatrix NOTIFY viewMatrixChanged)

    Q_PROPERTY(Qt3DRender::QCameraLens *lens READ lens CONSTANT REVISION(2, 14))
    Q_PROPERTY(Qt3DCore::QTransform *transform  READ transform CONSTANT REVISION(2, 14))

public:
    explicit QCamera(QNode *parent = nullptr);
    ~QCamera();

    enum CameraTranslationOption {
        TranslateViewCenter,
        DontTranslateViewCenter
    };
    Q_ENUM(CameraTranslationOption) // LCOV_EXCL_LINE

    QCameraLens *lens() const;
    Qt3DCore::QTransform *transform() const;

    Q_INVOKABLE QQuaternion tiltRotation(float angle) const;
    Q_INVOKABLE QQuaternion panRotation(float angle) const;
    Q_INVOKABLE QQuaternion rollRotation(float angle) const;
    Q_INVOKABLE QQuaternion rotation(float angle, const QVector3D &axis) const;

    // Translate relative to camera orientation axes
    Q_INVOKABLE void translate(const QVector3D& vLocal, CameraTranslationOption option = TranslateViewCenter);

    // Translate relative to world axes
    Q_INVOKABLE void translateWorld(const QVector3D& vWorld, CameraTranslationOption option = TranslateViewCenter);

    Q_INVOKABLE void tilt(float angle);
    Q_INVOKABLE void pan(float angle);
    Q_INVOKABLE void pan(float angle, const QVector3D &axis);
    Q_INVOKABLE void roll(float angle);

    Q_INVOKABLE void tiltAboutViewCenter(float angle);
    Q_INVOKABLE void panAboutViewCenter(float angle);
    Q_INVOKABLE void panAboutViewCenter(float angle, const QVector3D &axis);
    Q_INVOKABLE void rollAboutViewCenter(float angle);

    Q_INVOKABLE void rotate(const QQuaternion& q);
    Q_INVOKABLE void rotateAboutViewCenter(const QQuaternion& q);

    QCameraLens::ProjectionType projectionType() const;
    float nearPlane() const;
    float farPlane() const;
    float fieldOfView() const;
    float aspectRatio() const;
    float left() const;
    float right() const;
    float bottom() const;
    float top() const;
    QMatrix4x4 projectionMatrix() const;
    float exposure() const;
    QVector3D position() const;
    QVector3D upVector() const;
    QVector3D viewCenter() const;
    QVector3D viewVector() const;
    QMatrix4x4 viewMatrix() const;

public Q_SLOTS:
    void setProjectionType(QCameraLens::ProjectionType type);
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
    void setPosition(const QVector3D &position);
    void setUpVector(const QVector3D &upVector);
    void setViewCenter(const QVector3D &viewCenter);

    void viewAll();
    void viewSphere(const QVector3D &center, float radius);
    void viewEntity(Qt3DCore::QEntity *entity);

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
    void positionChanged(const QVector3D &position);
    void upVectorChanged(const QVector3D &upVector);
    void viewCenterChanged(const QVector3D &viewCenter);
    void viewVectorChanged(const QVector3D &viewVector);
    void viewMatrixChanged();

protected:
    Q_DECLARE_PRIVATE(QCamera)
    explicit QCamera(QCameraPrivate &dd, QNode *parent = nullptr);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_CAMERA_H
