// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "iosdeviceorientation.h"
#include "iosdeviceorientation_p.h"

iOSDeviceOrientation::iOSDeviceOrientation(QObject *parent)
    : AbstractDeviceOrientation(parent)
{
    [[iOSDeviceOrientationP instance] setHandler:this];
}

bool iOSDeviceOrientation::start()
{
    if (!m_enabled) {
        [[iOSDeviceOrientationP instance] start];
        m_enabled = true;
        emit enabledChanged(m_enabled);
    }
    return true;
}

void iOSDeviceOrientation::stop()
{
    if (m_enabled) {
        [[iOSDeviceOrientationP instance] stop];
        m_enabled = false;
        emit enabledChanged(m_enabled);
    }
}
