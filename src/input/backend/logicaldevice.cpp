// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "logicaldevice_p.h"

#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qlogicaldevice.h>

#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qlogicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

LogicalDevice::LogicalDevice()
    : BackendNode()
{
}

void LogicalDevice::cleanup()
{
    QBackendNode::setEnabled(false);
    m_actions.clear();
    m_axes.clear();
}

void LogicalDevice::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QLogicalDevice *node = qobject_cast<const QLogicalDevice *>(frontEnd);
    if (!node)
        return;

    m_actions = Qt3DCore::qIdsForNodes(node->actions());
    m_axes = Qt3DCore::qIdsForNodes(node->axes());
}

LogicalDeviceNodeFunctor::LogicalDeviceNodeFunctor(LogicalDeviceManager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *LogicalDeviceNodeFunctor::create(Qt3DCore::QNodeId id) const
{
    HLogicalDevice handle = m_manager->getOrAcquireHandle(id);
    LogicalDevice *backend = m_manager->data(handle);
    m_manager->addActiveDevice(handle);
    return backend;
}

Qt3DCore::QBackendNode *LogicalDeviceNodeFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void LogicalDeviceNodeFunctor::destroy(Qt3DCore::QNodeId id) const
{
    HLogicalDevice handle = m_manager->lookupHandle(id);
    m_manager->releaseResource(id);
    m_manager->removeActiveDevice(handle);
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE
