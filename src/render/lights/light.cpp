// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "light_p.h"
#include "qabstractlight.h"
#include "qabstractlight_p.h"
#include <private/abstractrenderer_p.h>
#include <private/nodemanagers_p.h>
#include <private/qbackendnode_p.h>
#include <private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

QNodeId Light::shaderData() const
{
    return m_shaderDataId;
}

void Light::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAbstractLight *node = qobject_cast<const QAbstractLight *>(frontEnd);
    if (!node)
        return;

    if (firstTime) {
        QAbstractLightPrivate *d = static_cast<QAbstractLightPrivate *>(QAbstractLightPrivate::get(const_cast<Qt3DCore::QNode *>(frontEnd)));
        m_shaderDataId = d->m_shaderData ? d->m_shaderData->id() : QNodeId{};
        BackendNode::markDirty(AbstractRenderer::LightsDirty);
    }
}

RenderLightFunctor::RenderLightFunctor(AbstractRenderer *renderer, NodeManagers *managers)
    : m_managers(managers)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderLightFunctor::create(Qt3DCore::QNodeId id) const
{
    Light *backend = m_managers->lightManager()->getOrCreateResource(id);
    backend->setRenderer(m_renderer);
    return backend;
}

Qt3DCore::QBackendNode *RenderLightFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_managers->lightManager()->lookupResource(id);
}

void RenderLightFunctor::destroy(Qt3DCore::QNodeId id) const
{
    Light *backend = m_managers->lightManager()->getOrCreateResource(id);
    m_renderer->markDirty(AbstractRenderer::LightsDirty, backend);
    m_managers->lightManager()->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
