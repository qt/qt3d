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
    QFrameGraph *fg = QEntity::findComponentInTree<QFrameGraph>(m_renderer->sceneGraphRoot());
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
        qCDebug(Backend) << Q_FUNC_INFO << "TechniqueFilter";
        return createBackendFrameGraphNode<TechniqueFilter, QTechniqueFilter>(block);
    }
    else if (qobject_cast<Qt3D::QViewport*>(block) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << "Viewport";
        return createBackendFrameGraphNode<ViewportNode, QViewport>(block);
    }
    else if (qobject_cast<Qt3D::QRenderPassFilter*>(block) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << "RenderPassFilter";
        return createBackendFrameGraphNode<RenderPassFilter, QRenderPassFilter>(block);
    }
    else if (qobject_cast<Qt3D::QCameraSelector*>(block) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << "CameraSelector";
        return createBackendFrameGraphNode<CameraSelector, QCameraSelector>(block);
    }
    else if (qobject_cast<Qt3D::QRenderTargetSelector*>(block) != Q_NULLPTR) {
        Qt3D::QRenderTargetSelector *renderTargetSelector = qobject_cast<Qt3D::QRenderTargetSelector*>(block);
        Render::RenderTargetSelector *renderTargetSelectorNode = new Render::RenderTargetSelector();

        // TO DO
        qCDebug(Backend) << Q_FUNC_INFO << "RenderTargetSelector";
        return renderTargetSelectorNode;
    }
    else if (qobject_cast<Qt3D::QLayerFilter*>(block) != Q_NULLPTR) {
        qCDebug(Backend) << Q_FUNC_INFO << "LayerFilter";
        return createBackendFrameGraphNode<LayerFilterNode, QLayerFilter>(block);
    }
    return Q_NULLPTR;
}

RenderEntity* RenderSceneBuilder::createRenderNode(QEntity *entity)
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

    return renderNode;
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
    RenderEntity *rEntity = createRenderNode(entity);

    if (m_rootNodeHandle.isNull())
        m_rootNodeHandle = rEntity->handle();
    else
        rEntity->setParentHandle(m_nodeStack.top());
    m_nodeStack.push(rEntity->handle());

    // TO DO : See if it can be converted to work with createRenderElement

    QList<QMaterial *> materials = entity->componentsOfType<QMaterial>();
    QList<QAbstractMesh *> meshes = entity->componentsOfType<QAbstractMesh>();
    QList<QCameraLens *> lenses = entity->componentsOfType<QCameraLens>();
    QList<QLayer *> layers = entity->componentsOfType<QLayer>();
    QList<QAbstractLight *> lights = entity->componentsOfType<QAbstractLight>();

    // Retrieve Material from Entity
    if (!materials.isEmpty())
        rEntity->createRenderComponent<QMaterial>(materials.first());
    // Retrieve Mesh from Entity
    if (!meshes.isEmpty())
        rEntity->createRenderComponent<QAbstractMesh>(meshes.first());
    // Retrieve Camera from Entity
    if (!lenses.isEmpty())
        rEntity->createRenderComponent<QCameraLens>(lenses.first());
    // Retrieve Layer from Entity
    if (!layers.isEmpty())
        rEntity->createRenderComponent<QLayer>(layers.first());
    // Retrieve Lights from Entity
    if (!lights.isEmpty())
        rEntity->createRenderComponent<QAbstractLight>(lights.first());

    NodeVisitor::visitEntity(entity);

    // Coming back up the tree
    m_nodeStack.pop();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
