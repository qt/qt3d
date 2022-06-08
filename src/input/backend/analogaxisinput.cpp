// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "analogaxisinput_p.h"

#include <Qt3DInput/qanalogaxisinput.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>
#include <Qt3DInput/private/qanalogaxisinput_p.h>
#include <Qt3DInput/private/utils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

AnalogAxisInput::AnalogAxisInput()
    : AbstractAxisInput()
    , m_axis(0)
{
}

void AnalogAxisInput::cleanup()
{
    m_axis = 0;
    AbstractAxisInput::cleanup();
}

void AnalogAxisInput::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    AbstractAxisInput::syncFromFrontEnd(frontEnd, firstTime);
    const QAnalogAxisInput *node = qobject_cast<const QAnalogAxisInput *>(frontEnd);
    if (!node)
        return;

    m_axis = node->axis();
}

float AnalogAxisInput::process(InputHandler *inputHandler, qint64 currentTime)
{
    Q_UNUSED(currentTime);

    if (!isEnabled())
        return 0.0f;

    if (m_axis == -1)
        return 0.0f;

    QAbstractPhysicalDeviceBackendNode *physicalDeviceBackend = Utils::physicalDeviceForInput(this, inputHandler);
    if (!physicalDeviceBackend)
        return 0.0f;

    return physicalDeviceBackend->processedAxisValue(m_axis);
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
