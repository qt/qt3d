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

#ifndef QT3DRENDER_RENDER_NODEMANAGERS_H
#define QT3DRENDER_RENDER_NODEMANAGERS_H

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

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;
class QOpenGLVertexArrayObject;

namespace Qt3DRender {

class QTexImageData;

namespace Render {

class CameraManager;
class EntityManager;
class BufferManager;
class AttributeManager;
class GeometryManager;
class GeometryRendererManager;
class ObjectPickerManager;
class BoundingVolumeDebugManager;
class MaterialManager;
class MatrixManager;
class VAOManager;
class ShaderManager;
class TechniqueManager;
class EffectManager;
class RenderPassManager;
class RenderTargetManager;
class SceneManager;
class AttachmentManager;
class SortCriterionManager;
class ParameterManager;
class ShaderDataManager;
class UBOManager;
class TextureImageManager;
class CriterionManager;
class FrameGraphManager;
class TransformManager;
class TextureManager;
class TextureDataManager;
class LayerManager;
class LightManager;

class FrameGraphNode;
class Annotation;
class Entity;
class CameraLens;
class Material;
class Shader;
class Technique;
class Effect;
class RenderPass;
class Texture;
class Layer;
class Annotation;
class FrameGraphNode;
class Transform;
class Scene;
class RenderAttachment;
class RenderTarget;
class ShaderData;
class SortCriterion;
class Parameter;
class UniformBuffer;
class TextureImage;
class Buffer;
class Attribute;
class Geometry;
class GeometryRenderer;
class ObjectPicker;
class BoundingVolumeDebug;
class Light;

class QT3DRENDERSHARED_PRIVATE_EXPORT NodeManagers
{
public:
    NodeManagers();

    template<class Backend, typename Manager>
    Manager *manager() const Q_DECL_NOEXCEPT
    {
        return Q_NULLPTR;
    }

    template<class Backend, typename Manager, typename Key>
    Backend *lookupResource(const Key &id) const
    {
        Manager *mgr = manager<Backend, Manager>();
        if (mgr != Q_NULLPTR)
            return mgr->lookupResource(id);
        return Q_NULLPTR;
    }

    template<class Backend, typename Manager, typename Handle, typename Key>
    Handle lookupHandle(const Key &id) const
    {
        Manager *mgr = manager<Backend, Manager>();
        if (mgr != Q_NULLPTR)
            return mgr->lookupHandle(id);
        return Handle();
    }

    template<class Backend, typename Manager, typename Handle>
    Backend *data(Handle handle)
    {
        Manager *mgr = manager<Backend, Manager>();
        if (mgr != Q_NULLPTR)
            return mgr->data(handle);
        return Q_NULLPTR;
    }


    inline CameraManager *cameraManager() const Q_DECL_NOEXCEPT { return m_cameraManager; }
    inline EntityManager *renderNodesManager() const Q_DECL_NOEXCEPT { return m_renderNodesManager; }
    inline MaterialManager *materialManager() const Q_DECL_NOEXCEPT { return m_materialManager; }
    inline MatrixManager *worldMatrixManager() const Q_DECL_NOEXCEPT { return m_worldMatrixManager; }
    inline VAOManager *vaoManager() const Q_DECL_NOEXCEPT { return m_vaoManager; }
    inline ShaderManager *shaderManager() const Q_DECL_NOEXCEPT { return m_shaderManager; }
    inline TechniqueManager *techniqueManager() const Q_DECL_NOEXCEPT { return m_techniqueManager; }
    inline EffectManager *effectManager() const Q_DECL_NOEXCEPT { return m_effectManager; }
    inline RenderPassManager *renderPassManager() const Q_DECL_NOEXCEPT { return m_renderPassManager; }
    inline TextureManager *textureManager() const Q_DECL_NOEXCEPT { return m_textureManager; }
    inline TextureDataManager *textureDataManager() const Q_DECL_NOEXCEPT { return m_textureDataManager; }
    inline LayerManager *layerManager() const Q_DECL_NOEXCEPT { return m_layerManager; }
    inline CriterionManager *criterionManager() const Q_DECL_NOEXCEPT { return m_criterionManager; }
    inline FrameGraphManager *frameGraphManager() const Q_DECL_NOEXCEPT { return m_frameGraphManager; }
    inline TransformManager *transformManager() const Q_DECL_NOEXCEPT { return m_transformManager; }
    inline RenderTargetManager *renderTargetManager() const Q_DECL_NOEXCEPT { return m_renderTargetManager; }
    inline SceneManager *sceneManager() const Q_DECL_NOEXCEPT { return m_sceneManager; }
    inline AttachmentManager *attachmentManager() const Q_DECL_NOEXCEPT { return m_attachmentManager; }
    inline SortCriterionManager *sortCriterionManager() const Q_DECL_NOEXCEPT { return m_sortCriterionManager; }
    inline ParameterManager *parameterManager() const Q_DECL_NOEXCEPT { return m_parameterManager; }
    inline ShaderDataManager *shaderDataManager() const Q_DECL_NOEXCEPT { return m_shaderDataManager; }
    inline UBOManager *uboManager() const Q_DECL_NOEXCEPT { return m_uboManager; }
    inline TextureImageManager *textureImageManager() const Q_DECL_NOEXCEPT { return m_textureImageManager; }
    inline BufferManager *bufferManager() const Q_DECL_NOEXCEPT { return m_bufferManager; }
    inline AttributeManager *attributeManager() const Q_DECL_NOEXCEPT { return m_attributeManager; }
    inline GeometryManager *geometryManager() const Q_DECL_NOEXCEPT { return m_geometryManager; }
    inline GeometryRendererManager *geometryRendererManager() const Q_DECL_NOEXCEPT { return m_geometryRendererManager; }
    inline ObjectPickerManager *objectPickerManager() const Q_DECL_NOEXCEPT { return m_objectPickerManager; }
    inline BoundingVolumeDebugManager *boundingVolumeDebugManager() const Q_DECL_NOEXCEPT { return m_boundingVolumeDebugManager; }
    inline LightManager *lightManager() const Q_DECL_NOEXCEPT { return m_lightManager; }

private:
    CameraManager *m_cameraManager;
    EntityManager *m_renderNodesManager;
    MaterialManager *m_materialManager;
    MatrixManager *m_worldMatrixManager;
    VAOManager *m_vaoManager;
    ShaderManager *m_shaderManager;
    TechniqueManager *m_techniqueManager;
    EffectManager *m_effectManager;
    RenderPassManager *m_renderPassManager;
    TextureManager *m_textureManager;
    TextureDataManager *m_textureDataManager;
    LayerManager *m_layerManager;
    CriterionManager *m_criterionManager;
    FrameGraphManager *m_frameGraphManager;
    TransformManager *m_transformManager;
    RenderTargetManager *m_renderTargetManager;
    SceneManager *m_sceneManager;
    AttachmentManager *m_attachmentManager;
    SortCriterionManager *m_sortCriterionManager;
    ParameterManager *m_parameterManager;
    ShaderDataManager *m_shaderDataManager;
    UBOManager *m_uboManager;
    TextureImageManager *m_textureImageManager;
    BufferManager *m_bufferManager;
    AttributeManager *m_attributeManager;
    GeometryManager *m_geometryManager;
    GeometryRendererManager *m_geometryRendererManager;
    ObjectPickerManager *m_objectPickerManager;
    BoundingVolumeDebugManager *m_boundingVolumeDebugManager;
    LightManager *m_lightManager;
};

// Specializations

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT CameraManager *NodeManagers::manager<CameraLens>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT EntityManager *NodeManagers::manager<Entity>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT MaterialManager *NodeManagers::manager<Material>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT MatrixManager *NodeManagers::manager<QMatrix4x4*>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT VAOManager *NodeManagers::manager<QOpenGLVertexArrayObject*>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT ShaderManager *NodeManagers::manager<Shader>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT TechniqueManager *NodeManagers::manager<Technique>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT EffectManager *NodeManagers::manager<Effect>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT RenderPassManager *NodeManagers::manager<RenderPass>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT TextureManager *NodeManagers::manager<Texture>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT TextureDataManager *NodeManagers::manager<QTexImageData>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT LayerManager *NodeManagers::manager<Layer>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT CriterionManager *NodeManagers::manager<Annotation>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT FrameGraphManager *NodeManagers::manager<FrameGraphNode*>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT TransformManager *NodeManagers::manager<Transform>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT RenderTargetManager *NodeManagers::manager<RenderTarget>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT SceneManager *NodeManagers::manager<Scene>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT AttachmentManager *NodeManagers::manager<RenderAttachment>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT SortCriterionManager *NodeManagers::manager<SortCriterion>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT ParameterManager *NodeManagers::manager<Parameter>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT ShaderDataManager *NodeManagers::manager<ShaderData>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT UBOManager *NodeManagers::manager<UniformBuffer>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT TextureImageManager *NodeManagers::manager<TextureImage>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT BufferManager *NodeManagers::manager<Buffer>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT AttributeManager *NodeManagers::manager<Attribute>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT GeometryManager *NodeManagers::manager<Geometry>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT GeometryRendererManager *NodeManagers::manager<GeometryRenderer>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT ObjectPickerManager *NodeManagers::manager<ObjectPicker>() const Q_DECL_NOEXCEPT;

template<>
QT3DRENDERSHARED_PRIVATE_EXPORT BoundingVolumeDebugManager *NodeManagers::manager<BoundingVolumeDebug>() const Q_DECL_NOEXCEPT;

template<>
LightManager *NodeManagers::manager<Light>() const Q_DECL_NOEXCEPT;

} // Render

} // Qt3DRender

QT_END_NAMESPACE


#endif // NODEMANAGERS_H
