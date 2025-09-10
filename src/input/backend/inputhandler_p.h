// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_INPUTHANDLER_P_H
#define QT3DINPUT_INPUT_INPUTHANDLER_P_H

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

#include <Qt3DInput/QKeyEvent>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qaspectjob.h>
#include <QtCore/QMutex>

#include <Qt3DInput/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEventFilterService;
class QNodeId;
class QScene;
}

namespace Qt3DInput {

class QInputDeviceIntegration;
class QAbstractPhysicalDevice;

namespace Input {

class AbstractActionInput;
class KeyboardInputManager;
class KeyboardDeviceManager;
class MouseDeviceManager;
class MouseInputManager;
class InternalEventFilter;
class AxisManager;
class AxisAccumulatorManager;
class ActionManager;
class AxisSettingManager;
class ActionInputManager;
class AnalogAxisInputManager;
class ButtonAxisInputManager;
class InputChordManager;
class InputSequenceManager;
class LogicalDeviceManager;
class GenericPhysicalDeviceManager;
class GenericDeviceBackendNodeManager;
class PhysicalDeviceProxyManager;
class InputSettings;

class Q_AUTOTEST_EXPORT InputHandler
{
public:
    InputHandler();
    ~InputHandler();

    void setScene(Qt3DCore::QScene *scene) { m_scene = scene; }

    inline KeyboardDeviceManager *keyboardDeviceManager() const { return m_keyboardDeviceManager; }
    inline KeyboardInputManager *keyboardInputManager() const  { return m_keyboardInputManager; }
    inline MouseDeviceManager *mouseDeviceManager() const { return m_mouseDeviceManager; }
    inline MouseInputManager *mouseInputManager() const { return m_mouseInputManager; }
    inline AxisManager *axisManager() const { return m_axisManager; }
    inline AxisAccumulatorManager *axisAccumulatorManager() const { return m_axisAccumulatorManager; }
    inline ActionManager *actionManager() const { return m_actionManager; }
    inline AxisSettingManager *axisSettingManager() const { return m_axisSettingManager; }
    inline ActionInputManager *actionInputManager() const { return m_actionInputManager; }
    inline AnalogAxisInputManager *analogAxisInputManager() const { return m_analogAxisInputManager; }
    inline ButtonAxisInputManager *buttonAxisInputManager() const { return m_buttonAxisInputManager; }
    inline InputChordManager *inputChordManager() const { return m_inputChordManager; }
    inline InputSequenceManager *inputSequenceManager() const { return m_inputSequenceManager; }
    inline LogicalDeviceManager *logicalDeviceManager() const { return m_logicalDeviceManager; }
    inline GenericDeviceBackendNodeManager *genericDeviceBackendNodeManager() const { return m_genericPhysicalDeviceBackendNodeManager; }
    inline PhysicalDeviceProxyManager *physicalDeviceProxyManager() const { return m_physicalDeviceProxyManager; }
    inline InputSettings *inputSettings() const { return m_settings; }

    void appendKeyboardDevice(HKeyboardDevice device);
    void removeKeyboardDevice(HKeyboardDevice device);

    void appendMouseDevice(HMouseDevice device);
    void removeMouseDevice(HMouseDevice device);

    void appendGenericDevice(HGenericDeviceBackendNode device);
    void removeGenericDevice(HGenericDeviceBackendNode device);

    void resetMouseAxisState();

    QList<Qt3DInput::QInputDeviceIntegration *> inputDeviceIntegrations() const;
    void addInputDeviceIntegration(QInputDeviceIntegration *inputIntegration);

    void setInputSettings(InputSettings *settings);

    QAbstractPhysicalDevice *createPhysicalDevice(const QString &name);

    void updateEventSource();
    void setEventFilterService(Qt3DCore::QEventFilterService *service);

    AbstractActionInput *lookupActionInput(Qt3DCore::QNodeId id) const;

private:
    friend class InternalEventFilter;

    Qt3DCore::QScene *m_scene;
    KeyboardDeviceManager *m_keyboardDeviceManager;
    KeyboardInputManager *m_keyboardInputManager;
    MouseDeviceManager *m_mouseDeviceManager;
    MouseInputManager *m_mouseInputManager;

    QList<HKeyboardDevice> m_activeKeyboardDevices;
    QList<HMouseDevice> m_activeMouseDevices;
    QList<HGenericDeviceBackendNode> m_activeGenericPhysicalDevices;
    InternalEventFilter *m_eventFilter;

    AxisManager *m_axisManager;
    AxisAccumulatorManager *m_axisAccumulatorManager;
    ActionManager *m_actionManager;
    AxisSettingManager *m_axisSettingManager;
    ActionInputManager *m_actionInputManager;
    AnalogAxisInputManager *m_analogAxisInputManager;
    ButtonAxisInputManager *m_buttonAxisInputManager;
    InputChordManager *m_inputChordManager;
    InputSequenceManager *m_inputSequenceManager;
    LogicalDeviceManager *m_logicalDeviceManager;
    GenericDeviceBackendNodeManager *m_genericPhysicalDeviceBackendNodeManager;
    PhysicalDeviceProxyManager *m_physicalDeviceProxyManager;
    QList<Qt3DInput::QInputDeviceIntegration *> m_inputDeviceIntegrations;
    InputSettings *m_settings;
    Qt3DCore::QEventFilterService *m_service;
    QObject *m_lastEventSource;

    void registerEventFilters();
    void unregisterEventFilters();
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_INPUTHANDLER_P_H
