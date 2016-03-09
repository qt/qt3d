/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfirstpersoncameracontroller_p.h"
#include "qfirstpersoncameracontroller.h"
#include <Qt3DRender/QCamera>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAxisInput>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QKeyboardController>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseEvent>
#include <Qt3DLogic/QLogicComponent>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {


QFirstPersonCameraControllerPrivate::QFirstPersonCameraControllerPrivate()
    : Qt3DCore::QEntityPrivate()
    , m_camera(Q_NULLPTR)
    , m_leftMouseButtonAction(new QAction())
    , m_fineMotionAction(new QAction())
    , m_rxAxis(new QAxis())
    , m_ryAxis(new QAxis())
    , m_txAxis(new QAxis())
    , m_tyAxis(new QAxis())
    , m_tzAxis(new QAxis())
    , m_leftMouseButtonInput(new QActionInput())
    , m_fineMotionKeyInput(new QActionInput())
    , m_mouseRxInput(new QAxisInput())
    , m_mouseRyInput(new QAxisInput())
    , m_keyboardTxPosInput(new QAxisInput())
    , m_keyboardTyPosInput(new QAxisInput())
    , m_keyboardTzPosInput(new QAxisInput())
    , m_keyboardTxNegInput(new QAxisInput())
    , m_keyboardTyNegInput(new QAxisInput())
    , m_keyboardTzNegInput(new QAxisInput())
    , m_keyboardController(new QKeyboardController())
    , m_mouseDevice(new QMouseDevice())
    , m_logicalDevice(new QLogicalDevice())
    , m_logicComponent(new Qt3DLogic::QLogicComponent())
    , m_linearSpeed(10.0f)
    , m_lookSpeed(180.0f)
    , m_firstPersonUp(QVector3D(0.0f, 1.0f, 0.0f))
{}

void QFirstPersonCameraControllerPrivate::init()
{
    //// Actions

    // Left Mouse Button Action
    m_leftMouseButtonInput->setButtons(QVariantList() << QMouseEvent::LeftButton);
    m_leftMouseButtonInput->setSourceDevice(m_mouseDevice);
    m_leftMouseButtonAction->addInput(m_leftMouseButtonInput);

    // Fine Motion Action
    m_fineMotionKeyInput->setButtons(QVariantList() << Qt::Key_Shift);
    m_fineMotionKeyInput->setSourceDevice(m_keyboardController);
    m_fineMotionAction->addInput(m_fineMotionKeyInput);

    //// Axes

    // Mouse X
    m_mouseRxInput->setAxis(QMouseDevice::X);
    m_mouseRxInput->setSourceDevice(m_mouseDevice);
    m_rxAxis->addInput(m_mouseRxInput);

    // Mouse Y
    m_mouseRyInput->setAxis(QMouseDevice::Y);
    m_mouseRyInput->setSourceDevice(m_mouseDevice);
    m_ryAxis->addInput(m_mouseRyInput);

    // Keyboard Pos Tx
    m_keyboardTxPosInput->setButtons(QVariantList() << Qt::Key_Right);
    m_keyboardTxPosInput->setScale(1.0f);
    m_keyboardTxPosInput->setSourceDevice(m_keyboardController);
    m_txAxis->addInput(m_keyboardTxPosInput);

    // Keyboard Pos Ty
    m_keyboardTyPosInput->setButtons(QVariantList() << Qt::Key_PageUp);
    m_keyboardTyPosInput->setScale(1.0f);
    m_keyboardTyPosInput->setSourceDevice(m_keyboardController);
    m_tyAxis->addInput(m_keyboardTyPosInput);

    // Keyboard Pos Tz
    m_keyboardTzPosInput->setButtons(QVariantList() << Qt::Key_Up);
    m_keyboardTzPosInput->setScale(1.0f);
    m_keyboardTzPosInput->setSourceDevice(m_keyboardController);
    m_tzAxis->addInput(m_keyboardTzPosInput);

    // Keyboard Neg Tx
    m_keyboardTxNegInput->setButtons(QVariantList() << Qt::Key_Left);
    m_keyboardTxNegInput->setScale(-1.0f);
    m_keyboardTxNegInput->setSourceDevice(m_keyboardController);
    m_txAxis->addInput(m_keyboardTxNegInput);

    // Keyboard Neg Ty
    m_keyboardTyNegInput->setButtons(QVariantList() << Qt::Key_PageDown);
    m_keyboardTyNegInput->setScale(-1.0f);
    m_keyboardTyNegInput->setSourceDevice(m_keyboardController);
    m_tyAxis->addInput(m_keyboardTyNegInput);

    // Keyboard Neg Tz
    m_keyboardTzNegInput->setButtons(QVariantList() << Qt::Key_Down);
    m_keyboardTzNegInput->setScale(-1.0f);
    m_keyboardTzNegInput->setSourceDevice(m_keyboardController);
    m_tzAxis->addInput(m_keyboardTzNegInput);

    //// Logical Device

    m_logicalDevice->addAction(m_fineMotionAction);
    m_logicalDevice->addAction(m_leftMouseButtonAction);
    m_logicalDevice->addAxis(m_rxAxis);
    m_logicalDevice->addAxis(m_ryAxis);
    m_logicalDevice->addAxis(m_txAxis);
    m_logicalDevice->addAxis(m_tyAxis);
    m_logicalDevice->addAxis(m_tzAxis);

    Q_Q(QFirstPersonCameraController);
    //// LogicComponent

    QObject::connect(m_logicComponent, SIGNAL(frameUpdate(float)),
                     q, SLOT(_q_onFrameUpdate(float)));

    q->addComponent(m_logicComponent);
    q->addComponent(m_logicalDevice);
}

void QFirstPersonCameraControllerPrivate::_q_onFrameUpdate(float dt)
{
    if (m_camera != Q_NULLPTR) {
        m_camera->translate(QVector3D(m_txAxis->value() * m_linearSpeed,
                                      m_tyAxis->value() * m_linearSpeed,
                                      m_tzAxis->value() * m_linearSpeed) * dt);
        if (m_leftMouseButtonAction->isActive()) {
            m_camera->pan(m_rxAxis->value() * m_lookSpeed * dt, m_firstPersonUp);
            m_camera->tilt(m_ryAxis->value() * m_lookSpeed * dt);
        }
    }
}

QFirstPersonCameraController::QFirstPersonCameraController(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(*new QFirstPersonCameraControllerPrivate, parent)
{
    Q_D(QFirstPersonCameraController);
    d->init();
}

QFirstPersonCameraController::~QFirstPersonCameraController()
{
}

Qt3DRender::QCamera *QFirstPersonCameraController::camera() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_camera;
}

float QFirstPersonCameraController::linearSpeed() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_linearSpeed;
}

float QFirstPersonCameraController::lookSpeed() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_lookSpeed;
}

void QFirstPersonCameraController::setCamera(Qt3DRender::QCamera *camera)
{
    Q_D(QFirstPersonCameraController);
    if (d->m_camera != camera) {
        d->m_camera = camera;
        emit cameraChanged();
    }
}

void QFirstPersonCameraController::setLinearSpeed(float linearSpeed)
{
    Q_D(QFirstPersonCameraController);
    if (d->m_linearSpeed != linearSpeed) {
        d->m_linearSpeed = linearSpeed;
        emit linearSpeedChanged();
    }
}

void QFirstPersonCameraController::setLookSpeed(float lookSpeed)
{
    Q_D(QFirstPersonCameraController);
    if (d->m_lookSpeed != lookSpeed) {
        d->m_lookSpeed = lookSpeed;
        emit lookSpeedChanged();
    }
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qfirstpersoncameracontroller.cpp"
