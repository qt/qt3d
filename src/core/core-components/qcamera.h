/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_CAMERA_H
#define QT3DCORE_CAMERA_H

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcameralens.h>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QLookAtTransform;
class QTransform;

class QCameraPrivate;
class QT3DCORESHARED_EXPORT QCamera : public QEntity
{
    Q_OBJECT
    // CameraLens
    Q_PROPERTY(Qt3DCore::QCameraLens::ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane NOTIFY farPlaneChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix NOTIFY projectionMatrixChanged)
    // LookAt
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(QVector3D viewCenter READ viewCenter WRITE setViewCenter NOTIFY viewCenterChanged)
    Q_PROPERTY(QMatrix4x4 matrix READ matrix NOTIFY matrixChanged)

public:
    explicit QCamera(QNode *parent = 0);
    ~QCamera();

    enum CameraTranslationOption {
        TranslateViewCenter,
        DontTranslateViewCenter
    };
    Q_ENUM(CameraTranslationOption)

    QCameraLens *lens() const;
    QTransform *transform() const;
    QLookAtTransform *lookAt() const;

    QQuaternion tiltRotation(float angle) const;
    QQuaternion panRotation(float angle) const;
    QQuaternion rollRotation(float angle) const;

    // Translate relative to camera orientation axes
    Q_INVOKABLE void translate(const QVector3D& vLocal, CameraTranslationOption option = TranslateViewCenter );

    // Translate relative to world axes
    Q_INVOKABLE void translateWorld(const QVector3D& vWorld, CameraTranslationOption option = TranslateViewCenter );

    Q_INVOKABLE void tilt(float angle);
    Q_INVOKABLE void pan(float angle);
    Q_INVOKABLE void roll(float angle);

    Q_INVOKABLE void tiltAboutViewCenter(float angle);
    Q_INVOKABLE void panAboutViewCenter(float angle);
    Q_INVOKABLE void rollAboutViewCenter(float angle);

    Q_INVOKABLE void rotate(const QQuaternion& q);
    Q_INVOKABLE void rotateAboutViewCenter(const QQuaternion& q);

    void setProjectionType(QCameraLens::ProjectionType type);
    QCameraLens::ProjectionType projectionType() const;

    void setNearPlane(float nearPlane);
    float nearPlane() const;

    void setFarPlane(float farPlane);
    float farPlane() const;

    void setFieldOfView(float fieldOfView);
    float fieldOfView() const;

    void setAspectRatio(float aspectRatio);
    float aspectRatio() const;

    void setLeft(float left);
    float left() const;

    void setRight(float right);
    float right() const;

    void setBottom(float bottom);
    float bottom() const;

    void setTop(float top);
    float top() const;

    QMatrix4x4 projectionMatrix() const;

    void setPosition(const QVector3D &position);
    QVector3D position() const;

    void setUpVector(const QVector3D &upVector);
    QVector3D upVector() const;

    void setViewCenter(const QVector3D &viewCenter);
    QVector3D viewCenter() const;

    QMatrix4x4 matrix() const;

Q_SIGNALS:
    void projectionTypeChanged();
    void nearPlaneChanged();
    void farPlaneChanged();
    void fieldOfViewChanged();
    void aspectRatioChanged();
    void leftChanged();
    void rightChanged();
    void bottomChanged();
    void topChanged();
    void projectionMatrixChanged();
    void positionChanged();
    void upVectorChanged();
    void viewCenterChanged();
    void matrixChanged();

protected:
    Q_DECLARE_PRIVATE(QCamera)
    QT3D_CLONEABLE(QCamera)
    QCamera(QCameraPrivate &dd, QNode *parent = 0);
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_CAMERA_H
