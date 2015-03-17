/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "keyboardcontroller_p.h"
#include "inputhandler_p.h"
#include "inputmanagers_p.h"
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

// TO DO: Send change to frontend when activeInput changes

KeyboardController::KeyboardController()
    : QBackendNode()
    , m_inputHandler(Q_NULLPTR)
{
}

void KeyboardController::updateFromPeer(QNode *)
{
}

void KeyboardController::requestFocusForInput(const QNodeId &inputId)
{
    // Saves the last inputId, this will then be used in an Aspect Job to determine which
    // input will have the focus. This in turn will call KeyboardInput::setFocus which will
    // decide if sending a notification to the frontend is necessary
    m_lastRequester = inputId;
}

void KeyboardController::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void KeyboardController::addKeyboardInput(const QNodeId &input)
{
    if (!m_keyboardInputs.contains(input)) {
        m_keyboardInputs.append(input);
        m_keyboardInputHandles.append(m_inputHandler->keyboardInputManager()->lookupHandle(input));
    }
}

void KeyboardController::removeKeyboardInput(const QNodeId &input)
{
    m_keyboardInputs.removeAll(input);
    m_keyboardInputHandles.removeAll(m_inputHandler->keyboardInputManager()->lookupHandle(input));
}

void KeyboardController::setCurrentFocusItem(const QNodeId &input)
{
    m_currentFocusItem = input;
}

void KeyboardController::sceneChangeEvent(const QSceneChangePtr &)
{
}

KeyboardControllerFunctor::KeyboardControllerFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

QBackendNode *KeyboardControllerFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    KeyboardController *controller = m_handler->keyboardControllerManager()->getOrCreateResource(frontend->id());
    controller->setFactory(factory);
    controller->setInputHandler(m_handler);
    controller->setPeer(frontend);
    m_handler->appendKeyboardController(m_handler->keyboardControllerManager()->lookupHandle(frontend->id()));
    return controller;
}

QBackendNode *KeyboardControllerFunctor::get(const QNodeId &id) const
{
    return m_handler->keyboardControllerManager()->lookupResource(id);
}

void KeyboardControllerFunctor::destroy(const QNodeId &id) const
{
    m_handler->removeKeyboardController(m_handler->keyboardControllerManager()->lookupHandle(id));
    m_handler->keyboardControllerManager()->releaseResource(id);
}

} // Inputs

} // Qt3D

QT_END_NAMESPACE
