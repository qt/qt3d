// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QABSTRACTPHYSICALDEVICEPROXY_P_P_H
#define QT3DINPUT_QABSTRACTPHYSICALDEVICEPROXY_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DInput/private/qabstractphysicaldevice_p.h>
#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p.h>
#include <Qt3DInput/private/qt3dinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDeviceProxy;
class QAbstractPhysicalDevice;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QAbstractPhysicalDeviceProxyPrivate : public QAbstractPhysicalDevicePrivate
{
public:
    explicit QAbstractPhysicalDeviceProxyPrivate(const QString &deviceName);
    ~QAbstractPhysicalDeviceProxyPrivate();

    Q_DECLARE_PUBLIC(QAbstractPhysicalDeviceProxy)
    QString m_deviceName;
    QAbstractPhysicalDeviceProxy::DeviceStatus m_status;
    QAbstractPhysicalDevice *m_device;

    void setDevice(QAbstractPhysicalDevice *device);
    void resetDevice(QAbstractPhysicalDevice *device);
    void setStatus(QAbstractPhysicalDeviceProxy::DeviceStatus status);
};

struct QAbstractPhysicalDeviceProxyData
{
    QString deviceName;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QABSTRACTPHYSICALDEVICEPROXY_P_P_H
