// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "touchsettings.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define DEVICE QInputDevice
#include <QtGui/QInputDevice>
#else
#define DEVICE QTouchDevice
#include <QtGui/QTouchDevice>
#endif

#include <QDebug>

TouchSettings::TouchSettings(QObject *parent)
    : QObject(parent)
{
}

bool TouchSettings::isHoverEnabled() const
{
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID) || defined(Q_OS_QNX) || defined(Q_OS_WINRT)
    return false;
#else
    const auto devices = DEVICE::devices();
    bool isTouch = false;
    for (const DEVICE *dev : devices)
        if (dev->type() == DEVICE::DeviceType::TouchScreen) {
            isTouch = true;
            break;
        }
    bool isMobile = false;
    if (qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_MOBILE")) {
        isMobile = true;
    }
    return !isTouch && !isMobile;
#endif
}
