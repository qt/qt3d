// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "dummydeviceorientation.h"

DummyDeviceOrientation::DummyDeviceOrientation(QObject *parent)
    : AbstractDeviceOrientation(parent)
{
}

bool DummyDeviceOrientation::start()
{
    if (!m_enabled) {
        m_enabled = true;
        emit enabledChanged(m_enabled);
    }
    return true;
}

void DummyDeviceOrientation::stop()
{
    if (m_enabled) {
        m_enabled = false;
        emit enabledChanged(m_enabled);
    }
}
