// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "keyboardhandler_p.h"

#include <Qt3DInput/qkeyboarddevice.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <QtCore/QVariant>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qkeyboardhandler_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DInput {
namespace Input {

using namespace Qt3DCore;

KeyboardHandler::KeyboardHandler()
    : BackendNode(QBackendNode::ReadWrite)
    , m_inputHandler(nullptr)
    , m_focus(false)
{
}

Qt3DCore::QNodeId KeyboardHandler::keyboardDevice() const
{
    return m_keyboardDevice;
}

void KeyboardHandler::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

// Called by the KeyboadDevice when the focus for the KeyboardHandler has changed
// Sends a change notification so that the frontend can update itself
void KeyboardHandler::setFocus(bool focus)
{
    if (focus != m_focus)
        m_focus = focus;
}

void KeyboardHandler::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DInput::QKeyboardHandler *node = qobject_cast<const Qt3DInput::QKeyboardHandler *>(frontEnd);
    if (!node)
        return;

    if (firstTime)
        m_focus = false;

    bool focusRequest = false;
    auto id = Qt3DCore::qIdForNode(node->sourceDevice());
    if (m_keyboardDevice != id) {
        setSourcerDevice(id);
        focusRequest = m_focus;
    }

    if (m_focus != node->focus())
        focusRequest = node->focus();

    if (focusRequest)
        requestFocus();
}

void KeyboardHandler::requestFocus()
{
    KeyboardDevice *keyboardDevice = m_inputHandler->keyboardDeviceManager()->lookupResource(m_keyboardDevice);
    if (keyboardDevice && isEnabled())
        keyboardDevice->requestFocusForInput(peerId());
}

void KeyboardHandler::setSourcerDevice(QNodeId device)
{
    m_keyboardDevice = device;
}

KeyboardHandlerFunctor::KeyboardHandlerFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

Qt3DCore::QBackendNode *KeyboardHandlerFunctor::create(Qt3DCore::QNodeId id) const
{
    KeyboardHandler *input = m_handler->keyboardInputManager()->getOrCreateResource(id);
    input->setInputHandler(m_handler);
    return input;
}

QBackendNode *KeyboardHandlerFunctor::get(QNodeId id) const
{
    return m_handler->keyboardInputManager()->lookupResource(id);
}

void KeyboardHandlerFunctor::destroy(QNodeId id) const
{
    m_handler->keyboardInputManager()->releaseResource(id);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
