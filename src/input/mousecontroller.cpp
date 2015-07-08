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

namespace Qt3D {

namespace Input {

MouseController::MouseController()
    : QBackendNode()
{
}

MouseController::~MouseController()
{
}

void MouseController::updateFromPeer(QNode *peer)
{
    Q_UNUSED(peer);
}

void MouseController::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void MouseController::addMouseInput(const QNodeId &input)
{
    if (!m_mouseInputs.contains(input))
        m_mouseInputs.append(input);
}

void MouseController::removeMouseInput(const QNodeId &input)
{
    m_mouseInputs.removeOne(input);
}

QVector<QNodeId> MouseController::mouseInputs() const
{
    return m_mouseInputs;
}

void MouseController::sceneChangeEvent(const QSceneChangePtr &e)
{
    Q_UNUSED(e);
}

MouseControllerFunctor::MouseControllerFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

QBackendNode *MouseControllerFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    MouseController *controller = m_handler->mouseControllerManager()->getOrCreateResource(frontend->id());
    controller->setFactory(factory);
    controller->setInputHandler(m_handler);
    controller->setPeer(frontend);
    m_handler->appendMouseController(m_handler->mouseControllerManager()->lookupHandle(frontend->id()));
    return controller;
}

QBackendNode *MouseControllerFunctor::get(const QNodeId &id) const
{
    return m_handler->mouseControllerManager()->lookupResource(id);
}

void MouseControllerFunctor::destroy(const QNodeId &id) const
{
    m_handler->removeMouseController(m_handler->mouseControllerManager()->lookupHandle(id));
    m_handler->mouseControllerManager()->releaseResource(id);
}

} // Input

} // Qt3D

QT_END_NAMESPACE
