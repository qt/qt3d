/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_MANAGERS_P_H
#define QT3DRENDER_RENDER_MANAGERS_P_H

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRenderer/private/renderattachment_p.h>
#include <Qt3DRenderer/private/cameralens_p.h>
#include <Qt3DRenderer/private/annotation_p.h>
#include <Qt3DRenderer/private/effect_p.h>
#include <Qt3DRenderer/private/entity_p.h>
#include <Qt3DRenderer/private/layer_p.h>
#include <Qt3DRenderer/private/material_p.h>
#include <Qt3DRenderer/private/shader_p.h>
#include <Qt3DRenderer/private/sortcriterion_p.h>
#include <Qt3DRenderer/private/technique_p.h>
#include <Qt3DRenderer/private/rendertexture_p.h>
#include <Qt3DRenderer/private/rendertransform_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/renderpass_p.h>
#include <Qt3DRenderer/private/parameter_p.h>
#include <Qt3DRenderer/private/shaderdata_p.h>
#include <Qt3DRenderer/private/handle_types_p.h>
#include <Qt3DRenderer/private/uniformbuffer_p.h>
#include <Qt3DRenderer/private/rendertextureimage_p.h>
#include <Qt3DRenderer/private/attribute_p.h>
#include <Qt3DRenderer/private/geometry_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class AttachmentManager : public Qt3D::QResourceManager<
        RenderAttachment,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    AttachmentManager() {}
};

class CameraManager : public Qt3D::QResourceManager<
        CameraLens,
        Qt3D::QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    CameraManager() {}
};

class CriterionManager : public Qt3D::QResourceManager<
        Annotation,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>

{
public:
    CriterionManager() {}
};

class EffectManager : public Qt3D::QResourceManager<
        Effect,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    EffectManager() {}
};

class EntityManager : public Qt3D::QResourceManager<Entity, Qt3D::QNodeId, 16>
{
public:
    EntityManager() {}
};

class FrameGraphManager : public Qt3D::QResourceManager<
        FrameGraphNode *,
        Qt3D::QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    FrameGraphManager() {}
};

class LayerManager : public Qt3D::QResourceManager<
        Layer,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    LayerManager() {}
};

class MaterialManager : public Qt3D::QResourceManager<
        Material,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    MaterialManager() {}
};

class MatrixManager : public Qt3D::QResourceManager<QMatrix4x4, Qt3D::QNodeId, 16>
{
public:
    MatrixManager() {}
};

class ShaderManager : public Qt3D::QResourceManager<
        Shader,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ShaderManager() {}
};

class SortCriterionManager : public Qt3D::QResourceManager<
        SortCriterion,
        Qt3D::QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    SortCriterionManager() {}
};

class TechniqueManager : public Qt3D::QResourceManager<
        Technique,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TechniqueManager() {}
};

class TextureManager : public Qt3D::QResourceManager<
        RenderTexture,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TextureManager() {}
};

class TransformManager : public Qt3D::QResourceManager<
        RenderTransform,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TransformManager() {}
};

class VAOManager : public Qt3D::QResourceManager<
        QOpenGLVertexArrayObject *,
        QPair<HGeometry, HShader>,
        16>
{
public:
    VAOManager() {}
};

class RenderTargetManager : public Qt3D::QResourceManager<
        RenderTarget,
        Qt3D::QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    RenderTargetManager() {}
};

class RenderPassManager : public Qt3D::QResourceManager<
        RenderPass,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    RenderPassManager() {}
};


class ParameterManager : public Qt3D::QResourceManager<
        Parameter,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ParameterManager() {}
};

class ShaderDataManager : public Qt3D::QResourceManager<
        ShaderData,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ShaderDataManager() {}
};

class UBOManager : public Qt3D::QResourceManager<
        UniformBuffer,
        ShaderDataShaderUboKey,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

class TextureImageManager : public Qt3D::QResourceManager<
        RenderTextureImage,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

class AttributeManager : public Qt3D::QResourceManager<
        Attribute,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

class GeometryManager : public Qt3D::QResourceManager<
        Geometry,
        Qt3D::QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

} // namespace Render
} // namespace Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Annotation, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Effect, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Entity, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Layer, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Material, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Shader, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::SortCriterion, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::RenderTarget, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Technique, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::RenderTexture, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::RenderPass, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::RenderTextureImage, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Attribute, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Geometry, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_MANAGERS_P_H
