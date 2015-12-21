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

#include "mousecontroller_p.h"
#include "inputmanagers_p.h"
#include "inputhandler_p.h"
#include "qmousecontroller.h"

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

MouseController::MouseController()
    : QAbstractPhysicalDeviceBackendNode(ReadOnly)
    , m_sensitivity(0.1f)
{
}

MouseController::~MouseController()
{
}

void MouseController::updateFromPeer(Qt3DCore::QNode *peer)
{
    QAbstractPhysicalDeviceBackendNode::updateFromPeer(peer);
    QMouseController *object = static_cast<QMouseController *>(peer);
    m_sensitivity = object->sensitivity();
}

void MouseController::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void MouseController::addMouseInput(const Qt3DCore::QNodeId &input)
{
    if (!m_mouseInputs.contains(input))
        m_mouseInputs.append(input);
}

void MouseController::removeMouseInput(const Qt3DCore::QNodeId &input)
{
    m_mouseInputs.removeOne(input);
}

float MouseController::axisValue(int axisIdentifier) const
{
    switch (axisIdentifier) {
    case QMouseController::X:
        return m_mouseState.xAxis;
    case QMouseController::Y:
        return m_mouseState.yAxis;
        break;
    default:
        break;
    }
    return 0.0f;
}

bool MouseController::isButtonPressed(int buttonIdentifier) const
{
    switch (buttonIdentifier) {
    case QMouseController::Left:
        return m_mouseState.leftPressed;
    case QMouseController::Center:
        return m_mouseState.centerPressed;
    case QMouseController::Right:
        return m_mouseState.rightPressed;
    default:
        break;
    }
    return false;
}

QVector<Qt3DCore::QNodeId> MouseController::mouseInputs() const
{
    return m_mouseInputs;
}

void MouseController::updateMouseEvents(const QList<QT_PREPEND_NAMESPACE(QMouseEvent)> &events)
{
    if (!events.isEmpty()) {
        Q_FOREACH (const QT_PREPEND_NAMESPACE(QMouseEvent) &e, events) {
            m_mouseState.leftPressed = e.buttons() & (Qt::LeftButton);
            m_mouseState.centerPressed = e.buttons() & (Qt::MiddleButton);
            m_mouseState.rightPressed = e.buttons() & (Qt::RightButton);
            m_mouseState.xAxis =  m_sensitivity * (e.screenPos().x() - m_previousPos.x());
            m_mouseState.yAxis = m_sensitivity * (m_previousPos.y() - e.screenPos().y());
            m_previousPos = e.screenPos();
        }
    } else {
        m_mouseState = MouseState();
    }
}

void MouseController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::NodeUpdated) {
        Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("sensitivity"))
            m_sensitivity = propertyChange->value().toFloat();
    }
}

MouseControllerFunctor::MouseControllerFunctor(QInputAspect *inputAspect, InputHandler *handler)
    : m_inputAspect(inputAspect)
    , m_handler(handler)
{
}

Qt3DCore::QBackendNode *MouseControllerFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    MouseController *controller = m_handler->mouseControllerManager()->getOrCreateResource(frontend->id());
    controller->setFactory(factory);
    controller->setInputAspect(m_inputAspect);
    controller->setInputHandler(m_handler);
    controller->setPeer(frontend);
    m_handler->appendMouseController(m_handler->mouseControllerManager()->lookupHandle(frontend->id()));
    return controller;
}

Qt3DCore::QBackendNode *MouseControllerFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_handler->mouseControllerManager()->lookupResource(id);
}

void MouseControllerFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    m_handler->removeMouseController(m_handler->mouseControllerManager()->lookupHandle(id));
    m_handler->mouseControllerManager()->releaseResource(id);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
