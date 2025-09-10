// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "rhiresourcemanagers_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

RHIResourceManagers::RHIResourceManagers()
    : m_rhiBufferManager(new RHIBufferManager()),
      m_rhiShaderManager(new RHIShaderManager()),
      m_rhiTextureManager(new RHITextureManager()),
      m_rhiRenderTargetManager(new RHIRenderTargetManager()),
      m_rhiGraphicsPipelineManager(new RHIGraphicsPipelineManager()),
      m_rhiComputePipelineManager(new RHIComputePipelineManager())
{
}

RHIResourceManagers::~RHIResourceManagers()
{
    delete m_rhiTextureManager;
    delete m_rhiShaderManager;
    delete m_rhiBufferManager;
    delete m_rhiRenderTargetManager;
    delete m_rhiGraphicsPipelineManager;
    delete m_rhiComputePipelineManager;
}

void RHIResourceManagers::releaseAllResources()
{
    auto releaseAll = [](auto *manager) noexcept {
        // Release all resources reference by a key
        manager->releaseAllResources();
        // Release remaining resources which were allocated manually but had no key
        const auto handles = manager->activeHandles();
        for (const auto &handle : handles) {
            manager->release(handle);
        }
    };

    releaseAll(m_rhiTextureManager);
    releaseAll(m_rhiBufferManager);
    qDeleteAll(m_rhiShaderManager->takeActiveResources());
    releaseAll(m_rhiRenderTargetManager);
    releaseAll(m_rhiGraphicsPipelineManager);
    releaseAll(m_rhiComputePipelineManager);
}

int RHIGraphicsPipelineManager::getIdForAttributeVec(const std::vector<AttributeInfo> &attributesInfo)
{
    auto it = std::find(m_attributesInfo.begin(),
                        m_attributesInfo.end(),
                        attributesInfo);
    if (it == m_attributesInfo.end()) {
        m_attributesInfo.emplace_back(attributesInfo);
        return int(m_attributesInfo.size()) - 1;
    }
    return std::distance(m_attributesInfo.begin(), it);
}

int RHIGraphicsPipelineManager::getIdForRenderStates(const RenderStateSetPtr &stateSet)
{
    if (!stateSet)
        return -1;
    const std::vector<StateVariant> &states = stateSet->states();
    auto it = std::find(m_renderStates.begin(),
                        m_renderStates.end(),
                        states);
    if (it == m_renderStates.end()) {
        m_renderStates.emplace_back(states);
        return int(m_renderStates.size()) - 1;
    }
    return std::distance(m_renderStates.begin(), it);
}

namespace {

template<typename Manager>
void erasePipelinesReferencingShader(Manager *manager, const Qt3DCore::QNodeId &shaderId)
{
    const auto handles = manager->activeHandles(); // copy
    for (const auto &handle : handles) {
        const auto &key = handle->key();
        if (key.shader == shaderId)
            manager->releaseResource(key);
    }
}

template<typename Manager>
void erasePipelinesReferencingRenderTargetId(Manager *manager, const Qt3DCore::QNodeId &renderTargetId)
{
    const auto handles = manager->activeHandles(); // copy
    for (const auto &handle : handles) {
        const auto &key = handle->key();
        if (key.renderTarget == renderTargetId)
            manager->releaseResource(key);
    }
}

} // anonymous

void RHIGraphicsPipelineManager::releasePipelinesReferencingShader(const Qt3DCore::QNodeId &shaderId)
{
    erasePipelinesReferencingShader(this, shaderId);
}

void RHIGraphicsPipelineManager::releasePipelinesReferencingRenderTarget(const Qt3DCore::QNodeId &renderTargetId)
{
    erasePipelinesReferencingRenderTargetId(this, renderTargetId);
}

void RHIComputePipelineManager::releasePipelinesReferencingShader(const Qt3DCore::QNodeId &shaderId)
{
    erasePipelinesReferencingShader(this, shaderId);
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
