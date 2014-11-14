/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
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

void KeyboardController::requestFocusForInput(const QNodeUuid &inputId)
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

void KeyboardController::addKeyboardInput(const QNodeUuid &input)
{
    if (!m_keyboardInputs.contains(input)) {
        m_keyboardInputs.append(input);
        m_keyboardInputHandles.append(m_inputHandler->keyboardInputManager()->lookupHandle(input));
    }
}

void KeyboardController::removeKeyboardInput(const QNodeUuid &input)
{
    m_keyboardInputs.removeAll(input);
    m_keyboardInputHandles.removeAll(m_inputHandler->keyboardInputManager()->lookupHandle(input));
}

void KeyboardController::setCurrentFocusItem(const QNodeUuid &input)
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

QBackendNode *KeyboardControllerFunctor::create(QNode *frontend) const
{
    KeyboardController *controller = m_handler->keyboardControllerManager()->getOrCreateResource(frontend->uuid());
    controller->setInputHandler(m_handler);
    controller->setPeer(frontend);
    m_handler->appendKeyboardController(m_handler->keyboardControllerManager()->lookupHandle(frontend->uuid()));
    return controller;
}

QBackendNode *KeyboardControllerFunctor::get(QNode *frontend) const
{
    return m_handler->keyboardControllerManager()->lookupResource(frontend->uuid());
}

void KeyboardControllerFunctor::destroy(QNode *frontend) const
{
    m_handler->removeKeyboardController(m_handler->keyboardControllerManager()->lookupHandle(frontend->uuid()));
    m_handler->keyboardControllerManager()->releaseResource(frontend->uuid());
}

} // Inputs

} // Qt3D

QT_END_NAMESPACE
