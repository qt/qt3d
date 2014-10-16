/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_MANAGERS_P_H
#define QT3D_RENDER_MANAGERS_P_H

#include <Qt3DCore/qresourcesmanager.h>
#include <Qt3DRenderer/private/renderattachment_p.h>
#include <Qt3DRenderer/private/rendercameralens_p.h>
#include <Qt3DRenderer/private/rendercriterion_p.h>
#include <Qt3DRenderer/private/rendereffect_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/renderlayer_p.h>
#include <Qt3DRenderer/private/renderlight_p.h>
#include <Qt3DRenderer/private/rendermaterial_p.h>
#include <Qt3DRenderer/private/rendermesh_p.h>
#include <Qt3DRenderer/private/rendershader_p.h>
#include <Qt3DRenderer/private/sortcriterion_p.h>
#include <Qt3DRenderer/private/rendertechnique_p.h>
#include <Qt3DRenderer/private/rendertexture_p.h>
#include <Qt3DRenderer/private/rendertransform_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/renderrenderpass_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QMeshData;

namespace Render {

class RenderShader;
class FrameGraphNode;

typedef QHandle<RenderAttachment, 16> HAttachment;
typedef QHandle<RenderCameraLens, 8> HCamera;
typedef QHandle<RenderCriterion, 16> HCriterion;
typedef QHandle<RenderEffect, 16> HEffect;
typedef QHandle<RenderEntity, 16> HEntity;
typedef QHandle<FrameGraphNode *, 8> HFrameGraphNode;
typedef QHandle<RenderLayer, 16> HLayer;
typedef QHandle<RenderLight, 16> HLight;
typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<QMatrix4x4, 16> HMatrix;
typedef QHandle<RenderShader, 16> HShader;
typedef QHandle<QOpenGLVertexArrayObject*, 16> HVao;
typedef QHandle<QMeshData, 16> HMeshData;
typedef QHandle<RenderShader, 16> HShader;
typedef QHandle<SortCriterion, 8> HSortCriterion;
typedef QHandle<RenderTechnique, 16> HTechnique;
typedef QHandle<RenderTexture, 16> HTexture;
typedef QHandle<RenderTransform, 16> HTransform;
typedef QHandle<RenderMesh, 16> HMesh;
typedef QHandle<RenderTarget, 8> HTarget;
typedef QHandle<RenderRenderPass, 16> HRenderPass;

class AttachmentManager : public QResourcesManager<
        RenderAttachment,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    AttachmentManager() {}
};

class CameraManager : public QResourcesManager<
        RenderCameraLens,
        QUuid,
        8,
        Qt3D::ListAllocatingPolicy>
{
public:
    CameraManager() {}
};

class CriterionManager : public QResourcesManager<
        RenderCriterion,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>

{
public:
    CriterionManager() {}
};

class EffectManager : public QResourcesManager<
        RenderEffect,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    EffectManager() {}
};

class EntityManager : public QResourcesManager<RenderEntity, QUuid, 16>
{
public:
    EntityManager() {}
};

class FrameGraphManager : public QResourcesManager<
        FrameGraphNode *,
        QUuid,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    FrameGraphManager() {}
};

class LayerManager : public QResourcesManager<
        RenderLayer,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    LayerManager() {}
};

class LightManager : public QResourcesManager<
        RenderLight,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    LightManager() {}
};

class MaterialManager : public QResourcesManager<
        RenderMaterial,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    MaterialManager() {}
};

class MatrixManager : public QResourcesManager<QMatrix4x4, QUuid, 16>
{
public:
    MatrixManager() {}
};

class MeshManager : public QResourcesManager<
        RenderMesh,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    MeshManager() {}
};

class ShaderManager : public QResourcesManager<
        RenderShader,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ShaderManager() {}
};

class SortCriterionManager : public QResourcesManager<
        SortCriterion,
        QUuid,
        8,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    SortCriterionManager() {}
};

class TechniqueManager : public QResourcesManager<
        RenderTechnique,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TechniqueManager() {}
};

class TextureManager : public QResourcesManager<
        RenderTexture,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TextureManager() {}
};

class TransformManager : public QResourcesManager<
        RenderTransform,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TransformManager() {}
};

class VAOManager : public QResourcesManager<
        QOpenGLVertexArrayObject *,
        QPair<HMeshData, HShader>,
        16>
{
public:
    VAOManager() {}
};

class RenderTargetManager : public QResourcesManager<
        RenderTarget,
        QUuid,
        8,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    RenderTargetManager() {}
};

class RenderPassManager : public QResourcesManager<
        RenderRenderPass,
        QUuid,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    RenderPassManager() {}
};

} // Render

Q_DECLARE_RESOURCE_INFO(Render::RenderCriterion, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderEffect, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderEntity, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderLayer, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderLight, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderMaterial, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderMesh, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderShader, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::SortCriterion, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTarget, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTechnique, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTexture, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderRenderPass, Q_REQUIRES_CLEANUP);

} // Qt3D

QT_END_NAMESPACE


#endif // QT3D_RENDER_MANAGERS_P_H
