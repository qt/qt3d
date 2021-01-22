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
****************************************************************************/

#ifndef QT3DINPUT_INPUT_INPUTMANAGERS_P_H
#define QT3DINPUT_INPUT_INPUTMANAGERS_P_H

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

#include <QtCore/QtGlobal>

#include <Qt3DInput/private/handle_types_p.h>
#include <Qt3DInput/private/keyboarddevice_p.h>
#include <Qt3DInput/private/keyboardhandler_p.h>
#include <Qt3DInput/private/mousehandler_p.h>
#include <Qt3DInput/private/mousedevice_p.h>
#include <Qt3DInput/private/actioninput_p.h>
#include <Qt3DInput/private/inputsequence_p.h>
#include <Qt3DInput/private/inputchord_p.h>
#include <Qt3DInput/private/action_p.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/axisaccumulator_p.h>
#include <Qt3DInput/private/axissetting_p.h>
#include <Qt3DInput/private/analogaxisinput_p.h>
#include <Qt3DInput/private/buttonaxisinput_p.h>
#include <Qt3DInput/private/logicaldevice_p.h>
#include <Qt3DInput/private/genericdevicebackendnode_p.h>
#include <Qt3DInput/private/physicaldeviceproxy_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class KeyboardInputManager : public Qt3DCore::QResourceManager<
        KeyboardHandler,
        Qt3DCore::QNodeId>
{
public:
    KeyboardInputManager() {}
};

class KeyboardDeviceManager : public Qt3DCore::QResourceManager<
        KeyboardDevice,
        Qt3DCore::QNodeId>
{
public:
    KeyboardDeviceManager() {}
};

class MouseDeviceManager : public Qt3DCore::QResourceManager<
        MouseDevice,
        Qt3DCore::QNodeId>
{
public:
    MouseDeviceManager() {}
};

class MouseInputManager : public Qt3DCore::QResourceManager<
        MouseHandler,
        Qt3DCore::QNodeId>
{
public:
    MouseInputManager() {}
};

class AxisManager : public Qt3DCore::QResourceManager<
        Axis,
        Qt3DCore::QNodeId>
{
public:
    AxisManager() {}
};

class AxisSettingManager : public Qt3DCore::QResourceManager<
        AxisSetting,
        Qt3DCore::QNodeId>
{
public:
    AxisSettingManager() {}
};

class ActionManager : public Qt3DCore::QResourceManager<
        Action,
        Qt3DCore::QNodeId>
{
public:
    ActionManager() {}
};

class ActionInputManager : public Qt3DCore::QResourceManager<
        ActionInput,
        Qt3DCore::QNodeId>
{
public:
    ActionInputManager() {}
};

class AnalogAxisInputManager : public Qt3DCore::QResourceManager<
        AnalogAxisInput,
        Qt3DCore::QNodeId>
{
public:
    AnalogAxisInputManager() {}
};

class ButtonAxisInputManager : public Qt3DCore::QResourceManager<
        ButtonAxisInput,
        Qt3DCore::QNodeId>
{
public:
    ButtonAxisInputManager() {}
};

class InputChordManager : public Qt3DCore::QResourceManager<
        InputChord,
        Qt3DCore::QNodeId>
{
public:
    InputChordManager() {}
};

class InputSequenceManager : public Qt3DCore::QResourceManager<
        InputSequence,
        Qt3DCore::QNodeId>
{
public:
    InputSequenceManager() {}
};

class LogicalDeviceManager : public Qt3DCore::QResourceManager<
        LogicalDevice,
        Qt3DCore::QNodeId>
{
public:
    LogicalDeviceManager() {}

    QVector<HLogicalDevice> activeDevices() const { return m_activeDevices; }
    void addActiveDevice(HLogicalDevice device) { m_activeDevices.push_back(device); }
    void removeActiveDevice(HLogicalDevice device) { m_activeDevices.removeOne(device); }

private:
    QVector<HLogicalDevice> m_activeDevices;
};

class GenericDeviceBackendNodeManager : public Qt3DCore::QResourceManager<
        GenericDeviceBackendNode,
        Qt3DCore::QNodeId>
{
public:
    GenericDeviceBackendNodeManager() {}
};

class Q_AUTOTEST_EXPORT PhysicalDeviceProxyManager : public Qt3DCore::QResourceManager<
        PhysicalDeviceProxy,
        Qt3DCore::QNodeId>
{
public:
    PhysicalDeviceProxyManager() {}

    void addPendingProxyToLoad(Qt3DCore::QNodeId id) { m_pendingProxies.push_back(id); }
    QVector<Qt3DCore::QNodeId> takePendingProxiesToLoad() { return std::move(m_pendingProxies); }

private:
    QVector<Qt3DCore::QNodeId> m_pendingProxies;
};

class AxisAccumulatorManager : public Qt3DCore::QResourceManager<
        AxisAccumulator,
        Qt3DCore::QNodeId>
{
public:
    AxisAccumulatorManager() {}
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_INPUTMANAGERS_P_H
