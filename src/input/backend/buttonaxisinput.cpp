// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "buttonaxisinput_p.h"

#include <Qt3DInput/qbuttonaxisinput.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>
#include <Qt3DInput/private/qbuttonaxisinput_p.h>
#include <Qt3DInput/private/utils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

ButtonAxisInput::ButtonAxisInput()
    : AbstractAxisInput()
    , m_scale(0.0f)
    , m_acceleration(-1.0f)
    , m_deceleration(-1.0f)
    , m_speedRatio(0.0f)
    , m_lastUpdateTime(0)
{
}

void ButtonAxisInput::cleanup()
{
    m_scale = 0.0f;
    m_buttons.clear();
    m_acceleration = -1.0f;
    m_deceleration = -1.0f;
    AbstractAxisInput::cleanup();
}

void ButtonAxisInput::updateSpeedRatio(qint64 currentTime, ButtonAxisInput::UpdateType type)
{
    const float accel = (type == Accelerate) ? acceleration() : -deceleration();

    // Was in nanoseconds, while acceleration will be in units per square seconds
    const float delta = m_lastUpdateTime ? (currentTime - m_lastUpdateTime) / 1000000000.0f : 0.0f;
    const float speedRatio = m_speedRatio + delta * accel;

    // Clamp it
    m_speedRatio = qMax(0.0f, qMin(speedRatio, 1.0f));

    // If we stopped, time to start over
    if (type == Decelerate && m_speedRatio == 0.0f)
        m_lastUpdateTime = 0;
    else
        m_lastUpdateTime = currentTime;
}

void ButtonAxisInput::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    AbstractAxisInput::syncFromFrontEnd(frontEnd, firstTime);
    const QButtonAxisInput *node = qobject_cast<const QButtonAxisInput *>(frontEnd);
    if (!node)
        return;

    m_scale = node->scale();
    m_buttons = node->buttons();
    m_acceleration = node->acceleration();
    m_deceleration = node->deceleration();
}

namespace {

bool anyOfRequiredButtonsPressed(const QList<int> &buttons, QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend)
{
    bool validButtonWasPressed = false;
    for (int button : buttons) {
        if (physicalDeviceBackend->isButtonPressed(button)) {
            validButtonWasPressed = true;
            break;
        }
    }
    return validButtonWasPressed;
}

} // anonymous

float ButtonAxisInput::process(InputHandler *inputHandler, qint64 currentTime)
{
    if (!isEnabled())
        return 0.0f;

    if (m_buttons.isEmpty())
        return 0.0f;

    QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = Utils::physicalDeviceForInput(this, inputHandler);
    if (!physicalDeviceBackend)
        return 0.0f;

    // TO DO: Linear Curver for the progression of the scale value
    if (anyOfRequiredButtonsPressed(m_buttons, physicalDeviceBackend))
        updateSpeedRatio(currentTime, ButtonAxisInput::Accelerate);
    else if (m_speedRatio != 0.0f)
        updateSpeedRatio(currentTime, ButtonAxisInput::Decelerate);

    return m_speedRatio * m_scale;
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
