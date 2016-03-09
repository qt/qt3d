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
#include "inputmanagers_p.h"
#include "keyboardeventfilter_p.h"
#include "mouseeventfilter_p.h"
#include "assignkeyboardfocusjob_p.h"
#include "keyeventdispatcherjob_p.h"
#include "mouseeventdispatcherjob_p.h"
#include <Qt3DCore/private/qeventfilterservice_p.h>
#include "inputsettings_p.h"
#include "eventsourcesetterhelper_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
namespace Input {

InputHandler::InputHandler()
    : m_keyboardControllerManager(new KeyboardControllerManager())
    , m_keyboardInputManager(new KeyboardInputManager())
    , m_mouseDeviceManager(new MouseDeviceManager())
    , m_mouseInputManager(new MouseInputManager())
    , m_keyboardEventFilter(new KeyboardEventFilter())
    , m_mouseEventFilter(new MouseEventFilter())
    , m_axisManager(new AxisManager())
    , m_actionManager(new ActionManager())
    , m_axisInputManager(new AxisInputManager())
    , m_axisSettingManager(new AxisSettingManager())
    , m_actionInputManager(new ActionInputManager())
    , m_inputChordManager(new InputChordManager())
    , m_inputSequenceManager(new InputSequenceManager())
    , m_logicalDeviceManager(new LogicalDeviceManager())
    , m_genericPhysicalDeviceBackendNodeManager(new GenericDeviceBackendNodeManager)
    , m_settings(Q_NULLPTR)
    , m_eventSourceSetter(new Qt3DInput::Input::EventSourceSetterHelper(this))
{
    m_keyboardEventFilter->setInputHandler(this);
    m_mouseEventFilter->setInputHandler(this);

    // Created in the main thread
    // m_eventSourceSetter needs to be in the main thread
}

InputHandler::~InputHandler()
{
}

// Called in MainThread (by the EventSourceHelperSetter)
void InputHandler::registerEventFilters(QEventFilterService *service)
{
    clearPendingKeyEvents();
    clearPendingMouseEvents();

    service->registerEventFilter(m_keyboardEventFilter, 512);
    service->registerEventFilter(m_mouseEventFilter, 513);
}

// Called by the keyboardEventFilter in the main thread
void InputHandler::appendKeyEvent(const QT_PREPEND_NAMESPACE(QKeyEvent) &event)
{
    QMutexLocker lock(&m_mutex);
    m_pendingEvents.append(event);
}

// Called by QInputASpect::jobsToExecute (aspectThread)
QList<QT_PREPEND_NAMESPACE(QKeyEvent)> InputHandler::pendingKeyEvents()
{
    QMutexLocker lock(&m_mutex);
    QList<QT_PREPEND_NAMESPACE(QKeyEvent)> pendingEvents = m_pendingEvents;
    m_pendingEvents.clear();
    return pendingEvents;
}

// Called by QInputASpect::jobsToExecute (aspectThread)
void InputHandler::clearPendingKeyEvents()
{
    QMutexLocker lock(&m_mutex);
    m_pendingEvents.clear();
}

void InputHandler::appendMouseEvent(const QT_PREPEND_NAMESPACE(QMouseEvent) &event)
{
    QMutexLocker lock(&m_mutex);
    m_pendingMouseEvents.append(event);
}

QList<QT_PREPEND_NAMESPACE(QMouseEvent)> InputHandler::pendingMouseEvents()
{
    QMutexLocker lock(&m_mutex);
    QList<QT_PREPEND_NAMESPACE(QMouseEvent)> pendingEvents = m_pendingMouseEvents;
    m_pendingMouseEvents.clear();
    return pendingEvents;
}

void InputHandler::clearPendingMouseEvents()
{
    QMutexLocker lock(&m_mutex);
    m_pendingMouseEvents.clear();
}

void InputHandler::appendKeyboardController(HKeyboardController controller)
{
    m_activeKeyboardControllers.append(controller);
}

void InputHandler::removeKeyboardController(HKeyboardController controller)
{
    m_activeKeyboardControllers.removeAll(controller);
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
    // One job for Keyboard focus change event per Keyboard Controller
    QVector<QAspectJobPtr> jobs;
    const QList<QT_PREPEND_NAMESPACE(QKeyEvent)> events = pendingKeyEvents();

    Q_FOREACH (const HKeyboardController cHandle, m_activeKeyboardControllers) {
        KeyboardController *controller = m_keyboardControllerManager->data(cHandle);
        if (controller) {
            controller->updateKeyEvents(events);
            QAspectJobPtr focusChangeJob;
            if (controller->lastKeyboardInputRequester() != controller->currentFocusItem()) {
                AssignKeyboardFocusJob *job = new AssignKeyboardFocusJob(controller->peerId());
                job->setInputHandler(this);
                focusChangeJob.reset(job);
                jobs.append(focusChangeJob);
                // One job for Keyboard events (depends on the focus change job if there was one)
            }
            // Event dispacthing job
            if (!events.isEmpty()) {
                KeyEventDispatcherJob *job = new KeyEventDispatcherJob(controller->currentFocusItem(), events);
                job->setInputHandler(this);
                if (focusChangeJob)
                    job->addDependency(focusChangeJob);
                jobs.append(QAspectJobPtr(job));
            }
        }
    }
    return jobs;
}

QVector<Qt3DCore::QAspectJobPtr> InputHandler::mouseJobs()
{
    QVector<QAspectJobPtr> jobs;
    const QList<QT_PREPEND_NAMESPACE(QMouseEvent)> events = pendingMouseEvents();

    Q_FOREACH (const HMouseDevice cHandle, m_activeMouseDevices) {
        MouseDevice *controller = m_mouseDeviceManager->data(cHandle);

        controller->updateMouseEvents(events);
        // Event dispacthing job
        if (!events.isEmpty()) {
            Q_FOREACH (QNodeId input, controller->mouseInputs()) {
                MouseEventDispatcherJob *job = new MouseEventDispatcherJob(input, events);
                job->setInputHandler(this);
                jobs.append(QAspectJobPtr(job));
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

void InputHandler::setInputSettings(InputSettings *settings)
{
    m_settings = settings;
}

void InputHandler::setEventSourceHelper(EventSourceSetterHelper *helper)
{
    m_eventSourceSetter.reset(helper);
}

EventSourceSetterHelper *InputHandler::eventSourceHelper() const
{
    return m_eventSourceSetter.data();
}

void InputHandler::updateEventSource()
{
    if (m_settings != Q_NULLPTR) {
        // Will be updated only if eventSource is different than
        // what was set last
        QObject *eventSource = m_settings->eventSource();
        m_eventSourceSetter->setEventSource(eventSource);
    }
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
