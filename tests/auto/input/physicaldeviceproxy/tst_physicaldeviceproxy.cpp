// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p.h>
#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p_p.h>
#include <Qt3DInput/private/physicaldeviceproxy_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "qbackendnodetester.h"
#include "testdeviceproxy.h"
#include "testarbiter.h"


class tst_PhysicalDeviceProxy : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DInput::Input::PhysicalDeviceProxy backendPhysicalDeviceProxy;

        // THEN
        QCOMPARE(backendPhysicalDeviceProxy.isEnabled(), false);
        QVERIFY(backendPhysicalDeviceProxy.peerId().isNull());
        QCOMPARE(backendPhysicalDeviceProxy.deviceName(), QString());
        QVERIFY(backendPhysicalDeviceProxy.manager() == nullptr);
        QVERIFY(backendPhysicalDeviceProxy.physicalDeviceId().isNull());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestProxy PhysicalDeviceProxy;
        Qt3DInput::Input::PhysicalDeviceProxyManager manager;

        {
            // WHEN
            Qt3DInput::Input::PhysicalDeviceProxy backendPhysicalDeviceProxy;
            backendPhysicalDeviceProxy.setManager(&manager);
            simulateInitializationSync(&PhysicalDeviceProxy, &backendPhysicalDeviceProxy);

            // THEN
            QCOMPARE(backendPhysicalDeviceProxy.isEnabled(), true);
            QCOMPARE(backendPhysicalDeviceProxy.peerId(), PhysicalDeviceProxy.id());
            QCOMPARE(backendPhysicalDeviceProxy.deviceName(), QStringLiteral("TestProxy"));
            QVERIFY(backendPhysicalDeviceProxy.manager() == &manager);
            QVERIFY(backendPhysicalDeviceProxy.physicalDeviceId().isNull());
        }
        {
            // WHEN
            Qt3DInput::Input::PhysicalDeviceProxy backendPhysicalDeviceProxy;
            backendPhysicalDeviceProxy.setManager(&manager);
            PhysicalDeviceProxy.setEnabled(false);
            simulateInitializationSync(&PhysicalDeviceProxy, &backendPhysicalDeviceProxy);

            // THEN
            QCOMPARE(backendPhysicalDeviceProxy.peerId(), PhysicalDeviceProxy.id());
            QCOMPARE(backendPhysicalDeviceProxy.isEnabled(), false);
        }
    }

    void checkLoadingRequested()
    {
        // GIVEN
        Qt3DInput::Input::PhysicalDeviceProxyManager manager;
        Qt3DInput::Input::PhysicalDeviceProxy backendPhysicalDeviceProxy;
        TestProxy deviceProxy;

        // WHEN
        backendPhysicalDeviceProxy.setManager(&manager);
        simulateInitializationSync(&deviceProxy, &backendPhysicalDeviceProxy);

        // THEN
        QCOMPARE(backendPhysicalDeviceProxy.deviceName(), QStringLiteral("TestProxy"));
        const QList<Qt3DCore::QNodeId> pendingWrappers = manager.takePendingProxiesToLoad();
        QCOMPARE(pendingWrappers.size(), 1);
        QCOMPARE(pendingWrappers.first(), deviceProxy.id());
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DInput::Input::PhysicalDeviceProxy backendPhysicalDeviceProxy;
        Qt3DInput::Input::PhysicalDeviceProxyManager manager;
        TestProxy deviceProxy;

        // WHEN
        backendPhysicalDeviceProxy.setManager(&manager);
        simulateInitializationSync(&deviceProxy, &backendPhysicalDeviceProxy);

        backendPhysicalDeviceProxy.cleanup();

        // THEN
        QCOMPARE(backendPhysicalDeviceProxy.isEnabled(), false);
        QCOMPARE(backendPhysicalDeviceProxy.deviceName(), QString());
        QVERIFY(backendPhysicalDeviceProxy.manager() == nullptr);
        QVERIFY(backendPhysicalDeviceProxy.physicalDeviceId().isNull());
    }

};

QTEST_MAIN(tst_PhysicalDeviceProxy)

#include "tst_physicaldeviceproxy.moc"
