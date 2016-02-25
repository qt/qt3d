/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "updateaxisactionjob_p.h"
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/qabstractphysicaldevicebackendnode.h>
#include <Qt3DInput/qinputdeviceintegration.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

namespace {

bool anyOfRequiredButtonsPressed(const QVector<int> &buttons, QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend)
{
    bool validButtonWasPressed = false;
    Q_FOREACH (int button, buttons) {
        if (physicalDeviceBackend->isButtonPressed(button)) {
            validButtonWasPressed = true;
            break;
        }
    }
    return validButtonWasPressed;
}

} // anonymous

UpdateAxisActionJob::UpdateAxisActionJob(qint64 currentTime, InputHandler *handler, HLogicalDevice handle)
    : Qt3DCore::QAspectJob()
    , m_currentTime(currentTime)
    , m_handler(handler)
    , m_handle(handle)
{
}

void UpdateAxisActionJob::run()
{
    // Note: we assume axis/action are not really shared:
    // there's no benefit in sharing those when it comes to computing
    LogicalDevice *device = m_handler->logicalDeviceManager()->data(m_handle);
    updateAction(device);
    updateAxis(device);
}

void UpdateAxisActionJob::updateAction(LogicalDevice *device)
{
    Q_FOREACH (const Qt3DCore::QNodeId actionId, device->actions()) {
        bool actionTriggered = false;
        Action *action = m_handler->actionManager()->lookupResource(actionId);

        Q_FOREACH (const Qt3DCore::QNodeId actionInputId, action->inputs()) {
            actionTriggered |= processActionInput(actionInputId);
        }
        action->setActionTriggered(actionTriggered);
    }
}

bool UpdateAxisActionJob::processActionInput(const Qt3DCore::QNodeId actionInputId)
{

    if (m_handler->actionInputManager()->lookupResource(actionInputId)) {
        ActionInput *actionInput = m_handler->actionInputManager()->lookupResource(actionInputId);
        QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = Q_NULLPTR;

        Q_FOREACH (QInputDeviceIntegration *integration, m_handler->inputDeviceIntegrations()) {
            if ((physicalDeviceBackend = integration->physicalDevice(actionInput->sourceDevice())) != Q_NULLPTR)
                break;
        }

        if (physicalDeviceBackend != Q_NULLPTR) {
            // Update the value
            return anyOfRequiredButtonsPressed(actionInput->buttons(), physicalDeviceBackend);
        }
    } else if (m_handler->inputSequenceManager()->lookupResource(actionInputId)) {
        InputSequence *inputSequence = m_handler->inputSequenceManager()->lookupResource(actionInputId);
        const qint64 startTime = inputSequence->startTime();
        if (startTime != 0) {
            // Check if we are still inside the time limit for the chord
            if ((m_currentTime - startTime) > inputSequence->timeout()) {
                inputSequence->reset();
                return false;
            }
        }
        bool actionTriggered = false;
        Q_FOREACH (const Qt3DCore::QNodeId actionInputId, inputSequence->inputs()) {
            if (processActionInput(actionInputId)){
                actionTriggered |= inputSequence->actionTriggered(actionInputId, m_currentTime);
                // Set the start time if it wasn't set before
                if (startTime == 0)
                    inputSequence->setStartTime(m_currentTime);
            }
        }
        return actionTriggered;
    } else if (m_handler->inputChordManager()->lookupResource(actionInputId)) {
        InputChord *inputChord = m_handler->inputChordManager()->lookupResource(actionInputId);
        const qint64 startTime = inputChord->startTime();
        if (startTime != 0) {
            // Check if we are still inside the time limit for the chord
            if ((m_currentTime - startTime) > inputChord->timeout()) {
                inputChord->reset();
                return false;
            }
        }
        bool actionTriggered = false;
        Q_FOREACH (const Qt3DCore::QNodeId actionInputId, inputChord->inputs()) {
            if (processActionInput(actionInputId)){
                actionTriggered |= inputChord->actionTriggered(actionInputId);
                if (startTime == 0)
                    inputChord->setStartTime(m_currentTime);
            }
        }
        return actionTriggered;
    }
    //Should Never reach this point
    return false;
}

void UpdateAxisActionJob::updateAxis(LogicalDevice *device)
{
    Q_FOREACH (const Qt3DCore::QNodeId axisId, device->axes()) {
        Axis *axis = m_handler->axisManager()->lookupResource(axisId);
        float axisValue = 0.0f;

        Q_FOREACH (const Qt3DCore::QNodeId axisInputId, axis->inputs()) {
            AxisInput *axisInput = m_handler->axisInputManager()->lookupResource(axisInputId);
            QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = Q_NULLPTR;

            Q_FOREACH (QInputDeviceIntegration *integration, m_handler->inputDeviceIntegrations()) {
                if ((physicalDeviceBackend = integration->physicalDevice(axisInput->sourceDevice())) != Q_NULLPTR)
                    break;
            }

            if (physicalDeviceBackend != Q_NULLPTR) {
                // Update the value
                const QVector<int> buttons = axisInput->buttons();
                // Axis was specified -> we take this as the base value
                if (axisInput->axis() != -1)
                    axisValue += physicalDeviceBackend->processedAxisValue(axisInput->axis());
                else if (!buttons.isEmpty()) {
                    // TO DO: Linear Curver for the progression of the scale value
                    if (anyOfRequiredButtonsPressed(buttons, physicalDeviceBackend))
                        axisValue += axisInput->scale();
                }
            }
        }

        // Clamp the axisValue -1/1
        axisValue = qMin(1.0f, qMax(axisValue, -1.0f));
        axis->setAxisValue(axisValue);
    }
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
