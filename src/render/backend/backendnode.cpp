// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/backendnode_p.h>
#include <private/abstractrenderer_p.h>
#include <private/resourceaccessor_p.h>
#include <private/nodemanagers_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

BackendNode::BackendNode(Mode mode)
    : QBackendNode(mode)
    , m_renderer(nullptr)
{
}

BackendNode::BackendNode(Qt3DCore::QBackendNodePrivate &dd)
    : QBackendNode(dd)
    , m_renderer(nullptr)
{
}

BackendNode::~BackendNode()
{
}

void BackendNode::setRenderer(AbstractRenderer *renderer)
{
    m_renderer = renderer;
}

AbstractRenderer *BackendNode::renderer() const
{
    return m_renderer;
}

void BackendNode::markDirty(AbstractRenderer::BackendNodeDirtySet changes)
{
    Q_ASSERT(m_renderer);
    m_renderer->markDirty(changes, this);
}

QSharedPointer<RenderBackendResourceAccessor> BackendNode::resourceAccessor() const
{
    return m_renderer->resourceAccessor();
}

void BackendNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(firstTime);

    d_ptr->setEnabled(frontEnd->isEnabled());
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
