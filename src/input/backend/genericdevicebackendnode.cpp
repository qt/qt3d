/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "genericdevicebackendnode_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/qgenericinputdevice_p.h>
#include <Qt3DInput/private/qabstractphysicaldevice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

GenericDeviceBackendNode::GenericDeviceBackendNode(QBackendNode::Mode mode)
    : QAbstractPhysicalDeviceBackendNode(mode)
    , m_mutex()
{
}

GenericDeviceBackendNode::~GenericDeviceBackendNode()
{
}

void GenericDeviceBackendNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    QAbstractPhysicalDeviceBackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DInput::QGenericInputDevice *node = qobject_cast<const Qt3DInput::QGenericInputDevice *>(frontEnd);
    if (!node)
        return;

    auto *d = static_cast<Qt3DInput::QAbstractPhysicalDevicePrivate *>( Qt3DCore::QNodePrivate::get(const_cast<Qt3DCore::QNode *>(frontEnd)) );

    {
        const QMutexLocker lock(&m_mutex);
        for (const auto &val: qAsConst(d->m_pendingAxisEvents))
            m_axesValues[val.first] = val.second;
        for (const auto &val: qAsConst(d->m_pendingButtonsEvents))
            m_buttonsValues[val.first] = val.second;

        d->m_pendingAxisEvents.clear();
        d->m_pendingButtonsEvents.clear();
    }
}

void GenericDeviceBackendNode::cleanup()
{
    const QMutexLocker lock(&m_mutex);
    m_axesValues.clear();
    m_buttonsValues.clear();
    QAbstractPhysicalDeviceBackendNode::cleanup();
}

float GenericDeviceBackendNode::axisValue(int axisIdentifier) const
{
    const QMutexLocker lock(&m_mutex);
    return m_axesValues[axisIdentifier];
}

bool GenericDeviceBackendNode::isButtonPressed(int buttonIdentifier) const
{
    const QMutexLocker lock(&m_mutex);
    return m_buttonsValues[buttonIdentifier];
}

GenericDeviceBackendFunctor::GenericDeviceBackendFunctor(QInputAspect *inputaspect, InputHandler *handler)
    : m_inputAspect(inputaspect)
    , m_handler(handler)
{
}

Qt3DCore::QBackendNode *GenericDeviceBackendFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    GenericDeviceBackendNode *backendNode = m_handler->genericDeviceBackendNodeManager()->getOrCreateResource(change->subjectId());
    backendNode->setInputAspect(m_inputAspect);
    m_handler->appendGenericDevice(m_handler->genericDeviceBackendNodeManager()->lookupHandle(change->subjectId()));
    return backendNode;
}

Qt3DCore::QBackendNode *GenericDeviceBackendFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_handler->genericDeviceBackendNodeManager()->lookupResource(id);
}

void GenericDeviceBackendFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_handler->removeGenericDevice(m_handler->genericDeviceBackendNodeManager()->lookupHandle(id));
    m_handler->genericDeviceBackendNodeManager()->releaseResource(id);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
