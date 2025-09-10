// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RHIRESOURCEMANAGERS_P_H
#define QT3DRENDER_RENDER_RHI_RHIRESOURCEMANAGERS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <texture_p.h>
#include <rhibuffer_p.h>
#include <rhishader_p.h>
#include <rhigraphicspipeline_p.h>
#include <rhirendertarget_p.h>
#include <rendercommand_p.h>
#include <Qt3DRender/private/apishadermanager_p.h>
#include <Qt3DRender/private/renderstateset_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class Q_AUTOTEST_EXPORT RHIBufferManager
    : public Qt3DCore::QResourceManager<RHIBuffer, Qt3DCore::QNodeId, Qt3DCore::NonLockingPolicy>
{
};

class Q_AUTOTEST_EXPORT RHITextureManager
    : public Qt3DCore::QResourceManager<RHITexture, Qt3DCore::QNodeId, Qt3DCore::NonLockingPolicy>
{
public:
    QHash<RHITexture *, Qt3DCore::QNodeId> texNodeIdForRHITexture;
};

class Q_AUTOTEST_EXPORT RHIRenderTargetManager
        : public Qt3DCore::QResourceManager<RHIRenderTarget, Qt3DCore::QNodeId, Qt3DCore::NonLockingPolicy>
{
};

class Q_AUTOTEST_EXPORT RHIShaderManager : public APIShaderManager<RHIShader>
{
public:
    explicit RHIShaderManager() : APIShaderManager<RHIShader>() { }
};

class Q_AUTOTEST_EXPORT RHIGraphicsPipelineManager
    : public Qt3DCore::QResourceManager<RHIGraphicsPipeline, GraphicsPipelineIdentifier,
                                        Qt3DCore::NonLockingPolicy>
{
public:
    RHIGraphicsPipelineManager() { }

    int getIdForAttributeVec(const std::vector<AttributeInfo> &attributesInfo);
    int getIdForRenderStates(const RenderStateSetPtr &stateSet);

    void releasePipelinesReferencingShader(const Qt3DCore::QNodeId &shaderId);
    void releasePipelinesReferencingRenderTarget(const Qt3DCore::QNodeId &renderTargetId);

private:
    using AttributeInfoVec= std::vector<AttributeInfo>;
    std::vector<AttributeInfoVec> m_attributesInfo;
    std::vector<std::vector<StateVariant>> m_renderStates;
};

class Q_AUTOTEST_EXPORT RHIComputePipelineManager
        : public Qt3DCore::QResourceManager<RHIComputePipeline, ComputePipelineIdentifier,
        Qt3DCore::NonLockingPolicy>
{
public:
    RHIComputePipelineManager() { }

    void releasePipelinesReferencingShader(const Qt3DCore::QNodeId &shaderId);
};

class Q_AUTOTEST_EXPORT RHIResourceManagers
{
public:
    RHIResourceManagers();
    ~RHIResourceManagers();

    inline RHIShaderManager *rhiShaderManager() const noexcept { return m_rhiShaderManager; }
    inline RHITextureManager *rhiTextureManager() const noexcept { return m_rhiTextureManager; }
    inline RHIBufferManager *rhiBufferManager() const noexcept { return m_rhiBufferManager; }
    inline RHIRenderTargetManager *rhiRenderTargetManager() const noexcept { return m_rhiRenderTargetManager; }
    inline RHIGraphicsPipelineManager *rhiGraphicsPipelineManager() const noexcept
    {
        return m_rhiGraphicsPipelineManager;
    }
    inline RHIComputePipelineManager *rhiComputePipelineManager() const noexcept
    {
        return m_rhiComputePipelineManager;
    }

    void releaseAllResources();

private:
    RHIBufferManager *m_rhiBufferManager;
    RHIShaderManager *m_rhiShaderManager;
    RHITextureManager *m_rhiTextureManager;
    RHIRenderTargetManager *m_rhiRenderTargetManager;
    RHIGraphicsPipelineManager *m_rhiGraphicsPipelineManager;
    RHIComputePipelineManager *m_rhiComputePipelineManager;
};

inline size_t qHash(const GraphicsPipelineIdentifier &key, size_t seed = 0)
{
    const QPair<int, Qt3DCore::QNodeId> p = { key.geometryLayoutKey, key.shader };
    using QT_PREPEND_NAMESPACE(qHash);
    seed = qHash(p, seed);
    seed = qHash(key.renderTarget, seed);
    seed = qHash(key.renderStatesKey, seed);
    seed = qHash(key.primitiveType, seed);
    return seed;
}

inline bool operator==(const GraphicsPipelineIdentifier &a, const GraphicsPipelineIdentifier &b)
{
    return a.geometryLayoutKey == b.geometryLayoutKey &&
           a.shader == b.shader &&
           a.renderTarget == b.renderTarget &&
           a.renderStatesKey == b.renderStatesKey &&
           a.primitiveType == b.primitiveType;
}

inline size_t qHash(const ComputePipelineIdentifier &key, size_t seed = 0)
{
    using QT_PREPEND_NAMESPACE(qHash);
    seed = qHash(key.shader, seed);
    seed = qHash(key.renderViewIndex, seed);
    return seed;
}

inline bool operator==(const ComputePipelineIdentifier &a, const ComputePipelineIdentifier &b)
{
    return a.shader == b.shader &&
           a.renderViewIndex == b.renderViewIndex;
}

} // Rhi

} // Render

} // Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIGraphicsPipeline, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIComputePipeline, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHITexture, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIBuffer, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIRenderTarget, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIShader, Q_REQUIRES_CLEANUP)
QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIRESOURCEMANAGERS_P_H
