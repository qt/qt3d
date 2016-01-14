/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "genericdevicebackendnode_p.h"
#include "inputhandler_p.h"
#include "inputmanagers_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

GenericDeviceBackendNode::GenericDeviceBackendNode(QBackendNode::Mode mode)
    : QAbstractPhysicalDeviceBackendNode(mode)
    , m_mutex(new QMutex)
{
}

GenericDeviceBackendNode::~GenericDeviceBackendNode()
{
}

void GenericDeviceBackendNode::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
    if (e->type() == Qt3DCore::NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("axisEvent")) {
            QPair<int, qreal> val = propertyChange->value().value<QPair<int, qreal>>();
            QMutexLocker lock(m_mutex.data());
            m_axesValues[val.first] = val.second;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("buttonEvent")) {
            QPair<int, qreal> val = propertyChange->value().value<QPair<int, qreal>>();
            QMutexLocker lock(m_mutex.data());
            m_buttonsValues[val.first] = val.second;
        }
    }
}

void GenericDeviceBackendNode::cleanup()
{
    QMutexLocker lock(m_mutex.data());
    m_axesValues.clear();
    m_buttonsValues.clear();
    QAbstractPhysicalDeviceBackendNode::cleanup();
}

float GenericDeviceBackendNode::axisValue(int axisIdentifier) const
{
    QMutexLocker lock(m_mutex.data());
    return m_axesValues[axisIdentifier];
}

bool GenericDeviceBackendNode::isButtonPressed(int buttonIdentifier) const
{
    QMutexLocker lock(m_mutex.data());
    return m_buttonsValues[buttonIdentifier];
}

GenericDeviceBackendFunctor::GenericDeviceBackendFunctor(QInputAspect *inputaspect, InputHandler *handler)
    : m_inputAspect(inputaspect)
    , m_handler(handler)
{
}

Qt3DCore::QBackendNode *GenericDeviceBackendFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    GenericDeviceBackendNode *backendNode = m_handler->genericDeviceBackendNodeManager()->getOrCreateResource(frontend->id());
    backendNode->setFactory(factory);
    backendNode->setInputAspect(m_inputAspect);
    backendNode->setPeer(frontend);
    m_handler->appendGenericDevice(m_handler->genericDeviceBackendNodeManager()->lookupHandle(frontend->id()));
    return backendNode;
}

Qt3DCore::QBackendNode *GenericDeviceBackendFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_handler->genericDeviceBackendNodeManager()->lookupResource(id);
}

void GenericDeviceBackendFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    m_handler->removeGenericDevice(m_handler->genericDeviceBackendNodeManager()->lookupHandle(id));
    m_handler->genericDeviceBackendNodeManager()->releaseResource(id);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
