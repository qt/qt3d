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

#ifndef QT3DRENDER_RENDER_HANDLE_TYPES_P_H
#define QT3DRENDER_RENDER_HANDLE_TYPES_P_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/qhandle.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;
class QOpenGLVertexArrayObject;

namespace Qt3DRender {

class TexImageData;

namespace Render {

class RenderAttachment;
class CameraLens;
class Annotation;
class Effect;
class Entity;
class RenderShader;
class FrameGraphNode;
class RenderLayer;
class RenderMaterial;
class SortCriterion;
class RenderTechnique;
class RenderTexture;
class RenderTransform;
class RenderTarget;
class RenderRenderPass;
class RenderParameter;
class RenderShaderData;
class RenderTextureImage;
class Buffer;
class Attribute;
class Geometry;
class RenderGeometryRenderer;

typedef Qt3D::QHandle<RenderAttachment, 16> HAttachment;
typedef Qt3D::QHandle<CameraLens, 8> HCamera;
typedef Qt3D::QHandle<Annotation, 16> HCriterion;
typedef Qt3D::QHandle<Effect, 16> HEffect;
typedef Qt3D::QHandle<Entity, 16> HEntity;
typedef Qt3D::QHandle<FrameGraphNode *, 8> HFrameGraphNode;
typedef Qt3D::QHandle<RenderLayer, 16> HLayer;
typedef Qt3D::QHandle<RenderMaterial, 16> HMaterial;
typedef Qt3D::QHandle<QMatrix4x4, 16> HMatrix;
typedef Qt3D::QHandle<RenderShader, 16> HShader;
typedef Qt3D::QHandle<QOpenGLVertexArrayObject*, 16> HVao;
typedef Qt3D::QHandle<RenderShader, 16> HShader;
typedef Qt3D::QHandle<SortCriterion, 8> HSortCriterion;
typedef Qt3D::QHandle<RenderTechnique, 16> HTechnique;
typedef Qt3D::QHandle<RenderTexture, 16> HTexture;
typedef Qt3D::QHandle<RenderTransform, 16> HTransform;
typedef Qt3D::QHandle<RenderTarget, 8> HTarget;
typedef Qt3D::QHandle<RenderRenderPass, 16> HRenderPass;
typedef Qt3D::QHandle<TexImageData, 16> HTextureData;
typedef Qt3D::QHandle<RenderParameter, 16> HParameter;
typedef Qt3D::QHandle<RenderShaderData, 16> HShaderData;
typedef Qt3D::QHandle<RenderTextureImage, 16> HTextureImage;
typedef Qt3D::QHandle<Buffer, 16> HBuffer;
typedef Qt3D::QHandle<Attribute, 16> HAttribute;
typedef Qt3D::QHandle<Geometry, 16> HGeometry;
typedef Qt3D::QHandle<RenderGeometryRenderer, 16> HGeometryRenderer;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_HANDLE_TYPES_P_H
