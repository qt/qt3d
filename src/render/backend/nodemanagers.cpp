// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "nodemanagers_p.h"

#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/armature_p.h>
#include <Qt3DRender/private/skeleton_p.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

NodeManagers::NodeManagers()
    : m_cameraManager(new CameraManager())
    , m_renderNodesManager(new EntityManager())
    , m_materialManager(new MaterialManager())
    , m_worldMatrixManager(new MatrixManager())
    , m_shaderManager(new ShaderManager())
    , m_shaderBuilderManager(new ShaderBuilderManager())
    , m_techniqueManager(new TechniqueManager())
    , m_effectManager(new EffectManager())
    , m_renderPassManager(new RenderPassManager())
    , m_textureManager(new TextureManager())
    , m_textureImageManager(new TextureImageManager())
    , m_layerManager(new LayerManager())
    , m_levelOfDetailManager(new LevelOfDetailManager())
    , m_filterKeyManager(new FilterKeyManager())
    , m_frameGraphManager(new FrameGraphManager())
    , m_transformManager(new TransformManager())
    , m_renderTargetManager(new RenderTargetManager())
    , m_sceneManager(new SceneManager())
    , m_attachmentManager(new AttachmentManager())
    , m_parameterManager(new ParameterManager())
    , m_shaderDataManager(new ShaderDataManager())
    , m_bufferManager(new BufferManager())
    , m_attributeManager(new AttributeManager())
    , m_geometryManager(new GeometryManager())
    , m_geometryRendererManager(new GeometryRendererManager)
    , m_objectPickerManager(new ObjectPickerManager())
    , m_rayCasterManager(new RayCasterManager())
//    , m_boundingVolumeDebugManager(new BoundingVolumeDebugManager())
    , m_lightManager(new LightManager())
    , m_environmentLightManager(new EnvironmentLightManager())
    , m_computeJobManager(new ComputeCommandManager())
    , m_renderStateManager(new RenderStateManager())
    , m_armatureManager(new ArmatureManager())
    , m_skeletonManager(new SkeletonManager())
    , m_jointManager(new JointManager())
    , m_shaderImageManager(new ShaderImageManager())
    , m_pickingProxyManager(new PickingProxyManager())
{
}

NodeManagers::~NodeManagers()
{
    delete m_cameraManager;
    delete m_materialManager;
    delete m_worldMatrixManager;
    delete m_shaderManager;
    delete m_shaderBuilderManager;
    delete m_techniqueManager;
    delete m_effectManager;
    delete m_renderPassManager;
    delete m_textureManager;
    delete m_layerManager;
    delete m_levelOfDetailManager;
    delete m_filterKeyManager;
    delete m_transformManager;
    delete m_renderTargetManager;
    delete m_sceneManager;
    delete m_attachmentManager;
    delete m_parameterManager;
    delete m_shaderDataManager;
    delete m_textureImageManager;
    delete m_bufferManager;
    delete m_attributeManager;
    delete m_geometryManager;
    delete m_geometryRendererManager;
    delete m_objectPickerManager;
    delete m_rayCasterManager;

    // Delete after m_objectPickerManager as that manager's shutdown needs to access
    // the frame graph manager still.
    delete m_frameGraphManager;
//    delete m_boundingVolumeDebugManager;
    delete m_lightManager;
    delete m_environmentLightManager;
    delete m_computeJobManager;
    delete m_renderStateManager;
    delete m_renderNodesManager;
    delete m_armatureManager;
    delete m_skeletonManager;
    delete m_jointManager;
    delete m_shaderImageManager;
}

template<>
CameraManager *NodeManagers::manager<CameraLens>() const noexcept
{
    return m_cameraManager;
}

template<>
EntityManager *NodeManagers::manager<Entity>() const noexcept
{
    return m_renderNodesManager;
}

template<>
MaterialManager *NodeManagers::manager<Material>() const noexcept
{
    return m_materialManager;
}

template<>
MatrixManager *NodeManagers::manager<Matrix4x4>() const noexcept
{
    return m_worldMatrixManager;
}

template<>
ShaderManager *NodeManagers::manager<Shader>() const noexcept
{
    return m_shaderManager;
}

template<>
ShaderBuilderManager *NodeManagers::manager<ShaderBuilder>() const noexcept
{
    return m_shaderBuilderManager;
}

template<>
TechniqueManager *NodeManagers::manager<Technique>() const noexcept
{
    return m_techniqueManager;
}

template<>
EffectManager *NodeManagers::manager<Effect>() const noexcept
{
    return m_effectManager;
}

template<>
RenderPassManager *NodeManagers::manager<RenderPass>() const noexcept
{
    return m_renderPassManager;
}

template<>
TextureManager *NodeManagers::manager<Texture>() const noexcept
{
    return m_textureManager;
}

template<>
LayerManager *NodeManagers::manager<Layer>() const noexcept
{
    return m_layerManager;
}

template<>
LevelOfDetailManager *NodeManagers::manager<LevelOfDetail>() const noexcept
{
    return m_levelOfDetailManager;
}

template<>
FilterKeyManager *NodeManagers::manager<FilterKey>() const noexcept
{
    return m_filterKeyManager;
}

template<>
FrameGraphManager *NodeManagers::manager<FrameGraphNode*>() const noexcept
{
    return m_frameGraphManager;
}

template<>
TransformManager *NodeManagers::manager<Transform>() const noexcept
{
    return m_transformManager;
}

template<>
RenderTargetManager *NodeManagers::manager<RenderTarget>() const noexcept
{
    return m_renderTargetManager;
}

template<>
SceneManager *NodeManagers::manager<Scene>() const noexcept
{
    return m_sceneManager;
}

template<>
AttachmentManager *NodeManagers::manager<RenderTargetOutput>() const noexcept
{
    return m_attachmentManager;
}

template<>
ParameterManager *NodeManagers::manager<Parameter>() const noexcept
{
    return m_parameterManager;
}

template<>
ShaderDataManager *NodeManagers::manager<ShaderData>() const noexcept
{
    return m_shaderDataManager;
}

template<>
TextureImageManager *NodeManagers::manager<TextureImage>() const noexcept
{
    return m_textureImageManager;
}

template<>
BufferManager *NodeManagers::manager<Buffer>() const noexcept
{
    return m_bufferManager;
}

template<>
AttributeManager *NodeManagers::manager<Attribute>() const noexcept
{
    return m_attributeManager;
}

template<>
GeometryManager *NodeManagers::manager<Geometry>() const noexcept
{
    return m_geometryManager;
}

template<>
GeometryRendererManager *NodeManagers::manager<GeometryRenderer>() const noexcept
{
    return m_geometryRendererManager;
}

template<>
ObjectPickerManager *NodeManagers::manager<ObjectPicker>() const noexcept
{
    return m_objectPickerManager;
}

template<>
PickingProxyManager *NodeManagers::manager<PickingProxy>() const noexcept
{
    return m_pickingProxyManager;
}

template<>
RayCasterManager *NodeManagers::manager<RayCaster>() const noexcept
{
    return m_rayCasterManager;
}

//template<>
//BoundingVolumeDebugManager *NodeManagers::manager<BoundingVolumeDebug>() const noexcept
//{
//    return m_boundingVolumeDebugManager;
//}

template<>
LightManager *NodeManagers::manager<Light>() const noexcept
{
    return m_lightManager;
}

template<>
EnvironmentLightManager *NodeManagers::manager<EnvironmentLight>() const noexcept
{
    return m_environmentLightManager;
}

template<>
ComputeCommandManager *NodeManagers::manager<ComputeCommand>() const noexcept
{
    return m_computeJobManager;
}

template<>
RenderStateManager *NodeManagers::manager<RenderStateNode>() const noexcept
{
    return m_renderStateManager;
}

template<>
ArmatureManager *NodeManagers::manager<Armature>() const noexcept
{
    return m_armatureManager;
}

template<>
SkeletonManager *NodeManagers::manager<Skeleton>() const noexcept
{
    return m_skeletonManager;
}

template<>
JointManager *NodeManagers::manager<Joint>() const noexcept
{
    return m_jointManager;
}

template<>
ShaderImageManager *NodeManagers::manager<ShaderImage>() const noexcept
{
    return m_shaderImageManager;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
