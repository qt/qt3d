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

#ifndef QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_P_H
#define QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_P_H

#include <Qt3DCore/private/qentity_p.h>
#include <QVector3D>
#include "qfirstpersoncameracontroller.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QCamera;
}

namespace Qt3DLogic {
class QLogicComponent;
}

namespace Qt3DInput {

class QKeyboardController;
class QMouseController;
class QLogicalDevice;
class QAction;
class QActionInput;
class QAxis;
class QAxisInput;
class QAxisActionHandler;

class QFirstPersonCameraControllerPrivate : public Qt3DCore::QEntityPrivate
{
public:
    QFirstPersonCameraControllerPrivate();

    void init();

    Qt3DRender::QCamera *m_camera;
    QAction *m_leftMouseButtonAction;
    QAction *m_fineMotionAction;
    QAxis *m_rxAxis;
    QAxis *m_ryAxis;
    QAxis *m_txAxis;
    QAxis *m_tyAxis;
    QAxis *m_tzAxis;
    QActionInput *m_leftMouseButtonInput;
    QActionInput *m_fineMotionKeyInput;
    QAxisInput *m_mouseRxInput;
    QAxisInput *m_mouseRyInput;
    QAxisInput *m_keyboardTxPosInput;
    QAxisInput *m_keyboardTyPosInput;
    QAxisInput *m_keyboardTzPosInput;
    QAxisInput *m_keyboardTxNegInput;
    QAxisInput *m_keyboardTyNegInput;
    QAxisInput *m_keyboardTzNegInput;
    QKeyboardController *m_keyboardController;
    QMouseController *m_mouseController;
    QLogicalDevice *m_logicalDevice;
    QAxisActionHandler *m_axisActionHandler;
    Qt3DLogic::QLogicComponent *m_logicComponent;
    float m_linearSpeed;
    float m_lookSpeed;
    QVector3D m_vxyz;
    bool m_fineMotion;
    QVector3D m_rxyz;
    bool m_leftMouseButtonPressed;
    QVector3D m_firstPersonUp;

    void _q_onFrameUpdate(float);
    void _q_onAxisValueChanged(QString, float);
    void _q_onActionStarted(QString);
    void _q_onActionFinished(QString);

    Q_DECLARE_PUBLIC(QFirstPersonCameraController)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_P_H
