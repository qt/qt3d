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

#ifndef QT3D_RENDER_HANDLE_TYPES_P_H
#define QT3D_RENDER_HANDLE_TYPES_P_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/qhandle.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;
class QOpenGLVertexArrayObject;

namespace Qt3D {

class QMeshData;
class TexImageData;

namespace Render {

class RenderAttachment;
class RenderCameraLens;
class RenderAnnotation;
class RenderEffect;
class RenderEntity;
class RenderShader;
class FrameGraphNode;
class RenderLayer;
class RenderMaterial;
class SortCriterion;
class RenderTechnique;
class RenderTexture;
class RenderTransform;
class RenderMesh;
class RenderTarget;
class RenderRenderPass;
class RenderParameter;
class RenderShaderData;
class RenderTextureImage;

typedef QHandle<RenderAttachment, 16> HAttachment;
typedef QHandle<RenderCameraLens, 8> HCamera;
typedef QHandle<RenderAnnotation, 16> HCriterion;
typedef QHandle<RenderEffect, 16> HEffect;
typedef QHandle<RenderEntity, 16> HEntity;
typedef QHandle<FrameGraphNode *, 8> HFrameGraphNode;
typedef QHandle<RenderLayer, 16> HLayer;
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
typedef QHandle<TexImageData, 16> HTextureData;
typedef QHandle<RenderParameter, 16> HParameter;
typedef QHandle<RenderShaderData, 16> HShaderData;
typedef QHandle<RenderTextureImage, 16> HTextureImage;

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_HANDLE_TYPES_P_H
