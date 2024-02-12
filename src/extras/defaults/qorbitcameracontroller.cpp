// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qorbitcameracontroller.h"
#include "qorbitcameracontroller_p.h"

#include <Qt3DRender/QCamera>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

QOrbitCameraControllerPrivate::QOrbitCameraControllerPrivate()
    : m_zoomInLimit(2.0f)
    , m_upVector(0.0f, 1.0f, 0.0f)
    , m_inverseXTranslate(false)
    , m_inverseYTranslate(false)
    , m_inversePan(false)
    , m_inverseTilt(false)
    , m_zoomTranslateViewCenter(true)
{}

/*!
    \class Qt3DExtras::QOrbitCameraController
    \ingroup qt3d-extras-cameracontrollers
    \brief The QOrbitCameraController class allows controlling the scene camera along orbital path.
    \inmodule Qt3DExtras
    \since 5.7
    \inherits Qt3DCore::QEntity

    The controls are:
    \table
    \header
        \li Input
        \li Action
    \row
        \li Left mouse button
        \li While the left mouse button is pressed, mouse movement along x-axis moves the camera
        left and right and movement along y-axis moves it up and down.
    \row
        \li Right mouse button
        \li While the right mouse button is pressed, mouse movement along x-axis pans the camera
        around the camera view center and movement along y-axis tilts it around the camera
        view center.
    \row
        \li Both left and right mouse button
        \li While both the left and the right mouse button are pressed, mouse movement along y-axis
        zooms the camera in and out without changing the view center.
    \row
        \li Mouse scroll wheel
        \li Zooms the camera in and out without changing the view center.
    \row
        \li Arrow keys
        \li Move the camera vertically and horizontally relative to camera viewport.
    \row
        \li Page up and page down keys
        \li Move the camera forwards and backwards.
    \row
        \li Shift key
        \li Changes the behavior of the up and down arrow keys to zoom the camera in and out
        without changing the view center. The other movement keys are disabled.
    \row
        \li Alt key
        \li Changes the behovior of the arrow keys to pan and tilt the camera around the view
        center. Disables the page up and page down keys.
    \row
        \li Escape
        \li Moves the camera so that entire scene is visible in the camera viewport.
    \endtable
*/

QOrbitCameraController::QOrbitCameraController(Qt3DCore::QNode *parent)
    : QOrbitCameraController(*new QOrbitCameraControllerPrivate, parent)
{
}

/*! \internal
 */
QOrbitCameraController::QOrbitCameraController(QOrbitCameraControllerPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractCameraController(dd, parent)
{
}

QOrbitCameraController::~QOrbitCameraController()
{
}

/*!
    \property Qt3DExtras::QOrbitCameraController::zoomInLimit

    Holds the current zoom-in limit. The zoom-in limit determines how close to the view center
    the camera can be zoomed.
*/
float QOrbitCameraController::zoomInLimit() const
{
    Q_D(const QOrbitCameraController);
    return d->m_zoomInLimit;
}

void QOrbitCameraController::setZoomInLimit(float zoomInLimit)
{
    Q_D(QOrbitCameraController);
    if (d->m_zoomInLimit != zoomInLimit) {
        d->m_zoomInLimit = zoomInLimit;
        emit zoomInLimitChanged();
    }
}

QVector3D QOrbitCameraController::upVector() const {
    Q_D(const QOrbitCameraController);
    return d->m_upVector;
}

void QOrbitCameraController::setUpVector(const QVector3D& upVector)
{
    Q_D(QOrbitCameraController);
    if (d->m_upVector != upVector) {
        d->m_upVector = upVector;
        emit upVectorChanged(d->m_upVector);
    }
}

bool QOrbitCameraController::inverseXTranslate() const
{
    Q_D(const QOrbitCameraController);
    return d->m_inverseXTranslate;
}

void QOrbitCameraController::setInverseXTranslate(bool isInverse)
{
    Q_D(QOrbitCameraController);
    if (d->m_inverseXTranslate != isInverse) {
        d->m_inverseXTranslate = isInverse;
        emit inverseXTranslateChanged(d->m_inverseXTranslate);
    }
}

bool QOrbitCameraController::inverseYTranslate() const
{
    Q_D(const QOrbitCameraController);
    return d->m_inverseYTranslate;
}

void QOrbitCameraController::setInverseYTranslate(bool isInverse)
{
    Q_D(QOrbitCameraController);
    if (d->m_inverseYTranslate != isInverse) {
        d->m_inverseYTranslate = isInverse;
        emit inverseYTranslateChanged(d->m_inverseYTranslate);
    }
}

bool QOrbitCameraController::inversePan() const
{
    Q_D(const QOrbitCameraController);
    return d->m_inversePan;
}

void QOrbitCameraController::setInversePan(bool isInverse)
{
    Q_D(QOrbitCameraController);
    if (d->m_inversePan != isInverse) {
        d->m_inversePan = isInverse;
        emit inversePanChanged(d->m_inversePan);
    }
}

bool QOrbitCameraController::inverseTilt() const
{
    Q_D(const QOrbitCameraController);
    return d->m_inverseTilt;
}

void QOrbitCameraController::setInverseTilt(bool isInverse)
{
    Q_D(QOrbitCameraController);
    if (d->m_inverseTilt != isInverse) {
        d->m_inverseTilt = isInverse;
        emit inverseTiltChanged(d->m_inverseTilt);
    }
}

bool QOrbitCameraController::zoomTranslateViewCenter() const
{
    Q_D(const QOrbitCameraController);
    return d->m_zoomTranslateViewCenter;
}

void QOrbitCameraController::setZoomTranslateViewCenter(bool isTranslate)
{
    Q_D(QOrbitCameraController);
    if (d->m_zoomTranslateViewCenter != isTranslate) {
        d->m_zoomTranslateViewCenter = isTranslate;
        emit zoomTranslateViewCenterChanged(d->m_zoomTranslateViewCenter);
    }
}

inline float clampInputs(float input1, float input2)
{
    float axisValue = input1 + input2;
    return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
}

inline float zoomDistance(QVector3D firstPoint, QVector3D secondPoint)
{
    return (secondPoint - firstPoint).lengthSquared();
}

void QOrbitCameraController::moveCamera(const QAbstractCameraController::InputState &state, float dt)
{
    Q_D(QOrbitCameraController);

    Qt3DRender::QCamera *theCamera = camera();

    if (theCamera == nullptr)
        return;

    // Mouse input
    if (state.leftMouseButtonActive) {
        if (state.rightMouseButtonActive) {
            if ( zoomDistance(camera()->position(), theCamera->viewCenter()) > d->m_zoomInLimit * d->m_zoomInLimit) {
                // Dolly up to limit
                theCamera->translate(QVector3D(0, 0, state.ryAxisValue), theCamera->DontTranslateViewCenter);
            } else {
                theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
            }
        } else {
            // Translate
            theCamera->translate(QVector3D((d->m_inverseXTranslate ? -1.0f : 1.0f) * clampInputs(state.rxAxisValue, state.txAxisValue) * linearSpeed(),
                                           (d->m_inverseYTranslate ? -1.0f : 1.0f) * clampInputs(state.ryAxisValue, state.tyAxisValue) * linearSpeed(),
                                           0) * dt);
        }
        return;
    }
    else if (state.rightMouseButtonActive) {
        // Orbit
        theCamera->panAboutViewCenter((d->m_inversePan ? -1.0f : 1.0f) * (state.rxAxisValue * lookSpeed()) * dt, d->m_upVector);
        theCamera->tiltAboutViewCenter((d->m_inverseTilt ? -1.0f : 1.0f) * (state.ryAxisValue * lookSpeed()) * dt);
    }

    // Keyboard Input
    if (state.altKeyActive) {
        // Orbit
        theCamera->panAboutViewCenter((state.txAxisValue * lookSpeed()) * dt, d->m_upVector);
        theCamera->tiltAboutViewCenter((state.tyAxisValue * lookSpeed()) * dt);
    } else if (state.shiftKeyActive) {
        if (zoomDistance(camera()->position(), theCamera->viewCenter()) > d->m_zoomInLimit * d->m_zoomInLimit) {
            // Dolly
            theCamera->translate(QVector3D(0, 0, state.tzAxisValue * linearSpeed() * dt), theCamera->DontTranslateViewCenter);
        } else {
            theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
        }
    } else {
        // Translate
        theCamera->translate(QVector3D(clampInputs(state.leftMouseButtonActive ? state.rxAxisValue : 0, state.txAxisValue) * linearSpeed(),
                                      clampInputs(state.leftMouseButtonActive ? state.ryAxisValue : 0, state.tyAxisValue) * linearSpeed(),
                                      state.tzAxisValue * linearSpeed()) * dt,
                             d->m_zoomTranslateViewCenter ? theCamera->TranslateViewCenter : theCamera->DontTranslateViewCenter);
    }
}

} // Qt3DExtras

QT_END_NAMESPACE

#include "moc_qorbitcameracontroller.cpp"
