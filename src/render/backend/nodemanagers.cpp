/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "nodemanagers_p.h"

#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>

#include <QOpenGLVertexArrayObject>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

NodeManagers::NodeManagers()
    : m_cameraManager(new CameraManager())
    , m_renderNodesManager(new EntityManager())
    , m_materialManager(new MaterialManager())
    , m_worldMatrixManager(new MatrixManager())
    , m_vaoManager(new VAOManager())
    , m_shaderManager(new ShaderManager())
    , m_techniqueManager(new TechniqueManager())
    , m_effectManager(new EffectManager())
    , m_renderPassManager(new RenderPassManager())
    , m_textureManager(new TextureManager())
    , m_textureDataManager(new TextureDataManager())
    , m_layerManager(new LayerManager())
    , m_criterionManager(new CriterionManager())
    , m_frameGraphManager(new FrameGraphManager())
    , m_transformManager(new TransformManager())
    , m_renderTargetManager(new RenderTargetManager())
    , m_sceneManager(new SceneManager())
    , m_attachmentManager(new AttachmentManager())
    , m_sortCriterionManager(new SortCriterionManager())
    , m_parameterManager(new ParameterManager())
    , m_shaderDataManager(new ShaderDataManager())
    , m_uboManager(new UBOManager())
    , m_textureImageManager(new TextureImageManager())
    , m_bufferManager(new BufferManager())
    , m_attributeManager(new AttributeManager())
    , m_geometryManager(new GeometryManager())
    , m_geometryRendererManager(new GeometryRendererManager)
    , m_objectPickerManager(new ObjectPickerManager())
    , m_boundingVolumeDebugManager(new BoundingVolumeDebugManager())
    , m_lightManager(new LightManager())
{
}

template<>
CameraManager *NodeManagers::manager<CameraLens>() const Q_DECL_NOEXCEPT
{
    return m_cameraManager;
}

template<>
EntityManager *NodeManagers::manager<Entity>() const Q_DECL_NOEXCEPT
{
    return m_renderNodesManager;
}

template<>
MaterialManager *NodeManagers::manager<Material>() const Q_DECL_NOEXCEPT
{
    return m_materialManager;
}

template<>
MatrixManager *NodeManagers::manager<QMatrix4x4*>() const Q_DECL_NOEXCEPT
{
    return m_worldMatrixManager;
}

template<>
VAOManager *NodeManagers::manager<QOpenGLVertexArrayObject*>() const Q_DECL_NOEXCEPT
{
    return m_vaoManager;
}

template<>
ShaderManager *NodeManagers::manager<Shader>() const Q_DECL_NOEXCEPT
{
    return m_shaderManager;
}

template<>
TechniqueManager *NodeManagers::manager<Technique>() const Q_DECL_NOEXCEPT
{
    return m_techniqueManager;
}

template<>
EffectManager *NodeManagers::manager<Effect>() const Q_DECL_NOEXCEPT
{
    return m_effectManager;
}

template<>
RenderPassManager *NodeManagers::manager<RenderPass>() const Q_DECL_NOEXCEPT
{
    return m_renderPassManager;
}

template<>
TextureManager *NodeManagers::manager<Texture>() const Q_DECL_NOEXCEPT
{
    return m_textureManager;
}

template<>
TextureDataManager *NodeManagers::manager<QTexImageData>() const Q_DECL_NOEXCEPT
{
    return m_textureDataManager;
}

template<>
LayerManager *NodeManagers::manager<Layer>() const Q_DECL_NOEXCEPT
{
    return m_layerManager;
}

template<>
CriterionManager *NodeManagers::manager<Annotation>() const Q_DECL_NOEXCEPT
{
    return m_criterionManager;
}

template<>
FrameGraphManager *NodeManagers::manager<FrameGraphNode*>() const Q_DECL_NOEXCEPT
{
    return m_frameGraphManager;
}

template<>
TransformManager *NodeManagers::manager<Transform>() const Q_DECL_NOEXCEPT
{
    return m_transformManager;
}

template<>
RenderTargetManager *NodeManagers::manager<RenderTarget>() const Q_DECL_NOEXCEPT
{
    return m_renderTargetManager;
}

template<>
SceneManager *NodeManagers::manager<Scene>() const Q_DECL_NOEXCEPT
{
    return m_sceneManager;
}

template<>
AttachmentManager *NodeManagers::manager<RenderAttachment>() const Q_DECL_NOEXCEPT
{
    return m_attachmentManager;
}

template<>
SortCriterionManager *NodeManagers::manager<SortCriterion>() const Q_DECL_NOEXCEPT
{
    return m_sortCriterionManager;
}

template<>
ParameterManager *NodeManagers::manager<Parameter>() const Q_DECL_NOEXCEPT
{
    return m_parameterManager;
}

template<>
ShaderDataManager *NodeManagers::manager<ShaderData>() const Q_DECL_NOEXCEPT
{
    return m_shaderDataManager;
}

template<>
UBOManager *NodeManagers::manager<UniformBuffer>() const Q_DECL_NOEXCEPT
{
    return m_uboManager;
}

template<>
TextureImageManager *NodeManagers::manager<TextureImage>() const Q_DECL_NOEXCEPT
{
    return m_textureImageManager;
}

template<>
BufferManager *NodeManagers::manager<Buffer>() const Q_DECL_NOEXCEPT
{
    return m_bufferManager;
}

template<>
AttributeManager *NodeManagers::manager<Attribute>() const Q_DECL_NOEXCEPT
{
    return m_attributeManager;
}

template<>
GeometryManager *NodeManagers::manager<Geometry>() const Q_DECL_NOEXCEPT
{
    return m_geometryManager;
}

template<>
GeometryRendererManager *NodeManagers::manager<GeometryRenderer>() const Q_DECL_NOEXCEPT
{
    return m_geometryRendererManager;
}

template<>
ObjectPickerManager *NodeManagers::manager<ObjectPicker>() const Q_DECL_NOEXCEPT
{
    return m_objectPickerManager;
}

template<>
BoundingVolumeDebugManager *NodeManagers::manager<BoundingVolumeDebug>() const Q_DECL_NOEXCEPT
{
    return m_boundingVolumeDebugManager;
}

template<>
LightManager *NodeManagers::manager<Light>() const Q_DECL_NOEXCEPT
{
    return m_lightManager;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
