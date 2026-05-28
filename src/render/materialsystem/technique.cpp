// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "technique_p.h"

#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/private/qtechnique_p.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Technique::Technique()
    : BackendNode()
    , m_isCompatibleWithRenderer(false)
    , m_nodeManager(nullptr)
{
}

Technique::~Technique()
{
    cleanup();
}

void Technique::cleanup()
{
    QBackendNode::setEnabled(false);
    m_parameterPack.clear();
    m_renderPasses.clear();
    m_filterKeyList.clear();
    m_isCompatibleWithRenderer = false;
}

void Technique::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QTechnique *node = qobject_cast<const QTechnique *>(frontEnd);

    if (!node)
        return;

    bool dirty = isEnabled() != frontEnd->isEnabled();
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    auto renderPasses = qIdsForNodes(node->renderPasses());
    std::sort(std::begin(renderPasses), std::end(renderPasses));
    if (m_renderPasses != renderPasses) {
        m_renderPasses = renderPasses;
        dirty = true;
    }

    auto parameters = qIdsForNodes(node->parameters());
    std::sort(std::begin(parameters), std::end(parameters));
    if (m_parameterPack.parameters() != parameters) {
        m_parameterPack.setParameters(parameters);
        dirty = true;
    }

    auto filterKeys = qIdsForNodes(node->filterKeys());
    std::sort(std::begin(filterKeys), std::end(filterKeys));
    if (m_filterKeyList != filterKeys) {
        m_filterKeyList = filterKeys;
        dirty = true;
    }

    auto graphicsApiFilterData = QGraphicsApiFilterPrivate::get(node->graphicsApiFilter())->m_data;
    if (m_graphicsApiFilterData != graphicsApiFilterData) {
        m_graphicsApiFilterData = graphicsApiFilterData;
        m_isCompatibleWithRenderer = false;
        dirty = true;
    }

    if (dirty) {
        m_nodeManager->techniqueManager()->addDirtyTechnique(peerId());
        markDirty(AbstractRenderer::TechniquesDirty);
    }
}

QList<Qt3DCore::QNodeId> Technique::parameters() const
{
    return m_parameterPack.parameters();
}

void Technique::appendRenderPass(Qt3DCore::QNodeId renderPassId)
{
    if (!m_renderPasses.contains(renderPassId))
        m_renderPasses.append(renderPassId);
}

void Technique::removeRenderPass(Qt3DCore::QNodeId renderPassId)
{
    m_renderPasses.removeOne(renderPassId);
}

QList<Qt3DCore::QNodeId> Technique::filterKeys() const
{
    return m_filterKeyList;
}

QList<Qt3DCore::QNodeId> Technique::renderPasses() const
{
    return m_renderPasses;
}

const GraphicsApiFilterData *Technique::graphicsApiFilter() const
{
    return &m_graphicsApiFilterData;
}

bool Technique::isCompatibleWithRenderer() const
{
    return m_isCompatibleWithRenderer;
}

void Technique::setCompatibleWithRenderer(bool compatible)
{
    m_isCompatibleWithRenderer = compatible;
}

bool Technique::isCompatibleWithFilters(const QNodeIdVector &filterKeyIds)
{
    // There is a technique filter so we need to check for a technique with suitable criteria.
    // Check for early bail out if the technique doesn't have sufficient number of criteria and
    // can therefore never satisfy the filter
    if (m_filterKeyList.size() < filterKeyIds.size())
        return false;

    // Iterate through the filter criteria and for each one search for a criteria on the
    // technique that satisfies it
    for (const QNodeId &filterKeyId : filterKeyIds) {
        FilterKey *filterKey = m_nodeManager->filterKeyManager()->lookupResource(filterKeyId);

        bool foundMatch = false;

        for (const QNodeId &techniqueFilterKeyId : std::as_const(m_filterKeyList)) {
            FilterKey *techniqueFilterKey = m_nodeManager->filterKeyManager()->lookupResource(techniqueFilterKeyId);
            if ((foundMatch = (*techniqueFilterKey == *filterKey)))
                break;
        }

        // No match for TechniqueFilter criterion in any of the technique's criteria.
        // So no way this can match. Don't bother checking the rest of the criteria.
        if (!foundMatch)
            return false;
    }
    return true;
}

void Technique::setNodeManager(NodeManagers *nodeManager)
{
    m_nodeManager = nodeManager;
}

NodeManagers *Technique::nodeManager() const
{
    return m_nodeManager;
}

void Technique::appendFilterKey(Qt3DCore::QNodeId criterionId)
{
    if (!m_filterKeyList.contains(criterionId))
        m_filterKeyList.append(criterionId);
}

void Technique::removeFilterKey(Qt3DCore::QNodeId criterionId)
{
    m_filterKeyList.removeOne(criterionId);
}

TechniqueFunctor::TechniqueFunctor(AbstractRenderer *renderer, NodeManagers *manager)
    : m_manager(manager)
    , m_renderer(renderer)
{
}

QBackendNode *TechniqueFunctor::create(Qt3DCore::QNodeId id) const
{
    Technique *technique = m_manager->techniqueManager()->getOrCreateResource(id);
    technique->setNodeManager(m_manager);
    technique->setRenderer(m_renderer);
    return technique;
}

QBackendNode *TechniqueFunctor::get(QNodeId id) const
{
    return m_manager->techniqueManager()->lookupResource(id);
}

void TechniqueFunctor::destroy(QNodeId id) const
{
    m_manager->techniqueManager()->releaseResource(id);

}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
