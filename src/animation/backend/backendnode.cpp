// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "backendnode_p.h"
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qbackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

BackendNode::BackendNode(Mode mode)
    : Qt3DCore::QBackendNode(mode)
    , m_handler(nullptr)
{
}

BackendNode::~BackendNode()
{
}

void BackendNode::setHandler(Handler *handler)
{
    m_handler = handler;
}

void BackendNode::setDirty(Handler::DirtyFlag flag)
{
    Q_ASSERT(m_handler);
    m_handler->setDirty(flag, peerId());
}

void BackendNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(firstTime);

    d_ptr->setEnabled(frontEnd->isEnabled());
}

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
