// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DInput/private/loadproxydevicejob_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/physicaldeviceproxy_p.h>
#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "qbackendnodetester.h"
#include "testdeviceproxy.h"
#include "testarbiter.h"

class FakeInputDeviceIntegration : public Qt3DInput::QInputDeviceIntegration
{
public:
    explicit FakeInputDeviceIntegration(const QString &name)
        : Qt3DInput::QInputDeviceIntegration()
        , m_name(name)
    {}

    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64) override
    {
        return std::vector<Qt3DCore::QAspectJobPtr>();
    }

    Qt3DInput::QAbstractPhysicalDevice *createPhysicalDevice(const QString &name) override
    {
        if (name == m_name)
            return new TestPhysicalDevice();
        return nullptr;
    }

    Qt3DInput::QAbstractPhysicalDeviceBackendNode *physicalDevice(Qt3DCore::QNodeId) const override
    {
        return nullptr;
    }

    QList<Qt3DCore::QNodeId> physicalDevices() const override
    {
        return {};
    }

    QStringList deviceNames() const override
    {
        return QStringList() << m_name;
    }

private:
    void onInitialize() override {}
    QString m_name;
};

class tst_LoadProxyDeviceJob : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DInput::Input::LoadProxyDeviceJob backendLoadProxyDeviceJob;

        // THEN
        QVERIFY(backendLoadProxyDeviceJob.inputHandler() == nullptr);
        QVERIFY(backendLoadProxyDeviceJob.proxies().empty());
    }

    void checkJobCreatesDeviceWhenValid()
    {
        // GIVEN
        Qt3DInput::Input::InputHandler inputHandler;
        Qt3DInput::Input::PhysicalDeviceProxyManager *manager = inputHandler.physicalDeviceProxyManager();

        FakeInputDeviceIntegration inputIntegration(QStringLiteral("TestProxy"));
        inputHandler.addInputDeviceIntegration(&inputIntegration);

        // THEN
        QCOMPARE(inputHandler.inputDeviceIntegrations().size(), 1);
        QCOMPARE(inputHandler.inputDeviceIntegrations().first(), &inputIntegration);

        // WHEN -> valid device name
        {
            // WHEN
            TestProxy proxy;
            TestArbiter arbiter;
            Qt3DInput::Input::PhysicalDeviceProxy *backendProxy = manager->getOrCreateResource(proxy.id());

            {
                backendProxy->setManager(manager);
                Qt3DCore::QBackendNodeTester backendNodeCreator;
                backendNodeCreator.simulateInitializationSync(&proxy, backendProxy);
            }

            // THEN
            QCOMPARE(manager->lookupResource(proxy.id()), backendProxy);
            QCOMPARE(backendProxy->deviceName(), QStringLiteral("TestProxy"));
            QVERIFY(backendProxy->physicalDeviceId().isNull());

            const QList<Qt3DCore::QNodeId> pendingProxies = manager->takePendingProxiesToLoad();
            QCOMPARE(pendingProxies.size(), 1);
            QCOMPARE(pendingProxies.first(), backendProxy->peerId());

            // WHEN
            Qt3DInput::Input::LoadProxyDeviceJob job;
            job.setInputHandler(&inputHandler);
            job.setProxiesToLoad(std::move(pendingProxies));

            job.run();

            // THEN -> PhysicalDeviceWrapper::setDevice should have been called
            QVERIFY(!backendProxy->physicalDeviceId().isNull());
        }

        // WHEN -> invalid name
        {
            // WHEN
            TestProxy proxy(QStringLiteral("NonExisting"));
            TestArbiter arbiter;
            Qt3DInput::Input::PhysicalDeviceProxy *backendProxy = manager->getOrCreateResource(proxy.id());

            {
                backendProxy->setManager(manager);
                Qt3DCore::QBackendNodeTester backendNodeCreator;
                backendNodeCreator.simulateInitializationSync(&proxy, backendProxy);
            }

            // THEN
            QCOMPARE(manager->lookupResource(proxy.id()), backendProxy);
            QCOMPARE(backendProxy->deviceName(), QStringLiteral("NonExisting"));

            const QList<Qt3DCore::QNodeId> pendingProxies = manager->takePendingProxiesToLoad();
            QCOMPARE(pendingProxies.size(), 1);
            QCOMPARE(pendingProxies.first(), backendProxy->peerId());

            // WHEN
            Qt3DInput::Input::LoadProxyDeviceJob job;
            job.setInputHandler(&inputHandler);
            job.setProxiesToLoad(std::move(pendingProxies));

            job.run();

            // THEN -> PhysicalDeviceWrapper::setDevice should not have been called
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

};

QTEST_MAIN(tst_LoadProxyDeviceJob)

#include "tst_loadproxydevicejob.moc"
