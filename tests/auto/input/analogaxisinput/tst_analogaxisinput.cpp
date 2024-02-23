// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include "testdevice.h"

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DInput/private/analogaxisinput_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/QAnalogAxisInput>

class tst_AnalogAxisInput: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::AnalogAxisInput backendAxisInput;
        Qt3DInput::QAnalogAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setAxis(327);
        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        simulateInitializationSync(&axisInput, &backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput.peerId(), axisInput.id());
        QCOMPARE(backendAxisInput.isEnabled(), axisInput.isEnabled());
        QCOMPARE(backendAxisInput.axis(), axisInput.axis());
        QCOMPARE(backendAxisInput.sourceDevice(), sourceDevice.id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::AnalogAxisInput backendAxisInput;

        // THEN
        QVERIFY(backendAxisInput.peerId().isNull());
        QCOMPARE(backendAxisInput.axis(), 0);
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());

        // GIVEN
        Qt3DInput::QAnalogAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setAxis(327);
        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        simulateInitializationSync(&axisInput, &backendAxisInput);
        backendAxisInput.cleanup();

        // THEN
        QCOMPARE(backendAxisInput.axis(), 0);
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::QAnalogAxisInput axisInput;
        Qt3DInput::Input::AnalogAxisInput backendAxisInput;
        simulateInitializationSync(&axisInput, &backendAxisInput);

        // WHEN
        axisInput.setAxis(32);
        backendAxisInput.syncFromFrontEnd(&axisInput, false);

        // THEN
        QCOMPARE(backendAxisInput.axis(), 32);

        // WHEN
        axisInput.setEnabled(false);
        backendAxisInput.syncFromFrontEnd(&axisInput, false);

        // THEN
        QCOMPARE(backendAxisInput.isEnabled(), false);

        // WHEN
        TestDevice device;
        axisInput.setSourceDevice(&device);
        backendAxisInput.syncFromFrontEnd(&axisInput, false);

        // THEN
        QCOMPARE(backendAxisInput.sourceDevice(), device.id());
    }

    void shouldProcessAxisValue()
    {
        const qint64 s = 1000000000;

        // GIVEN
        TestDeviceIntegration deviceIntegration;
        TestDevice *device = deviceIntegration.createPhysicalDevice("keyboard");
        TestDeviceBackendNode *deviceBackend = deviceIntegration.physicalDevice(device->id());
        Qt3DInput::Input::InputHandler handler;
        handler.addInputDeviceIntegration(&deviceIntegration);

        Qt3DInput::Input::AnalogAxisInput backendAxisInput;
        Qt3DInput::QAnalogAxisInput axisInput;
        axisInput.setEnabled(true);
        axisInput.setAxis(2);
        axisInput.setSourceDevice(device);
        simulateInitializationSync(&axisInput, &backendAxisInput);
        QCOMPARE(backendAxisInput.axis(), 2);

        // WHEN
        deviceBackend->setAxisValue(2, 0.1f);

        // THEN
        QCOMPARE(backendAxisInput.process(&handler, 30 * s), 0.1f);
        QCOMPARE(backendAxisInput.process(&handler, 31 * s), 0.1f);

        // WHEN
        deviceBackend->setAxisValue(2, 0.2f);

        // THEN
        QCOMPARE(backendAxisInput.process(&handler, 32 * s), 0.2f);
        QCOMPARE(backendAxisInput.process(&handler, 33 * s), 0.2f);
    }

    void shouldNotProcessAxisValueWhenDisabled()
    {
        const qint64 s = 1000000000;

        // GIVEN
        TestDeviceIntegration deviceIntegration;
        TestDevice *device = deviceIntegration.createPhysicalDevice("keyboard");
        TestDeviceBackendNode *deviceBackend = deviceIntegration.physicalDevice(device->id());
        Qt3DInput::Input::InputHandler handler;
        handler.addInputDeviceIntegration(&deviceIntegration);

        Qt3DInput::Input::AnalogAxisInput backendAxisInput;
        Qt3DInput::QAnalogAxisInput axisInput;
        axisInput.setEnabled(false);
        axisInput.setAxis(2);
        axisInput.setSourceDevice(device);
        simulateInitializationSync(&axisInput, &backendAxisInput);
        QCOMPARE(backendAxisInput.axis(), 2);

        // WHEN
        deviceBackend->setAxisValue(2, 0.1f);

        // THEN
        QCOMPARE(backendAxisInput.process(&handler, 30 * s), 0.0f);
        QCOMPARE(backendAxisInput.process(&handler, 31 * s), 0.0f);

        // WHEN
        deviceBackend->setAxisValue(2, 0.2f);

        // THEN
        QCOMPARE(backendAxisInput.process(&handler, 32 * s), 0.0f);
        QCOMPARE(backendAxisInput.process(&handler, 33 * s), 0.0f);
    }
};

QTEST_APPLESS_MAIN(tst_AnalogAxisInput)

#include "tst_analogaxisinput.moc"
