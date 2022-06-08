// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/private/qlogicaldevice_p.h>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAction>

#include "testarbiter.h"

class tst_QLogicalDevice: public QObject
{
    Q_OBJECT
public:
    tst_QLogicalDevice()
    {
        qRegisterMetaType<Qt3DCore::QNode *>();
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QLogicalDevice> logicalDevice(new Qt3DInput::QLogicalDevice());
        arbiter.setArbiterOnNode(logicalDevice.data());

        // WHEN
        Qt3DInput::QAction *action = new Qt3DInput::QAction(logicalDevice.data());
        QCoreApplication::processEvents();
        arbiter.clear();

        logicalDevice->addAction(action);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();

        // WHEN
        logicalDevice->removeAction(action);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();

        // WHEN
        Qt3DInput::QAxis *axis = new Qt3DInput::QAxis(logicalDevice.data());
        QCoreApplication::processEvents();
        arbiter.clear();

        logicalDevice->addAxis(axis);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();

        // WHEN
        logicalDevice->removeAxis(axis);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();
    }

    void checkAxisBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QLogicalDevice> device(new Qt3DInput::QLogicalDevice);
        {
            // WHEN
            Qt3DInput::QAxis axis;
            device->addAxis(&axis);

            // THEN
            QCOMPARE(axis.parent(), device.data());
            QCOMPARE(device->axes().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(device->axes().empty());

        {
            // WHEN
            Qt3DInput::QLogicalDevice someOtherDevice;
            QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis(&someOtherDevice));
            device->addAxis(axis.data());

            // THEN
            QCOMPARE(axis->parent(), &someOtherDevice);
            QCOMPARE(device->axes().size(), 1);

            // WHEN
            device.reset();
            axis.reset();

            // THEN Should not crash when the axis is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkActionBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QLogicalDevice> device(new Qt3DInput::QLogicalDevice);
        {
            // WHEN
            Qt3DInput::QAction action;
            device->addAction(&action);

            // THEN
            QCOMPARE(action.parent(), device.data());
            QCOMPARE(device->actions().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(device->actions().empty());

        {
            // WHEN
            Qt3DInput::QLogicalDevice someOtherDevice;
            QScopedPointer<Qt3DInput::QAction> action(new Qt3DInput::QAction(&someOtherDevice));
            device->addAction(action.data());

            // THEN
            QCOMPARE(action->parent(), &someOtherDevice);
            QCOMPARE(device->actions().size(), 1);

            // WHEN
            device.reset();
            action.reset();

            // THEN Should not crash when the action is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QLogicalDevice)

#include "tst_qlogicaldevice.moc"
