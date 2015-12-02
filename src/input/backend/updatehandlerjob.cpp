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

int containsAxis(const QString &axisName, const AxisActionPayload &payload)
{
    for (int i = 0, m = payload.axes.size(); i < m; ++i)
        if (payload.axes.at(i).name == axisName)
            return i;
    return -1;
}

int containsAction(const QString &actionName, const AxisActionPayload &payload)
{
    for (int i = 0, m = payload.actions.size(); i < m; ++i)
        if (payload.actions.at(i).name == actionName)
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

    AxisActionPayload payload; // = m_axisActionHandler->lastPayload();
    LogicalDevice *logicalDevice = m_handler->logicalDeviceManager()->data(m_logicalDeviceHandle);

    Q_FOREACH (const Qt3DCore::QNodeId axesId, logicalDevice->axes()) {
        const Axis *axis = m_handler->axisManager()->lookupResource(axesId);
        const int axisPositionInPayload = containsAxis(axis->name(), payload);

        if (axisPositionInPayload < 0) {
            // Not contained in the payload
            AxisUpdate axisUpdate;
            axisUpdate.name = axis->name();
            axisUpdate.value = axis->axisValue();
            payload.axes.push_back(axisUpdate);
        } else {
            // The axis has the same value as before -> remove from the payload
            if (payload.axes.at(axisPositionInPayload).value == axis->axisValue())
                payload.axes.removeAt(axisPositionInPayload);
            else
                payload.axes[axisPositionInPayload].value = axis->axisValue();
        }
    }

    Q_FOREACH (const Qt3DCore::QNodeId actionId, logicalDevice->actions()) {
        const Action *action = m_handler->actionManager()->lookupResource(actionId);
        const int actionPositionInPayload = containsAction(action->name(), payload);

        if (actionPositionInPayload < 0) {
            // Not contained in the payload
            ActionUpdate actionUpdate;
            actionUpdate.name = action->name();
            actionUpdate.triggered = action->actionTriggered();
            payload.actions.push_back(actionUpdate);
        } else {
            // The stored action has the same triggered flas as the current action -> no change
            if (payload.actions.at(actionPositionInPayload).triggered == action->actionTriggered())
                payload.actions.removeAt(actionPositionInPayload);
            else
                payload.actions[actionPositionInPayload].triggered = action->actionTriggered();
        }
    }

    m_axisActionHandler->setAndTransmitPayload(payload);
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE

