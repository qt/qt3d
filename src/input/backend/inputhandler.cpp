/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "inputhandler_p.h"

#include <Qt3DInput/private/assignkeyboardfocusjob_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/inputsettings_p.h>
#include <Qt3DInput/private/keyboardeventfilter_p.h>
#include <Qt3DInput/private/keyeventdispatcherjob_p.h>
#include <Qt3DInput/private/mouseeventdispatcherjob_p.h>
#include <Qt3DInput/private/mouseeventfilter_p.h>
#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
namespace Input {

InputHandler::InputHandler()
    : m_keyboardDeviceManager(new KeyboardDeviceManager())
    , m_keyboardInputManager(new KeyboardInputManager())
    , m_mouseDeviceManager(new MouseDeviceManager())
    , m_mouseInputManager(new MouseInputManager())
    , m_keyboardEventFilter(new KeyboardEventFilter())
    , m_mouseEventFilter(new MouseEventFilter())
    , m_axisManager(new AxisManager())
    , m_axisAccumulatorManager(new AxisAccumulatorManager())
    , m_actionManager(new ActionManager())
    , m_axisSettingManager(new AxisSettingManager())
    , m_actionInputManager(new ActionInputManager())
    , m_analogAxisInputManager(new AnalogAxisInputManager())
    , m_buttonAxisInputManager(new ButtonAxisInputManager())
    , m_inputChordManager(new InputChordManager())
    , m_inputSequenceManager(new InputSequenceManager())
    , m_logicalDeviceManager(new LogicalDeviceManager())
    , m_genericPhysicalDeviceBackendNodeManager(new GenericDeviceBackendNodeManager)
    , m_physicalDeviceProxyManager(new PhysicalDeviceProxyManager())
    , m_settings(nullptr)
    , m_service(nullptr)
    , m_lastEventSource(nullptr)
{
    m_keyboardEventFilter->setInputHandler(this);
    m_mouseEventFilter->setInputHandler(this);
}

InputHandler::~InputHandler()
{
    delete m_keyboardDeviceManager;
    delete m_keyboardInputManager;
    delete m_mouseDeviceManager;
    delete m_mouseInputManager;
    delete m_keyboardEventFilter;
    delete m_mouseEventFilter;
    delete m_axisManager;
    delete m_axisAccumulatorManager;
    delete m_actionManager;
    delete m_axisSettingManager;
    delete m_analogAxisInputManager;
    delete m_buttonAxisInputManager;
    delete m_actionInputManager;
    delete m_inputChordManager;
    delete m_inputSequenceManager;
    delete m_logicalDeviceManager;
    delete m_genericPhysicalDeviceBackendNodeManager;
    delete m_physicalDeviceProxyManager;
}

// Called in MainThread (by the EventSourceHelperSetter)
void InputHandler::registerEventFilters()
{
    clearPendingKeyEvents();
    clearPendingMouseEvents();

    if (m_service) {
        m_service->registerEventFilter(m_keyboardEventFilter, 512);
        m_service->registerEventFilter(m_mouseEventFilter, 513);
    }
}

void InputHandler::unregisterEventFilters()
{
    if (m_service) {
        m_service->unregisterEventFilter(m_keyboardEventFilter);
        m_service->unregisterEventFilter(m_mouseEventFilter);
    }
}

void InputHandler::setInputSettings(InputSettings *settings)
{
    if (m_settings && settings == nullptr) {
        unregisterEventFilters();
        m_lastEventSource = nullptr;
        if (m_settings->eventSource() && m_service)
            m_service->shutdown(m_settings->eventSource());
    }
    if (m_service) {
        unregisterEventFilters();
        if (m_settings && m_settings->eventSource())
            m_service->shutdown(m_settings->eventSource());
    }
    m_settings = settings;
}

void InputHandler::updateEventSource()
{
    // Called every frame from input aspect
    // Should probably just listen to changes in source property on settings object
    if (!m_settings || !m_service)
        return;

    // Will be updated only if eventSource is different than
    // what was set last
    QObject *eventSource = m_settings->eventSource();
    if (eventSource && m_lastEventSource != eventSource) {
        m_service->initialize(eventSource);
        registerEventFilters();
        m_lastEventSource = eventSource;
    }
}

// Called by the keyboardEventFilter in the main thread
void InputHandler::appendKeyEvent(const QT_PREPEND_NAMESPACE(QKeyEvent) &event)
{
    m_pendingKeyEvents.append(event);
}

// Called by QInputASpect::jobsToExecute (Main Thread)
QList<QT_PREPEND_NAMESPACE(QKeyEvent)> InputHandler::pendingKeyEvents()
{
    return std::move(m_pendingKeyEvents);
}

// Called by QInputASpect::jobsToExecute (Main Thread)
void InputHandler::clearPendingKeyEvents()
{
    m_pendingKeyEvents.clear();
}

// Main Thread
void InputHandler::appendMouseEvent(const QT_PREPEND_NAMESPACE(QMouseEvent) &event)
{
    m_pendingMouseEvents.append(event);
}

// Main Thread
QList<QT_PREPEND_NAMESPACE(QMouseEvent)> InputHandler::pendingMouseEvents()
{
    return std::move(m_pendingMouseEvents);
}

// Main Thread
void InputHandler::clearPendingMouseEvents()
{
    m_pendingMouseEvents.clear();
}

#if QT_CONFIG(wheelevent)
// Main Thread
void InputHandler::appendWheelEvent(const QT_PREPEND_NAMESPACE(QWheelEvent) &event)
{
    m_pendingWheelEvents.append(event);
}

// Main Thread
QList<QT_PREPEND_NAMESPACE (QWheelEvent)> Qt3DInput::Input::InputHandler::pendingWheelEvents()
{
    return std::move(m_pendingWheelEvents);
}

// Main Thread
void InputHandler::clearPendingWheelEvents()
{
    m_pendingWheelEvents.clear();
}
#endif

void InputHandler::appendKeyboardDevice(HKeyboardDevice device)
{
    m_activeKeyboardDevices.append(device);
}

void InputHandler::removeKeyboardDevice(HKeyboardDevice device)
{
    m_activeKeyboardDevices.removeAll(device);
}

void InputHandler::appendMouseDevice(HMouseDevice device)
{
    m_activeMouseDevices.append(device);
}

void InputHandler::removeMouseDevice(HMouseDevice device)
{
    m_activeMouseDevices.removeAll(device);
}

void Qt3DInput::Input::InputHandler::appendGenericDevice(HGenericDeviceBackendNode device)
{
    m_activeGenericPhysicalDevices.append(device);
}

void Qt3DInput::Input::InputHandler::removeGenericDevice(HGenericDeviceBackendNode device)
{
    m_activeGenericPhysicalDevices.removeAll(device);
}

// Return a vector of jobs to be performed for keyboard events
// Handles all dependencies between jobs
QVector<Qt3DCore::QAspectJobPtr> InputHandler::keyboardJobs()
{
    // One job for Keyboard focus change event per Keyboard device
    QVector<QAspectJobPtr> jobs;
    const QList<QT_PREPEND_NAMESPACE(QKeyEvent)> events = pendingKeyEvents();

    for (const HKeyboardDevice &cHandle : qAsConst(m_activeKeyboardDevices)) {
        KeyboardDevice *keyboardDevice = m_keyboardDeviceManager->data(cHandle);
        if (keyboardDevice) {
            keyboardDevice->updateKeyEvents(events);
            bool haveFocusChangeJob = false;
            if (keyboardDevice->lastKeyboardInputRequester() != keyboardDevice->currentFocusItem()) {
                auto job = QSharedPointer<AssignKeyboardFocusJob>::create(keyboardDevice->peerId());
                job->setInputHandler(this);
                haveFocusChangeJob= true;
                jobs.append(std::move(job));
                // One job for Keyboard events (depends on the focus change job if there was one)
            }
            // Event dispacthing job
            if (!events.isEmpty()) {
                auto job = QSharedPointer<KeyEventDispatcherJob>::create(keyboardDevice->currentFocusItem(), events);
                job->setInputHandler(this);
                if (haveFocusChangeJob)
                    job->addDependency(qAsConst(jobs).back());
                jobs.append(std::move(job));
            }
        }
    }
    return jobs;
}

QVector<Qt3DCore::QAspectJobPtr> InputHandler::mouseJobs()
{
    QVector<QAspectJobPtr> jobs;
    const QList<QT_PREPEND_NAMESPACE(QMouseEvent)> mouseEvents = pendingMouseEvents();
#if QT_CONFIG(wheelevent)
    const QList<QT_PREPEND_NAMESPACE(QWheelEvent)> wheelEvents = pendingWheelEvents();
#endif
    for (const HMouseDevice &cHandle : qAsConst(m_activeMouseDevices)) {
        MouseDevice *controller = m_mouseDeviceManager->data(cHandle);

        controller->updateMouseEvents(mouseEvents);
#if QT_CONFIG(wheelevent)
        controller->updateWheelEvents(wheelEvents);
#endif
        // Event dispacthing job
        if (!mouseEvents.isEmpty()
#if QT_CONFIG(wheelevent)
            || !wheelEvents.empty()
#endif
                                    ) {
            // Send the events to the mouse handlers that have for sourceDevice controller
            const std::vector<HMouseHandler> &activeMouseHandlers = m_mouseInputManager->activeHandles();
            for (const HMouseHandler &mouseHandlerHandle : activeMouseHandlers) {

                MouseHandler *mouseHandler = m_mouseInputManager->data(mouseHandlerHandle);
                Q_ASSERT(mouseHandler);

                if (mouseHandler->mouseDevice() == controller->peerId()) {
                    MouseEventDispatcherJob *job = new MouseEventDispatcherJob(mouseHandler->peerId(),
                                                                               mouseEvents
#if QT_CONFIG(wheelevent)
                                                                             , wheelEvents
#endif
                                                                                           );
                    job->setInputHandler(this);
                    jobs.append(QAspectJobPtr(job));
                }
            }
        }
    }

    return jobs;
}

QVector<QInputDeviceIntegration *> InputHandler::inputDeviceIntegrations() const
{
    return m_inputDeviceIntegrations;
}

void InputHandler::addInputDeviceIntegration(QInputDeviceIntegration *inputIntegration)
{
    m_inputDeviceIntegrations.push_back(inputIntegration);
}

QAbstractPhysicalDevice *Qt3DInput::Input::InputHandler::createPhysicalDevice(const QString &name)
{
    QAbstractPhysicalDevice *device = nullptr;
    for (Qt3DInput::QInputDeviceIntegration *integration : qAsConst(m_inputDeviceIntegrations)) {
        if ((device = integration->createPhysicalDevice(name)) != nullptr)
            break;
    }
    return device;
}

void Qt3DInput::Input::InputHandler::setEventFilterService(QEventFilterService *service)
{
    m_service = service;
}

AbstractActionInput *InputHandler::lookupActionInput(Qt3DCore::QNodeId id) const
{
    AbstractActionInput *input = nullptr;
    if ((input = actionInputManager()->lookupResource(id)) != nullptr)
        return input;
    if ((input = inputSequenceManager()->lookupResource(id)) != nullptr)
        return input;
    return inputChordManager()->lookupResource(id); // nullptr if not found
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
