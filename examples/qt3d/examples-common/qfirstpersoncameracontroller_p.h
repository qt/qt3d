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

#ifndef QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_P_H
#define QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_P_H

#include <Qt3DCore/private/qentity_p.h>
#include <QVector3D>
#include "qfirstpersoncameracontroller.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
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

    Qt3DCore::QCamera *m_camera;
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
