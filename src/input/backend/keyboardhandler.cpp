/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "keyboardhandler_p.h"

#include <Qt3DInput/qkeyboarddevice.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <QtCore/QVariant>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qkeyboardhandler_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
namespace Input {

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

Qt3DCore::QBackendNode *KeyboardHandlerFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    KeyboardHandler *input = m_handler->keyboardInputManager()->getOrCreateResource(change->subjectId());
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
