// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "backendnode_p.h"
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qbackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

BackendNode::BackendNode(Mode mode)
    : Qt3DCore::QBackendNode(mode)
{
}

BackendNode::BackendNode(Qt3DCore::QBackendNodePrivate &dd)
    : Qt3DCore::QBackendNode(dd)
{

}

BackendNode::~BackendNode()
{
}

void BackendNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(firstTime);

    d_ptr->setEnabled(frontEnd->isEnabled());
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
