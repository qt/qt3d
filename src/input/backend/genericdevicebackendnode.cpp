// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "genericdevicebackendnode_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qgenericinputdevice_p.h>
#include <Qt3DInput/private/qabstractphysicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

GenericDeviceBackendNode::GenericDeviceBackendNode(QBackendNode::Mode mode)
    : QAbstractPhysicalDeviceBackendNode(mode)
    , m_mutex()
{
}

GenericDeviceBackendNode::~GenericDeviceBackendNode()
{
}

void GenericDeviceBackendNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    QAbstractPhysicalDeviceBackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DInput::QGenericInputDevice *node = qobject_cast<const Qt3DInput::QGenericInputDevice *>(frontEnd);
    if (!node)
        return;

    auto *d = static_cast<Qt3DInput::QAbstractPhysicalDevicePrivate *>( Qt3DCore::QNodePrivate::get(const_cast<Qt3DCore::QNode *>(frontEnd)) );

    {
        const QMutexLocker lock(&m_mutex);
        for (const auto &val: std::as_const(d->m_pendingAxisEvents))
            m_axesValues[val.first] = val.second;
        for (const auto &val: std::as_const(d->m_pendingButtonsEvents))
            m_buttonsValues[val.first] = val.second;

        d->m_pendingAxisEvents.clear();
        d->m_pendingButtonsEvents.clear();
    }
}

void GenericDeviceBackendNode::cleanup()
{
    const QMutexLocker lock(&m_mutex);
    m_axesValues.clear();
    m_buttonsValues.clear();
    QAbstractPhysicalDeviceBackendNode::cleanup();
}

float GenericDeviceBackendNode::axisValue(int axisIdentifier) const
{
    const QMutexLocker lock(&m_mutex);
    return m_axesValues[axisIdentifier];
}

bool GenericDeviceBackendNode::isButtonPressed(int buttonIdentifier) const
{
    const QMutexLocker lock(&m_mutex);
    return m_buttonsValues[buttonIdentifier];
}

GenericDeviceBackendFunctor::GenericDeviceBackendFunctor(QInputAspect *inputaspect, InputHandler *handler)
    : m_inputAspect(inputaspect)
    , m_handler(handler)
{
}

Qt3DCore::QBackendNode *GenericDeviceBackendFunctor::create(Qt3DCore::QNodeId id) const
{
    GenericDeviceBackendNode *backendNode = m_handler->genericDeviceBackendNodeManager()->getOrCreateResource(id);
    backendNode->setInputAspect(m_inputAspect);
    m_handler->appendGenericDevice(m_handler->genericDeviceBackendNodeManager()->lookupHandle(id));
    return backendNode;
}

Qt3DCore::QBackendNode *GenericDeviceBackendFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_handler->genericDeviceBackendNodeManager()->lookupResource(id);
}

void GenericDeviceBackendFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_handler->removeGenericDevice(m_handler->genericDeviceBackendNodeManager()->lookupHandle(id));
    m_handler->genericDeviceBackendNodeManager()->releaseResource(id);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
