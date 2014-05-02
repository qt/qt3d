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
#include "material.h"
#include "effect.h"
#include "renderer.h"
#include "rendercamera.h"
#include "rendercommand.h"
#include "rendernode.h"
#include "meshdatamanager.h"
#include "vaomanager.h"
#include "meshmanager.h"
#include "meshdata.h"
#include "cameramanager.h"
#include "rendernodesmanager.h"
#include "materialmanager.h"
#include "techniquemanager.h"
#include "shadermanager.h"
#include <entity.h>
#include <cameraselectornode.h>
#include <framegraphnode.h>
#include <renderpassfilternode.h>
#include <techniquefilternode.h>
#include <viewportnode.h>

#include "renderlogging.h"

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
    FrameGraphNode *node = fgLeaf;
    while (node != Q_NULLPTR) {
        FrameGraphNode::FrameGraphNodeType type = node->nodeType();
        switch (type) {
        case FrameGraphNode::CameraSelector: {
            CameraSelector *cameraSelector = static_cast<CameraSelector *>(node);
            Entity *cameraEntity = cameraSelector->cameraEntity();
            if (cameraEntity != Q_NULLPTR) {
                m_camera = m_renderer->cameraManager()->lookupHandle(cameraEntity->uuid());
                RenderCamera *tmpCam = m_renderer->cameraManager()->data(m_camera);
                RenderNode *tmpCamNode = m_renderer->renderNodesManager()->lookupResource(cameraEntity->uuid());
                if (tmpCam && tmpCamNode)
                    tmpCam->setViewMatrix(tmpCamNode->worldTransform()->inverted());
            }
            break;
        }

        case FrameGraphNode::LayerFilter:
            qCWarning(Backend) << "LayerFilter not implemented yet";
            break;

        case FrameGraphNode::RenderPassFilter:
            m_passFilter = static_cast<RenderPassFilter *>(node);
            break;

        case FrameGraphNode::RenderTarget:
            qCWarning(Backend) << "RenderTarget not implemented yet";
            break;

        case FrameGraphNode::TechniqueFilter:
            m_techniqueFilter = static_cast<TechniqueFilter *>(node);
            break;

        case FrameGraphNode::Viewport:
            // If the Viewport has already been set in a lower node
            // Make it so that the new viewport is actually
            // a subregion relative to that of the parent viewport
            computeViewport(static_cast<ViewportNode *>(node));
            break;

        default:
            // Should never get here
            qCWarning(Backend) << "Unhandled FrameGraphNode type";
        }

        node = node->parent();
    }
}

void RenderView::sort()
{
    // TODO: Implement me!

    // The goal here is to sort RenderCommand by :
    // 1) Shader
    // 2) DrawStateSet
    // 2) Texture
}

void RenderView::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

// Traverse Scene graphTree or culledSceneGraphTree
// ideally m_commands has been sized properly after the
// scene has been culled to the number of nodes in the culled
// scene using reserve().
// Tries to order renderCommand by shader so as to minimize shader changes
void RenderView::buildRenderCommands(RenderNode *node)
{
    // Build renderCommand for current node

    Entity *frontEndEntity = Q_NULLPTR;
    if (node->frontEndPeer() != Q_NULLPTR
            && (frontEndEntity = node->frontEndPeer()->asEntity()) != Q_NULLPTR) {
        HMesh meshHandle;
        if (m_renderer->meshManager()->contains(frontEndEntity->uuid()) &&
                (meshHandle = m_renderer->meshManager()->lookupHandle(frontEndEntity->uuid())) != HMesh()) {
            RenderMesh *mesh = m_renderer->meshManager()->data(meshHandle);
            if (mesh == Q_NULLPTR || mesh->peer() == Q_NULLPTR)
                return ;
            RenderCommand *command = new RenderCommand();
            command->m_stateSet = Q_NULLPTR;
            command->m_mesh = meshHandle;
            if (mesh->meshDirty()) {
                mesh->setMeshData(m_renderer->meshDataManager()->lookupHandle(mesh->peer()->uuid()));
                qCDebug(Backend) << Q_FUNC_INFO << "Updating RenderMesh -> MeshData handle";
            }
            command->m_meshData = mesh->meshData();
            command->m_instancesCount = 0;
            command->m_worldMatrix = *(node->worldTransform());
            // Sets handle to entity material. If there is no material associated to the entity,
            // the handle will be invalid and a default material will be used during rendering
            command->m_material = m_renderer->materialManager()->lookupHandle(frontEndEntity->uuid());

            // The RenderTechnique && RenderPass instances have to be created in the RenderViewJobs
            // As it offers a way to create instances only for techniques and passes that are used.
            setCommandShaderTechniqueEffect(command);

            // Append renderCommand
            m_commands.append(command);
        }
    }

    // Traverse children
    foreach (RenderNode *child, node->children()) {
        buildRenderCommands(child);
    }
}

QRectF RenderView::viewport() const
{
    return m_viewport;
}

RenderCamera *RenderView::camera() const
{
    return m_renderer->cameraManager()->data(m_camera);
}

void RenderView::setCommandShaderTechniqueEffect(RenderCommand *command)
{
    RenderMaterial *material = m_renderer->materialManager()->data(command->m_material);
    if (m_techniqueFilter != Q_NULLPTR && material != Q_NULLPTR &&
            !m_techniqueFilter->filters().empty() && material->peer() != Q_NULLPTR) {
        QMutexLocker locker(&m_mutex);
        // The VAO Handle is set directly in the renderer thread so as to avoid having to use a mutex here
        // Set shader, technique, and effect by basically doing :
        // ShaderProgramManager[MaterialManager[frontentEntity->uuid()]->Effect->Techniques[TechniqueFilter->name]->RenderPasses[RenderPassFilter->name]];
        // The Renderer knows that if one of those is null, a default material / technique / effect as to be used
        Effect *effect = qobject_cast<Effect*>(material->peer()->effect());
        QString techniqueName = m_techniqueFilter->filters().values().first().toString();
        command->m_technique = m_renderer->techniqueManager()->lookupHandle(EffectTechniquePair(effect, techniqueName));
        if (effect != Q_NULLPTR && m_renderer->techniqueManager()->data(command->m_technique) == Q_NULLPTR) {
            RenderTechnique *technique = Q_NULLPTR;
            Q_FOREACH (Technique *t, effect->techniques()) {
                if (t->name() == techniqueName) {
                    command->m_technique = m_renderer->techniqueManager()->getOrAcquireHandle(EffectTechniquePair(effect, techniqueName));
                    technique = m_renderer->techniqueManager()->data(command->m_technique);
                    technique->setPeer(t);
                    break;
                }
            }
            if (technique == Q_NULLPTR) {
                command->m_technique = HTechnique();
                qCWarning(Render::Backend) << Q_FUNC_INFO << "No technique found for technique filter";
            }
            // Load RenderPass and ShaderPrograms
            QString passName = m_passFilter->filter();
            if (technique != Q_NULLPTR)
                Q_FOREACH (RenderPass *pass, technique->peer()->renderPasses()) {
                    if (pass->name() == passName) {
                        // Index RenderShader by Shader UUID
                        command->m_shader = m_renderer->shaderManager()->lookupHandle(pass->shaderProgram()->uuid());
                        if (command->m_shader.isNull()) {
                            RenderShader *shader = m_renderer->shaderManager()->getOrCreateResource(pass->shaderProgram()->uuid());
                            shader->setPeer(pass->shaderProgram());
                            command->m_shader = m_renderer->shaderManager()->lookupHandle(pass->shaderProgram()->uuid());
                        }
                        command->m_stateSet = pass->stateSet();
                        break;
                    }
                }
        }
    }
}

void RenderView::computeViewport(ViewportNode *viewportNode)
{
    QRectF tmpViewport(viewportNode->xMin(),
                       viewportNode->yMin(),
                       viewportNode->xMax(),
                       viewportNode->yMax());
    if (m_viewport.isEmpty()) {
        m_viewport = tmpViewport;
    }
    else {
        QRectF oldViewport = m_viewport;
        m_viewport = QRectF(tmpViewport.x() + tmpViewport.width() * oldViewport.x(),
                            tmpViewport.y() + tmpViewport.height() * oldViewport.y(),
                            tmpViewport.width() * oldViewport.width(),
                            tmpViewport.height() * oldViewport.height());
    }
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
