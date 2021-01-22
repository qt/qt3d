/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "light_p.h"
#include "qabstractlight.h"
#include "qabstractlight_p.h"
#include <private/abstractrenderer_p.h>
#include <private/nodemanagers_p.h>
#include <private/qbackendnode_p.h>
#include <private/managers_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

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

Qt3DCore::QBackendNode *RenderLightFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Light *backend = m_managers->lightManager()->getOrCreateResource(change->subjectId());
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
