/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    // releaseAll(m_rhiShaderManager);
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
