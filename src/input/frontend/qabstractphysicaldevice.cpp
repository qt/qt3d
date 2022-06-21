// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractphysicaldevice.h"
#include "qabstractphysicaldevice_p.h"

#include <Qt3DInput/qaxissetting.h>

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

/*! \internal */
QAbstractPhysicalDevicePrivate::QAbstractPhysicalDevicePrivate()
    : m_axisSettings()
{
}

/*! \internal */
QAbstractPhysicalDevicePrivate::~QAbstractPhysicalDevicePrivate()
{
}

/*!
    \class Qt3DInput::QAbstractPhysicalDevice
    \inmodule Qt3DInput
    \inherits Qt3DCore::QNode
    \brief QAbstractPhysicalDevice is the base class used by Qt3d to interact with arbitrary input devices.

    \since 5.6
*/

/*!
    \qmltype AbstractPhysicalDevice
    \inqmlmodule Qt3D.Input
    \instantiates Qt3DInput::QAbstractPhysicalDevice
    \brief QML frontend for the abstract Qt3DInput::QAbstractPhysicalDevice C++ class.

    The base class used by Qt3d to interact with arbitrary input devices.

    \since 5.6
*/

/*!
    Constructs a new QAbstractPhysicalDevice instance with \a parent.
 */
QAbstractPhysicalDevice::QAbstractPhysicalDevice(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QAbstractPhysicalDevicePrivate, parent)
{
}

/*! \internal */
QAbstractPhysicalDevice::~QAbstractPhysicalDevice()
{
}

QAbstractPhysicalDevice::QAbstractPhysicalDevice(QAbstractPhysicalDevicePrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

/*!
    \return the number of axis this device has.
 */
int QAbstractPhysicalDevice::axisCount() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_axesHash.size();
}

/*!
    \return the number of buttons this device has.
 */
int QAbstractPhysicalDevice::buttonCount() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_buttonsHash.size();
}

/*!
    \return a list of the names of device's axis.
 */
QStringList QAbstractPhysicalDevice::axisNames() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_axesHash.keys();
}

/*!
    \return a list of the names of device's buttons.
 */
QStringList QAbstractPhysicalDevice::buttonNames() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_buttonsHash.keys();
}

/*!
    \return the integer identifer of the axis \a name or -1 if it does not exist on this device.
 */
int QAbstractPhysicalDevice::axisIdentifier(const QString &name) const
{
    Q_D(const QAbstractPhysicalDevice);
    auto it = d->m_axesHash.find(name);
    if (it != d->m_axesHash.end())
        return *it;
    return -1;
}

/*!
    \return the integer identifer of the button \a name or -1 if it does not exist on this device.
 */
int QAbstractPhysicalDevice::buttonIdentifier(const QString &name) const
{
    Q_D(const QAbstractPhysicalDevice);
    auto it = d->m_buttonsHash.find(name);
    if (it != d->m_buttonsHash.end())
        return *it;
    return -1;
}

/*!
    Add the axisSetting \a axisSetting to this device.
 */
void QAbstractPhysicalDevice::addAxisSetting(QAxisSetting *axisSetting)
{
    Q_D(QAbstractPhysicalDevice);
    if (axisSetting && !d->m_axisSettings.contains(axisSetting)) {
        d->update();
        d->m_axisSettings.push_back(axisSetting);
    }
}

/*!
    Remove the axisSetting \a axisSetting to this device.
 */
void QAbstractPhysicalDevice::removeAxisSetting(QAxisSetting *axisSetting)
{
    Q_D(QAbstractPhysicalDevice);
    if (axisSetting && d->m_axisSettings.contains(axisSetting)) {
        d->update();
        d->m_axisSettings.removeOne(axisSetting);
    }
}

/*!
    Returns the axisSettings associated with this device.
 */
QList<QAxisSetting *> QAbstractPhysicalDevice::axisSettings() const
{
    Q_D(const QAbstractPhysicalDevice);
    return d->m_axisSettings;
}

/*
    Used to notify observers that an axis value has been changed.
 */
void QAbstractPhysicalDevicePrivate::postAxisEvent(int axis, qreal value)
{
    m_pendingAxisEvents.push_back({axis, value});
    update();
}

/*
    Used to notify observers that an button value has been changed.
 */
void QAbstractPhysicalDevicePrivate::postButtonEvent(int button, qreal value)
{
    m_pendingButtonsEvents.push_back({button, value});
    update();
}

}

QT_END_NAMESPACE

#include "moc_qabstractphysicaldevice.cpp"
