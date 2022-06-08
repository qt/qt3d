// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "inputsettings_p.h"

#include <Qt3DInput/qinputsettings.h>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/qinputsettings_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

InputSettings::InputSettings()
    : BackendNode(QBackendNode::ReadOnly)
    , m_eventSource(nullptr)
{
}

void InputSettings::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QInputSettings *node = qobject_cast<const QInputSettings *>(frontEnd);
    if (!node)
        return;

    m_eventSource = node->eventSource();
}

InputSettingsFunctor::InputSettingsFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

Qt3DCore::QBackendNode *InputSettingsFunctor::create(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    if (m_handler->inputSettings() != nullptr) {
        qWarning() << "Input settings already specified";
        return nullptr;
    }

    InputSettings *settings = new InputSettings();
    m_handler->setInputSettings(settings);
    return settings;
}

Qt3DCore::QBackendNode *InputSettingsFunctor::get(Qt3DCore::QNodeId id) const
{
    InputSettings *settings = m_handler->inputSettings();
    if (settings != nullptr && settings->peerId() == id)
        return settings;
    return nullptr;
}

void InputSettingsFunctor::destroy(Qt3DCore::QNodeId id) const
{
    InputSettings *settings = m_handler->inputSettings();
    if (settings != nullptr && settings->peerId() == id) {
        m_handler->setInputSettings(nullptr);
        delete settings;
    }
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
