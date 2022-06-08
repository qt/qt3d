// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "physicaldeviceproxy_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <QtCore/QCoreApplication>

#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p.h>
#include <Qt3DInput/private/qabstractphysicaldeviceproxy_p_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

PhysicalDeviceProxy::PhysicalDeviceProxy()
    : BackendNode(QBackendNode::ReadWrite)
    , m_manager(nullptr)
{
}

void PhysicalDeviceProxy::cleanup()
{
    QBackendNode::setEnabled(false);
    m_deviceName.clear();
    m_manager = nullptr;
    m_physicalDeviceId = Qt3DCore::QNodeId();
}

QString PhysicalDeviceProxy::deviceName() const
{
    return m_deviceName;
}

void PhysicalDeviceProxy::setManager(PhysicalDeviceProxyManager *manager)
{
    m_manager = manager;
}

PhysicalDeviceProxyManager *PhysicalDeviceProxy::manager() const
{
    return m_manager;
}

void PhysicalDeviceProxy::setDevice(QAbstractPhysicalDevice *device)
{
    m_physicalDeviceId = Qt3DCore::QNodeId();
    // Move the device to the main thread
    if (device != nullptr) {
        m_physicalDeviceId = device->id();
        device->moveToThread(QCoreApplication::instance()->thread());
    }
}

Qt3DCore::QNodeId PhysicalDeviceProxy::physicalDeviceId() const
{
    return m_physicalDeviceId;
}

void PhysicalDeviceProxy::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (firstTime) {
        const QAbstractPhysicalDeviceProxy *node = qobject_cast<const QAbstractPhysicalDeviceProxy *>(frontEnd);
        if (!node)
            return;

        m_deviceName = node->deviceName();

        // Request to load the actual device
        m_manager->addPendingProxyToLoad(peerId());
    }
}

PhysicalDeviceProxyNodeFunctor::PhysicalDeviceProxyNodeFunctor(PhysicalDeviceProxyManager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *PhysicalDeviceProxyNodeFunctor::create(Qt3DCore::QNodeId id) const
{
    HPhysicalDeviceProxy handle = m_manager->getOrAcquireHandle(id);
    PhysicalDeviceProxy *backend = m_manager->data(handle);
    backend->setManager(m_manager);
    return backend;
}

Qt3DCore::QBackendNode *PhysicalDeviceProxyNodeFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void PhysicalDeviceProxyNodeFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_manager->releaseResource(id);
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

