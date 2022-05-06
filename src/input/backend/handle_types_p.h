/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

class KeyboardDevice;
class KeyboardHandler;
class MouseHandler;
class MouseDevice;
class Axis;
class AxisActionHandler;
class AbstractAxisInput;
class AxisSetting;
class Action;
class ActionInput;
class InputSequence;
class InputChord;
class LogicalDevice;
class GenericDeviceBackendNode;
class PhysicalDeviceProxy;
class AxisAccumulator;

typedef Qt3DCore::QHandle<KeyboardDevice> HKeyboardDevice;
typedef Qt3DCore::QHandle<KeyboardHandler> HKeyboardHandler;
typedef Qt3DCore::QHandle<MouseHandler> HMouseHandler;
typedef Qt3DCore::QHandle<MouseDevice> HMouseDevice;
typedef Qt3DCore::QHandle<Axis> HAxis;
typedef Qt3DCore::QHandle<AxisActionHandler> HAxisActionHandler;
typedef Qt3DCore::QHandle<AxisSetting> HAxisSetting;
typedef Qt3DCore::QHandle<Action> HAction;
typedef Qt3DCore::QHandle<AbstractAxisInput> HAxisInput;
typedef Qt3DCore::QHandle<ActionInput> HActionInput;
typedef Qt3DCore::QHandle<InputSequence> HInputSequence;
typedef Qt3DCore::QHandle<InputChord> HInputChord;
typedef Qt3DCore::QHandle<LogicalDevice> HLogicalDevice;
typedef Qt3DCore::QHandle<GenericDeviceBackendNode> HGenericDeviceBackendNode;
typedef Qt3DCore::QHandle<PhysicalDeviceProxy> HPhysicalDeviceProxy;
typedef Qt3DCore::QHandle<AxisAccumulator> HAxisAccumulator;

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_HANDLE_TYPES_P_H
