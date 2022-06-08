// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/private/qrendertarget_p.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/private/managers_p.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RenderTarget::RenderTarget()
    : BackendNode()
    , m_dirty(false)
{
}

void RenderTarget::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QRenderTarget *node = qobject_cast<const QRenderTarget *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    auto outputIds = qIdsForNodes(node->outputs());
    std::sort(std::begin(outputIds), std::end(outputIds));

    if (m_renderOutputs != outputIds) {
        m_renderOutputs = outputIds;
        m_dirty = true;
        markDirty(AbstractRenderer::AllDirty);
    }
}

void RenderTarget::cleanup()
{
    m_renderOutputs.clear();
    m_dirty = false;
    QBackendNode::setEnabled(false);
}

QList<Qt3DCore::QNodeId> RenderTarget::renderOutputs() const
{
    return m_renderOutputs;
}

bool RenderTarget::isDirty() const
{
    return m_dirty;
}

void RenderTarget::unsetDirty()
{
    m_dirty = false;
}

RenderTargetFunctor::RenderTargetFunctor(AbstractRenderer *renderer, RenderTargetManager *manager)
    : m_renderer(renderer)
    , m_renderTargetManager(manager)
{
}

QBackendNode *RenderTargetFunctor::create(QNodeId id) const
{
    RenderTarget *backend = m_renderTargetManager->getOrCreateResource(id);
    // Remove from the list of ids to destroy in case we were added to it
    m_renderTargetManager->removeRenderTargetToCleanup(id);
    backend->setRenderer(m_renderer);
    return backend;
}

QBackendNode *RenderTargetFunctor::get(QNodeId id) const
{
    return m_renderTargetManager->lookupResource(id);
}

void RenderTargetFunctor::destroy(QNodeId id) const
{
    // We add ourselves to the dirty list to tell the renderer that this rendertarget has been destroyed
    m_renderTargetManager->addRenderTargetIdToCleanup(id);
    m_renderTargetManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
