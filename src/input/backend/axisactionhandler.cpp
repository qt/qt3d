/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

Qt3DCore::QBackendNode *AxisActionHandlerNodeFunctor::create(Qt3DCore::QNode *frontend) const
{
    HAxisActionHandler handle = m_manager->getOrAcquireHandle(frontend->id());
    AxisActionHandler *backend = m_manager->data(handle);
    m_manager->addActiveAxisActionHandler(handle);
    backend->setPeer(frontend);
    return backend;
}

Qt3DCore::QBackendNode *AxisActionHandlerNodeFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void AxisActionHandlerNodeFunctor::destroy(Qt3DCore::QNodeId id) const
{
    HAxisActionHandler handle = m_manager->lookupHandle(id);
    m_manager->releaseResource(id);
    m_manager->removeActiveAxisActionHandler(handle);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
