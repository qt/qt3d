/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "setfence_p.h"
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

SetFence::SetFence()
    : FrameGraphNode(FrameGraphNode::SetFence, QBackendNode::ReadWrite)
{
}

SetFence::~SetFence()
{
}

void SetFence::setHandle(const QVariant &handle)
{
    auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    change->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
    change->setPropertyName("handle");
    change->setValue(handle);
    notifyObservers(change);
}

void SetFence::setHandleType(QSetFence::HandleType type)
{
    auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    change->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
    change->setPropertyName("handleType");
    change->setValue(QVariant::fromValue(type));
    notifyObservers(change);
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
