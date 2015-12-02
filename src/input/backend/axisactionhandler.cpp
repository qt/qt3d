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

#include "axisactionhandler_p.h"
#include <Qt3DInput/qaxisactionhandler.h>
#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

AxisActionHandler::AxisActionHandler()
    : Qt3DCore::QBackendNode(ReadWrite)
    , m_logicalDevice()
{

}

void AxisActionHandler::updateFromPeer(Qt3DCore::QNode *peer)
{
    Qt3DInput::QAxisActionHandler *handler = static_cast<Qt3DInput::QAxisActionHandler *>(peer);
    if (handler->logicalDevice())
        m_logicalDevice = handler->logicalDevice()->id();
    // TO DO: Store the state of the actions
}

void AxisActionHandler::cleanup()
{
    m_logicalDevice = Qt3DCore::QNodeId();
}

void AxisActionHandler::setAndTransmitActionPayload(const ActionStates &actionStates, const ActionPayload &deltaPayload)
{
    if (m_lastActionStates != actionStates) {
        m_lastActionStates = actionStates;

        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
        e->setTargetNode(peerUuid());
        e->setPropertyName("actionPayload");
        e->setValue(QVariant::fromValue(deltaPayload));
        notifyObservers(e);
    }
}

void AxisActionHandler::setAndTransmitAxisPayload(const AxisPayload &payload)
{
    if (m_lastAxisPayload != payload) {
        m_lastAxisPayload = payload;

        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, peerUuid()));
        e->setTargetNode(peerUuid());
        e->setPropertyName("axisPayload");
        e->setValue(QVariant::fromValue(payload));
        notifyObservers(e);
    }
}

void AxisActionHandler::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::NodeUpdated) {
        Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("logicalDevice")) {
            m_logicalDevice = propertyChange->value().value<Qt3DCore::QNodeId>();
        }
    }
}

AxisActionHandlerNodeFunctor::AxisActionHandlerNodeFunctor(AxisActionHandlerManager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *AxisActionHandlerNodeFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    HAxisActionHandler handle = m_manager->getOrAcquireHandle(frontend->id());
    AxisActionHandler *backend = m_manager->data(handle);
    m_manager->addActiveAxisActionHandler(handle);
    backend->setFactory(factory);
    backend->setPeer(frontend);
    return backend;
}

Qt3DCore::QBackendNode *AxisActionHandlerNodeFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_manager->lookupResource(id);
}

void AxisActionHandlerNodeFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    HAxisActionHandler handle = m_manager->lookupHandle(id);
    m_manager->releaseResource(id);
    m_manager->removeActiveAxisActionHandler(handle);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
