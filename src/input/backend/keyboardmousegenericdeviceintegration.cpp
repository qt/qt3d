// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "keyboardmousegenericdeviceintegration_p.h"

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

KeyboardMouseGenericDeviceIntegration::KeyboardMouseGenericDeviceIntegration(InputHandler *handler)
    : Qt3DInput::QInputDeviceIntegration()
    , m_handler(handler)
{
}

KeyboardMouseGenericDeviceIntegration::~KeyboardMouseGenericDeviceIntegration()
{
}

void KeyboardMouseGenericDeviceIntegration::onInitialize()
{
}

std::vector<Qt3DCore::QAspectJobPtr> KeyboardMouseGenericDeviceIntegration::jobsToExecute(qint64 time)
{
    Q_UNUSED(time);
    return {};
}

QAbstractPhysicalDevice *KeyboardMouseGenericDeviceIntegration::createPhysicalDevice(const QString &name)
{
    Q_UNUSED(name);
    return nullptr;
}

QList<Qt3DCore::QNodeId> KeyboardMouseGenericDeviceIntegration::physicalDevices() const
{
    // TO DO: could return the ids of active KeyboardDevice/MouseDevice
    return { };
}

QAbstractPhysicalDeviceBackendNode *KeyboardMouseGenericDeviceIntegration::physicalDevice(Qt3DCore::QNodeId id) const
{
    QAbstractPhysicalDeviceBackendNode *device = m_handler->keyboardDeviceManager()->lookupResource(id);
    if (!device)
        device = m_handler->mouseDeviceManager()->lookupResource(id);
    if (!device)
        device = m_handler->genericDeviceBackendNodeManager()->lookupResource(id);
    return device;
}

QStringList KeyboardMouseGenericDeviceIntegration::deviceNames() const
{
    return QStringList() << tr("Keyboard") << tr("Mouse");
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#include "moc_keyboardmousegenericdeviceintegration_p.cpp"
