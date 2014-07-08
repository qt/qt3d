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

#include "entitymanager.h"
#include "meshdatamanager.h"
#include "meshmanager.h"
#include "cameramanager.h"
#include "layermanager.h"
#include "lightmanager.h"
#include "renderer.h"
#include "renderentity.h"
#include "renderlogging.h"
#include "materialmanager.h"
#include "qabstractscene.h"
#include <qmaterial.h>
#include <qmesh.h>
#include <qabstractshapemesh.h>
#include <qlayer.h>
#include <qabstractlight.h>

#include <qframegraph.h>
#include <qviewport.h>
#include <qtechniquefilter.h>
#include <qrenderpassfilter.h>
#include <qcameraselector.h>
#include <qrendertargetselector.h>
#include <qlayerfilter.h>

#include <techniquefilternode.h>
#include <cameraselectornode.h>
#include <renderpassfilternode.h>
#include <viewportnode.h>
#include <rendertargetselectornode.h>
#include <layerfilternode.h>


#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderSceneBuilder::RenderSceneBuilder(Renderer *renderer)
    : Qt3D::NodeVisitor()
    , m_renderer(renderer)
{
}

RenderEntity *RenderSceneBuilder::rootNode() const
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
    QFrameGraph *fg = QEntity::findComponentInTree<QFrameGraph>(rootNode()->frontEndPeer());
    Q_ASSERT_X(fg, Q_FUNC_INFO, "No FrameGraph component found. The FrameGraph defines the renderer configuration. Cannot continue");
    m_frameGraphEntityNode = m_renderer->renderNodesManager()->lookupHandle(fg->parentNode()->asEntity()->uuid());
    createFrameGraph(fg);
}

/*!
 * Returns a FrameGraphNode and all its children from \a node which points to the activeFrameGraph.
 * Returns Q_NULLPTR if \a is also Q_NULLPTR or if there is no FrameGraphComponent
 * in node's children tree.
 */
Render::FrameGraphNode *RenderSceneBuilder::buildFrameGraph(QNode *node)
{
    Q_ASSERT(node);

    qCDebug(Backend) << Q_FUNC_INFO << node->objectName();

    Render::FrameGraphNode *fgNode = Q_NULLPTR;
    Qt3D::QFrameGraphItem *fgItem = Q_NULLPTR;

    if ((fgItem = qobject_cast<Qt3D::QFrameGraphItem *>(node)) != Q_NULLPTR) {
        // Instantiate proper backend node corresponding to the frontend node
        fgNode = backendFrameGraphNode(node);
    }

    // we need to travel the node's children tree to find either the FG root Node or
    // its children
    QList<FrameGraphNode *> fgChildNodes;
    Q_FOREACH (QNode *child, node->children()) {
        FrameGraphNode* fgChildNode = buildFrameGraph(child);
        if (fgChildNode != Q_NULLPTR)
            fgChildNodes << fgChildNode;
    }
    if (!fgChildNodes.isEmpty()) {
        if (fgNode == Q_NULLPTR && fgChildNodes.size() == 1) {
            fgNode = fgChildNodes.first();
        }
        else if (fgNode != Q_NULLPTR) {
            Q_FOREACH (FrameGraphNode *fgChildNode, fgChildNodes)
                fgNode->appendChild(fgChildNode);
        }
    }
    return fgNode;
}

/*!
 * Returns a proper FrameGraphNode subclass instance from \a block.
 * If no subclass corresponds, Q_NULLPTR is returned.
 */
Render::FrameGraphNode *RenderSceneBuilder::backendFrameGraphNode(QNode *block)
{
    if (qobject_cast<Qt3D::QTechniqueFilter*>(block) != Q_NULLPTR) {
        Qt3D::QTechniqueFilter* techniqueFilter = qobject_cast<Qt3D::QTechniqueFilter*>(block);
        Render::TechniqueFilter *techniqueFilterNode = new Render::TechniqueFilter();

        qCDebug(Backend) << Q_FUNC_INFO << "TechniqueFilter";
        Q_FOREACH (TechniqueCriterion *criterion, techniqueFilter->criteria())
            techniqueFilterNode->appendFilter(criterion);
        return techniqueFilterNode;
    }
    else if (qobject_cast<Qt3D::QViewport*>(block) != Q_NULLPTR) {
        Qt3D::QViewport *viewport = qobject_cast<Qt3D::QViewport*>(block);
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
    else if (qobject_cast<Qt3D::QRenderPassFilter*>(block) != Q_NULLPTR) {
        Qt3D::QRenderPassFilter *renderPassFilter = qobject_cast<Qt3D::QRenderPassFilter*>(block);
        Render::RenderPassFilter *renderPassFilterNode = new Render::RenderPassFilter();

        qCDebug(Backend) << Q_FUNC_INFO << "RenderPassFilter";
        Q_FOREACH (QRenderPassCriterion *criterion, renderPassFilter->criteria())
            renderPassFilterNode->appendFilter(criterion);
        return renderPassFilterNode;
    }
    else if (qobject_cast<Qt3D::QCameraSelector*>(block) != Q_NULLPTR) {
        Qt3D::QCameraSelector *cameraSelector = qobject_cast<Qt3D::QCameraSelector*>(block);
        Render::CameraSelector *cameraSelectorNode = new Render::CameraSelector();
        cameraSelectorNode->setRenderer(m_renderer);
        cameraSelectorNode->setPeer(cameraSelector);
        // TO DO : We might as well store an Entity in CameraSelector
        QEntity *cameraEntity = cameraSelector->camera()->asEntity();
        // If the Entity is declared inline on the QML Side, the Entity is not set as part of the Scene tree
        // So we need to make sure the RenderNode for the given Entity exists in the RenderNodesMananger
        if (cameraEntity && m_renderer->cameraManager()->lookupHandle(cameraEntity->uuid()).isNull()) {
            HRenderNode nodeHandle = createRenderNode(cameraSelector->camera());
            m_renderer->renderNodesManager()->data(nodeHandle)->setParentHandle(m_frameGraphEntityNode);
            createRenderElement<QCameraLens, RenderCamera, CameraManager>(cameraEntity, m_renderer->cameraManager());
        }
        cameraSelectorNode->setCameraEntity(cameraEntity);
        qCDebug(Backend) << Q_FUNC_INFO << "CameraSelector" << cameraSelectorNode->cameraEntity();
        if (cameraSelectorNode->cameraEntity() == Q_NULLPTR ||
                cameraSelectorNode->cameraEntity()->componentsOfType<QCameraLens>().isEmpty())
            qCWarning(Backend) << Q_FUNC_INFO << "No camera or camera lens present in the referenced entity";
        return cameraSelectorNode;
    }
    else if (qobject_cast<Qt3D::QRenderTargetSelector*>(block) != Q_NULLPTR) {
        Qt3D::QRenderTargetSelector *renderTargetSelector = qobject_cast<Qt3D::QRenderTargetSelector*>(block);
        Render::RenderTargetSelector *renderTargetSelectorNode = new Render::RenderTargetSelector();

        // TO DO
        qCDebug(Backend) << Q_FUNC_INFO << "RenderTargetSelector";
        return renderTargetSelectorNode;
    }
    else if (qobject_cast<Qt3D::QLayerFilter*>(block) != Q_NULLPTR) {
        QLayerFilter *layerFilter = qobject_cast<Qt3D::QLayerFilter *>(block);
        LayerFilterNode *layerFilterNode = new LayerFilterNode();

        layerFilterNode->setRenderer(m_renderer);
        layerFilterNode->setPeer(layerFilter);
        // Done here once, layers are then updated by QChangeArbiter notifications
        layerFilterNode->setLayers(layerFilter->layers());
        return layerFilterNode;
    }
    return Q_NULLPTR;
}

HRenderNode RenderSceneBuilder::createRenderNode(QEntity *entity)
{
    HRenderNode renderNodeHandle;
    renderNodeHandle = m_renderer->renderNodesManager()->getOrAcquireHandle(entity->uuid());
    RenderEntity *renderNode = m_renderer->renderNodesManager()->data(renderNodeHandle);
    renderNode->setHandle(renderNodeHandle);
    renderNode->setRenderer(m_renderer);
    renderNode->setPeer(entity);

    QList<QTransform *> transforms = entity->componentsOfType<QTransform>();
    if (!transforms.isEmpty())
        renderNode->setTransform(transforms.first());

    return renderNodeHandle;
}

void RenderSceneBuilder::createRenderMaterial(QEntity *entity)
{
    // Parse Materials to retrieve
    // Material
    // Using Material, we can create the following at runtime
    // Effect
    // Techniques
    // RenderPass
    // ShaderPrograms
    QList<QMaterial *> materials = entity->componentsOfType<QMaterial>();
    foreach (QMaterial *material, materials) {
        HMaterial rMaterialHandle = m_renderer->materialManager()->getOrAcquireHandle(material);
        m_renderer->materialManager()->linkMaterialToEntity(entity->uuid(), rMaterialHandle);
        RenderMaterial *rMaterial = m_renderer->materialManager()->data(rMaterialHandle);
        rMaterial->setRendererAspect(m_renderer->rendererAspect());
        rMaterial->setPeer(material);
    }
}

void RenderSceneBuilder::createFrameGraph(QFrameGraph *fg)
{
    // Entity has a reference to a framegraph configuration
    // Build a tree of FrameGraphNodes by reading the tree of FrameGraphBuildingBlocks
    Render::FrameGraphNode* frameGraphRootNode = buildFrameGraph(qobject_cast<QNode*>(fg->activeFrameGraph()));
    qCDebug(Backend) << Q_FUNC_INFO << "FrameGraphRoot" <<  frameGraphRootNode;
    m_renderer->setFrameGraphRoot(frameGraphRootNode);
}

void RenderSceneBuilder::visitEntity(Qt3D::QEntity *entity)
{
    // Create a RenderNode corresponding to the Entity. Most data will
    // be calculated later by jobs
    qCDebug(Backend) << Q_FUNC_INFO << "Entity " << entity->objectName();
    // Retrieve or created RenderNode for entity

    // Create RenderNode and Transforms
    HRenderNode renderNodeHandle = createRenderNode(entity);

    if (m_rootNodeHandle.isNull())
        m_rootNodeHandle = renderNodeHandle;
    else
        m_renderer->renderNodesManager()->data(renderNodeHandle)->setParentHandle(m_nodeStack.top());
    m_nodeStack.push(renderNodeHandle);

    // Retrieve Material from Entity
    // TO DO : See if it can be converted to work with createRenderElement
    createRenderMaterial(entity);

    // Retrieve Mesh from Entity
    createRenderElement<QAbstractMesh, RenderMesh, MeshManager>(entity, m_renderer->meshManager());
    // Retrieve Camera from Entity
    createRenderElement<QCameraLens, RenderCamera, CameraManager>(entity, m_renderer->cameraManager());
    // Retrieve Layer from Entity
    createRenderElement<QLayer, RenderLayer, LayerManager>(entity, m_renderer->layerManager());
    // Retrieve Lights from Entity
    createRenderElement<QAbstractLight, RenderLight, LightManager>(entity, m_renderer->lightManager());

    NodeVisitor::visitEntity(entity);

    // Coming back up the tree
    m_nodeStack.pop();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
