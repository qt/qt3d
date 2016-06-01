/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_MANAGERS_P_H
#define QT3DRENDER_RENDER_MANAGERS_P_H

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
#include <Qt3DRender/private/rendertargetoutput_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/layer_p.h>
#include <Qt3DRender/private/material_p.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/private/technique_p.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/transform_p.h>
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/renderpass_p.h>
#include <Qt3DRender/private/renderstatenode_p.h>
#include <Qt3DRender/private/parameter_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/glbuffer_p.h>
#include <Qt3DRender/private/textureimage_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/boundingvolumedebug_p.h>
#include <Qt3DRender/private/openglvertexarrayobject_p.h>
#include <Qt3DRender/private/light_p.h>
#include <Qt3DRender/private/computecommand_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class AttachmentManager : public Qt3DCore::QResourceManager<
        RenderTargetOutput,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    AttachmentManager() {}
};

class CameraManager : public Qt3DCore::QResourceManager<
        CameraLens,
        Qt3DCore::QNodeId,
        8,
        Qt3DCore::ArrayAllocatingPolicy>
{
public:
    CameraManager() {}
};

class FilterKeyManager : public Qt3DCore::QResourceManager<
        FilterKey,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>

{
public:
    FilterKeyManager() {}
};

class EffectManager : public Qt3DCore::QResourceManager<
        Effect,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    EffectManager() {}
};

class Q_AUTOTEST_EXPORT EntityManager : public Qt3DCore::QResourceManager<
        Entity,
        Qt3DCore::QNodeId,
        16>
{
public:
    EntityManager() {}
    ~EntityManager()
    {
        const auto entries = m_handleManager.entries();
        for (Entity *e : entries) {
            if (e)
                e->setNodeManagers(nullptr);
        }
    }
};

class FrameGraphNode;

class Q_AUTOTEST_EXPORT FrameGraphManager
{
public:
    FrameGraphManager() {}
    ~FrameGraphManager();

    bool containsNode(Qt3DCore::QNodeId id) const;
    void appendNode(Qt3DCore::QNodeId id, FrameGraphNode *node);
    FrameGraphNode* lookupNode(Qt3DCore::QNodeId id) const;
    void releaseNode(Qt3DCore::QNodeId id);

private:
    QHash<Qt3DCore::QNodeId, FrameGraphNode*> m_nodes;
};

class LayerManager : public Qt3DCore::QResourceManager<
        Layer,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    LayerManager() {}
};

class MaterialManager : public Qt3DCore::QResourceManager<
        Material,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    MaterialManager() {}
};

class MatrixManager : public Qt3DCore::QResourceManager<QMatrix4x4, Qt3DCore::QNodeId, 16>
{
public:
    MatrixManager() {}
};

class ShaderManager : public Qt3DCore::QResourceManager<
        Shader,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    ShaderManager() {}
};

class TechniqueManager : public Qt3DCore::QResourceManager<
        Technique,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    TechniqueManager() {}
};

class TextureManager : public Qt3DCore::QResourceManager<
        Texture,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    TextureManager() {}
};

class TransformManager : public Qt3DCore::QResourceManager<
        Transform,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    TransformManager() {}
};

class VAOManager : public Qt3DCore::QResourceManager<
        OpenGLVertexArrayObject,
        QPair<HGeometry, HShader>,
        16>
{
public:
    VAOManager() {}
};

class RenderTargetManager : public Qt3DCore::QResourceManager<
        RenderTarget,
        Qt3DCore::QNodeId,
        8,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    RenderTargetManager() {}
};

class RenderPassManager : public Qt3DCore::QResourceManager<
        RenderPass,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    RenderPassManager() {}
};


class ParameterManager : public Qt3DCore::QResourceManager<
        Parameter,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    ParameterManager() {}
};

class ShaderDataManager : public Qt3DCore::QResourceManager<
        ShaderData,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    ShaderDataManager() {}
};

class GLBufferManager : public Qt3DCore::QResourceManager<
        GLBuffer,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};

class TextureImageManager : public Qt3DCore::QResourceManager<
        TextureImage,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};

class AttributeManager : public Qt3DCore::QResourceManager<
        Attribute,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};

class GeometryManager : public Qt3DCore::QResourceManager<
        Geometry,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};

class ObjectPickerManager : public Qt3DCore::QResourceManager<
        ObjectPicker,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};

#if 0
class BoundingVolumeDebugManager : public Qt3DCore::QResourceManager<
        BoundingVolumeDebug,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};
#endif

class LightManager : public Qt3DCore::QResourceManager<
        Light,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    LightManager() {}
};

class ComputeCommandManager : public Qt3DCore::QResourceManager<
        ComputeCommand,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    ComputeCommandManager() {}
};

class RenderStateManager : public Qt3DCore::QResourceManager<
        RenderStateNode,
        Qt3DCore::QNodeId,
        16,
        Qt3DCore::ArrayAllocatingPolicy,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};


} // namespace Render
} // namespace Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::FilterKey, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Effect, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Entity, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Layer, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Material, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Shader, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::RenderTarget, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Technique, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Texture, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::RenderPass, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::TextureImage, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Attribute, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Geometry, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::ObjectPicker, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::BoundingVolumeDebug, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::ComputeCommand, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_MANAGERS_P_H
