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

#include "updateaxisactionjob_p.h"
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/qabstractphysicaldevicebackendnode.h>
#include <Qt3DInput/qinputdeviceintegration.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

namespace {

bool anyOfRequiredKeysPressed(const QVector<int> &keys, QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend)
{
    bool validKeyWasPressed = false;
    Q_FOREACH (int key, keys) {
        if (physicalDeviceBackend->isButtonPressed(key)) {
            validKeyWasPressed = true;
            break;
        }
    }
    return validKeyWasPressed;
}

} // anonymous

UpdateAxisActionJob::UpdateAxisActionJob(InputHandler *handler, HLogicalDevice handle)
    : Qt3DCore::QAspectJob()
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
            ActionInput *actionInput = m_handler->actionInputManager()->lookupResource(actionInputId);
            QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = Q_NULLPTR;

            Q_FOREACH (QInputDeviceIntegration *integration, m_handler->inputDeviceIntegrations()) {
                if ((physicalDeviceBackend = integration->physicalDevice(actionInput->sourceDevice())) != Q_NULLPTR)
                    break;
            }

            if (physicalDeviceBackend != Q_NULLPTR) {
                // Update the value
                actionTriggered |= anyOfRequiredKeysPressed(actionInput->keys(), physicalDeviceBackend);
            }
        }
        action->setActionTriggered(actionTriggered);
    }
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
                const QVector<int> keys = axisInput->keys();
                // Axis was specified -> we take this as the base value
                if (axisInput->axis() != -1)
                    axisValue += physicalDeviceBackend->processedAxisValue(axisInput->axis());
                else if (!keys.isEmpty()) {
                    // TO DO: Linear Curver for the progression of the scale value
                    if (anyOfRequiredKeysPressed(keys, physicalDeviceBackend))
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
