// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "levelofdetail_p.h"
#include <Qt3DRender/QLevelOfDetail>
#include <Qt3DRender/private/qlevelofdetail_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/QCamera>
#include <QVariant>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

LevelOfDetail::LevelOfDetail()
    : BackendNode(BackendNode::ReadWrite)
    , m_currentIndex(0)
    , m_thresholdType(QLevelOfDetail::DistanceToCameraThreshold)
    , m_volumeOverride()
{
}

LevelOfDetail::~LevelOfDetail()
{
    cleanup();
}

void LevelOfDetail::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QLevelOfDetail *node = qobject_cast<const QLevelOfDetail *>(frontEnd);
    if (!node)
        return;

    const bool oldEnabled = isEnabled();
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    const Qt3DCore::QNodeId cameraId = Qt3DCore::qIdForNode(node->camera());
    if (cameraId != m_camera) {
        m_camera = cameraId;
        markDirty(AbstractRenderer::GeometryDirty);
    }

    if (node->currentIndex() != m_currentIndex) {
        m_currentIndex = node->currentIndex();
        markDirty(AbstractRenderer::GeometryDirty);
    }

    if (node->thresholdType() != m_thresholdType) {
        m_thresholdType = node->thresholdType();
        markDirty(AbstractRenderer::GeometryDirty);
    }

    if (node->thresholds() != m_thresholds) {
        m_thresholds = node->thresholds();
        markDirty(AbstractRenderer::GeometryDirty);
    }

    if (node->volumeOverride() != m_volumeOverride) {
        m_volumeOverride = node->volumeOverride();
        markDirty(AbstractRenderer::GeometryDirty);
    }

    if (isEnabled() != oldEnabled || firstTime)
        markDirty(AbstractRenderer::LayersDirty);
}

void LevelOfDetail::cleanup()
{
    QBackendNode::setEnabled(false);
}

void LevelOfDetail::setCurrentIndex(int currentIndex)
{
    if (m_currentIndex != currentIndex)
        m_currentIndex = currentIndex;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
