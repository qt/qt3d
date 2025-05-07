// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial


#include <Qt3DRender/private/qdebugoverlay_p.h>
#include <Qt3DRender/private/debugoverlay_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

DebugOverlay::DebugOverlay()
    : FrameGraphNode(FrameGraphNode::DebugOverlay, QBackendNode::ReadWrite)
{

}

} //Render

} //Qt3DRender

QT_END_NAMESPACE
