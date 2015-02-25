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

#include "keyboardinput_p.h"
#include "qkeyboardinput.h"
#include "qkeyboardcontroller.h"
#include "inputhandler_p.h"
#include "inputmanagers_p.h"
#include <QVariant>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

KeyboardInput::KeyboardInput()
    : QBackendNode(QBackendNode::ReadWrite)
    , m_inputHandler(Q_NULLPTR)
    , m_focus(false)
{
}

void KeyboardInput::updateFromPeer(QNode *peer)
{
    QKeyboardInput *input = static_cast<QKeyboardInput *>(peer);
    if (input->controller() != Q_NULLPTR)
        setController(input->controller()->id());
    m_focus = false;
    m_enabled = input->isEnabled();
    if (input->focus())
        requestFocus();
}

QNodeId KeyboardInput::keyboardController() const
{
    return m_keyboardController;
}

void KeyboardInput::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

// Called by the KeyboadController when the focus for the KeyboardInput has changed
// Sends a change notification so that the frontend can update itself
void KeyboardInput::setFocus(bool focus)
{
    if (focus != m_focus) {
        m_focus = focus;
        QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
        e->setTargetNode(peerUuid());
        e->setPropertyName("focus");
        e->setValue(m_focus);
        notifyObservers(e);
    }
}

void KeyboardInput::keyEvent(const Q3DKeyEventPtr &event)
{
    QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
    e->setTargetNode(peerUuid());
    e->setPropertyName("event");
    e->setValue(QVariant::fromValue(event));
    notifyObservers(e);
}

void KeyboardInput::sceneChangeEvent(const QSceneChangePtr &e)
{
    bool focusRequest = false;
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("controller")) {
            const QNodeId newId = propertyChange->value().value<QNodeId>();
            if (m_keyboardController != newId) {
                setController(newId);
                focusRequest = m_focus;
            }
        } else if (propertyChange->propertyName() == QByteArrayLiteral("focus")) {
            focusRequest = propertyChange->value().toBool();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        }
    }
    if (focusRequest)
        requestFocus();
}

void KeyboardInput::requestFocus()
{
    KeyboardController *controller = m_inputHandler->keyboardControllerManager()->lookupResource(m_keyboardController);
    if (controller && m_enabled)
        controller->requestFocusForInput(peerUuid());
}

void KeyboardInput::setController(const QNodeId &controller)
{
    if (!m_keyboardController.isNull()) {
        KeyboardController *controller =  m_inputHandler->keyboardControllerManager()->lookupResource(m_keyboardController);
        if (controller)
            controller->removeKeyboardInput(peerUuid());
    }
    m_keyboardController = controller;
    if (!m_keyboardController.isNull()) {
        KeyboardController *controller =  m_inputHandler->keyboardControllerManager()->lookupResource(m_keyboardController);
        if (controller)
            controller->addKeyboardInput(peerUuid());
    }
}

KeyboardInputFunctor::KeyboardInputFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

QBackendNode *KeyboardInputFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    KeyboardInput *input = m_handler->keyboardInputManager()->getOrCreateResource(frontend->id());
    input->setFactory(factory);
    input->setInputHandler(m_handler);
    input->setPeer(frontend);
    return input;
}

QBackendNode *KeyboardInputFunctor::get(const QNodeId &id) const
{
    return m_handler->keyboardInputManager()->lookupResource(id);
}

void KeyboardInputFunctor::destroy(const QNodeId &id) const
{
    m_handler->keyboardInputManager()->releaseResource(id);
}

} // Input

} // Qt3D

QT_END_NAMESPACE
