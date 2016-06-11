/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcamera.h"
#include "qcamera_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \internal
 */
QCameraPrivate::QCameraPrivate()
    : Qt3DCore::QEntityPrivate()
    , m_position(0.0f, 0.0f, 0.0f)
    , m_viewCenter(0.0f, 0.0f, -100.0f)
    , m_upVector(0.0f, 1.0f, 0.0f)
    , m_cameraToCenter(m_viewCenter - m_position)
    , m_viewMatrixDirty(false)
    , m_lens(new QCameraLens())
    , m_transform(new Qt3DCore::QTransform())
{
    updateViewMatrix();
}

/*!
 * \class Qt3DRender::QCamera
 * \brief The QCamera class defines a view point through which the scene will be
 * rendered.
 * \inmodule Qt3DRender
 * \since 5.5
 */

/*!
 * \qmltype Camera
 * \instantiates Qt3DRender::QCamera
 * \inherits Entity
 * \inqmlmodule Qt3D.Render
 * \since 5.5
 * \brief Defines a view point through which the scene will be rendered.
 */

/*!
 * Creates a new QCamera instance with the
 * specified \a parent.
 */
QCamera::QCamera(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(*new QCameraPrivate, parent)
{
    QObject::connect(d_func()->m_lens, SIGNAL(projectionTypeChanged(QCameraLens::ProjectionType)), this, SIGNAL(projectionTypeChanged(QCameraLens::ProjectionType)));
    QObject::connect(d_func()->m_lens, SIGNAL(nearPlaneChanged(float)), this, SIGNAL(nearPlaneChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(farPlaneChanged(float)), this, SIGNAL(farPlaneChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(fieldOfViewChanged(float)), this, SIGNAL(fieldOfViewChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(aspectRatioChanged(float)), this, SIGNAL(aspectRatioChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(leftChanged(float)), this, SIGNAL(leftChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(rightChanged(float)), this, SIGNAL(rightChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(bottomChanged(float)), this, SIGNAL(bottomChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(topChanged(float)), this, SIGNAL(topChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(projectionMatrixChanged(const QMatrix4x4 &)), this, SIGNAL(projectionMatrixChanged(const QMatrix4x4 &)));
    QObject::connect(d_func()->m_transform, SIGNAL(matrixChanged()), this, SIGNAL(viewMatrixChanged()));
    addComponent(d_func()->m_lens);
    addComponent(d_func()->m_transform);
}

/*!
 * \internal
 */
QCamera::~QCamera()
{
}

/*!
 * \internal
 */
QCamera::QCamera(QCameraPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(dd, parent)
{
    QObject::connect(d_func()->m_lens, SIGNAL(projectionTypeChanged(QCameraLens::ProjectionType)), this, SIGNAL(projectionTypeChanged(QCameraLens::ProjectionType)));
    QObject::connect(d_func()->m_lens, SIGNAL(nearPlaneChanged(float)), this, SIGNAL(nearPlaneChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(farPlaneChanged(float)), this, SIGNAL(farPlaneChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(fieldOfViewChanged(float)), this, SIGNAL(fieldOfViewChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(aspectRatioChanged(float)), this, SIGNAL(aspectRatioChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(leftChanged(float)), this, SIGNAL(leftChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(rightChanged(float)), this, SIGNAL(rightChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(bottomChanged(float)), this, SIGNAL(bottomChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(topChanged(float)), this, SIGNAL(topChanged(float)));
    QObject::connect(d_func()->m_lens, SIGNAL(projectionMatrixChanged(const QMatrix4x4 &)), this, SIGNAL(projectionMatrixChanged(const QMatrix4x4 &)));
    QObject::connect(d_func()->m_transform, SIGNAL(matrixChanged()), this, SIGNAL(viewMatrixChanged()));
    addComponent(d_func()->m_lens);
    addComponent(d_func()->m_transform);
}

/*!
 * Returns the current lens.
 */
QCameraLens *QCamera::lens() const
{
    Q_D(const QCamera);
    return d->m_lens;
}

/*!
 * Returns the camera's position via transform.
 */
Qt3DCore::QTransform *QCamera::transform() const
{
    Q_D(const QCamera);
    return d->m_transform;
}

/*!
 * Translates the camera's position and its view vector by \a vLocal in local coordinates.
 * The \a option allows for toggling whether the view center should be translated.
 */
void QCamera::translate(const QVector3D &vLocal, CameraTranslationOption option)
{
    QVector3D viewVector = viewCenter() - position(); // From "camera" position to view center

    // Calculate the amount to move by in world coordinates
    QVector3D vWorld;
    if (!qFuzzyIsNull(vLocal.x())) {
        // Calculate the vector for the local x axis
        const QVector3D x = QVector3D::crossProduct(viewVector, upVector()).normalized();
        vWorld += vLocal.x() * x;
    }

    if (!qFuzzyIsNull(vLocal.y()))
        vWorld += vLocal.y() * upVector();

    if (!qFuzzyIsNull(vLocal.z()))
        vWorld += vLocal.z() * viewVector.normalized();

    // Update the camera position using the calculated world vector
    setPosition(position() + vWorld);

    // May be also update the view center coordinates
    if (option == TranslateViewCenter)
        setViewCenter(viewCenter() + vWorld);

    // Refresh the camera -> view center vector
    viewVector = viewCenter() - position();

    // Calculate a new up vector. We do this by:
    // 1) Calculate a new local x-direction vector from the cross product of the new
    //    camera to view center vector and the old up vector.
    // 2) The local x vector is the normal to the plane in which the new up vector
    //    must lay. So we can take the cross product of this normal and the new
    //    x vector. The new normal vector forms the last part of the orthonormal basis
    const QVector3D x = QVector3D::crossProduct(viewVector, upVector()).normalized();
    setUpVector(QVector3D::crossProduct(x, viewVector).normalized());
}

/*!
 * Translates the camera's position and its view vector by \a vWorld in world coordinates.
 * The \a option allows for toggling whether the view center should be translated.
 */
void QCamera::translateWorld(const QVector3D &vWorld, CameraTranslationOption option)
{
    // Update the camera position using the calculated world vector
    setPosition(position() + vWorld);

    // May be also update the view center coordinates
    if (option == TranslateViewCenter)
        setViewCenter(viewCenter() + vWorld);
}

/*!
 * Returns the calculated tilt rotation in relation to the \a angle taken in to adjust the camera's
 * tilt or up/down rotation on the X axis.
 */
QQuaternion QCamera::tiltRotation(float angle) const
{
    const QVector3D viewVector = viewCenter() - position();
    const QVector3D xBasis = QVector3D::crossProduct(upVector(), viewVector.normalized()).normalized();
    return QQuaternion::fromAxisAndAngle(xBasis, -angle);
}

/*!
 * Returns the calculated pan rotation in relation to the \a angle taken in to adjust the camera's
 * pan or left/right rotation on the Y axis.
 */
QQuaternion QCamera::panRotation(float angle) const
{
    return QQuaternion::fromAxisAndAngle(upVector(), angle);
}

/*!
 * Returns the calculated roll rotation in relation to the \a angle taken in to adjust the camera's
 * roll or lean left/right rotation on the Z axis.
 */
QQuaternion QCamera::rollRotation(float angle) const
{
    QVector3D viewVector = viewCenter() - position();
    return QQuaternion::fromAxisAndAngle(viewVector, -angle);
}

/*!
 * Returns the calculated rotation in relation to the \a angle and chosen \a axis taken in.
 */
QQuaternion QCamera::rotation(float angle, const QVector3D &axis) const
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

/*!
 * Adjusts the tilt angle of the camera by \a angle.
 */
void QCamera::tilt(float angle)
{
    QQuaternion q = tiltRotation(angle);
    rotate(q);
}

/*!
 * Adjusts the pan angle of the camera by \a angle.
 */
void QCamera::pan(float angle)
{
    QQuaternion q = panRotation(-angle);
    rotate(q);
}

/*!
 * Adjusts the pan angle of the camera by \a angle on a chosen \a axis.
 */
void QCamera::pan(float angle, const QVector3D &axis)
{
    QQuaternion q = rotation(-angle, axis);
    rotate(q);
}

/*!
 * Adjusts the camera roll by \a angle.
 */
void QCamera::roll(float angle)
{
    QQuaternion q = rollRotation(-angle);
    rotate(q);
}

/*!
 * Adjusts the camera tilt about view center by \a angle.
 */
void QCamera::tiltAboutViewCenter(float angle)
{
    QQuaternion q = tiltRotation(-angle);
    rotateAboutViewCenter(q);
}

/*!
 * Adjusts the camera pan about view center by \a angle.
 */
void QCamera::panAboutViewCenter(float angle)
{
    QQuaternion q = panRotation(angle);
    rotateAboutViewCenter(q);
}

/*!
 * Adjusts the camera pan about view center by \a angle on \a axis.
 */
void QCamera::panAboutViewCenter(float angle, const QVector3D &axis)
{
    QQuaternion q = rotation(angle, axis);
    rotateAboutViewCenter(q);
}

/*!
 * Adjusts the camera roll about view center by \a angle.
 */
void QCamera::rollAboutViewCenter(float angle)
{
    QQuaternion q = rollRotation(angle);
    rotateAboutViewCenter(q);
}

/*!
 * Rotates the camera with the use of a Quaternion in \a q.
 */
void QCamera::rotate(const QQuaternion& q)
{
    setUpVector(q * upVector());
    QVector3D viewVector = viewCenter() - position();
    QVector3D cameraToCenter = q * viewVector;
    setViewCenter(position() + cameraToCenter);
}

/*!
 * Rotates the camera about the view center with the use of a Quaternion
 * in \a q.
 */
void QCamera::rotateAboutViewCenter(const QQuaternion& q)
{
    setUpVector(q * upVector());
    QVector3D viewVector = viewCenter() - position();
    QVector3D cameraToCenter = q * viewVector;
    setPosition(viewCenter() - cameraToCenter);
    setViewCenter(position() + cameraToCenter);
}

/*!
 * Sets the camera's projection type to \a type.
 */
void QCamera::setProjectionType(QCameraLens::ProjectionType type)
{
    Q_D(QCamera);
    d->m_lens->setProjectionType(type);
}

/*!
 * \qmlproperty enumeration Qt3DCore::Camera::projectionType
 *
 * Holds the type of the camera projection (orthogonal or perspective).
 *
 * \value CameraLens.OrthographicProjection Orthographic projection
 * \value CameraLens.PerspectiveProjection Perspective projection
 */
QCameraLens::ProjectionType QCamera::projectionType() const
{
    Q_D(const QCamera);
    return d->m_lens->projectionType();
}

/*!
 * Sets the camera's near plane to \a nearPlane.
 */
void QCamera::setNearPlane(float nearPlane)
{
    Q_D(QCamera);
    d->m_lens->setNearPlane(nearPlane);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::nearPlane
 * The current camera near plane.
 */
float QCamera::nearPlane() const
{
    Q_D(const QCamera);
    return d->m_lens->nearPlane();
}

/*!
 * Sets the camera's far plane to \a farPlane
 */
void QCamera::setFarPlane(float farPlane)
{
    Q_D(QCamera);
    d->m_lens->setFarPlane(farPlane);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::farPlane
 * The current camera far plane.
 */
float QCamera::farPlane() const
{
    Q_D(const QCamera);
    return d->m_lens->farPlane();
}

/*!
 * Sets the camera's field of view to \a fiedOfView.
 */
void QCamera::setFieldOfView(float fieldOfView)
{
    Q_D(QCamera);
    d->m_lens->setFieldOfView(fieldOfView);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::fieldOfView
 * The current field of view.
 */
float QCamera::fieldOfView() const
{
    Q_D(const QCamera);
    return d->m_lens->fieldOfView();
}

/*!
 * Sets the camera's aspect ratio to \a aspectRatio.
 */
void QCamera::setAspectRatio(float aspectRatio)
{
    Q_D(QCamera);
    d->m_lens->setAspectRatio(aspectRatio);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::aspectRatio
 * The current aspect ratio.
 */
float QCamera::aspectRatio() const
{
    Q_D(const QCamera);
    return d->m_lens->aspectRatio();
}

/*!
 * Sets the left of the camera to \a left.
 */
void QCamera::setLeft(float left)
{
    Q_D(QCamera);
    d->m_lens->setLeft(left);
}

/*!
 *\qmlproperty float Qt3DCore::Camera::left
 * The current left of the camera.
 */
float QCamera::left() const
{
    Q_D(const QCamera);
    return d->m_lens->left();
}

/*!
 * Sets the right of the camera to \a right.
 */
void QCamera::setRight(float right)
{
    Q_D(QCamera);
    d->m_lens->setRight(right);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::right
 * The current right of the camera.
 */
float QCamera::right() const
{
    Q_D(const QCamera);
    return d->m_lens->right();
}

/*!
 * Sets the bottom of the camera to \a bottom.
 */
void QCamera::setBottom(float bottom)
{
    Q_D(QCamera);
    d->m_lens->setBottom(bottom);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::bottom
 * The current bottom of the camera.
 */
float QCamera::bottom() const
{
    Q_D(const QCamera);
    return d->m_lens->bottom();
}

/*!
 * Sets the top of the camera to \a top.
 */
void QCamera::setTop(float top)
{
    Q_D(QCamera);
    d->m_lens->setTop(top);
}

/*!
 * \qmlproperty float Qt3DCore::Camera::top
 * The current top of the camera.
 */
float QCamera::top() const
{
    Q_D(const QCamera);
    return d->m_lens->top();
}

/*!
 * Sets the camera's projection matrix.
 */
void QCamera::setProjectionMatrix(const QMatrix4x4 &projectionMatrix)
{
    Q_D(QCamera);
    d->m_lens->setProjectionMatrix(projectionMatrix);
}

/*!
 * \qmlproperty QMatri4x4 Qt3DCore::Camera:projectionMatrix
 * The current projection matrix of the camera.
 */
QMatrix4x4 QCamera::projectionMatrix() const
{
    Q_D(const QCamera);
    return d->m_lens->projectionMatrix();
}

/*!
 * Sets the camera's position in 3D space to \a position.
 */
void QCamera::setPosition(const QVector3D &position)
{
    Q_D(QCamera);
    if (d->m_position != position) {
        d->m_position = position;
        d->m_cameraToCenter = d->m_viewCenter - position;
        d->m_viewMatrixDirty = true;
        emit positionChanged(position);
        emit viewVectorChanged(d->m_cameraToCenter);
        d->updateViewMatrix();
    }
}

/*!
 * \qmlproperty vector3d Qt3DCore::Camera::position
 * The camera's position.
 */
QVector3D QCamera::position() const
{
    Q_D(const QCamera);
    return d->m_position;
}

/*!
 * Sets the camera's up vector to \a upVector.
 */
void QCamera::setUpVector(const QVector3D &upVector)
{
    Q_D(QCamera);
    if (d->m_upVector != upVector) {
        d->m_upVector = upVector;
        d->m_viewMatrixDirty = true;
        emit upVectorChanged(upVector);
        d->updateViewMatrix();
    }
}

/*!
 * \qmlproperty vector3d Qt3DCore::Camera::upVector
 * The camera's up vector.
 */
QVector3D QCamera::upVector() const
{
    Q_D(const QCamera);
    return d->m_upVector;
}

/*!
 * Sets the camera's view center to \a viewCenter.
 */
void QCamera::setViewCenter(const QVector3D &viewCenter)
{
    Q_D(QCamera);
    if (d->m_viewCenter != viewCenter) {
        d->m_viewCenter = viewCenter;
        d->m_cameraToCenter = viewCenter - d->m_position;
        d->m_viewMatrixDirty = true;
        emit viewCenterChanged(viewCenter);
        emit viewVectorChanged(d->m_cameraToCenter);
        d->updateViewMatrix();
    }
}

/*!
 * \qmlproperty vector3d Qt3DCore::Camera::viewCenter
 * The camera's view center.
 */
QVector3D QCamera::viewCenter() const
{
    Q_D(const QCamera);
    return d->m_viewCenter;
}

/*!
 * \qmlproperty vector3d Qt3DCore::Camera::viewVector
 * The camera's view vector.
 */
QVector3D QCamera::viewVector() const
{
    Q_D(const QCamera);
    return d->m_cameraToCenter;
}

/*!
 * \qmlproperty matrix4x4 Qt3DCore::Camera::viewMatrix
 * The camera's view matrix.
 */
QMatrix4x4 QCamera::viewMatrix() const
{
    Q_D(const QCamera);
    return d->m_transform->matrix();
}

} // Qt3DRender

QT_END_NAMESPACE
