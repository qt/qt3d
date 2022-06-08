// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "cameraselectornode_p.h"
#include <Qt3DRender/private/qcameraselector_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

CameraSelector::CameraSelector()
    : FrameGraphNode(FrameGraphNode::CameraSelector)
{
}

void CameraSelector::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QCameraSelector *node = qobject_cast<const QCameraSelector *>(frontEnd);
    if (!node)
        return;

    FrameGraphNode::syncFromFrontEnd(frontEnd, firstTime);

    const QNodeId cameraId = qIdForNode(node->camera());
    if (m_cameraUuid != cameraId) {
        m_cameraUuid = cameraId;
        markDirty(AbstractRenderer::FrameGraphDirty);
    }
}

QNodeId CameraSelector::cameraUuid() const
{
    return m_cameraUuid;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
