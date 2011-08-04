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

#include "qglcameraanimation.h"
#include "qglcamera.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLCameraAnimation
    \brief The QGLCameraAnimation class implements smooth animations between two camera positions.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    QGLCameraAnimation modifies the camera() object to fall on a smooth
    path between startEye(), startUpVector(), startCenter() and
    endEye(), endUpVector(), endCenter.  The animation will attempt to
    keep the eye and center at the same relative distance from each other
    throughout the animation, as though one object is flying around another.

    \sa QGLCamera
*/

class QGLCameraAnimationPrivate
{
public:
    enum PointOfRotation
    {
        Center,
        CenterDual,
        Eye,
        EyeDual,
        Both
    };

    QGLCameraAnimationPrivate()
        : camera(0)
        , startEye(0.0f, 0.0f, 10.f)
        , startUpVector(0.0f, 1.0f, 0.0f)
        , startCenter(0.0f, 0.0f, 0.0f)
        , endEye(0.0f, 0.0f, 10.f)
        , endUpVector(0.0f, 1.0f, 0.0f)
        , endCenter(0.0f, 0.0f, 0.0f)
        , duration(250)
        , dirty(true)
        , pointOfRotation(Both)
        , lengthStart(1.0f)
        , lengthEnd(1.0f)
    {
    }

    QGLCamera *camera;
    QVector3D startEye;
    QVector3D startUpVector;
    QVector3D startCenter;
    QVector3D endEye;
    QVector3D endUpVector;
    QVector3D endCenter;
    int duration;
    bool dirty;
    QEasingCurve easingCurve;

    // Derived values for use during the animation.
    PointOfRotation pointOfRotation;
    QVector3D upVectorAxis;
    qreal upVectorAngle;
    QVector3D pointAxis;
    qreal pointAngle;
    QVector3D centerTranslate;
    QVector3D eyeTranslate;
    qreal lengthStart;
    qreal lengthEnd;

    static void rotateBetween(const QVector3D &start, const QVector3D &end,
                              const QVector3D &defaultAxis,
                              QVector3D *rotationAxis, qreal *rotationAngle);
    void deriveRotations();
};

static inline bool fuzzyCompareVectors(const QVector3D &v1, const QVector3D &v2)
{
    return qAbs(v1.x() - v2.x()) <= 0.00001 &&
           qAbs(v1.y() - v2.y()) <= 0.00001 &&
           qAbs(v1.z() - v2.z()) <= 0.00001;
}

// Determine how to rotate between the "start" and "end" vectors.
// We use the cross-product of the two vectors as the axis of rotation
// or "defaultAxis" if the cross-product is zero (180 degree rotation).
void QGLCameraAnimationPrivate::rotateBetween
    (const QVector3D &start, const QVector3D &end, const QVector3D &defaultAxis,
     QVector3D *rotationAxis, qreal *rotationAngle)
{
    QVector3D nstart = start.normalized();
    QVector3D nend = end.normalized();
    if (qFuzzyCompare(nstart, nend)) {
        // Vectors are the same, so no rotation to perform.
        *rotationAxis = QVector3D(0, 1, 0);
        *rotationAngle = 0.0f;
    } else {
        QVector3D axis = QVector3D::crossProduct(nstart, nend);
        if (qFuzzyIsNull(axis.x()) && qFuzzyIsNull(axis.y()) &&
                qFuzzyIsNull(axis.z())) {
            // Vectors are probably at 180 degrees to each other.
            // We can pick either direction; arbitrarily pick anti-clockwise.
            *rotationAxis = defaultAxis;
            *rotationAngle = 180.0f;
        } else {
            // Compute the smallest angle between the two vectors
            // from the cosine of the angle (i.e. the dot product).
            *rotationAxis = axis;
            *rotationAngle =
                qAcos(QVector3D::dotProduct(nstart, nend)) * 180.0f / M_PI;
        }
    }
}

void QGLCameraAnimationPrivate::deriveRotations()
{
    // If the center points are the same, rotate the eye around the center.
    // If the eye points are the same, rotate the center around the eye.
    // If both are different, then interpolate along linear vectors.
    if (qFuzzyCompare(startCenter, endCenter)) {
        if (qFuzzyCompare(startEye, endEye)) {
            // Center and eye both stay the same: nothing to do.
            pointOfRotation = Both;
            centerTranslate = QVector3D();
            eyeTranslate = QVector3D();
        } else {
            // Centers are the same, rotate the eye position.
            pointOfRotation = Center;
            rotateBetween(startEye - startCenter, endEye - startCenter,
                          startUpVector, &pointAxis, &pointAngle);
            lengthStart = (startEye - startCenter).length();
            lengthEnd = (endEye - startCenter).length();

            // Rotate the start up vector to the final position.  If it is
            // different than the end up vector, we need to perform the
            // animation in two steps: rotate eye, then rotate up.
            QQuaternion q =
                QQuaternion::fromAxisAndAngle(pointAxis, pointAngle);
            QVector3D up = q.rotatedVector(startUpVector);
            if (!fuzzyCompareVectors(startUpVector, endUpVector) &&
                    !fuzzyCompareVectors(up, endUpVector)) {
                pointOfRotation = CenterDual;
                rotateBetween(up, endUpVector, endEye - endCenter,
                              &upVectorAxis, &upVectorAngle);
            }
        }
    } else if (qFuzzyCompare(startEye, endEye)) {
        // Eyes are the same, rotate the center position.
        pointOfRotation = Eye;
        rotateBetween(startCenter - startEye, endCenter - startEye,
                      startUpVector, &pointAxis, &pointAngle);
        lengthStart = (startCenter - startEye).length();
        lengthEnd = (startCenter - endEye).length();

        // Rotate the start up vector to the final position.  If it is
        // different than the end up vector, we need to perform the
        // animation in two steps: rotate eye, then rotate up.
        QQuaternion q =
            QQuaternion::fromAxisAndAngle(pointAxis, pointAngle);
        QVector3D up = q.rotatedVector(startUpVector);
        if (!fuzzyCompareVectors(startUpVector, endUpVector) &&
                !fuzzyCompareVectors(up, endUpVector)) {
            pointOfRotation = EyeDual;
            rotateBetween(up, endUpVector, endCenter - endEye,
                          &upVectorAxis, &upVectorAngle);
        }
    } else {
        // Both center and eye are changing, so perform a linear translation.
        pointOfRotation = Both;
        centerTranslate = endCenter - startCenter;
        eyeTranslate = endEye - startEye;
    }

    // If we are doing a single movement, then determine how to
    // rotate the up vector during the movement.
    if (pointOfRotation != CenterDual) {
        rotateBetween(startUpVector, endUpVector, startCenter - startEye,
                      &upVectorAxis, &upVectorAngle);
    }
}

/*!
    Constructs a new camera animation object and attaches it to \a parent.
*/
QGLCameraAnimation::QGLCameraAnimation(QObject *parent)
    : QAbstractAnimation(parent)
    , d_ptr(new QGLCameraAnimationPrivate)
{
}

/*!
    Destroys this camera animation object.
*/
QGLCameraAnimation::~QGLCameraAnimation()
{
}

/*!
    \property QGLCameraAnimation::camera
    \brief the camera that will be animated by this animation object;
    null if the camera has not yet been set.
*/

QGLCamera *QGLCameraAnimation::camera() const
{
    Q_D(const QGLCameraAnimation);
    return d->camera;
}

void QGLCameraAnimation::setCamera(QGLCamera *camera)
{
    Q_D(QGLCameraAnimation);
    d->camera = camera;
}

/*!
    \property QGLCameraAnimation::startEye
    \brief the position of the viewer's eye at the start of the animation.
    The default value is (0, 0, 10).

    \sa startUpVector(), startCenter(), endEye()
*/

QVector3D QGLCameraAnimation::startEye() const
{
    Q_D(const QGLCameraAnimation);
    return d->startEye;
}

void QGLCameraAnimation::setStartEye(const QVector3D &eye)
{
    Q_D(QGLCameraAnimation);
    d->startEye = eye;
    d->dirty = true;
}

/*!
    \property QGLCameraAnimation::startUpVector
    \brief the up vector for the viewer at the start of the animation.
    The default value is (0, 1, 0).

    \sa startEye(), startCenter(), endUpVector()
*/

QVector3D QGLCameraAnimation::startUpVector() const
{
    Q_D(const QGLCameraAnimation);
    return d->startUpVector;
}

void QGLCameraAnimation::setStartUpVector(const QVector3D &upVector)
{
    Q_D(QGLCameraAnimation);
    d->startUpVector = upVector;
    d->dirty = true;
}

/*!
    \property QGLCameraAnimation::startCenter
    \brief the center of the view visible from the viewer's position
    at the start of the animation.  The default value is (0, 0, 0).

    \sa startEye(), startUpVector(), endCenter()
*/

QVector3D QGLCameraAnimation::startCenter() const
{
    Q_D(const QGLCameraAnimation);
    return d->startCenter;
}

void QGLCameraAnimation::setStartCenter(const QVector3D &center)
{
    Q_D(QGLCameraAnimation);
    d->startCenter = center;
    d->dirty = true;
}

/*!
    \property QGLCameraAnimation::endEye
    \brief the position of the viewer's eye at the end of the animation.
    The default value is (0, 0, 10).

    \sa endUpVector(), endCenter(), startEye()
*/

QVector3D QGLCameraAnimation::endEye() const
{
    Q_D(const QGLCameraAnimation);
    return d->endEye;
}

void QGLCameraAnimation::setEndEye(const QVector3D &eye)
{
    Q_D(QGLCameraAnimation);
    d->endEye = eye;
    d->dirty = true;
}

/*!
    \property QGLCameraAnimation::endUpVector
    \brief the up vector for the viewer at the end of the animation.
    The default value is (0, 1, 0).

    \sa endEye(), endCenter(), startUpVector()
*/

QVector3D QGLCameraAnimation::endUpVector() const
{
    Q_D(const QGLCameraAnimation);
    return d->endUpVector;
}

void QGLCameraAnimation::setEndUpVector(const QVector3D &upVector)
{
    Q_D(QGLCameraAnimation);
    d->endUpVector = upVector;
    d->dirty = true;
}

/*!
    \property QGLCameraAnimation::endCenter
    \brief the center of the view visible from the viewer's position
    at the end of the animation.  The default value is (0, 0, 0).

    \sa endEye(), endUpVector(), startCenter()
*/

QVector3D QGLCameraAnimation::endCenter() const
{
    Q_D(const QGLCameraAnimation);
    return d->endCenter;
}

void QGLCameraAnimation::setEndCenter(const QVector3D &center)
{
    Q_D(QGLCameraAnimation);
    d->endCenter = center;
    d->dirty = true;
}

/*!
    \property QGLCameraAnimation::duration
    \brief the duration of the animation in milliseconds.  The default
    value is 250 milliseconds.

    \sa easingCurve()
*/

int QGLCameraAnimation::duration() const
{
    Q_D(const QGLCameraAnimation);
    return d->duration;
}

void QGLCameraAnimation::setDuration(int duration)
{
    Q_D(QGLCameraAnimation);
    d->duration = duration;
}

/*!
    \property QGLCameraAnimation::easingCurve
    \brief the easing curve to use for the animation.  The default
    value is a linear easing curve.

    \sa duration()
*/

QEasingCurve QGLCameraAnimation::easingCurve() const
{
    Q_D(const QGLCameraAnimation);
    return d->easingCurve;
}

void QGLCameraAnimation::setEasingCurve(const QEasingCurve &easing)
{
    Q_D(QGLCameraAnimation);
    d->easingCurve = easing;
}

/*!
    \internal
*/
void QGLCameraAnimation::updateCurrentTime(int currentTime)
{
    Q_D(QGLCameraAnimation);
    if (!d->camera)
        return;     // Nothing to do if no camera to modify.
    if (currentTime >= d->duration) {
        d->camera->setEye(d->endEye);
        d->camera->setUpVector(d->endUpVector);
        d->camera->setCenter(d->endCenter);
    } else if (currentTime <= 0) {
        d->camera->setEye(d->startEye);
        d->camera->setUpVector(d->startUpVector);
        d->camera->setCenter(d->startCenter);
    } else {
        // Derive the rotation parameters we need if arguments have changed.
        if (d->dirty) {
            d->dirty = false;
            d->deriveRotations();
        }

        // Calculate the progress and modify it with the easing curve.
        qreal progress = d->easingCurve.valueForProgress
            (qreal(currentTime) / qreal(d->duration));

        // Calculate the new eye and center locations.
        QVector3D eye = d->startEye;
        QVector3D center = d->startCenter;
        QVector3D upVector = d->startUpVector;
        if (d->pointOfRotation == QGLCameraAnimationPrivate::Center) {
            QQuaternion q = QQuaternion::fromAxisAndAngle
                (d->pointAxis, d->pointAngle * progress);
            eye = q.rotatedVector(eye - d->startCenter);
            if (d->lengthStart != d->lengthEnd) {
                qreal length = (1.0f - progress) * d->lengthStart +
                               progress * d->lengthEnd;
                eye = eye.normalized() * length;
            }
            eye += d->startCenter;
        } else if (d->pointOfRotation == QGLCameraAnimationPrivate::CenterDual) {
            if (progress < 0.5f) {
                // First half of the animation - rotate the eye position.
                QQuaternion q = QQuaternion::fromAxisAndAngle
                    (d->pointAxis, d->pointAngle * progress * 2.0f);
                eye = q.rotatedVector(eye - d->startCenter);
                if (d->lengthStart != d->lengthEnd) {
                    qreal length = (1.0f - progress) * d->lengthStart +
                                   progress * d->lengthEnd;
                    eye = eye.normalized() * length;
                }
                eye += d->startCenter;
                upVector = q.rotatedVector(upVector);
            } else {
                // Second half of the animation - rotate the up vector.
                QQuaternion q = QQuaternion::fromAxisAndAngle
                        (d->upVectorAxis,
                         d->upVectorAngle * (progress - 0.5f) * 2.0f) *
                    QQuaternion::fromAxisAndAngle
                        (d->pointAxis, d->pointAngle);
                eye = d->endEye;
                upVector = q.rotatedVector(upVector);
            }
        } else if (d->pointOfRotation == QGLCameraAnimationPrivate::Eye) {
            QQuaternion q = QQuaternion::fromAxisAndAngle
                (d->pointAxis, d->pointAngle * progress);
            center = q.rotatedVector(center - d->startEye);
            if (d->lengthStart != d->lengthEnd) {
                qreal length = (1.0f - progress) * d->lengthStart +
                               progress * d->lengthEnd;
                center = center.normalized() * length;
            }
            center += d->startEye;
        } else if (d->pointOfRotation == QGLCameraAnimationPrivate::EyeDual) {
            if (progress < 0.5f) {
                // First half of the animation - rotate the center position.
                QQuaternion q = QQuaternion::fromAxisAndAngle
                    (d->pointAxis, d->pointAngle * progress * 2.0f);
                center = q.rotatedVector(center - d->startEye);
                if (d->lengthStart != d->lengthEnd) {
                    qreal length = (1.0f - progress) * d->lengthStart +
                                   progress * d->lengthEnd;
                    center = center.normalized() * length;
                }
                center += d->startEye;
                upVector = q.rotatedVector(upVector);
            } else {
                // Second half of the animation - rotate the up vector.
                QQuaternion q = QQuaternion::fromAxisAndAngle
                        (d->upVectorAxis,
                         d->upVectorAngle * (progress - 0.5f) * 2.0f) *
                    QQuaternion::fromAxisAndAngle
                        (d->pointAxis, d->pointAngle);
                center = d->endCenter;
                upVector = q.rotatedVector(upVector);
            }
        } else {
            eye += d->eyeTranslate * progress;
            center += d->centerTranslate * progress;
        }

        // Calculate the new up vector for single-step animations.
        if (d->pointOfRotation != QGLCameraAnimationPrivate::CenterDual &&
                d->pointOfRotation != QGLCameraAnimationPrivate::EyeDual) {
            if (d->upVectorAngle != 0.0f) {
                QQuaternion q = QQuaternion::fromAxisAndAngle
                    (d->upVectorAxis, d->upVectorAngle * progress);
                upVector = q.rotatedVector(upVector);
            }
        }

        d->camera->setEye(eye);
        d->camera->setUpVector(upVector);
        d->camera->setCenter(center);
    }
}

QT_END_NAMESPACE
