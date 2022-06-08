// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "nopicking_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

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
