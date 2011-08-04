/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLCAMERA_H
#define QGLCAMERA_H

#include "qt3dglobal.h"
#include "qglnamespace.h"
#include <QtCore/qobject.h>
#include <QtCore/qsize.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qquaternion.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLCameraPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLCamera : public QObject
{
    Q_OBJECT
    Q_ENUMS(ProjectionType RotateOrder)
    Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionChanged)
    Q_PROPERTY(qreal fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY projectionChanged)
    Q_PROPERTY(qreal nearPlane READ nearPlane WRITE setNearPlane NOTIFY projectionChanged)
    Q_PROPERTY(qreal farPlane READ farPlane WRITE setFarPlane NOTIFY projectionChanged)
    Q_PROPERTY(QSizeF viewSize READ viewSize WRITE setViewSize NOTIFY projectionChanged SCRIPTABLE false)
    Q_PROPERTY(QSizeF minViewSize READ minViewSize WRITE setMinViewSize NOTIFY projectionChanged SCRIPTABLE false)
    Q_PROPERTY(int screenRotation READ screenRotation WRITE setScreenRotation NOTIFY projectionChanged SCRIPTABLE false)
    Q_PROPERTY(QVector3D eye READ eye WRITE setEye NOTIFY viewChanged)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY viewChanged)
    Q_PROPERTY(QVector3D center READ center WRITE setCenter NOTIFY viewChanged)
    Q_PROPERTY(qreal eyeSeparation READ eyeSeparation WRITE setEyeSeparation NOTIFY viewChanged)
    Q_PROPERTY(QVector3D motionAdjustment READ motionAdjustment WRITE setMotionAdjustment DESIGNABLE false NOTIFY viewChanged SCRIPTABLE false)
    Q_PROPERTY(bool adjustForAspectRatio READ adjustForAspectRatio WRITE setAdjustForAspectRatio NOTIFY viewChanged)
public:
    explicit QGLCamera(QObject *parent = 0);
    ~QGLCamera();

    enum ProjectionType
    {
        Perspective,
        Orthographic
    };

    QGLCamera::ProjectionType projectionType() const;
    void setProjectionType(QGLCamera::ProjectionType value);

    qreal fieldOfView() const;
    void setFieldOfView(qreal angle);

    qreal nearPlane() const;
    void setNearPlane(qreal value);

    qreal farPlane() const;
    void setFarPlane(qreal value);

    QSizeF viewSize() const;
    void setViewSize(const QSizeF& size);

    QSizeF minViewSize() const;
    void setMinViewSize(const QSizeF& size);

    int screenRotation() const;
    void setScreenRotation(int angle);

    QVector3D eye() const;
    void setEye(const QVector3D& vertex);

    QVector3D upVector() const;
    void setUpVector(const QVector3D& vector);

    QVector3D center() const;
    void setCenter(const QVector3D& vertex);

    qreal eyeSeparation() const;
    void setEyeSeparation(qreal value);

    QVector3D motionAdjustment() const;
    void setMotionAdjustment(const QVector3D& vector);

    bool adjustForAspectRatio() const;
    void setAdjustForAspectRatio(bool value);

    QQuaternion tilt(qreal angle) const;
    QQuaternion pan(qreal angle) const;
    QQuaternion roll(qreal angle) const;

    void rotateEye(const QQuaternion& q);
    void rotateCenter(const QQuaternion& q);

    QVector3D translation(qreal x, qreal y, qreal z) const;

    QMatrix4x4 projectionMatrix(qreal aspectRatio) const;
    QMatrix4x4 modelViewMatrix(QGL::Eye eye = QGL::NoEye) const;

    QVector3D mapPoint
        (const QPoint& point, qreal aspectRatio,
         const QSize& viewportSize) const;

    enum RotateOrder
    {
        TiltPanRoll,
        TiltRollPan,
        PanTiltRoll,
        PanRollTilt,
        RollTiltPan,
        RollPanTilt
    };

public Q_SLOTS:
    void translateEye(qreal x, qreal y, qreal z);
    void translateCenter(qreal x, qreal y, qreal z);

    void tiltPanRollCenter(qreal tiltAngle, qreal panAngle, qreal rollAngle,
                           QGLCamera::RotateOrder order = TiltPanRoll);
    void tiltPanRollEye(qreal tiltAngle, qreal panAngle, qreal rollAngle,
                        QGLCamera::RotateOrder order = TiltPanRoll);

Q_SIGNALS:
    void projectionChanged();
    void viewChanged();

private:
    QGLCameraPrivate *d_ptr;

    QGLCameraPrivate *d_func() const { return d_ptr; }

    Q_DISABLE_COPY(QGLCamera)
};

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLCamera &order);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
