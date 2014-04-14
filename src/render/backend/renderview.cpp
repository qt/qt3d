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

#include "renderview.h"

#include "renderer.h"
#include "rendercamera.h"
#include "rendercommand.h"
#include "rendernode.h"
#include "meshmanager.h"
#include "meshdata.h"
#include <entity.h>
#include "cameramanager.h"
#include <cameraselectornode.h>
#include <framegraphnode.h>
#include <renderpassfilternode.h>
#include <techniquefilternode.h>
#include <viewportnode.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderView::~RenderView()
{
    qDeleteAll(m_commands);
}

void RenderView::setConfigFromFrameGraphLeafNode(FrameGraphNode *fgLeaf)
{
    // The specific RenderPass to be used is also dependent upon the Effect and TechniqueFilter
    // which is referenced by the Material which is referenced by the RenderMesh. So we can
    // only store the filter info in the RenderView structure and use it to do the resolving
    // when we build the RenderCommand list.
    //
    // The Camera and Viewport can be resolved at this stage as they are independent of the object
    // material.
    FrameGraphNode *node = fgLeaf;
    while (node != Q_NULLPTR) {
        FrameGraphNode::FrameGraphNodeType type = node->nodeType();
        switch (type) {
        case FrameGraphNode::CameraSelector: {
            CameraSelector *cameraSelector = static_cast<CameraSelector *>(node);
            Entity *cameraEntity = cameraSelector->cameraEntity();
            if (cameraEntity != Q_NULLPTR)
               m_camera = m_renderer->cameraManager()->lookupHandle(cameraEntity->uuid());
            break;
        }

        case FrameGraphNode::LayerFilter:
            qWarning() << "LayerFilter not implemented yet";
            break;

        case FrameGraphNode::RenderPassFilter:
            m_passFilter = static_cast<RenderPassFilter *>(node);
            break;

        case FrameGraphNode::RenderTarget:
            qWarning() << "RenderTarget not implemented yet";
            break;

        case FrameGraphNode::TechniqueFilter:
            m_techniqueFilter = static_cast<TechniqueFilter *>(node);
            break;

        case FrameGraphNode::Viewport:
            m_viewport = static_cast<ViewportNode *>(node);
            break;

        default:
            // Should never get here
            qWarning() << "Unhandled FrameGraphNode type";
        }

        node = node->parent();
    }
}

void RenderView::submit(Renderer *renderer)
{
    // Apply the framegraph configuration state
    //renderer->applyRenderViewState(this);

    // Submit the RenderCommands
    renderer->executeCommands(m_commands);
}

void RenderView::sort()
{
    // TODO: Implement me!
}

void RenderView::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

// Traverse Scene graphTree or culledSceneGraphTree
// ideally m_commands has been sized properly after the
// scene has been culled to the number of nodes in the culled
// scene using reserve().
void RenderView::buildRenderCommands(RenderNode *node)
{
    // Build renderCommand for current node
    RenderCommand *command = new RenderCommand();
    qDebug() << Q_FUNC_INFO << node->frontEndPeer()->objectName();
    Entity *frontEndEntity = Q_NULLPTR;
    if (node->frontEndPeer() != Q_NULLPTR
        && (frontEndEntity = node->frontEndPeer()->asEntity()) != Q_NULLPTR) {
        MeshData *meshData = m_renderer->meshManager()->meshForEntityUuid(frontEndEntity->uuid());
        if (meshData != Q_NULLPTR) {
            qDebug() << Q_FUNC_INFO << meshData->attributeNames();
            // Find MeshData and assigns it to command
            command->m_meshData = *meshData;
            command->m_instancesCount = 0;
            command->m_worldMatrix = *(node->worldTransform());
            qDebug() << Q_FUNC_INFO << command->m_worldMatrix;

            // Use a default shader and uniform bindings for the moment
            // Shader and Uniforms obtained from Material/Effect/Technique/RenderPass/ShaderProgram
            // ShaderProgram = shaderForMeshMaterialAndPassForTechnique
            // ShaderProgramManager[MaterialManager[frontentEntity->uuid()]->Effect->Techniques[TechniqueFilter->name]->RenderPasses[RenderPassFilter->name]];
            //            command->m_shaderProgram = ;
            //            command->m_uniforms = ;

            // Append renderCommand
            m_commands.append(command);
        }
    }

    // Traver children
    foreach (RenderNode *child, node->children()) {
        buildRenderCommands(child);
    }
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
