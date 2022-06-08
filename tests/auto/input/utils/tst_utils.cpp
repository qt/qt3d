// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DInput/private/utils_p.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/qanalogaxisinput.h>
#include "qbackendnodetester.h"
#include "testdeviceproxy.h"

namespace {

class FakeBackendDevice : public Qt3DInput::QAbstractPhysicalDeviceBackendNode
{
    // QAbstractPhysicalDeviceBackendNode interface
public:
    FakeBackendDevice()
        : Qt3DInput::QAbstractPhysicalDeviceBackendNode(Qt3DCore::QBackendNode::ReadOnly)
    {}
    float axisValue(int) const override { return 0.0f; }
    bool isButtonPressed(int) const  override { return false; }
};

class FakeInputDeviceIntegration : public Qt3DInput::QInputDeviceIntegration
{
public:
    explicit FakeInputDeviceIntegration(Qt3DInput::QAbstractPhysicalDevice *device)
        : Qt3DInput::QInputDeviceIntegration()
        , m_device(device)
    {}

    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64) override { return std::vector<Qt3DCore::QAspectJobPtr>(); }
    Qt3DInput::QAbstractPhysicalDevice *createPhysicalDevice(const QString &) override { return nullptr; }
    QList<Qt3DCore::QNodeId> physicalDevices() const override { return {}; }
    QStringList deviceNames() const override { return QStringList(); }

    Qt3DInput::QAbstractPhysicalDeviceBackendNode *physicalDevice(Qt3DCore::QNodeId deviceId) const override
    {
        if (m_device->id() == deviceId)
            return new FakeBackendDevice();
        return nullptr;
    }

private:
    void onInitialize() override {}
    Qt3DInput::QAbstractPhysicalDevice *m_device;
};


} // anonymous

class tst_Utils : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPhysicalDeviceForValidAxisInput()
    {
        // GIVEN
        Qt3DInput::QAnalogAxisInput analogAxisInput;
        Qt3DInput::Input::InputHandler handler;
        TestPhysicalDevice testPhysicalDevice;
        FakeInputDeviceIntegration fakeIntegration(&testPhysicalDevice);

        analogAxisInput.setSourceDevice(&testPhysicalDevice);

        // WHEN -> Create backend AnalogAxisInput
        Qt3DInput::Input::AnalogAxisInput *backendAxisInput = handler.analogAxisInputManager()->getOrCreateResource(analogAxisInput.id());
        simulateInitializationSync(&analogAxisInput, backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput->axis(), analogAxisInput.axis());
        QCOMPARE(backendAxisInput->sourceDevice(), testPhysicalDevice.id());

        // Create backend integration
        handler.addInputDeviceIntegration(&fakeIntegration);

        // WHEN
        Qt3DInput::QAbstractPhysicalDeviceBackendNode *backendDevice = Qt3DInput::Input::Utils::physicalDeviceForInput(backendAxisInput, &handler);

        // THEN -> FakeIntegration returns something non null if it receives
        // the same id as the device used to create it
        QVERIFY(backendDevice != nullptr);
        delete backendDevice;
    }

    void checkProxyPhysicalDeviceForValidAxisInput()
    {
        // GIVEN
        Qt3DInput::QAnalogAxisInput analogAxisInput;
        Qt3DInput::Input::InputHandler handler;
        TestProxy testProxyPhysicalDevice;
        TestPhysicalDevice testPhysicalDevice;
        FakeInputDeviceIntegration fakeIntegration(&testPhysicalDevice);

        analogAxisInput.setSourceDevice(&testProxyPhysicalDevice);

        // WHEN -> Create backend AnalogAxisInput
        Qt3DInput::Input::AnalogAxisInput *backendAxisInput = handler.analogAxisInputManager()->getOrCreateResource(analogAxisInput.id());
        simulateInitializationSync(&analogAxisInput, backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput->axis(), analogAxisInput.axis());
        QCOMPARE(backendAxisInput->sourceDevice(), testProxyPhysicalDevice.id());

        // WHEN -> Create backend PhysicalProxiDevice
        Qt3DInput::Input::PhysicalDeviceProxy *backendProxyDevice = handler.physicalDeviceProxyManager()->getOrCreateResource(testProxyPhysicalDevice.id());
        backendProxyDevice->setManager(handler.physicalDeviceProxyManager());
        simulateInitializationSync(&testProxyPhysicalDevice, backendProxyDevice);
        backendProxyDevice->setDevice(&testPhysicalDevice);

        // THEN
        QCOMPARE(backendProxyDevice->physicalDeviceId(), testPhysicalDevice.id());

        // Create backend integration
        handler.addInputDeviceIntegration(&fakeIntegration);

        // WHEN
        Qt3DInput::QAbstractPhysicalDeviceBackendNode *backendDevice = Qt3DInput::Input::Utils::physicalDeviceForInput(backendAxisInput, &handler);

        // THEN -> FakeIntegration returns something non null if it receives
        // the same id as the device used to create it
        QVERIFY(backendDevice != nullptr);
        delete backendDevice;
    }

    void checkNoPhysicalDeviceForInvalidAxisInput()
    {
        // GIVEN
        Qt3DInput::QAnalogAxisInput analogAxisInput;
        Qt3DInput::Input::InputHandler handler;
        TestPhysicalDevice testPhysicalDevice;
        TestPhysicalDevice testPhysicalDevice2;
        FakeInputDeviceIntegration fakeIntegration(&testPhysicalDevice);

        analogAxisInput.setSourceDevice(&testPhysicalDevice2);

        // WHEN -> Create backend AnalogAxisInput
        Qt3DInput::Input::AnalogAxisInput *backendAxisInput = handler.analogAxisInputManager()->getOrCreateResource(analogAxisInput.id());
        simulateInitializationSync(&analogAxisInput, backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput->axis(), analogAxisInput.axis());
        QCOMPARE(backendAxisInput->sourceDevice(), testPhysicalDevice2.id());

        // Create backend integration
        handler.addInputDeviceIntegration(&fakeIntegration);

        // WHEN
        Qt3DInput::QAbstractPhysicalDeviceBackendNode *backendDevice = Qt3DInput::Input::Utils::physicalDeviceForInput(backendAxisInput, &handler);

        // THEN ->  FakeIntegration returns something non null if it receives
        // the same id as the device used to create it (testPhysicalDevice != testPhysicalDevice2)
        QVERIFY(backendDevice == nullptr);
    }

    void checkNoPysicalDeviceForInvalidProxyPhysicalDevice()
    {
        // GIVEN
        Qt3DInput::QAnalogAxisInput analogAxisInput;
        Qt3DInput::Input::InputHandler handler;
        TestProxy testProxyPhysicalDevice;
        TestPhysicalDevice testPhysicalDevice;
        TestPhysicalDevice testPhysicalDevice2;
        FakeInputDeviceIntegration fakeIntegration(&testPhysicalDevice);

        analogAxisInput.setSourceDevice(&testProxyPhysicalDevice);

        // WHEN -> Create backend AnalogAxisInput
        Qt3DInput::Input::AnalogAxisInput *backendAxisInput = handler.analogAxisInputManager()->getOrCreateResource(analogAxisInput.id());
        simulateInitializationSync(&analogAxisInput, backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput->axis(), analogAxisInput.axis());
        QCOMPARE(backendAxisInput->sourceDevice(), testProxyPhysicalDevice.id());

        // WHEN -> Create backend PhysicalProxiDevice
        Qt3DInput::Input::PhysicalDeviceProxy *backendProxyDevice = handler.physicalDeviceProxyManager()->getOrCreateResource(testProxyPhysicalDevice.id());
        backendProxyDevice->setManager(handler.physicalDeviceProxyManager());
        simulateInitializationSync(&testProxyPhysicalDevice, backendProxyDevice);
        backendProxyDevice->setDevice(&testPhysicalDevice2);

        // THEN
        QCOMPARE(backendProxyDevice->physicalDeviceId(), testPhysicalDevice2.id());

        // Create backend integration
        handler.addInputDeviceIntegration(&fakeIntegration);

        // WHEN
        Qt3DInput::QAbstractPhysicalDeviceBackendNode *backendDevice = Qt3DInput::Input::Utils::physicalDeviceForInput(backendAxisInput, &handler);

        // THEN ->  FakeIntegration returns something non null if it receives
        // the same id as the device used to create it (testPhysicalDevice != testPhysicalDevice2)
        QVERIFY(backendDevice == nullptr);
    }
};

QTEST_MAIN(tst_Utils)

#include "tst_utils.moc"
