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

#ifndef CAMERA_H
#define CAMERA_H

#include "component.h"
#include <qt3dcore_global.h>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>

namespace Qt3D {

class CameraPrivate;

class QT3DCORESHARED_EXPORT Camera : public Component
{
    Q_OBJECT

    // TODO Move view matrix out of the camera and use Transform component (once it exists)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector)
    Q_PROPERTY(QVector3D viewCenter READ viewCenter WRITE setViewCenter)

    Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane NOTIFY farPlaneChanged)

    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)

    Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)

    Q_ENUMS( ProjectionType )

public:
    explicit Camera(Node *parent = 0);

    enum ProjectionType {
        OrthogonalProjection,
        PerspectiveProjection
    };

    enum CameraTranslationOption {
        TranslateViewCenter,
        DontTranslateViewCenter
    };

    void setViewMatrix(const QMatrix4x4& m);

    QVector3D position() const;
    QVector3D upVector() const;
    QVector3D viewCenter() const;

    QVector3D viewVector() const;

    void setProjectionType(ProjectionType projectioType);
    ProjectionType projectionType() const;

    void setNearPlane(float nearPlane);
    float nearPlane() const;

    void setFarPlane(float nearPlane);
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

    QMatrix4x4 viewMatrix() const;
    QMatrix4x4 projectionMatrix() const;
    QMatrix4x4 viewProjectionMatrix() const;

    QQuaternion tiltRotation(float angle) const;
    QQuaternion panRotation(float angle) const;
    QQuaternion rollRotation(float angle) const;

public slots:
    void setOrthographicProjection(float left, float right,
                                   float bottom, float top,
                                   float nearPlane, float farPlane);

    void setPerspectiveProjection(float fieldOfView, float aspect,
                                  float nearPlane, float farPlane);

    // TODO These slots should likely be on a modifier one layer up
    void setPosition( const QVector3D& position );
    void setUpVector( const QVector3D& upVector );
    void setViewCenter( const QVector3D& viewCenter );

    // Translate relative to camera orientation axes
    void translate( const QVector3D& vLocal, CameraTranslationOption option = TranslateViewCenter );

    // Translate relative to world axes
    void translateWorld( const QVector3D& vWorld, CameraTranslationOption option = TranslateViewCenter );

    void tilt( const float& angle );
    void pan( const float& angle );
    void roll( const float& angle );

    void tiltAboutViewCenter( const float& angle );
    void panAboutViewCenter( const float& angle );
    void rollAboutViewCenter( const float& angle );

    void rotate( const QQuaternion& q );
    void rotateAboutViewCenter( const QQuaternion& q );

signals:
    void projectionTypeChanged();
    void nearPlaneChanged();
    void farPlaneChanged();
    void fieldOfViewChanged();
    void aspectRatioChanged();
    void leftChanged();
    void rightChanged();
    void bottomChanged();
    void topChanged();

protected:
    Q_DECLARE_PRIVATE(Camera)

private:
    CameraPrivate* d_ptr;
};

}

#endif // CAMERA_H
