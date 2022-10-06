// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "actioninput_p.h"

#include <Qt3DInput/qactioninput.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/private/qactioninput_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/utils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

ActionInput::ActionInput()
    : AbstractActionInput()
{
}

void ActionInput::cleanup()
{
    setEnabled(false);
    m_sourceDevice = Qt3DCore::QNodeId();
    m_buttons.clear();
}

void ActionInput::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    AbstractActionInput::syncFromFrontEnd(frontEnd, firstTime);
    const QActionInput *node = qobject_cast<const QActionInput *>(frontEnd);
    if (!node)
        return;

    m_sourceDevice = Qt3DCore::qIdForNode(node->sourceDevice());
    m_buttons = node->buttons();
}

bool ActionInput::process(InputHandler *inputHandler, qint64 currentTime)
{
    Q_UNUSED(currentTime);

    if (!isEnabled())
        return false;

    QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = Utils::physicalDeviceForInput(this, inputHandler);
    if (!physicalDeviceBackend)
        return false;

    for (int button : std::as_const(m_buttons)) {
        if (physicalDeviceBackend->isButtonPressed(button))
            return true;
    }

    return false;
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

