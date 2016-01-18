/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qfirstpersoncameracontroller_p.h"
#include "qfirstpersoncameracontroller.h"
#include <Qt3DRender/QCamera>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAxisInput>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QAxisActionHandler>
#include <Qt3DInput/QKeyboardController>
#include <Qt3DInput/QMouseController>
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
    , m_mouseController(new QMouseController())
    , m_logicalDevice(new QLogicalDevice())
    , m_axisActionHandler(new QAxisActionHandler())
    , m_logicComponent(new Qt3DLogic::QLogicComponent())
    , m_linearSpeed(10.0f)
    , m_lookSpeed(180.0f)
    , m_fineMotion(false)
    , m_leftMouseButtonPressed(false)
    , m_firstPersonUp(QVector3D(0.0f, 1.0f, 0.0f))
{}

void QFirstPersonCameraControllerPrivate::init()
{
    //// Actions

    // Left Mouse Button Action
    m_leftMouseButtonInput->setKeys(QVariantList() << QMouseController::Left);
    m_leftMouseButtonInput->setSourceDevice(m_mouseController);
    m_leftMouseButtonAction->addInput(m_leftMouseButtonInput);
    m_leftMouseButtonAction->setName(QStringLiteral("LMB"));

    // Fine Motion Action
    m_fineMotionKeyInput->setKeys(QVariantList() << Qt::Key_Shift);
    m_fineMotionKeyInput->setSourceDevice(m_keyboardController);
    m_fineMotionAction->addInput(m_fineMotionKeyInput);
    m_fineMotionAction->setName(QStringLiteral("fineMotion"));

    //// Axes

    // Mouse X
    m_mouseRxInput->setAxis(QMouseController::X);
    m_mouseRxInput->setSourceDevice(m_mouseController);
    m_rxAxis->addInput(m_mouseRxInput);
    m_rxAxis->setName(QStringLiteral("RX"));

    // Mouse Y
    m_mouseRyInput->setAxis(QMouseController::Y);
    m_mouseRyInput->setSourceDevice(m_mouseController);
    m_ryAxis->addInput(m_mouseRyInput);
    m_ryAxis->setName(QStringLiteral("RY"));

    // Keyboard Pos Tx
    m_keyboardTxPosInput->setKeys(QVariantList() << Qt::Key_Right);
    m_keyboardTxPosInput->setScale(1.0f);
    m_keyboardTxPosInput->setSourceDevice(m_keyboardController);
    m_txAxis->addInput(m_keyboardTxPosInput);

    // Keyboard Pos Ty
    m_keyboardTyPosInput->setKeys(QVariantList() << Qt::Key_PageUp);
    m_keyboardTyPosInput->setScale(1.0f);
    m_keyboardTyPosInput->setSourceDevice(m_keyboardController);
    m_tyAxis->addInput(m_keyboardTyPosInput);

    // Keyboard Pos Tz
    m_keyboardTzPosInput->setKeys(QVariantList() << Qt::Key_Up);
    m_keyboardTzPosInput->setScale(1.0f);
    m_keyboardTzPosInput->setSourceDevice(m_keyboardController);
    m_tzAxis->addInput(m_keyboardTzPosInput);

    // Keyboard Neg Tx
    m_keyboardTxNegInput->setKeys(QVariantList() << Qt::Key_Left);
    m_keyboardTxNegInput->setScale(-1.0f);
    m_keyboardTxNegInput->setSourceDevice(m_keyboardController);
    m_txAxis->addInput(m_keyboardTxNegInput);

    // Keyboard Neg Ty
    m_keyboardTyNegInput->setKeys(QVariantList() << Qt::Key_PageDown);
    m_keyboardTyNegInput->setScale(-1.0f);
    m_keyboardTyNegInput->setSourceDevice(m_keyboardController);
    m_tyAxis->addInput(m_keyboardTyNegInput);

    // Keyboard Neg Tz
    m_keyboardTzNegInput->setKeys(QVariantList() << Qt::Key_Down);
    m_keyboardTzNegInput->setScale(-1.0f);
    m_keyboardTzNegInput->setSourceDevice(m_keyboardController);
    m_tzAxis->addInput(m_keyboardTzNegInput);

    m_txAxis->setName(QStringLiteral("TX"));
    m_tyAxis->setName(QStringLiteral("TY"));
    m_tzAxis->setName(QStringLiteral("TZ"));

    //// Logical Device

    m_logicalDevice->addAction(m_fineMotionAction);
    m_logicalDevice->addAction(m_leftMouseButtonAction);
    m_logicalDevice->addAxis(m_rxAxis);
    m_logicalDevice->addAxis(m_ryAxis);
    m_logicalDevice->addAxis(m_txAxis);
    m_logicalDevice->addAxis(m_tyAxis);
    m_logicalDevice->addAxis(m_tzAxis);

    Q_Q(QFirstPersonCameraController);

    //// AxisActionHandler

    QObject::connect(m_axisActionHandler, SIGNAL(actionStarted(QString)),
                     q, SLOT(_q_onActionStarted(QString)));
    QObject::connect(m_axisActionHandler, SIGNAL(actionFinished(QString)),
                     q, SLOT(_q_onActionFinished(QString)));
    QObject::connect(m_axisActionHandler, SIGNAL(axisValueChanged(QString,float)),
                     q, SLOT(_q_onAxisValueChanged(QString,float)));

    m_axisActionHandler->setLogicalDevice(m_logicalDevice);

    //// LogicComponent

    QObject::connect(m_logicComponent, SIGNAL(frameUpdate(float)),
                     q, SLOT(_q_onFrameUpdate(float)));

    q->addComponent(m_logicComponent);
    q->addComponent(m_axisActionHandler);
}

void QFirstPersonCameraControllerPrivate::_q_onFrameUpdate(float dt)
{
    if (m_camera != Q_NULLPTR) {
        m_camera->translate(m_vxyz * dt);
        if (m_leftMouseButtonPressed) {
            m_camera->pan(m_lookSpeed * m_rxyz.x() * dt, m_firstPersonUp);
            m_camera->pan(m_lookSpeed * m_rxyz.y() * dt);
        }
    }
}

void QFirstPersonCameraControllerPrivate::_q_onAxisValueChanged(QString name, float value)
{
    if (name == QStringLiteral("RX"))
        m_rxyz.setX(value);
    else if (name == QStringLiteral("RY"))
        m_rxyz.setY(value);
    else if (name == QStringLiteral("TX"))
        m_vxyz.setX(value);
    else if (name == QStringLiteral("TY"))
        m_vxyz.setY(value);
    else if (name == QStringLiteral("TZ"))
        m_vxyz.setZ(value);
}

void QFirstPersonCameraControllerPrivate::_q_onActionStarted(QString name)
{
    if (name == QStringLiteral("LMB"))
        m_leftMouseButtonPressed = true;
    else if (name == QStringLiteral("fineMotion"))
        m_fineMotion = true;
}

void QFirstPersonCameraControllerPrivate::_q_onActionFinished(QString name)
{
    if (name == QStringLiteral("LMB"))
        m_leftMouseButtonPressed = false;
    else if (name == QStringLiteral("fineMotion"))
        m_fineMotion = false;
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
