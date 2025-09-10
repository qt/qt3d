// Copyright (C) 2023 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK_INPUT_FOREIGN_P_H
#define QT3D_QUICK_INPUT_FOREIGN_P_H

#include <QQmlEngine>

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qactioninput.h>
#include <Qt3DInput/qanalogaxisinput.h>
#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qaxisaccumulator.h>
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DInput/qbuttonaxisinput.h>
#include <Qt3DInput/qinputchord.h>
#include <Qt3DInput/qinputsequence.h>
#include <Qt3DInput/qinputsettings.h>
#include <Qt3DInput/qkeyboarddevice.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <Qt3DInput/qkeyevent.h>
#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DInput/qmousedevice.h>
#include <Qt3DInput/qmouseevent.h>
#include <Qt3DInput/qmousehandler.h>

#include <Qt3DInput/private/qgenericinputdevice_p.h>
#include <Qt3DQuickInput/private/quick3daction_p.h>
#include <Qt3DQuickInput/private/quick3daxis_p.h>
#include <Qt3DQuickInput/private/quick3dinputchord_p.h>
#include <Qt3DQuickInput/private/quick3dinputsequence_p.h>
#include <Qt3DQuickInput/private/quick3dlogicaldevice_p.h>
#include <Qt3DQuickInput/private/quick3dphysicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Input {

struct QKeyEventForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(KeyEvent)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QKeyEvent)
    QML_UNCREATABLE("Events cannot be created")
};

struct QKeyboardDeviceForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(KeyboardDevice)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QKeyboardDevice)
};

struct QKeyboardHandlerForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(KeyboardHandler)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QKeyboardHandler)
};

struct QInputSettingsForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(InputSettings)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QInputSettings)
};

struct QQMouseEventForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(MouseEvent)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QMouseEvent)
    QML_UNCREATABLE("Events cannot be created")
};

#if QT_CONFIG(wheelevent)
struct QWheelEventForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(WheelEvent)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QWheelEvent)
    QML_UNCREATABLE("Events cannot be created")
};
#endif

struct QMouseHandlerForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(MouseHandler)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QMouseHandler)
};

struct QMouseDeviceForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(MouseDevice)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QMouseDevice)
};

struct QLogicalDeviceDeviceForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(LogicalDevice)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QLogicalDevice)
    QML_EXTENDED(Qt3DInput::Input::Quick::Quick3DLogicalDevice)
};

struct QAbstractActionInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractActionInput)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAbstractActionInput)
    QML_UNCREATABLE("AbstractActionInput is abstract")
};

struct QActionInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ActionInput)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QActionInput)
};

struct QAbstractAxisInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractAxisInput)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAbstractAxisInput)
    QML_UNCREATABLE("AbstractAxisInput is abstract")
};

struct QAxisSettingForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AxisSetting)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAxisSetting)
};

struct QAnalogAxisInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AnalogAxisInput)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAnalogAxisInput)
};

struct QButtonAxisInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ButtonAxisInput)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QButtonAxisInput)
};

struct QAxisForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Axis)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAxis)
    QML_EXTENDED(Qt3DInput::Input::Quick::Quick3DAxis)
};

struct QActionForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Action)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAction)
    QML_EXTENDED(Qt3DInput::Input::Quick::Quick3DAction)
};

struct QInputSequenceForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(InputSequence)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QInputSequence)
    QML_EXTENDED(Qt3DInput::Input::Quick::Quick3DInputSequence)
};

struct QInputChordForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(InputChord)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QInputChord)
    QML_EXTENDED(Qt3DInput::Input::Quick::Quick3DInputChord)
};

struct QAbstractPhysicalDeviceForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractPhysicalDevice)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QAbstractPhysicalDevice)
    QML_EXTENDED(Qt3DInput::Input::Quick::Quick3DPhysicalDevice)
    QML_UNCREATABLE("QAbstractPhysicalDevice is abstract")
};

struct QAxisAccumulatorForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AxisAccumulator)
    QML_ADDED_IN_VERSION(2, 1)
    QML_FOREIGN(Qt3DInput::QAxisAccumulator)
};

#ifdef HAVE_QGAMEPAD
struct QGamepadInputForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(GamepadInput)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DInput::QGamepadInput)
};
#endif

} // namespace Input
} // namespace Qt3D

QT_END_NAMESPACE

#endif
