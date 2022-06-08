// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QABSTRACTCAMERACONTROLLER_P_H
#define QT3DEXTRAS_QABSTRACTCAMERACONTROLLER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DExtras/qabstractcameracontroller.h>
#include <QtGui/QVector3D>

#include <Qt3DCore/private/qentity_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QCamera;
}

namespace Qt3DLogic {
class QFrameAction;
}

namespace Qt3DInput {
class QKeyboardDevice;
class QMouseDevice;
class QLogicalDevice;
class QAction;
class QActionInput;
class QAxis;
class QAnalogAxisInput;
class QButtonAxisInput;
class QAxisActionHandler;
}

namespace Qt3DExtras {

class QAbstractCameraControllerPrivate : public Qt3DCore::QEntityPrivate
{
public:
    QAbstractCameraControllerPrivate();

    void init();
    void applyInputAccelerations();

    Qt3DRender::QCamera *m_camera;

    Qt3DInput::QAction *m_leftMouseButtonAction;
    Qt3DInput::QAction *m_middleMouseButtonAction;
    Qt3DInput::QAction *m_rightMouseButtonAction;
    Qt3DInput::QAction *m_altButtonAction;
    Qt3DInput::QAction *m_shiftButtonAction;
    Qt3DInput::QAction *m_escapeButtonAction;

    Qt3DInput::QAxis *m_rxAxis;
    Qt3DInput::QAxis *m_ryAxis;
    Qt3DInput::QAxis *m_txAxis;
    Qt3DInput::QAxis *m_tyAxis;
    Qt3DInput::QAxis *m_tzAxis;

    Qt3DInput::QActionInput *m_leftMouseButtonInput;
    Qt3DInput::QActionInput *m_middleMouseButtonInput;
    Qt3DInput::QActionInput *m_rightMouseButtonInput;
    Qt3DInput::QActionInput *m_altButtonInput;
    Qt3DInput::QActionInput *m_shiftButtonInput;
    Qt3DInput::QActionInput *m_escapeButtonInput;

    Qt3DInput::QAnalogAxisInput *m_mouseRxInput;
    Qt3DInput::QAnalogAxisInput *m_mouseRyInput;
    Qt3DInput::QAnalogAxisInput *m_mouseTzXInput;
    Qt3DInput::QAnalogAxisInput *m_mouseTzYInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTxPosInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTyPosInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTzPosInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTxNegInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTyNegInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTzNegInput;

    Qt3DInput::QKeyboardDevice *m_keyboardDevice;
    Qt3DInput::QMouseDevice *m_mouseDevice;

    Qt3DInput::QLogicalDevice *m_logicalDevice;

    Qt3DLogic::QFrameAction *m_frameAction;

    float m_linearSpeed;
    float m_lookSpeed;

    float m_acceleration;
    float m_deceleration;

    QVector3D m_sceneUp;

    Q_DECLARE_PUBLIC(QAbstractCameraController)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QABSTRACTCAMERACONTROLLER_P_H
