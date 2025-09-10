// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qfirstpersoncameracontroller.h"

#include <Qt3DRender/QCamera>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
    \class Qt3DExtras::QFirstPersonCameraController
    \ingroup qt3d-extras-cameracontrollers
    \brief The QFirstPersonCameraController class allows controlling the scene camera
    from the first person perspective.
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
        \li While the left mouse button is pressed, mouse movement along x-axis pans the camera and
        movement along y-axis tilts it.
    \row
        \li Mouse scroll wheel
        \li Zooms the camera in and out without changing the view center.
    \row
        \li Shift key
        \li Turns the fine motion control active while pressed. Makes mouse pan and tilt less
        sensitive.
    \row
        \li Arrow keys
        \li Move the camera horizontally relative to camera viewport.
    \row
        \li Page up and page down keys
        \li Move the camera vertically relative to camera viewport.
    \row
        \li Escape
        \li Moves the camera so that entire scene is visible in the camera viewport.
    \endtable
*/

QFirstPersonCameraController::QFirstPersonCameraController(Qt3DCore::QNode *parent)
    : QAbstractCameraController(parent)
{
}

QFirstPersonCameraController::~QFirstPersonCameraController()
{
}


void QFirstPersonCameraController::moveCamera(const QAbstractCameraController::InputState &state, float dt)
{
    Qt3DRender::QCamera *theCamera = camera();

    if (theCamera == nullptr)
        return;

    theCamera->translate(QVector3D(state.txAxisValue * linearSpeed(),
                                  state.tyAxisValue * linearSpeed(),
                                  state.tzAxisValue * linearSpeed()) * dt);
    if (state.leftMouseButtonActive) {
        float theLookSpeed = lookSpeed();
        if (state.shiftKeyActive) {
            theLookSpeed *= 0.2f;
        }

        const QVector3D upVector(0.0f, 1.0f, 0.0f);

        theCamera->pan(state.rxAxisValue * theLookSpeed * dt, upVector);
        theCamera->tilt(state.ryAxisValue * theLookSpeed * dt);
    }
}

} // Qt3DExtras

QT_END_NAMESPACE

#include "moc_qfirstpersoncameracontroller.cpp"
