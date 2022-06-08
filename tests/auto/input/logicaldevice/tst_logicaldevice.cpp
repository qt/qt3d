// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/private/qlogicaldevice_p.h>
#include <Qt3DInput/private/logicaldevice_p.h>
#include "qbackendnodetester.h"

class tst_LogicalDevice : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DInput::Input::LogicalDevice backendLogicalDevice;

        // THEN
        QVERIFY(backendLogicalDevice.peerId().isNull());
        QCOMPARE(backendLogicalDevice.isEnabled(), false);
        QVERIFY(backendLogicalDevice.axes().empty());
        QVERIFY(backendLogicalDevice.actions().empty());
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DInput::QLogicalDevice logicalDevice;
        Qt3DInput::Input::LogicalDevice backendLogicalDevice;
        simulateInitializationSync(&logicalDevice, &backendLogicalDevice);

        // WHEN
        backendLogicalDevice.setEnabled(true);

        // WHEN
        Qt3DInput::QAxis newAxisValue;
        Qt3DInput::QAction newActionValue;
        logicalDevice.addAxis(&newAxisValue);
        logicalDevice.addAction(&newActionValue);
        backendLogicalDevice.syncFromFrontEnd(&logicalDevice, false);

        // THEN
        QCOMPARE(backendLogicalDevice.axes().size(), 1);
        QCOMPARE(backendLogicalDevice.actions().size(), 1);

        // WHEN
        backendLogicalDevice.cleanup();

        // THEN
        QCOMPARE(backendLogicalDevice.isEnabled(), false);
        QCOMPARE(backendLogicalDevice.axes().size(), 0);
        QCOMPARE(backendLogicalDevice.actions().size(), 0);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DInput::QLogicalDevice logicalDevice;

        Qt3DInput::QAction *action = new Qt3DInput::QAction(&logicalDevice);
        Qt3DInput::QAxis *axis = new Qt3DInput::QAxis(&logicalDevice);
        logicalDevice.addAction(action);
        logicalDevice.addAxis(axis);

        {
            // WHEN
            Qt3DInput::Input::LogicalDevice backendLogicalDevice;
            simulateInitializationSync(&logicalDevice, &backendLogicalDevice);

            // THEN
            QCOMPARE(backendLogicalDevice.isEnabled(), true);
            QCOMPARE(backendLogicalDevice.axes().size(), 1);
            QCOMPARE(backendLogicalDevice.axes().first(), axis->id());
            QCOMPARE(backendLogicalDevice.actions().size(), 1);
            QCOMPARE(backendLogicalDevice.actions().first(), action->id());
            QCOMPARE(backendLogicalDevice.peerId(), logicalDevice.id());
        }
        {
            // WHEN
            Qt3DInput::Input::LogicalDevice backendLogicalDevice;
            logicalDevice.setEnabled(false);
            simulateInitializationSync(&logicalDevice, &backendLogicalDevice);

            // THEN
            QCOMPARE(backendLogicalDevice.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DInput::QLogicalDevice logicalDevice;
        Qt3DInput::Input::LogicalDevice backendLogicalDevice;
        simulateInitializationSync(&logicalDevice, &backendLogicalDevice);

        {
            // WHEN
            const bool newValue = false;
            logicalDevice.setEnabled(newValue);
            backendLogicalDevice.syncFromFrontEnd(&logicalDevice, false);

            // THEN
            QCOMPARE(backendLogicalDevice.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DInput::QAxis newValue;
            logicalDevice.addAxis(&newValue);
            backendLogicalDevice.syncFromFrontEnd(&logicalDevice, false);

            // THEN
            QCOMPARE(backendLogicalDevice.axes().size(), 1);
            QCOMPARE(backendLogicalDevice.axes().first(), newValue.id());

            // WHEN
            logicalDevice.removeAxis(&newValue);
            backendLogicalDevice.syncFromFrontEnd(&logicalDevice, false);

            // THEN
            QCOMPARE(backendLogicalDevice.axes().size(), 0);
        }
        {
            // WHEN
            Qt3DInput::QAction newValue;
            logicalDevice.addAction(&newValue);
            backendLogicalDevice.syncFromFrontEnd(&logicalDevice, false);

            // THEN
            QCOMPARE(backendLogicalDevice.actions().size(), 1);
            QCOMPARE(backendLogicalDevice.actions().first(), newValue.id());

            // WHEN
            logicalDevice.removeAction(&newValue);
            backendLogicalDevice.syncFromFrontEnd(&logicalDevice, false);

            // THEN
            QCOMPARE(backendLogicalDevice.actions().size(), 0);
        }
    }

};

QTEST_APPLESS_MAIN(tst_LogicalDevice)

#include "tst_logicaldevice.moc"
