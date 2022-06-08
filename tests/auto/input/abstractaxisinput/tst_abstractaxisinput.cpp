// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DInput/private/abstractaxisinput_p.h>
#include <Qt3DInput/QAbstractAxisInput>
#include <Qt3DInput/private/qabstractaxisinput_p.h>
#include "testdevice.h"

class DummyAxisInput : public Qt3DInput::QAbstractAxisInput
{
    Q_OBJECT
public:
    explicit DummyAxisInput(QNode *parent = nullptr)
        : Qt3DInput::QAbstractAxisInput(*new Qt3DInput::QAbstractAxisInputPrivate, parent)
    {
    }
};

class DummyAxisInputBackend : public Qt3DInput::Input::AbstractAxisInput
{
public:
    explicit DummyAxisInputBackend()
        : AbstractAxisInput()
    {
    }

    float process(Qt3DInput::Input::InputHandler *inputHandler, qint64 currentTime) override
    {
        Q_UNUSED(inputHandler);
        Q_UNUSED(currentTime);
        return 0.0f;
    }
};


class tst_AbstractAxisInput : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        DummyAxisInputBackend backendAxisInput;
        DummyAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        simulateInitializationSync(&axisInput, &backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput.peerId(), axisInput.id());
        QCOMPARE(backendAxisInput.isEnabled(), axisInput.isEnabled());
        QCOMPARE(backendAxisInput.sourceDevice(), sourceDevice.id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        DummyAxisInputBackend backendAxisInput;

        // THEN
        QVERIFY(backendAxisInput.peerId().isNull());
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());

        // GIVEN
        DummyAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        simulateInitializationSync(&axisInput, &backendAxisInput);
        backendAxisInput.cleanup();

        // THEN
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        DummyAxisInput axisInput;
        DummyAxisInputBackend backendAxisInput;
        simulateInitializationSync(&axisInput, &backendAxisInput);

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
};

QTEST_APPLESS_MAIN(tst_AbstractAxisInput)

#include "tst_abstractaxisinput.moc"
