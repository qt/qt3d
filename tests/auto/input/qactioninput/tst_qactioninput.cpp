// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/private/qactioninput_p.h>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAbstractPhysicalDevice>

#include "testdevice.h"
#include <testarbiter.h>

class tst_QActionInput: public QObject
{
    Q_OBJECT
public:
    tst_QActionInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QActionInput> actionInput(new Qt3DInput::QActionInput());
        arbiter.setArbiterOnNode(actionInput.data());

        // WHEN
        const QList<int> buttons = { 555 };
        actionInput->setButtons(buttons);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), actionInput.data());

        arbiter.clear();

        // WHEN
        TestDevice *device = new TestDevice(actionInput.data());
        QCoreApplication::processEvents();

        actionInput->setSourceDevice(device);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), actionInput.data());

        arbiter.clear();
    }

    void checkSourceDeviceBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QActionInput> actionInput(new Qt3DInput::QActionInput);
        {
            // WHEN
            Qt3DInput::QAbstractPhysicalDevice device;
            actionInput->setSourceDevice(&device);

            // THEN
            QCOMPARE(device.parent(), actionInput.data());
            QCOMPARE(actionInput->sourceDevice(), &device);
        }
        // THEN (Should not crash and effect be unset)
        QVERIFY(actionInput->sourceDevice() == nullptr);

        {
            // WHEN
            Qt3DInput::QActionInput someOtherActionInput;
            QScopedPointer<Qt3DInput::QAbstractPhysicalDevice> device(new Qt3DInput::QAbstractPhysicalDevice(&someOtherActionInput));
            actionInput->setSourceDevice(device.data());

            // THEN
            QCOMPARE(device->parent(), &someOtherActionInput);
            QCOMPARE(actionInput->sourceDevice(), device.data());

            // WHEN
            actionInput.reset();
            device.reset();

            // THEN Should not crash when the device is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QActionInput)

#include "tst_qactioninput.moc"
