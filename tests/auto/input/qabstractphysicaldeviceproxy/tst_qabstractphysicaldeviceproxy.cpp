// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>

#include "testdeviceproxy.h"

class tst_QAbstractPhysicalDeviceProxy : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        TestProxy abstractPhysicalDeviceProxy;

        // THEN
        QCOMPARE(abstractPhysicalDeviceProxy.deviceName(), QLatin1String("TestProxy"));
        QCOMPARE(abstractPhysicalDeviceProxy.status(), Qt3DInput::QAbstractPhysicalDeviceProxy::NotFound);
        QCOMPARE(abstractPhysicalDeviceProxy.axisCount(), 0);
        QCOMPARE(abstractPhysicalDeviceProxy.buttonCount(), 0);
        QCOMPARE(abstractPhysicalDeviceProxy.axisNames(), QStringList());
        QCOMPARE(abstractPhysicalDeviceProxy.buttonNames(), QStringList());
        QVERIFY(abstractPhysicalDeviceProxy.device() == nullptr);
    }

    void checkDeviceBookkeeping()
    {
        // GIVEN
        TestProxy *abstractPhysicalDeviceProxy = new TestProxy();

        // WHEN
        TestPhysicalDevice *device = new TestPhysicalDevice();
        abstractPhysicalDeviceProxy->setDevice(device);

        // THEN
        QVERIFY(abstractPhysicalDeviceProxy->device() == device);

        // WHEN
        delete device;

        // THEN -> should not crash
        QVERIFY(abstractPhysicalDeviceProxy->device() == nullptr);
    }
};

QTEST_MAIN(tst_QAbstractPhysicalDeviceProxy)

#include "tst_qabstractphysicaldeviceproxy.moc"
