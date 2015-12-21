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

#include <QtGlobal>
#include <Qt3DInput/private/handle_types_p.h>
#include <Qt3DInput/private/keyboardcontroller_p.h>
#include <Qt3DInput/private/keyboardinput_p.h>
#include <Qt3DInput/private/mousecontroller_p.h>
#include <Qt3DInput/private/mouseinput_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DInput/private/actioninput_p.h>
#include <Qt3DInput/private/axisinput_p.h>
#include <Qt3DInput/private/action_p.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/axisactionhandler_p.h>
#include <Qt3DInput/private/axissetting_p.h>
#include <Qt3DInput/private/logicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class KeyboardInputManager : public Qt3DCore::QResourceManager<
        KeyboardInput,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    KeyboardInputManager() {}
};

class KeyboardControllerManager : public Qt3DCore::QResourceManager<
        KeyboardController,
        Qt3DCore::QNodeId,
        8,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    KeyboardControllerManager() {}
};

class MouseControllerManager : public Qt3DCore::QResourceManager<
        MouseController,
        Qt3DCore::QNodeId,
        8,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    MouseControllerManager() {}
};

class MouseInputManager : public Qt3DCore::QResourceManager<
        MouseInput,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    MouseInputManager() {}
};

class AxisManager : public Qt3DCore::QResourceManager<
        Axis,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    AxisManager() {}
};

class AxisActionHandlerManager : public Qt3DCore::QResourceManager<
        AxisActionHandler,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    AxisActionHandlerManager() {}

    void addActiveAxisActionHandler(HAxisActionHandler handle) { m_activeAxisActionHandlers.push_back(handle); }
    void removeActiveAxisActionHandler(HAxisActionHandler handle) { m_activeAxisActionHandlers.removeOne(handle); }
    QVector<HAxisActionHandler> activeAxisActionHandlers() const { return m_activeAxisActionHandlers; }

private:
    QVector<HAxisActionHandler> m_activeAxisActionHandlers;
};

class AxisSettingManager : public Qt3DCore::QResourceManager<
        AxisSetting,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    AxisSettingManager() {}
};

class AxisInputManager : public Qt3DCore::QResourceManager<
        AxisInput,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    AxisInputManager() {}
};

class ActionManager : public Qt3DCore::QResourceManager<
        Action,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    ActionManager() {}
};

class ActionInputManager : public Qt3DCore::QResourceManager<
        ActionInput,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    ActionInputManager() {}
};

class LogicalDeviceManager : public Qt3DCore::QResourceManager<
        LogicalDevice,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    LogicalDeviceManager() {}

    QVector<HLogicalDevice> activeDevices() const { return m_activeDevices; }
    void addActiveDevice(HLogicalDevice device) { m_activeDevices.push_back(device); }
    void removeActiveDevice(HLogicalDevice device) { m_activeDevices.removeOne(device); }

private:
    QVector<HLogicalDevice> m_activeDevices;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_INPUTMANAGERS_P_H
