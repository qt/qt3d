// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QABSTRACTPHYSICALDEVICEPROXY_P_H
#define QT3DINPUT_QABSTRACTPHYSICALDEVICEPROXY_P_H

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

#include <Qt3DInput/qabstractphysicaldevice.h>

#ifndef Q_QDOC
#include <Qt3DInput/private/qt3dinput_global_p.h>
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDeviceProxyPrivate;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QAbstractPhysicalDeviceProxy : public QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName CONSTANT)
    Q_PROPERTY(QAbstractPhysicalDeviceProxy::DeviceStatus status READ status NOTIFY statusChanged)

public:
    enum DeviceStatus {
        Ready = 0,
        NotFound
    };
    Q_ENUM(DeviceStatus) // LCOV_EXCL_LINE

    QString deviceName() const;
    DeviceStatus status() const;

    int axisCount() const override;
    int buttonCount() const override;
    QStringList axisNames() const override;
    QStringList buttonNames() const override;
    int axisIdentifier(const QString &name) const override;
    int buttonIdentifier(const QString &name) const override;

Q_SIGNALS:
    void statusChanged(QAbstractPhysicalDeviceProxy::DeviceStatus status);

protected:
    QAbstractPhysicalDeviceProxy(QAbstractPhysicalDeviceProxyPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractPhysicalDeviceProxy)

};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QABSTRACTPHYSICALDEVICEPROXY_P_H
