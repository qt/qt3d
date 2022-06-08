// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "mousehandler_p.h"

#include <Qt3DInput/qmousehandler.h>
#include <Qt3DInput/qmousedevice.h>

#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/mousedevice_p.h>
#include <Qt3DInput/private/qmousehandler_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
namespace Input {

MouseHandler::MouseHandler()
    : BackendNode(ReadWrite)
    , m_inputHandler(nullptr)
{
}

MouseHandler::~MouseHandler()
{
}

Qt3DCore::QNodeId MouseHandler::mouseDevice() const
{
    return m_mouseDevice;
}

void MouseHandler::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void MouseHandler::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DInput::QMouseHandler *node = qobject_cast<const Qt3DInput::QMouseHandler *>(frontEnd);
    if (!node)
        return;

    const auto newId = Qt3DCore::qIdForNode(node->sourceDevice());
    if (m_mouseDevice != newId)
        setDevice(newId);
}

void MouseHandler::setDevice(Qt3DCore::QNodeId device)
{
    m_mouseDevice = device;
}

MouseHandlerFunctor::MouseHandlerFunctor(InputHandler *handler)
    : m_handler(handler)
{
}

Qt3DCore::QBackendNode *MouseHandlerFunctor::create(Qt3DCore::QNodeId id) const
{
    MouseHandler *input = m_handler->mouseInputManager()->getOrCreateResource(id);
    input->setInputHandler(m_handler);
    return input;
}

Qt3DCore::QBackendNode *MouseHandlerFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_handler->mouseInputManager()->lookupResource(id);
}

void MouseHandlerFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_handler->mouseInputManager()->releaseResource(id);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
