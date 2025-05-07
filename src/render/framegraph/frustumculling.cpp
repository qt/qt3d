// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "frustumculling_p.h"
#include <Qt3DRender/qfrustumculling.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

FrustumCulling::FrustumCulling()
    : FrameGraphNode(FrameGraphNode::FrustumCulling)
{
}

FrustumCulling::~FrustumCulling()
{
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
