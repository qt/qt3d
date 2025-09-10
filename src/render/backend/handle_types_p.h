// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_HANDLE_TYPES_P_H
#define QT3DRENDER_RENDER_HANDLE_TYPES_P_H

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

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/private/qhandle_p.h>
#include <Qt3DCore/private/matrix4x4_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureImageData;

namespace Render {

class RenderTargetOutput;
class CameraLens;
class FilterKey;
class Effect;
class Entity;
class Shader;
class ShaderBuilder;
class FrameGraphNode;
class Layer;
class LevelOfDetail;
class Material;
class Technique;
class Texture;
class Transform;
class RenderTarget;
class RenderPass;
class Parameter;
class ShaderData;
class TextureImage;
class Buffer;
class Attribute;
class Geometry;
class GeometryRenderer;
class PickingProxy;
class ObjectPicker;
class RayCaster;
class BoundingVolumeDebug;
class Light;
class EnvironmentLight;
class ComputeCommand;
class RenderStateNode;
class Armature;
class Skeleton;
class Joint;
class ShaderImage;

typedef Qt3DCore::QHandle<RenderTargetOutput> HAttachment;
typedef Qt3DCore::QHandle<CameraLens> HCamera;
typedef Qt3DCore::QHandle<FilterKey> HFilterKey;
typedef Qt3DCore::QHandle<Effect> HEffect;
typedef Qt3DCore::QHandle<Entity> HEntity;
typedef Qt3DCore::QHandle<FrameGraphNode *> HFrameGraphNode;
typedef Qt3DCore::QHandle<Layer> HLayer;
typedef Qt3DCore::QHandle<LevelOfDetail> HLevelOfDetail;
typedef Qt3DCore::QHandle<Material> HMaterial;
typedef Qt3DCore::QHandle<Matrix4x4> HMatrix;
typedef Qt3DCore::QHandle<Shader> HShader;
typedef Qt3DCore::QHandle<ShaderBuilder> HShaderBuilder;
typedef Qt3DCore::QHandle<Technique> HTechnique;
typedef Qt3DCore::QHandle<Texture> HTexture;
typedef Qt3DCore::QHandle<Transform> HTransform;
typedef Qt3DCore::QHandle<RenderTarget> HTarget;
typedef Qt3DCore::QHandle<RenderPass> HRenderPass;
typedef Qt3DCore::QHandle<QTextureImageData> HTextureData;
typedef Qt3DCore::QHandle<Parameter> HParameter;
typedef Qt3DCore::QHandle<ShaderData> HShaderData;
typedef Qt3DCore::QHandle<TextureImage> HTextureImage;
typedef Qt3DCore::QHandle<Buffer> HBuffer;
typedef Qt3DCore::QHandle<Attribute> HAttribute;
typedef Qt3DCore::QHandle<Geometry> HGeometry;
typedef Qt3DCore::QHandle<GeometryRenderer> HGeometryRenderer;
typedef Qt3DCore::QHandle<PickingProxy> HPickingProxy;
typedef Qt3DCore::QHandle<ObjectPicker> HObjectPicker;
typedef Qt3DCore::QHandle<RayCaster> HRayCaster;
typedef Qt3DCore::QHandle<BoundingVolumeDebug> HBoundingVolumeDebug;
typedef Qt3DCore::QHandle<Light> HLight;
typedef Qt3DCore::QHandle<EnvironmentLight> HEnvironmentLight;
typedef Qt3DCore::QHandle<ComputeCommand> HComputeCommand;
typedef Qt3DCore::QHandle<RenderStateNode> HRenderState;
typedef Qt3DCore::QHandle<Armature> HArmature;
typedef Qt3DCore::QHandle<Skeleton> HSkeleton;
typedef Qt3DCore::QHandle<Joint> HJoint;
typedef Qt3DCore::QHandle<ShaderImage> HShaderImage;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_HANDLE_TYPES_P_H
