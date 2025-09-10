// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractphysicaldeviceproxy_p.h"
#include "qabstractphysicaldeviceproxy_p_p.h"


QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*!
    \internal
 */
QAbstractPhysicalDeviceProxyPrivate::QAbstractPhysicalDeviceProxyPrivate(const QString &deviceName)
    : QAbstractPhysicalDevicePrivate()
    , m_deviceName(deviceName)
    , m_status(QAbstractPhysicalDeviceProxy::NotFound)
    , m_device(nullptr)
{
}

/*!
    \internal
 */
QAbstractPhysicalDeviceProxyPrivate::~QAbstractPhysicalDeviceProxyPrivate()
{
}

/*!
    \internal
 */
void QAbstractPhysicalDeviceProxyPrivate::setStatus(QAbstractPhysicalDeviceProxy::DeviceStatus status)
{
    if (status != m_status) {
        m_status = status;
        emit q_func()->statusChanged(status);
    }
}

/*!
    \class Qt3DInput::QAbstractPhysicalDeviceProxy
    \inmodule Qt3DInput

    \brief Qt3DInput::QAbstractPhysicalDeviceProxy acts as a proxy
    for an actual Qt3DInput::QQAbstractPhysicalDevice device.

    Qt3DInput::QAbstractPhysicalDeviceProxy can be used to facilitate
    exposing a physical device to users. It alleviates the need to introspect
    the axis and buttons based on their names.

    It is typcally used through subclassing allowing to set the device name and
    defining enums for the various axis and buttons of your targeted device.

    At runtime, the status property will be updated to reflect whether an
    actual device matching the device name could be created.

    \since 5.8
 */

QString QAbstractPhysicalDeviceProxy::deviceName() const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    return d->m_deviceName;
}

QAbstractPhysicalDeviceProxy::DeviceStatus QAbstractPhysicalDeviceProxy::status() const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    return d->m_status;
}

int QAbstractPhysicalDeviceProxy::axisCount() const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    if (d->m_device != nullptr)
        return d->m_device->axisCount();
    return 0;
}

int QAbstractPhysicalDeviceProxy::buttonCount() const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    if (d->m_device != nullptr)
        return d->m_device->buttonCount();
    return 0;
}

QStringList QAbstractPhysicalDeviceProxy::axisNames() const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    if (d->m_device != nullptr)
        return d->m_device->axisNames();
    return QStringList();
}

QStringList QAbstractPhysicalDeviceProxy::buttonNames() const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    if (d->m_device != nullptr)
        return d->m_device->buttonNames();
    return QStringList();
}

int QAbstractPhysicalDeviceProxy::axisIdentifier(const QString &name) const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    if (d->m_device != nullptr)
        return d->m_device->axisIdentifier(name);
    return -1;
}

int QAbstractPhysicalDeviceProxy::buttonIdentifier(const QString &name) const
{
    Q_D(const QAbstractPhysicalDeviceProxy);
    if (d->m_device != nullptr)
        return d->m_device->buttonIdentifier(name);
    return -1;
}

/*!
    \internal
 */
QAbstractPhysicalDeviceProxy::QAbstractPhysicalDeviceProxy(QAbstractPhysicalDeviceProxyPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractPhysicalDevice(dd, parent)
{
}

/*!
    \internal
 */
void QAbstractPhysicalDeviceProxyPrivate::setDevice(QAbstractPhysicalDevice *device)
{
    Q_Q(QAbstractPhysicalDeviceProxy);

    // Note: technically book keeping could be optional since we are the parent
    // of the device. But who knows if someone plays with the object tree...

    // Unset bookkeeper
    if (m_device != nullptr) {
        // Note: we cannot delete the device here as we don't how if we are
        // called by the bookkeeper (in which case we would do a double free)
        // or by the sceneChangeEvent
        unregisterDestructionHelper(m_device);
        setStatus(QAbstractPhysicalDeviceProxy::NotFound);
    }

    // Set parent so that node is created in the backend
    if (device != nullptr && device->parent() == nullptr)
        device->setParent(q);

    m_device = device;

    // Set bookkeeper
    if (device != nullptr) {
       setStatus(QAbstractPhysicalDeviceProxy::Ready);
       registerPrivateDestructionHelper(m_device, &QAbstractPhysicalDeviceProxyPrivate::resetDevice);
    }
}

void QAbstractPhysicalDeviceProxyPrivate::resetDevice(QAbstractPhysicalDevice *device)
{
    if (m_device == device) {
        // Note: we cannot delete the device here as we don't how if we are
        // called by the bookkeeper (in which case we would do a double free)
        // or by the sceneChangeEvent
        unregisterDestructionHelper(m_device);
        setStatus(QAbstractPhysicalDeviceProxy::NotFound);

        m_device = nullptr;
    }
}

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_qabstractphysicaldeviceproxy_p.cpp"
