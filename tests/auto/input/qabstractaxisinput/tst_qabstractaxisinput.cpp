// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QAbstractAxisInput>
#include <Qt3DInput/QAbstractPhysicalDevice>
#include <Qt3DInput/private/qabstractaxisinput_p.h>

#include "testdevice.h"
#include <testarbiter.h>

class DummyAxisInput : public Qt3DInput::QAbstractAxisInput
{
    Q_OBJECT
public:
    explicit DummyAxisInput(QNode *parent = nullptr)
        : Qt3DInput::QAbstractAxisInput(*new Qt3DInput::QAbstractAxisInputPrivate, parent)
    {
    }
};

class tst_QAbstractAxisInput: public QObject
{
    Q_OBJECT
public:
    tst_QAbstractAxisInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
        qRegisterMetaType<Qt3DCore::QNode*>("Qt3DCore::QNode*");
    }

private Q_SLOTS:
    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QAbstractAxisInput> axisInput(new DummyAxisInput());
        arbiter.setArbiterOnNode(axisInput.data());

        // WHEN
        TestDevice *device = new TestDevice(axisInput.data());
        QCoreApplication::processEvents();
        arbiter.clear();

        axisInput->setSourceDevice(device);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();
    }

    void checkSourceDeviceBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAbstractAxisInput> axisInput(new DummyAxisInput);
        {
            // WHEN
            Qt3DInput::QAbstractPhysicalDevice device;
            axisInput->setSourceDevice(&device);

            // THEN
            QCOMPARE(device.parent(), axisInput.data());
            QCOMPARE(axisInput->sourceDevice(), &device);
        }
        // THEN (Should not crash and effect be unset)
        QVERIFY(axisInput->sourceDevice() == nullptr);

        {
            // WHEN
            DummyAxisInput someOtherAxisInput;
            QScopedPointer<Qt3DInput::QAbstractPhysicalDevice> device(new Qt3DInput::QAbstractPhysicalDevice(&someOtherAxisInput));
            axisInput->setSourceDevice(device.data());

            // THEN
            QCOMPARE(device->parent(), &someOtherAxisInput);
            QCOMPARE(axisInput->sourceDevice(), device.data());

            // WHEN
            axisInput.reset();
            device.reset();

            // THEN Should not crash when the device is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAbstractAxisInput)

#include "tst_qabstractaxisinput.moc"
