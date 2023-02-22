// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "abstractdeviceorientation.h"

#include <QDateTime>
#include <QDebug>
#include <QtMath>
#include <QScreen>


AbstractDeviceOrientation::AbstractDeviceOrientation(QObject *parent)
    : QObject(parent)
    , m_roll(0)
    , m_pitch(0)
    , m_yaw(0)
    , m_enabled(false)
{
}

bool AbstractDeviceOrientation::enabled() const
{
    return m_enabled;
}

void AbstractDeviceOrientation::stop()
{
    if (m_enabled) {
        m_enabled = false;
        emit enabledChanged(m_enabled);
    }
}

void AbstractDeviceOrientation::setRoll(qreal v)
{
    if (v != m_roll) {
        m_roll = v;
        emit rollChanged(v);
    }
}

void AbstractDeviceOrientation::setPitch(qreal v)
{
    if (v != m_pitch) {
        m_pitch = v;
        emit pitchChanged(v);
    }
}

void AbstractDeviceOrientation::setYaw(qreal v)
{
    if (v != m_yaw) {
        m_yaw = v;
        emit yawChanged(v);
    }
}

void AbstractDeviceOrientation::setEnabled(bool v)
{
    if (v && !m_enabled) {
        start();
    } else if (!v && m_enabled) {
        stop();
    }
}
