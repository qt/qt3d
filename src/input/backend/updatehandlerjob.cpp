/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "updatehandlerjob_p.h"
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/axisactionpayload_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput
{

namespace Input
{

namespace {

int containsAxis(const Qt3DCore::QNodeId axisId, const AxisPayload &payload)
{
    for (int i = 0, m = payload.axes.size(); i < m; ++i)
        if (payload.axes.at(i).id == axisId)
            return i;
    return -1;
}

int containsAction(const Qt3DCore::QNodeId actionId, const ActionPayload &payload)
{
    for (int i = 0, m = payload.actions.size(); i < m; ++i)
        if (payload.actions.at(i).id == actionId)
            return i;
    return -1;
}

} // anonymous

UpdateHandlerJob::UpdateHandlerJob(AxisActionHandler *axisActionHandler, HLogicalDevice logicalDeviceHandler, InputHandler *handler)
    : m_axisActionHandler(axisActionHandler)
    , m_logicalDeviceHandle(logicalDeviceHandler)
    , m_handler(handler)
{
}

void UpdateHandlerJob::run()
{
    // Find the LogicalDevice
    //   Find each Axis
    //      Check if their values have changed since the last time
    //          If so -> add to notification payload

    //   Find each Action
    //      Check if action state has changed since last frame
    //          If so -> add to notification payload

    LogicalDevice *logicalDevice = m_handler->logicalDeviceManager()->data(m_logicalDeviceHandle);
    updateActions(logicalDevice);
    updateAxes(logicalDevice);
}

void UpdateHandlerJob::updateAxes(LogicalDevice *device)
{
    AxisPayload payload = m_axisActionHandler->lastAxisPayload();

    Q_FOREACH (const Qt3DCore::QNodeId axisId, device->axes()) {
        const Axis *axis = m_handler->axisManager()->lookupResource(axisId);
        const int axisPositionInPayload = containsAxis(axisId, payload);

        if (axisPositionInPayload < 0) {
            // Not contained in the payload
            AxisUpdate axisUpdate;
            axisUpdate.name = axis->name();
            axisUpdate.id = axisId;
            axisUpdate.value = axis->axisValue();
            payload.axes.push_back(axisUpdate);
        } else {
            // The axis has the same value as before -> remove from the payload
            if (payload.axes.at(axisPositionInPayload).value != axis->axisValue())
                payload.axes[axisPositionInPayload].value = axis->axisValue();
        }
    }

    m_axisActionHandler->setAndTransmitAxisPayload(payload);
}

void UpdateHandlerJob::updateActions(LogicalDevice *device)
{
    ActionStates actionStates;

    // Push each action into the payload to build the current action state for the frame
    Q_FOREACH (const Qt3DCore::QNodeId actionId, device->actions()) {
        const Action *action = m_handler->actionManager()->lookupResource(actionId);
        ActionUpdate actionState;
        actionState.id = actionId;
        actionState.name = action->name();
        actionState.triggered = action->actionTriggered();
        actionStates.actions.push_back(actionState);
    }

    // Compare the action state against the previous payload state
    ActionStates oldActionStates = m_axisActionHandler->lastActionStates();
    ActionPayload updatePayload;

    // Build up a delta payload
    Q_FOREACH (const ActionUpdate &actionUpdate, actionStates.actions) {
        const int posOfActionInOldPayload = containsAction(actionUpdate.id, oldActionStates);
        // If the action is not in the old payload or
        // is in the old payload state but has a different value
        // we add it to the delta payload
        if (posOfActionInOldPayload < 0 ||
                oldActionStates.actions.at(posOfActionInOldPayload).triggered != actionUpdate.triggered)
            updatePayload.actions.push_back(actionUpdate);
    }

    m_axisActionHandler->setAndTransmitActionPayload(actionStates, updatePayload);
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE

