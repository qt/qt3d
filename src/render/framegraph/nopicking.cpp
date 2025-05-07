// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "nopicking_p.h"

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

NoPicking::NoPicking()
    : FrameGraphNode(FrameGraphNode::NoPicking)
{
}

NoPicking::~NoPicking()
{
}

} // Render
} // Qt3DRender

QT_END_NAMESPACE
