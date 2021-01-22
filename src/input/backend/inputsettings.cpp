/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

Qt3DCore::QBackendNode *InputSettingsFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Q_UNUSED(change);
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
