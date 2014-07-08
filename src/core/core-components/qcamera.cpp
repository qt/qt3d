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

#include "qcamera.h"
#include "qcamera_p.h"
#include "qcameralens.h"
#include <qtransform.h>
#include <qlookattransform.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QCameraPrivate::QCameraPrivate(QCamera *qq)
    : QEntityPrivate(qq),
      m_lens(Q_NULLPTR),
      m_transform(Q_NULLPTR)
{
}

QCamera::QCamera(QNode *parent) :
    QEntity(*new QCameraPrivate(this), parent)
{
    qDebug() << Q_FUNC_INFO;
}

QCamera::QCamera(QCameraPrivate &dd, QNode *parent)
    : QEntity(dd, parent)
{
}

QCameraLens *QCamera::lens() const
{
    Q_D(const QCamera);
    return d->m_lens;
}

void QCamera::setLens(QCameraLens *lens)
{
    Q_D(QCamera);
    if (d->m_lens != lens) {
        if (d->m_lens)
            removeComponent(d->m_lens);
        d->m_lens = lens;
        if (d->m_lens)
            addComponent(d->m_lens);
        emit lensChanged();
    }
}

QTransform *QCamera::transform() const
{
    Q_D(const QCamera);
    return d->m_transform;
}

void QCamera::setTransform(QTransform *transform)
{
    Q_D(QCamera);
    if (d->m_transform != transform) {
        if (d->m_transform)
            removeComponent(d->m_transform);
        d->m_transform = transform;
        if (d->m_transform)
            addComponent(d->m_transform);
        emit transformChanged();
    }
}

void QCamera::translate( const QVector3D& vLocal, CameraTranslationOption option )
{
    Q_D(QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return;

    // Calculate the amount to move by in world coordinates
    QVector3D vWorld;
    if ( !qFuzzyIsNull( vLocal.x() ) )
    {
        // Calculate the vector for the local x axis
        QVector3D x = QVector3D::crossProduct(lookAt->viewVector(), lookAt->upVector()).normalized();
        vWorld += vLocal.x() * x;
    }

    if ( !qFuzzyIsNull( vLocal.y() ) )
        vWorld += vLocal.y() * lookAt->upVector();

    if ( !qFuzzyIsNull( vLocal.z() ) )
        vWorld += vLocal.z() * lookAt->viewVector().normalized();

    // Update the camera position using the calculated world vector
    lookAt->setPosition(lookAt->position() + vWorld);

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        lookAt->setViewCenter(lookAt->viewCenter() + vWorld);

    // Refresh the camera -> view center vector
    lookAt->setViewVector(lookAt->viewCenter() - lookAt->position());

    // Calculate a new up vector. We do this by:
    // 1) Calculate a new local x-direction vector from the cross product of the new
    //    camera to view center vector and the old up vector.
    // 2) The local x vector is the normal to the plane in which the new up vector
    //    must lay. So we can take the cross product of this normal and the new
    //    x vector. The new normal vector forms the last part of the orthonormal basis
    QVector3D x = QVector3D::crossProduct( lookAt->viewVector(), lookAt->upVector() ).normalized();
    lookAt->setUpVector(QVector3D::crossProduct( x, lookAt->viewVector() ).normalized());
}

void QCamera::translateWorld(const QVector3D& vWorld , CameraTranslationOption option )
{
    Q_D(QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return ;

    // Update the camera position using the calculated world vector
    lookAt->setPosition(lookAt->position() + vWorld);

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        lookAt->setViewCenter(lookAt->viewCenter() + vWorld);

    // Refresh the camera -> view center vector
    lookAt->setViewVector(lookAt->viewCenter() - lookAt->position());
}

QQuaternion QCamera::tiltRotation(float angle) const
{
    Q_D(const QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return QQuaternion();
    QVector3D xBasis = QVector3D::crossProduct( lookAt->upVector(),
                                                lookAt->viewVector().normalized() ).normalized();
    return QQuaternion::fromAxisAndAngle( xBasis, -angle );
}

QQuaternion QCamera::panRotation(float angle) const
{
    Q_D(const QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return QQuaternion();
    return QQuaternion::fromAxisAndAngle( lookAt->upVector(), angle );
}

QQuaternion QCamera::rollRotation(float angle) const
{
    Q_D(const QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return QQuaternion();
    return QQuaternion::fromAxisAndAngle( lookAt->viewVector(), -angle );
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
    Q_D(QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return ;
    lookAt->setUpVector(q.rotatedVector(lookAt->upVector()));
    QVector3D cameraToCenter = q.rotatedVector(lookAt->viewVector());
    lookAt->setViewCenter(lookAt->position() + cameraToCenter);
}

void QCamera::rotateAboutViewCenter( const QQuaternion& q )
{
    Q_D(QCamera);
    QLookAtTransform *lookAt = d->m_transform->findFirstTransform<QLookAtTransform>();
    if (lookAt == Q_NULLPTR)
        return ;
    lookAt->setUpVector(q.rotatedVector(lookAt->upVector()));
    QVector3D cameraToCenter = q.rotatedVector(lookAt->viewVector());
    lookAt->setViewVector(cameraToCenter);
    lookAt->setPosition(lookAt->viewCenter() - cameraToCenter);
}

} // Qt3D

QT_END_NAMESPACE
