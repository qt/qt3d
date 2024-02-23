// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#ifndef TESTDEVICEPROXY_H
#define TESTDEVICEPROXY_H

#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p.h>
#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p_p.h>

class TestPhysicalDevice : public Qt3DInput::QAbstractPhysicalDevice
{
    Q_OBJECT
public:
    explicit TestPhysicalDevice(Qt3DCore::QNode *parent = nullptr)
        : Qt3DInput::QAbstractPhysicalDevice(parent)
    {}

    ~TestPhysicalDevice()
    {
    }

    enum AxisValues {
        X = 0,
        Y,
        Z
    };

    enum ButtonValues {
        Left = 0,
        Right
    };

    int axisCount() const final { return 3; }
    int buttonCount() const final { return 2; }
    QStringList axisNames() const final { return QStringList() << QStringLiteral("x") << QStringLiteral("y") << QStringLiteral("z"); }
    QStringList buttonNames() const final { return QStringList() << QStringLiteral("Left") << QStringLiteral("Right"); }

    int axisIdentifier(const QString &name) const final
    {
        if (name == QLatin1String("x"))
            return TestPhysicalDevice::X;
        if (name == QLatin1String("y"))
            return TestPhysicalDevice::Y;
        if (name == QLatin1String("z"))
            return TestPhysicalDevice::Z;
        return -1;
    }
    int buttonIdentifier(const QString &name) const final
    {
        if (name == QLatin1String("Left"))
            return TestPhysicalDevice::Left;
        if (name == QLatin1String("Right"))
            return TestPhysicalDevice::Right;
        return -1;
    }
};

class TestProxyPrivate : public Qt3DInput::QAbstractPhysicalDeviceProxyPrivate
{
public:
    explicit TestProxyPrivate(const QString &name)
        : Qt3DInput::QAbstractPhysicalDeviceProxyPrivate(name)
    {}
};

class TestProxy : public Qt3DInput::QAbstractPhysicalDeviceProxy
{
    Q_OBJECT
public:
    explicit TestProxy(const QString &name = QStringLiteral("TestProxy"))
        : Qt3DInput::QAbstractPhysicalDeviceProxy(*new TestProxyPrivate(name))
    {}

    Qt3DInput::QAbstractPhysicalDevice *device() const
    {
        Q_D(const TestProxy);
        return d->m_device;
    }

    void setDevice(TestPhysicalDevice *device)
    {
        Q_D(TestProxy);
        d->setDevice(device);
    }

private:
    Q_DECLARE_PRIVATE(TestProxy)
};

#endif // TESTDEVICEPROXY_H
