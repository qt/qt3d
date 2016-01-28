/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QTINPUT_QORBITCONTROL_P_H
#define QTINPUT_QORBITCONTROL_P_H

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

#include <Qt3DCore/private/qentity_p.h>
#include <QVector3D>
#include "qorbitcontrol.h"

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
class QAxisInput;
class QAxisActionHandler;

class QOrbitControlPrivate : public Qt3DCore::QEntityPrivate
{
public:
    QOrbitControlPrivate();

    void init();

    Qt3DRender::QCamera *m_camera;

    QAction *m_leftMouseButtonAction;
    QAction *m_rightMouseButtonAction;
    QAction *m_altButtonAction;
    QAction *m_shiftButtonAction;

    QAxis *m_rxAxis;
    QAxis *m_ryAxis;
    QAxis *m_txAxis;
    QAxis *m_tyAxis;
    QAxis *m_tzAxis;

    QActionInput *m_leftMouseButtonInput;
    QActionInput *m_rightMouseButtonInput;
    QActionInput *m_altButtonInput;
    QActionInput *m_shiftButtonInput;

    QAxisInput *m_mouseRxInput;
    QAxisInput *m_mouseRyInput;
    QAxisInput *m_keyboardTxPosInput;
    QAxisInput *m_keyboardTyPosInput;
    QAxisInput *m_keyboardTzPosInput;
    QAxisInput *m_keyboardTxNegInput;
    QAxisInput *m_keyboardTyNegInput;
    QAxisInput *m_keyboardTzNegInput;

    QKeyboardDevice *m_keyboardDevice;
    QMouseDevice *m_mouseDevice;

    QLogicalDevice *m_logicalDevice;

    Qt3DLogic::QFrameAction *m_frameAction;

    float m_linearSpeed;
    float m_lookSpeed;
    float m_zoomInLimit;
    QVector3D m_cameraUp;

    void _q_onTriggered(float);

    Q_DECLARE_PUBLIC(QOrbitControl)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QTINPUT_QORBITCONTROL_P_H
