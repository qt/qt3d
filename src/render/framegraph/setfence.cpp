// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "setfence_p.h"

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

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
