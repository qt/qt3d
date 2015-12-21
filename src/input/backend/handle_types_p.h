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

#ifndef QT3DINPUT_INPUT_HANDLE_TYPES_P_H
#define QT3DINPUT_INPUT_HANDLE_TYPES_P_H

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

#include <Qt3DCore/private/qhandle_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class KeyboardController;
class KeyboardInput;
class MouseController;
class MouseInput;
class Axis;
class AxisActionHandler;
class AxisInput;
class AxisSetting;
class Action;
class ActionInput;
class LogicalDevice;

typedef Qt3DCore::QHandle<KeyboardController, 8> HKeyboardController;
typedef Qt3DCore::QHandle<KeyboardInput, 16> HKeyboardInput;
typedef Qt3DCore::QHandle<MouseController, 8> HMouseController;
typedef Qt3DCore::QHandle<MouseInput, 16> HMouseInput;
typedef Qt3DCore::QHandle<Axis, 16> HAxis;
typedef Qt3DCore::QHandle<AxisActionHandler, 16> HAxisActionHandler;
typedef Qt3DCore::QHandle<AxisSetting, 16> HAxisSetting;
typedef Qt3DCore::QHandle<Action, 16> HAction;
typedef Qt3DCore::QHandle<AxisInput, 16> HAxisInput;
typedef Qt3DCore::QHandle<ActionInput, 16> HActionInput;
typedef Qt3DCore::QHandle<LogicalDevice, 16> HLogicalDevice;

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_HANDLE_TYPES_P_H
