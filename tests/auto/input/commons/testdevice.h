// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef TESTDEVICE_H
#define TESTDEVICE_H

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/QAbstractPhysicalDevice>
#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>
#include <Qt3DInput/private/qinputdeviceintegration_p.h>
#include <qbackendnodetester.h>

class TestDevice : public Qt3DInput::QAbstractPhysicalDevice
{
    Q_OBJECT
public:
    explicit TestDevice(Qt3DCore::QNode *parent = nullptr)
        : Qt3DInput::QAbstractPhysicalDevice(parent)
    {}

    int axisCount() const final { return 0; }
    int buttonCount() const final { return 0; }
    QStringList axisNames() const final { return QStringList(); }
    QStringList buttonNames() const final { return QStringList(); }
    int axisIdentifier(const QString &name) const final { Q_UNUSED(name); return 0; }
    int buttonIdentifier(const QString &name) const final { Q_UNUSED(name); return 0; }

private:
    friend class TestDeviceBackendNode;
};

class TestDeviceBackendNode : public Qt3DInput::QAbstractPhysicalDeviceBackendNode
{
public:
    explicit TestDeviceBackendNode(TestDevice *device)
        : Qt3DInput::QAbstractPhysicalDeviceBackendNode(ReadOnly)
    {
        Qt3DCore::QBackendNodeTester().simulateInitializationSync(device, this);
    }

    float axisValue(int axisIdentifier) const final
    {
        return m_axisValues.value(axisIdentifier);
    }

    void setAxisValue(int axisIdentifier, float value)
    {
        m_axisValues.insert(axisIdentifier, value);
    }

    bool isButtonPressed(int buttonIdentifier) const final
    {
        return m_buttonStates.value(buttonIdentifier);
    }

    void setButtonPressed(int buttonIdentifier, bool pressed)
    {
        m_buttonStates.insert(buttonIdentifier, pressed);
    }

private:
    QHash<int, float> m_axisValues;
    QHash<int, bool> m_buttonStates;
};

class TestDeviceIntegration : public Qt3DInput::QInputDeviceIntegration
{
    Q_OBJECT
public:
    explicit TestDeviceIntegration(QObject *parent = nullptr)
        : Qt3DInput::QInputDeviceIntegration(parent),
          m_devicesParent(new Qt3DCore::QNode)
    {
    }

    ~TestDeviceIntegration()
    {
        qDeleteAll(m_deviceBackendNodes);
    }

    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) final
    {
        Q_UNUSED(time);
        return std::vector<Qt3DCore::QAspectJobPtr>();
    }

    TestDevice *createPhysicalDevice(const QString &name) final
    {
        Q_ASSERT(!deviceNames().contains(name));
        auto device = new TestDevice(m_devicesParent.data()); // Avoids unwanted reparenting
        device->setObjectName(name);
        m_devices.append(device);
        m_deviceBackendNodes.append(new TestDeviceBackendNode(device));
        return device;
    }

    QList<Qt3DCore::QNodeId> physicalDevices() const final
    {
        QList<Qt3DCore::QNodeId> ids;
        std::transform(m_devices.constBegin(), m_devices.constEnd(),
                       std::back_inserter(ids),
                       [] (TestDevice *device) { return device->id(); });
        return ids;
    }

    TestDeviceBackendNode *physicalDevice(Qt3DCore::QNodeId id) const final
    {
        auto it = std::find_if(m_deviceBackendNodes.constBegin(), m_deviceBackendNodes.constEnd(),
                               [id] (TestDeviceBackendNode *node) { return node->peerId() == id; });
        if (it == m_deviceBackendNodes.constEnd())
            return nullptr;
        else
            return *it;
    }

    QStringList deviceNames() const final
    {
        QStringList names;
        std::transform(m_devices.constBegin(), m_devices.constEnd(),
                       std::back_inserter(names),
                       [] (TestDevice *device) { return device->objectName(); });
        return names;
    }

private:
    void onInitialize() final {}

    QScopedPointer<Qt3DCore::QNode> m_devicesParent;
    QList<TestDevice *> m_devices;
    QList<TestDeviceBackendNode *> m_deviceBackendNodes;
};

#endif // TESTDEVICE_H
