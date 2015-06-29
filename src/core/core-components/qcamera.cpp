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

#include "qcamera.h"
#include "qcamera_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QCameraPrivate
    \internal
*/
QCameraPrivate::QCameraPrivate()
    : QEntityPrivate()
    , m_lens(new QCameraLens())
    , m_transform(new QTransform())
    , m_lookAt(new QLookAtTransform())
{
}

/*!
    \qmltype Camera
    \instantiates Qt3D::QCamera
    \inherits Entity
    \inqmlmodule Qt3D
    \since 5.5
*/

QCamera::QCamera(QNode *parent) :
    QEntity(*new QCameraPrivate, parent)
{
    QObject::connect(d_func()->m_lens, SIGNAL(projectionTypeChanged()), this, SIGNAL(projectionMatrixChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(nearPlaneChanged()), this, SIGNAL(nearPlaneChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(farPlaneChanged()), this, SIGNAL(farPlaneChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(fieldOfViewChanged()), this, SIGNAL(fieldOfViewChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(aspectRatioChanged()), this, SIGNAL(aspectRatioChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(leftChanged()), this, SIGNAL(leftChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(rightChanged()), this, SIGNAL(rightChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(bottomChanged()), this, SIGNAL(bottomChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(topChanged()), this, SIGNAL(topChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(projectionMatrixChanged()), this, SIGNAL(projectionMatrixChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(positionChanged()), this, SIGNAL(positionChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(upVectorChanged()), this, SIGNAL(upVectorChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(viewCenterChanged()), this, SIGNAL(viewCenterChanged()));
    QObject::connect(d_func()->m_transform, SIGNAL(matrixChanged()), this, SIGNAL(matrixChanged()));
    d_func()->m_transform->addTransform(d_func()->m_lookAt);
    addComponent(d_func()->m_lens);
    addComponent(d_func()->m_transform);
}

QCamera::~QCamera()
{
    QNode::cleanup();
}

/*! \internal */
QCamera::QCamera(QCameraPrivate &dd, QNode *parent)
    : QEntity(dd, parent)
{
    QObject::connect(d_func()->m_lens, SIGNAL(projectionTypeChanged()), this, SIGNAL(projectionMatrixChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(nearPlaneChanged()), this, SIGNAL(nearPlaneChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(farPlaneChanged()), this, SIGNAL(farPlaneChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(fieldOfViewChanged()), this, SIGNAL(fieldOfViewChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(aspectRatioChanged()), this, SIGNAL(aspectRatioChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(leftChanged()), this, SIGNAL(leftChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(rightChanged()), this, SIGNAL(rightChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(bottomChanged()), this, SIGNAL(bottomChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(topChanged()), this, SIGNAL(topChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(projectionMatrixChanged()), this, SIGNAL(projectionMatrixChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(positionChanged()), this, SIGNAL(positionChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(upVectorChanged()), this, SIGNAL(upVectorChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(viewCenterChanged()), this, SIGNAL(viewCenterChanged()));
    QObject::connect(d_func()->m_transform, SIGNAL(matrixChanged()), this, SIGNAL(matrixChanged()));
    d_func()->m_transform->addTransform(d_func()->m_lookAt);
    addComponent(d_func()->m_lens);
    addComponent(d_func()->m_transform);
}

QCameraLens *QCamera::lens() const
{
    Q_D(const QCamera);
    return d->m_lens;
}

QTransform *QCamera::transform() const
{
    Q_D(const QCamera);
    return d->m_transform;
}

QLookAtTransform *QCamera::lookAt() const
{
    Q_D(const QCamera);
    return d->m_lookAt;
}

void QCamera::translate( const QVector3D& vLocal, CameraTranslationOption option )
{
    QVector3D viewVector = viewCenter() - position(); // From "camera" position to view center

    // Calculate the amount to move by in world coordinates
    QVector3D vWorld;
    if ( !qFuzzyIsNull( vLocal.x() ) )
    {
        // Calculate the vector for the local x axis
        QVector3D x = QVector3D::crossProduct(viewVector, upVector()).normalized();
        vWorld += vLocal.x() * x;
    }

    if ( !qFuzzyIsNull( vLocal.y() ) )
        vWorld += vLocal.y() * upVector();

    if ( !qFuzzyIsNull( vLocal.z() ) )
        vWorld += vLocal.z() * viewVector.normalized();

    // Update the camera position using the calculated world vector
    setPosition(position() + vWorld);

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        setViewCenter(viewCenter() + vWorld);

    // Refresh the camera -> view center vector
    viewVector = viewCenter() - position();

    // Calculate a new up vector. We do this by:
    // 1) Calculate a new local x-direction vector from the cross product of the new
    //    camera to view center vector and the old up vector.
    // 2) The local x vector is the normal to the plane in which the new up vector
    //    must lay. So we can take the cross product of this normal and the new
    //    x vector. The new normal vector forms the last part of the orthonormal basis
    QVector3D x = QVector3D::crossProduct(viewVector, upVector()).normalized();
    setUpVector(QVector3D::crossProduct(x, viewVector).normalized());
}

void QCamera::translateWorld(const QVector3D& vWorld , CameraTranslationOption option )
{
    // Update the camera position using the calculated world vector
    setPosition(position() + vWorld);

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        setViewCenter(viewCenter() + vWorld);
}

QQuaternion QCamera::tiltRotation(float angle) const
{
    QVector3D viewVector = viewCenter() - position();
    QVector3D xBasis = QVector3D::crossProduct(upVector(), viewVector.normalized()).normalized();
    return QQuaternion::fromAxisAndAngle( xBasis, -angle );
}

QQuaternion QCamera::panRotation(float angle) const
{
    return QQuaternion::fromAxisAndAngle(upVector(), angle);
}

QQuaternion QCamera::rollRotation(float angle) const
{
    QVector3D viewVector = viewCenter() - position();
    return QQuaternion::fromAxisAndAngle(viewVector, -angle);
}

void QCamera::tilt( const float& angle )
{
    QQuaternion q = tiltRotation( angle );
    rotate( q );
}

void QCamera::pan( const float& angle )
{
    QQuaternion q = panRotation( -angle );
    rotate( q );
}

void QCamera::roll( const float& angle )
{
    QQuaternion q = rollRotation( -angle );
    rotate( q );
}

void QCamera::tiltAboutViewCenter( const float& angle )
{
    QQuaternion q = tiltRotation( -angle );
    rotateAboutViewCenter( q );
}

void QCamera::panAboutViewCenter( const float& angle )
{
    QQuaternion q = panRotation( angle );
    rotateAboutViewCenter( q );
}

void QCamera::rollAboutViewCenter( const float& angle )
{
    QQuaternion q = rollRotation( angle );
    rotateAboutViewCenter( q );
}

void QCamera::rotate( const QQuaternion& q )
{
    setUpVector(q * upVector());
    QVector3D viewVector = viewCenter() - position();
    QVector3D cameraToCenter = q * viewVector;
    setViewCenter(position() + cameraToCenter);
}

void QCamera::rotateAboutViewCenter( const QQuaternion& q )
{
    setUpVector(q * upVector());
    QVector3D viewVector = viewCenter() - position();
    QVector3D cameraToCenter = q * viewVector;
    setPosition(viewCenter() - cameraToCenter);
    setViewCenter(position() + cameraToCenter);
}

void QCamera::setProjectionType(QCameraLens::ProjectionType type)
{
    Q_D(QCamera);
    d->m_lens->setProjectionType(type);
}

/*!
    \qmlproperty enumeration Qt3D::Camera::projectionType

    Holds the type of the camera projection (orthogonal or perspective).

    \value CameraLens.OrthogonalProjection Orthogonal projection
    \value CameraLens.PerspectiveProjection Perspective projection
*/

QCameraLens::ProjectionType QCamera::projectionType() const
{
    Q_D(const QCamera);
    return d->m_lens->projectionType();
}

void QCamera::setNearPlane(float nearPlane)
{
    Q_D(QCamera);
    d->m_lens->setNearPlane(nearPlane);
}

/*!
    \qmlproperty float Qt3D::Camera::nearPlane
*/

float QCamera::nearPlane() const
{
    Q_D(const QCamera);
    return d->m_lens->nearPlane();
}

void QCamera::setFarPlane(float farPlane)
{
    Q_D(QCamera);
    d->m_lens->setFarPlane(farPlane);
}

/*!
    \qmlproperty float Qt3D::Camera::farPlane
*/

float QCamera::farPlane() const
{
    Q_D(const QCamera);
    return d->m_lens->farPlane();
}

void QCamera::setFieldOfView(float fieldOfView)
{
    Q_D(QCamera);
    d->m_lens->setFieldOfView(fieldOfView);
}

/*!
    \qmlproperty float Qt3D::Camera::fieldOfView
*/

float QCamera::fieldOfView() const
{
    Q_D(const QCamera);
    return d->m_lens->fieldOfView();
}

void QCamera::setAspectRatio(float aspectRatio)
{
    Q_D(QCamera);
    d->m_lens->setAspectRatio(aspectRatio);
}

/*!
    \qmlproperty float Qt3D::Camera::aspectRatio
*/

float QCamera::aspectRatio() const
{
    Q_D(const QCamera);
    return d->m_lens->aspectRatio();
}

void QCamera::setLeft(float left)
{
    Q_D(QCamera);
    d->m_lens->setLeft(left);
}

/*!
    \qmlproperty float Qt3D::Camera::left
*/

float QCamera::left() const
{
    Q_D(const QCamera);
    return d->m_lens->left();
}

void QCamera::setRight(float right)
{
    Q_D(QCamera);
    d->m_lens->setRight(right);
}

/*!
    \qmlproperty float Qt3D::Camera::right
*/

float QCamera::right() const
{
    Q_D(const QCamera);
    return d->m_lens->right();
}

void QCamera::setBottom(float bottom)
{
    Q_D(QCamera);
    d->m_lens->setBottom(bottom);
}

/*!
    \qmlproperty float Qt3D::Camera::bottom
*/

float QCamera::bottom() const
{
    Q_D(const QCamera);
    return d->m_lens->bottom();
}

void QCamera::setTop(float top)
{
    Q_D(QCamera);
    d->m_lens->setTop(top);
}

/*!
    \qmlproperty float Qt3D::Camera::top
*/

float QCamera::top() const
{
    Q_D(const QCamera);
    return d->m_lens->top();
}

/*!
    \qmlproperty matrix4x4 Qt3D::Camera::projectionMatrix
    \readonly
*/

QMatrix4x4 QCamera::projectionMatrix() const
{
    Q_D(const QCamera);
    return d->m_lens->projectionMatrix();
}

void QCamera::setPosition(const QVector3D &position)
{
    Q_D(QCamera);
    d->m_lookAt->setPosition(position);
}

/*!
    \qmlproperty vector3d Qt3D::Camera::position
*/

QVector3D QCamera::position() const
{
    Q_D(const QCamera);
    return d->m_lookAt->position();

}

void QCamera::setUpVector(const QVector3D &upVector)
{
    Q_D(QCamera);
    d->m_lookAt->setUpVector(upVector);
}

/*!
    \qmlproperty vector3d Qt3D::Camera::upVector
*/

QVector3D QCamera::upVector() const
{
    Q_D(const QCamera);
    return d->m_lookAt->upVector();
}

void QCamera::setViewCenter(const QVector3D &viewCenter)
{
    Q_D(QCamera);
    d->m_lookAt->setViewCenter(viewCenter);
}

/*!
    \qmlproperty vector3d Qt3D::Camera::viewCenter
*/

QVector3D QCamera::viewCenter() const
{
    Q_D(const QCamera);
    return d->m_lookAt->viewCenter();
}

/*!
    \qmlproperty matrix4x4 Qt3D::Camera::matrix
*/
QMatrix4x4 QCamera::matrix() const
{
    Q_D(const QCamera);
    return d->m_transform->matrix();
}

} // Qt3D

QT_END_NAMESPACE
