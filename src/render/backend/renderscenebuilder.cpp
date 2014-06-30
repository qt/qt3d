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
#include "meshmanager.h"
#include "cameramanager.h"
#include "renderer.h"
#include "rendernode.h"
#include "renderlogging.h"
#include "materialmanager.h"
#include "qabstractscene.h"
#include <material.h>
#include <mesh.h>
#include <qabstractshapemesh.h>

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

#include <Qt3DCore/camera.h>
#include <Qt3DCore/cameralens.h>
#include <Qt3DCore/transform.h>
#include <Qt3DCore/entity.h>

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

void RenderSceneBuilder::initializeFrameGraph()
{
    // We trigger the Backend FrameGraph building once we're sure all
    // References that the FrameGraphItem may be usings have been inserted
    // Into the scenegraph
    qCDebug(Render::Backend) << Q_FUNC_INFO << "FrameGraph";
    // Retrieve and set Renderer FrameGraph
    FrameGraph *fg = Entity::findComponentInTree<FrameGraph>(rootNode()->frontEndPeer());
    m_frameGraphEntityNode = m_renderer->renderNodesManager()->lookupHandle(fg->parentNode()->asEntity()->uuid());
    createFrameGraph(fg);
}

/*!
 * Returns a FrameGraphNode and all its children from \a node which points to the activeFrameGraph.
 * Returns Q_NULLPTR if \a is also Q_NULLPTR or if there is no FrameGraphComponent
 * in node's children tree.
 */
Render::FrameGraphNode *RenderSceneBuilder::buildFrameGraph(Node *node)
{
    Q_ASSERT(node);

    qCDebug(Backend) << Q_FUNC_INFO << node->objectName();

    Render::FrameGraphNode *fgNode = Q_NULLPTR;
    Qt3D::FrameGraphItem *fgItem = Q_NULLPTR;

    if ((fgItem = qobject_cast<Qt3D::FrameGraphItem *>(node)) != Q_NULLPTR) {
        // Instantiate proper backend node corresponding to the frontend node
        fgNode = backendFrameGraphNode(node);
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
            foreach (FrameGraphNode *fgChildNode, fgChildNodes)
                fgNode->appendChild(fgChildNode);
        }
    }
    return fgNode;
}

/*!
 * Returns a proper FrameGraphNode subclass instance from \a block.
 * If no subclass corresponds, Q_NULLPTR is returned.
 */
Render::FrameGraphNode *RenderSceneBuilder::backendFrameGraphNode(Node *block)
{
    if (qobject_cast<Qt3D::TechniqueFilter*>(block) != Q_NULLPTR) {
        Qt3D::TechniqueFilter* techniqueFilter = qobject_cast<Qt3D::TechniqueFilter*>(block);
        Render::TechniqueFilter *techniqueFilterNode = new Render::TechniqueFilter();

        qCDebug(Backend) << Q_FUNC_INFO << "TechniqueFilter";
        Q_FOREACH (TechniqueCriterion *criterion, techniqueFilter->criteria())
            techniqueFilterNode->appendFilter(criterion);
        return techniqueFilterNode;
    }
    else if (qobject_cast<Qt3D::Viewport*>(block) != Q_NULLPTR) {
        Qt3D::Viewport *viewport = qobject_cast<Qt3D::Viewport*>(block);
        Render::ViewportNode *viewportNode = new Render::ViewportNode();
        viewportNode->setRenderer(m_renderer);
        viewportNode->setPeer(viewport);
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
        Q_FOREACH (RenderPassCriterion *criterion, renderPassFilter->criteria())
            renderPassFilterNode->appendFilter(criterion);
        return renderPassFilterNode;
    }
    else if (qobject_cast<Qt3D::CameraSelector*>(block) != Q_NULLPTR) {
        Qt3D::CameraSelector *cameraSelector = qobject_cast<Qt3D::CameraSelector*>(block);
        Render::CameraSelector *cameraSelectorNode = new Render::CameraSelector();
        cameraSelectorNode->setRenderer(m_renderer);
        cameraSelectorNode->setPeer(cameraSelector);
        // TO DO : We might as well store an Entity in CameraSelector
        Entity *cameraEntity = cameraSelector->camera()->asEntity();
        // If the Entity is declared inline on the QML Side, the Entity is not set as part of the Scene tree
        // So we need to make sure the RenderNode for the given Entity exists in the RenderNodesMananger
        if (cameraEntity && m_renderer->cameraManager()->lookupHandle(cameraEntity->uuid()).isNull()) {
            HRenderNode nodeHandle = createRenderNode(cameraSelector->camera());
            m_renderer->renderNodesManager()->data(nodeHandle)->setParentHandle(m_frameGraphEntityNode);
            createRenderCamera(cameraEntity);
        }
        cameraSelectorNode->setCameraEntity(cameraEntity);
        qCDebug(Backend) << Q_FUNC_INFO << "CameraSelector" << cameraSelectorNode->cameraEntity();
        if (cameraSelectorNode->cameraEntity() == Q_NULLPTR ||
                cameraSelectorNode->cameraEntity()->componentsOfType<CameraLens>().isEmpty())
            qCWarning(Backend) << Q_FUNC_INFO << "No camera or camera lens present in the referenced entity";
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

HRenderNode RenderSceneBuilder::createRenderNode(Node *node)
{
    Entity *entity = qobject_cast<Entity *>(node);
    HRenderNode renderNodeHandle;
    if (entity != Q_NULLPTR)
        renderNodeHandle = m_renderer->renderNodesManager()->getOrAcquireHandle(entity->uuid());
    else
        renderNodeHandle = m_renderer->renderNodesManager()->acquire();
    RenderNode *renderNode = m_renderer->renderNodesManager()->data(renderNodeHandle);
    renderNode->setHandle(renderNodeHandle);
    renderNode->setRenderer(m_renderer);
    renderNode->setFrontEndPeer(node);

    if (entity != Q_NULLPTR) {
        QList<Transform *> transforms = entity->componentsOfType<Transform>();
        if (!transforms.isEmpty())
            m_renderer->renderNodesManager()->data(renderNodeHandle)->setTransform(transforms.first());
    }

    return renderNodeHandle;
}

void RenderSceneBuilder::createRenderCamera(Entity *entity)
{
    QList<CameraLens*> cameraLenses = entity->componentsOfType<CameraLens>();
    if (!cameraLenses.isEmpty()) {
        // Retrieves or create RenderCamera for entity->uuid
        RenderCamera *camera = m_renderer->cameraManager()->getOrCreateRenderCamera(entity->uuid());
        camera->setRendererAspect(m_renderer->rendererAspect());

        // Set the front end peer object for the lens
        camera->setPeer(cameraLenses.first());
        camera->setProjection(cameraLenses.first()->projectionMatrix());
    }
}

void RenderSceneBuilder::createRenderMesh(Entity *entity)
{
    QList<Mesh *> meshes = entity->componentsOfType<Mesh>();
    // Build Meshes from Shapes
    QList<QAbstractShapeMesh *> shapes = entity->componentsOfType<QAbstractShapeMesh>();
    if (!shapes.empty())
       meshes.append(shapes.first()->mesh());
    if (!meshes.isEmpty()) {
        HMesh meshHandle = m_renderer->meshManager()->getOrAcquireHandle(entity->uuid());
        RenderMesh *renderMesh = m_renderer->meshManager()->data(meshHandle);
        renderMesh->setRendererAspect(m_renderer->rendererAspect());
        renderMesh->setPeer(meshes.first());
        // That should be done here initially as we might have
        // missed prior QChangeArbiter notifications
        m_renderer->meshDataManager()->addMeshData(meshes.first());
        // If MeshData is updated afterward, it should be handled directly by a notification
        // Through the QChangeArbiter
    }
}

void RenderSceneBuilder::createRenderMaterial(Entity *entity)
{
    // Parse Materials to retrieve
    // Material
    // Using Material, we can create the following at runtime
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
}

void RenderSceneBuilder::createFrameGraph(FrameGraph *fg)
{
    // Entity has a reference to a framegraph configuration
    // Build a tree of FrameGraphNodes by reading the tree of FrameGraphBuildingBlocks
    Render::FrameGraphNode* frameGraphRootNode = buildFrameGraph(qobject_cast<Node*>(fg->activeFrameGraph()));
    qCDebug(Backend) << Q_FUNC_INFO << "FrameGraphRoot" <<  frameGraphRootNode;
    m_renderer->setFrameGraphRoot(frameGraphRootNode);
}

void RenderSceneBuilder::visitEntity(Qt3D::Entity *entity)
{
    // Create a RenderNode corresponding to the Entity. Most data will
    // be calculated later by jobs
    qCDebug(Backend) << Q_FUNC_INFO << "Entity " << entity->objectName();
    // Retrieve or created RenderNode for entity

    // Create RenderNode and Transforms
    HRenderNode renderNodeHandle = createRenderNode(entity);

    if (m_rootNodeHandle.isNull()) {
        m_rootNodeHandle = renderNodeHandle;
        m_nodeStack.push(m_rootNodeHandle);
    }
    else {
        m_renderer->renderNodesManager()->data(renderNodeHandle)->setParentHandle(m_nodeStack.top());
    }
    // REPLACE WITH ENTITY MATRIX FROM TRANSFORMS
    m_nodeStack.push(renderNodeHandle);

    // Retrieve Material from Entity
    createRenderMaterial(entity);
    // Retrieve Mesh from Entity
    createRenderMesh(entity);
    // Retrieve Camera from Entity
    createRenderCamera(entity);

    // Check if entity is a Scene and if so parses the scene
    QAbstractScene *sceneEntity = Q_NULLPTR;
    if ((sceneEntity = qobject_cast<QAbstractScene *>(entity)) != Q_NULLPTR) {
        qDebug() << Q_FUNC_INFO << "Found a Scene";
    }

    NodeVisitor::visitEntity(entity);

    // Coming back up the tree
    m_nodeStack.pop();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
