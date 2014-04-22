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

#include "renderscenebuilder.h"

#include "rendernodesmanager.h"
#include "meshdatamanager.h"
#include "cameramanager.h"
#include "renderer.h"
#include "rendernode.h"
#include "renderlogging.h"
#include "materialmanager.h"

#include <camera.h>
#include <cameralens.h>
#include <material.h>
#include <mesh.h>
#include <transform.h>
#include <entity.h>

#include <framegraph.h>
#include <viewport.h>
#include <techniquefilter.h>
#include <renderpassfilter.h>
#include <cameraselector.h>
#include <rendertargetselector.h>

#include <techniquefilternode.h>
#include <cameraselectornode.h>
#include <renderpassfilternode.h>
#include <viewportnode.h>
#include <rendertargetselectornode.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderSceneBuilder::RenderSceneBuilder(Renderer *renderer)
    : Qt3D::NodeVisitor()
    , m_renderer(renderer)
{
}

RenderNode *RenderSceneBuilder::rootNode() const
{
    return m_renderer->renderNodesManager()->data(m_rootNodeHandle);
}

/*!
 * Returns a FrameGraphNode and all its children from \a node which points to the activeFrameGraph.
 * Returns Q_NULLPTR if \a is also Q_NULLPTR or if there is no FrameGraphComponent
 * in node's children tree.
 */
Render::FrameGraphNode *RenderSceneBuilder::buildFrameGraph(Node *node)
{
    if (node == Q_NULLPTR)
        return Q_NULLPTR;

    qCDebug(Backend) << Q_FUNC_INFO << node->objectName();

    Render::FrameGraphNode *fgNode = Q_NULLPTR;
    Qt3D::FrameGraphItem *fgItem = Q_NULLPTR;

    if ((fgItem = qobject_cast<Qt3D::FrameGraphItem *>(node)) != Q_NULLPTR) {
        // Instantiate proper backend node corresponding to the frontend node
        fgNode = backendFrameGraphNode(fgItem);
    }

    // we need to travel the node's children tree to find either the FG root Node or
    // its children
    QList<FrameGraphNode *> fgChildNodes;
    foreach (Node *child, node->children()) {
        FrameGraphNode* fgChildNode = buildFrameGraph(child);
        if (fgChildNode != Q_NULLPTR)
            fgChildNodes << fgChildNode;
    }
    if (!fgChildNodes.isEmpty()) {
        if (fgNode == Q_NULLPTR && fgChildNodes.size() == 1) {
            fgNode = fgChildNodes.first();
        }
        else {
            if (fgNode == Q_NULLPTR)
                fgNode = new FrameGraphNode();
            foreach (FrameGraphNode *fgChildNodes, fgChildNodes)
                fgNode->appendChild(fgChildNodes);
        }
    }
    return fgNode;
}

/*!
 * Returns a proper FrameGraphNode subclass instance from \a block.
 * If no subclass corresponds, Q_NULLPTR is returned.
 */
Render::FrameGraphNode *RenderSceneBuilder::backendFrameGraphNode(Qt3D::FrameGraphItem *block)
{
    if (qobject_cast<Qt3D::TechniqueFilter*>(block) != Q_NULLPTR) {
        Qt3D::TechniqueFilter* techniqueFilter = qobject_cast<Qt3D::TechniqueFilter*>(block);
        Render::TechniqueFilter *techniqueFilterNode = new Render::TechniqueFilter();

        qCDebug(Backend) << Q_FUNC_INFO << "TechniqueFilter";
        foreach (Tag *tag, techniqueFilter->tags())
            techniqueFilterNode->appendFilter(tag->name(), tag->value());
        return techniqueFilterNode;
    }
    else if (qobject_cast<Qt3D::Viewport*>(block) != Q_NULLPTR) {
        Qt3D::Viewport *viewport = qobject_cast<Qt3D::Viewport*>(block);
        Render::ViewportNode *viewportNode = new Render::ViewportNode();

        qCDebug(Backend) << Q_FUNC_INFO << "Viewport";
        viewportNode->setXMin(viewport->rect().x());
        viewportNode->setXMax(viewport->rect().width());
        viewportNode->setYMin(viewport->rect().y());
        viewportNode->setYMax(viewport->rect().height());
        return viewportNode;
    }
    else if (qobject_cast<Qt3D::RenderPassFilter*>(block) != Q_NULLPTR) {
        Qt3D::RenderPassFilter *renderPassFilter = qobject_cast<Qt3D::RenderPassFilter*>(block);
        Render::RenderPassFilter *renderPassFilterNode = new Render::RenderPassFilter();

        qCDebug(Backend) << Q_FUNC_INFO << "RenderPassFilter";
        renderPassFilterNode->setFilter(renderPassFilter->renderPassName());
        return renderPassFilterNode;
    }
    else if (qobject_cast<Qt3D::CameraSelector*>(block) != Q_NULLPTR) {
        Qt3D::CameraSelector *cameraSelector = qobject_cast<Qt3D::CameraSelector*>(block);
        Render::CameraSelector *cameraSelectorNode = new Render::CameraSelector();

        qCDebug(Backend) << Q_FUNC_INFO << "CameraSelector";
        cameraSelectorNode->setCameraEntity(cameraSelector->camera());
        return cameraSelectorNode;
    }
    else if (qobject_cast<Qt3D::RenderTargetSelector*>(block) != Q_NULLPTR) {
        Qt3D::RenderTargetSelector *renderTargetSelector = qobject_cast<Qt3D::RenderTargetSelector*>(block);
        Render::RenderTargetSelector *renderTargetSelectorNode = new Render::RenderTargetSelector();

        // TO DO
        qCDebug(Backend) << Q_FUNC_INFO << "RenderTargetSelector";
        return renderTargetSelectorNode;
    }
    return Q_NULLPTR;
}

void RenderSceneBuilder::visitNode(Qt3D::Node *node)
{
    if (!rootNode()) {
        // Note : the root node doesn't have any entity uuid it can be referenced by
        // as it is node an Entity. However as it is the scene root, it can always be referenced
        // as the scene root
        m_rootNodeHandle = m_renderer->renderNodesManager()->acquire();
        RenderNode *rootRenderNode = rootNode();
        rootRenderNode->setRenderer(m_renderer);
        rootRenderNode->setFrontEndPeer(node);
        m_nodeStack.push(m_rootNodeHandle);
    }
    qCDebug(Backend) << Q_FUNC_INFO << "Node " << node->objectName();
    Qt3D::NodeVisitor::visitNode(node);
}

void RenderSceneBuilder::visitEntity(Qt3D::Entity *entity)
{
    // Create a RenderNode corresponding to the Entity. Most data will
    // be calculated later by jobs
    qCDebug(Backend) << Q_FUNC_INFO << "Entity " << entity->objectName();
    // Retrieve or created RenderNode for entity
    HRenderNode renderNodeHandle = m_renderer->renderNodesManager()->getOrAcquireHandle(entity->uuid());
    RenderNode *renderNode = m_renderer->renderNodesManager()->data(renderNodeHandle);
    renderNode->setRenderer(m_renderer);
    renderNode->setParentHandle(m_nodeStack.top());
    renderNode->setFrontEndPeer(entity);
    // REPLACE WITH ENTITY MATRIX FROM TRANSFORMS
    m_nodeStack.push(renderNodeHandle);

    // Look for a transform component
    QList<Transform *> transforms = entity->componentsOfType<Transform>();
    if (!transforms.isEmpty()) {
        renderNode->setTransform(transforms.first());
        // Sets default transform matrix, this will later be updated by the QChangeArbiter
        // and the sceneChangeEvent in RenderNode
        *(renderNode->localTransform()) = transforms.first()->matrix();
    }


    QList<FrameGraph *> framegraphRefs = entity->componentsOfType<FrameGraph>();
    if (!framegraphRefs.isEmpty()) {
        FrameGraph *fg = framegraphRefs.first();
        // Entity has a reference to a framegraph configuration
        // Build a tree of FrameGraphNodes by reading the tree of FrameGraphBuildingBlocks
        Render::FrameGraphNode* frameGraphRootNode = buildFrameGraph(fg->activeFrameGraph());
        qCDebug(Backend) << Q_FUNC_INFO << "SceneGraphRoot" <<  frameGraphRootNode;
        m_renderer->setFrameGraphRoot(frameGraphRootNode);
    }

    // Parse Materials to retrieve
    // Material
    // Effect
    // Techniques
    // RenderPass
    // ShaderPrograms
    QList<Material *> materials = entity->componentsOfType<Material>();
    foreach (Material *material, materials) {
        HMaterial rMaterialHandle = m_renderer->materialManager()->getOrAcquireHandle(material);
        m_renderer->materialManager()->linkMaterialToEntity(entity->uuid(), rMaterialHandle);
        RenderMaterial *rMaterial = m_renderer->materialManager()->data(rMaterialHandle);
        rMaterial->setRendererAspect(m_renderer->rendererAspect());
        rMaterial->setPeer(material);
    }

    // We'll update matrices in a job later. In fact should the matrix be decoupled from the mesh?
    foreach (Mesh *mesh, entity->componentsOfType<Mesh>()) {
        m_renderer->meshDataManager()->linkMeshToEntity(entity->uuid(), m_renderer->meshDataManager()->addMesh(mesh));
        //m_renderer->buildMeshes(mesh, material, QMatrix4x4());
    }

    QList<CameraLens*> cameraLenses = entity->componentsOfType<CameraLens>();
    if (!cameraLenses.isEmpty()) {
        // Retrieves or create RenderCamera for entity->uuid
        RenderCamera *camera = m_renderer->cameraManager()->getOrCreateRenderCamera(entity->uuid());
        camera->setRendererAspect(m_renderer->rendererAspect());
        camera->setPeer(cameraLenses.first());
        camera->setProjection(cameraLenses.first()->projectionMatrix());
        // TO DO : Transforms
        // Should we apply a combination of all transform of the parent entities in the tree
        // Or use only the one associated with the Camera Lens ?
        // Applying all parents transform could lead to some strange behavior.
    }

    NodeVisitor::visitEntity(entity);

    // Coming back up the tree
    m_nodeStack.pop();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
