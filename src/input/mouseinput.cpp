/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "mouseinput_p.h"
#include "mousecontroller_p.h"
#include "inputmanagers_p.h"
#include "inputhandler_p.h"

#include <Qt3DInput/qmouseinput.h>
#include <Qt3DInput/qmousecontroller.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

MouseInput::MouseInput()
    : QBackendNode(ReadWrite)
    , m_enabled(false)
    , m_inputHandler(Q_NULLPTR)
{
}

MouseInput::~MouseInput()
{
}

void MouseInput::updateFromPeer(QNode *peer)
{
    QMouseInput *input = static_cast<QMouseInput *>(peer);
    if (input->controller() != Q_NULLPTR)
        setController(input->controller()->id());
    m_enabled = input->isEnabled();
}

QNodeId MouseInput::mouseController() const
{
    return m_mouseController;
}

void MouseInput::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void MouseInput::mouseEvent(const Q3DMouseEventPtr &event)
{
    QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
    e->setTargetNode(peerUuid());
    e->setPropertyName("mouse");
    e->setValue(QVariant::fromValue(event));
    notifyObservers(e);
}

void MouseInput::wheelEvent(const Q3DWheelEventPtr &event)
{
    QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
    e->setTargetNode(peerUuid());
    e->setPropertyName("wheel");
    e->setValue(QVariant::fromValue(event));
    notifyObservers(e);
}

void MouseInput::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("controller")) {
            const QNodeId newId = propertyChange->value().value<QNodeId>();
            if (m_mouseController != newId) {
                setController(newId);
            }
        }
    }
}

void MouseInput::setController(const QNodeId &controller)
{
    if (!m_mouseController.isNull()) {
        MouseController *controller = m_inputHandler->mouseControllerManager()->lookupResource(m_mouseController);
        if (controller)
            controller->removeMouseInput(peerUuid());
    }
    m_mouseController = controller;
    if (!m_mouseController.isNull()) {
        MouseController *controller = m_inputHandler->mouseControllerManager()->lookupResource(m_mouseController);
        if (controller)
            controller->addMouseInput(peerUuid());
    }
}

MouseInputFunctor::MouseInputFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

QBackendNode *MouseInputFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    MouseInput *input = m_handler->mouseInputManager()->getOrCreateResource(frontend->id());
    input->setFactory(factory);
    input->setInputHandler(m_handler);
    input->setPeer(frontend);
    return input;
}

QBackendNode *MouseInputFunctor::get(const QNodeId &id) const
{
    return m_handler->mouseInputManager()->lookupResource(id);
}

void MouseInputFunctor::destroy(const QNodeId &id) const
{
    m_handler->mouseInputManager()->releaseResource(id);
}

} // Input

} // Qt3D

QT_END_NAMESPACE
