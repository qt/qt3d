/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "camera.h"
#include "camera_p.h"

namespace Qt3D {

Camera::Camera(Node *parent)
    : Component(parent)
    , d_ptr(new CameraPrivate(this))
{
    qDebug() << Q_FUNC_INFO;
}

void Camera::setViewMatrix(const QMatrix4x4 &m)
{
    Q_D(Camera);

    // derive the position and up vecotr

    bool ok;
    QMatrix4x4 iv = m.inverted(&ok);
    if (!ok) {
        qWarning() << Q_FUNC_INFO << "passed non-invertible matrix";
        return;
    }

    // position in eye-coordinates is simple the origin
    d->m_position = (iv * QVector4D(0,0,0,1)).toVector3D();

    // up vector, w=0 (no translation), positive Y-axis
    d->m_upVector = (iv * QVector4D(0,1,0,0)).toVector3D();

    // look vector, w=0, positive Z-axis
    d->m_cameraToCenter = (iv * QVector4D(0,0,1,0)).toVector3D();

    d->m_viewMatrix = m;
    d->m_syncNeeded = true;
}

void Camera::setProjectionType(ProjectionType projectionType)
{
    Q_D(Camera);
    if (d->m_projectionType != projectionType) {
        d->m_projectionType = projectionType;
        emit projectionTypeChanged();
    }
}

Camera::ProjectionType Camera::projectionType() const
{
    Q_D(const Camera);
    return d->m_projectionType;
}

QVector3D Camera::position() const
{
    Q_D(const Camera);
    return d->m_position;
}

void Camera::setPosition( const QVector3D& position )
{
    Q_D(Camera);
    d->m_position = position;
    d->m_cameraToCenter = d->m_viewCenter - position;
    d->m_viewMatrixDirty = true;
    d->m_syncNeeded = true;
}

void Camera::setUpVector( const QVector3D& upVector )
{
    Q_D(Camera);
    d->m_upVector = upVector;
    d->m_viewMatrixDirty = true;
    d->m_syncNeeded = true;
}

QVector3D Camera::upVector() const
{
    Q_D(const Camera);
    return d->m_upVector;
}

void Camera::setViewCenter( const QVector3D& viewCenter )
{
    Q_D(Camera);
    d->m_viewCenter = viewCenter;
    d->m_cameraToCenter = viewCenter - d->m_position;
    d->m_viewMatrixDirty = true;
    d->m_syncNeeded = true;
}

QVector3D Camera::viewCenter() const
{
    Q_D(const Camera);
    return d->m_viewCenter;
}

QVector3D Camera::viewVector() const
{
    Q_D(const Camera);
    return d->m_cameraToCenter;
}

void Camera::setOrthographicProjection( float left, float right,
                                        float bottom, float top,
                                        float nearPlane, float farPlane )
{
    Q_D(Camera);
    d->m_left = left;
    d->m_right = right;
    d->m_bottom = bottom;
    d->m_top = top;
    d->m_nearPlane = nearPlane;
    d->m_farPlane = farPlane;
    d->m_projectionType = OrthogonalProjection;
    d->updateOrthogonalProjection();
}

void Camera::setPerspectiveProjection( float fieldOfView, float aspectRatio,
                                       float nearPlane, float farPlane )
{
    Q_D(Camera);
    d->m_fieldOfView = fieldOfView;
    d->m_aspectRatio = aspectRatio;
    d->m_nearPlane = nearPlane;
    d->m_farPlane = farPlane;
    d->m_projectionType = PerspectiveProjection;
    d->updatePerpectiveProjection();
}

void Camera::setNearPlane(float nearPlane)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_nearPlane, nearPlane))
        return;
    d->m_nearPlane = nearPlane;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit nearPlaneChanged();
}

float Camera::nearPlane() const
{
    Q_D(const Camera);
    return d->m_nearPlane;
}

void Camera::setFarPlane(float farPlane)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_farPlane, farPlane))
        return;
    d->m_farPlane = farPlane;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit farPlaneChanged();
}

float Camera::farPlane() const
{
    Q_D(const Camera);
    return d->m_farPlane;
}

void Camera::setFieldOfView(float fieldOfView)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_fieldOfView, fieldOfView))
        return;
    d->m_fieldOfView = fieldOfView;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit fieldOfViewChanged();
}

float Camera::fieldOfView() const
{
    Q_D(const Camera);
    return d->m_fieldOfView;
}

void Camera::setAspectRatio(float aspectRatio)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_aspectRatio, aspectRatio))
        return;
    d->m_aspectRatio = aspectRatio;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit aspectRatioChanged();
}

float Camera::aspectRatio() const
{
    Q_D(const Camera);
    return d->m_aspectRatio;
}

void Camera::setLeft(float left)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_left, left))
        return;
    d->m_left = left;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit leftChanged();
}

float Camera::left() const
{
    Q_D(const Camera);
    return d->m_left;
}

void Camera::setRight(float right)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_right, right))
        return;
    d->m_right = right;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit rightChanged();
}

float Camera::right() const
{
    Q_D(const Camera);
    return d->m_right;
}

void Camera::setBottom(float bottom)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_bottom, bottom))
        return;
    d->m_bottom = bottom;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit bottomChanged();
}

float Camera::bottom() const
{
    Q_D(const Camera);
    return d->m_bottom;
}

void Camera::setTop(float top)
{
    Q_D(Camera);
    if (qFuzzyCompare(d->m_top, top))
        return;
    d->m_top = top;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit topChanged();
}

float Camera::top() const
{
    Q_D(const Camera);
    return d->m_top;
}

QMatrix4x4 Camera::viewMatrix() const
{
    Q_D(const Camera);
    if ( d->m_viewMatrixDirty )
    {
        d->m_viewMatrix.setToIdentity();
        d->m_viewMatrix.lookAt( d->m_position, d->m_viewCenter, d->m_upVector );
        d->m_viewMatrixDirty = false;
    }
    return d->m_viewMatrix;
}

QMatrix4x4 Camera::projectionMatrix() const
{
    Q_D(const Camera);
    return d->m_projectionMatrix;
}

QMatrix4x4 Camera::viewProjectionMatrix() const
{
    Q_D(const Camera);
    if ( d->m_viewMatrixDirty || d->m_viewProjectionMatrixDirty )
    {
        d->m_viewProjectionMatrix = d->m_projectionMatrix * viewMatrix();
        d->m_viewProjectionMatrixDirty = false;
    }
    return d->m_viewProjectionMatrix;
}

void Camera::translate( const QVector3D& vLocal, CameraTranslationOption option )
{
    Q_D(Camera);

    // Calculate the amount to move by in world coordinates
    QVector3D vWorld;
    if ( !qFuzzyIsNull( vLocal.x() ) )
    {
        // Calculate the vector for the local x axis
        QVector3D x = QVector3D::crossProduct( d->m_cameraToCenter, d->m_upVector ).normalized();
        vWorld += vLocal.x() * x;
    }

    if ( !qFuzzyIsNull( vLocal.y() ) )
        vWorld += vLocal.y() * d->m_upVector;

    if ( !qFuzzyIsNull( vLocal.z() ) )
        vWorld += vLocal.z() * d->m_cameraToCenter.normalized();

    // Update the camera position using the calculated world vector
    d->m_position += vWorld;

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        d->m_viewCenter += vWorld;

    // Refresh the camera -> view center vector
    d->m_cameraToCenter = d->m_viewCenter - d->m_position;

    // Calculate a new up vector. We do this by:
    // 1) Calculate a new local x-direction vector from the cross product of the new
    //    camera to view center vector and the old up vector.
    // 2) The local x vector is the normal to the plane in which the new up vector
    //    must lay. So we can take the cross product of this normal and the new
    //    x vector. The new normal vector forms the last part of the orthonormal basis
    QVector3D x = QVector3D::crossProduct( d->m_cameraToCenter, d->m_upVector ).normalized();
    d->m_upVector = QVector3D::crossProduct( x, d->m_cameraToCenter ).normalized();

    d->m_viewMatrixDirty = true;
    d->m_viewProjectionMatrixDirty = true;
}

void Camera::translateWorld(const QVector3D& vWorld , CameraTranslationOption option )
{
    Q_D(Camera);

    // Update the camera position using the calculated world vector
    d->m_position += vWorld;

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        d->m_viewCenter += vWorld;

    // Refresh the camera -> view center vector
    d->m_cameraToCenter = d->m_viewCenter - d->m_position;

    d->m_viewMatrixDirty = true;
    d->m_viewProjectionMatrixDirty = true;
}

QQuaternion Camera::tiltRotation(float angle) const
{
    Q_D(const Camera);
    QVector3D xBasis = QVector3D::crossProduct( d->m_upVector, d->m_cameraToCenter.normalized() ).normalized();
    return QQuaternion::fromAxisAndAngle( xBasis, -angle );
}

QQuaternion Camera::panRotation(float angle) const
{
    Q_D(const Camera);
    return QQuaternion::fromAxisAndAngle( d->m_upVector, angle );
}

QQuaternion Camera::rollRotation(float angle) const
{
    Q_D(const Camera);
    return QQuaternion::fromAxisAndAngle( d->m_cameraToCenter, -angle );
}

void Camera::tilt( const float& angle )
{
    QQuaternion q = tiltRotation( angle );
    rotate( q );
}

void Camera::pan( const float& angle )
{
    QQuaternion q = panRotation( -angle );
    rotate( q );
}

void Camera::roll( const float& angle )
{
    QQuaternion q = rollRotation( -angle );
    rotate( q );
}

void Camera::tiltAboutViewCenter( const float& angle )
{
    QQuaternion q = tiltRotation( -angle );
    rotateAboutViewCenter( q );
}

void Camera::panAboutViewCenter( const float& angle )
{
    QQuaternion q = panRotation( angle );
    rotateAboutViewCenter( q );
}

void Camera::rollAboutViewCenter( const float& angle )
{
    QQuaternion q = rollRotation( angle );
    rotateAboutViewCenter( q );
}

void Camera::rotate( const QQuaternion& q )
{
    Q_D(Camera);
    d->m_upVector = q.rotatedVector( d->m_upVector );
    d->m_cameraToCenter = q.rotatedVector( d->m_cameraToCenter );
    d->m_viewCenter = d->m_position + d->m_cameraToCenter;
    d->m_viewMatrixDirty = true;
    d->m_viewProjectionMatrixDirty = true;
    d->m_syncNeeded = true;
}

void Camera::rotateAboutViewCenter( const QQuaternion& q )
{
    Q_D(Camera);
    d->m_upVector = q.rotatedVector( d->m_upVector );
    d->m_cameraToCenter = q.rotatedVector( d->m_cameraToCenter );
    d->m_position = d->m_viewCenter - d->m_cameraToCenter;
    d->m_viewMatrixDirty = true;
    d->m_viewProjectionMatrixDirty = true;
    d->m_syncNeeded = true;
}

//void Camera::update(UpdateVisitor *)
//{
//    Q_D(Camera);
//    if (d->m_syncNeeded) {
//        // ...
//        d->m_syncNeeded = false;
//    }
//}

}
