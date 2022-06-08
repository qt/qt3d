// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderpass_p.h"
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qparameter.h>

#include <Qt3DRender/private/qrenderpass_p.h>
#include <Qt3DRender/private/renderstates_p.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RenderPass::RenderPass()
    : BackendNode()
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::cleanup()
{
    QBackendNode::setEnabled(false);
    m_renderStates.clear();
    m_filterKeyList.clear();
    m_parameterPack.clear();
    m_shaderUuid = Qt3DCore::QNodeId();
}

void RenderPass::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QRenderPass *node = qobject_cast<const QRenderPass *>(frontEnd);
    if (!node)
        return;

    if ((node->shaderProgram() && node->shaderProgram()->id() != m_shaderUuid) ||
        (!node->shaderProgram() && !m_shaderUuid.isNull())) {
        m_shaderUuid = node->shaderProgram() ? node->shaderProgram()->id() : QNodeId{};
    }

    auto filterList = qIdsForNodes(node->filterKeys());
    std::sort(std::begin(filterList), std::end(filterList));
    if (m_filterKeyList != filterList)
        m_filterKeyList = filterList;

    auto parameters = qIdsForNodes(node->parameters());
    std::sort(std::begin(parameters), std::end(parameters));
    if (m_parameterPack.parameters() != parameters)
        m_parameterPack.setParameters(parameters);

    auto renderStates = qIdsForNodes(node->renderStates());
    std::sort(std::begin(renderStates), std::end(renderStates));
    if (m_renderStates != renderStates)
        m_renderStates = renderStates;

    markDirty(AbstractRenderer::AllDirty);
}

Qt3DCore::QNodeId RenderPass::shaderProgram() const
{
    return m_shaderUuid;
}

QList<Qt3DCore::QNodeId> RenderPass::filterKeys() const
{
    return m_filterKeyList;
}

QList<Qt3DCore::QNodeId> RenderPass::parameters() const
{
    return m_parameterPack.parameters();
}

QList<QNodeId> RenderPass::renderStates() const
{
    return m_renderStates;
}

void RenderPass::appendFilterKey(Qt3DCore::QNodeId filterKeyId)
{
    if (!m_filterKeyList.contains(filterKeyId))
        m_filterKeyList.append(filterKeyId);
}

void RenderPass::removeFilterKey(Qt3DCore::QNodeId filterKeyId)
{
    m_filterKeyList.removeOne(filterKeyId);
}

void RenderPass::addRenderState(QNodeId renderStateId)
{
    if (!m_renderStates.contains(renderStateId))
        m_renderStates.push_back(renderStateId);
}

void RenderPass::removeRenderState(QNodeId renderStateId)
{
    m_renderStates.removeOne(renderStateId);
}


} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
