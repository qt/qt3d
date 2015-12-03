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

#include "inputhandler_p.h"
#include "inputmanagers_p.h"
#include "keyboardeventfilter_p.h"
#include "mouseeventfilter_p.h"
#include "assignkeyboardfocusjob_p.h"
#include "keyeventdispatcherjob_p.h"
#include "mouseeventdispatcherjob_p.h"
#include <Qt3DCore/private/qeventfilterservice_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
namespace Input {

InputHandler::InputHandler()
    : m_keyboardControllerManager(new KeyboardControllerManager())
    , m_keyboardInputManager(new KeyboardInputManager())
    , m_mouseControllerManager(new MouseControllerManager())
    , m_mouseInputManager(new MouseInputManager())
    , m_keyboardEventFilter(new KeyboardEventFilter())
    , m_mouseEventFilter(new MouseEventFilter())
    , m_axisManager(new AxisManager())
    , m_actionManager(new ActionManager())
    , m_axisInputManager(new AxisInputManager())
    , m_axisActionHandlerManager(new AxisActionHandlerManager())
    , m_axisSettingManager(new AxisSettingManager())
    , m_actionInputManager(new ActionInputManager())
    , m_logicalDeviceManager(new LogicalDeviceManager())
{
    m_keyboardEventFilter->setInputHandler(this);
    m_mouseEventFilter->setInputHandler(this);
}

// Called in MainThread
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

void InputHandler::appendMouseController(HMouseController controller)
{
    m_activeMouseControllers.append(controller);
}

void InputHandler::removeMouseController(HMouseController controller)
{
    m_activeMouseControllers.removeAll(controller);
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
        controller->updateKeyEvents(events);
        if (controller) {
            QAspectJobPtr focusChangeJob;
            if (controller->lastKeyboardInputRequester() != controller->currentFocusItem()) {
                AssignKeyboardFocusJob *job = new AssignKeyboardFocusJob(controller->peerUuid());
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

    Q_FOREACH (const HMouseController cHandle, m_activeMouseControllers) {
        MouseController *controller = m_mouseControllerManager->data(cHandle);

        controller->updateMouseEvents(events);
        // Event dispacthing job
        if (!events.isEmpty()) {
            Q_FOREACH (const QNodeId &input, controller->mouseInputs()) {
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

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
