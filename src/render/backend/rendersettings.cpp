// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "rendersettings_p.h"

#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/qrendersettings_p.h>
#include <Qt3DRender/private/qrendercapabilities_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

RenderSettings::RenderSettings()
    : BackendNode()
    , m_renderPolicy(QRenderSettings::OnDemand)
    , m_pickMethod(QPickingSettings::BoundingVolumePicking)
    , m_pickResultMode(QPickingSettings::NearestPick)
    , m_faceOrientationPickingMode(QPickingSettings::FrontFace)
    , m_pickWorldSpaceTolerance(.1f)
    , m_activeFrameGraph()
{
}

void RenderSettings::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QRenderSettings *node = qobject_cast<const QRenderSettings *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    const Qt3DCore::QNodeId activeFGId = Qt3DCore::qIdForNode(node->activeFrameGraph());
    if (activeFGId != m_activeFrameGraph) {
        m_activeFrameGraph = activeFGId;
    }

    if (node->renderPolicy() != m_renderPolicy) {
        m_renderPolicy = node->renderPolicy();
    }

    auto ncnode = const_cast<QRenderSettings *>(node);
    if (ncnode->pickingSettings()->pickMethod() != m_pickMethod) {
        m_pickMethod = ncnode->pickingSettings()->pickMethod();
    }

    if (ncnode->pickingSettings()->pickResultMode() != m_pickResultMode) {
        m_pickResultMode = ncnode->pickingSettings()->pickResultMode();
    }

    if (!qFuzzyCompare(ncnode->pickingSettings()->worldSpaceTolerance(), m_pickWorldSpaceTolerance)) {
        m_pickWorldSpaceTolerance = ncnode->pickingSettings()->worldSpaceTolerance();
    }

    if (ncnode->pickingSettings()->faceOrientationPickingMode() != m_faceOrientationPickingMode) {
        m_faceOrientationPickingMode = ncnode->pickingSettings()->faceOrientationPickingMode();
    }

    if (firstTime)
        m_capabilities = QRenderCapabilitiesPrivate::get(const_cast<QRenderSettings *>(node)->renderCapabilities())->toString();

    // Either because something above as changed or if QRenderSettingsPrivate::invalidFrame()
    // was called
    markDirty(AbstractRenderer::AllDirty);
}

RenderSettingsFunctor::RenderSettingsFunctor(AbstractRenderer *renderer)
    : m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderSettingsFunctor::create(Qt3DCore::QNodeId) const
{
    if (m_renderer->settings() != nullptr) {
        qWarning() << "Renderer settings already exists";
        return nullptr;
    }

    RenderSettings *settings = new RenderSettings;
    settings->setRenderer(m_renderer);
    m_renderer->setSettings(settings);
    return settings;
}

Qt3DCore::QBackendNode *RenderSettingsFunctor::get(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    return m_renderer->settings();
}

void RenderSettingsFunctor::destroy(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    // Deletes the old settings object
    auto settings = m_renderer->settings();
    if (settings && settings->peerId() == id) {
        m_renderer->setSettings(nullptr);
        delete settings;
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
