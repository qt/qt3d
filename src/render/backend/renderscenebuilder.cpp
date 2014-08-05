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
#include "techniquemanager.h"
#include "effectmanager.h"
#include "renderpassmanager.h"
#include "renderer.h"
#include "renderentity.h"
#include "renderlogging.h"
#include "materialmanager.h"
#include "qabstractscene.h"
#include "qparameter.h"
#include "qparametermapper.h"
#include "rendercriterion.h"
#include "criterionmanager.h"
#include "rendereraspect.h"
#include "transformmanager.h"
#include "shadermanager.h"
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
#include <Qt3DCore/qaspectmanager.h>

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
    // Retrieve and set Renderer FrameGraph
    QFrameGraph *fg = QEntity::findComponentInTree<QFrameGraph>(m_renderer->sceneGraphRoot());
    Q_ASSERT_X(fg, Q_FUNC_INFO, "No FrameGraph component found. The FrameGraph defines the renderer configuration. Cannot continue");
    //    m_frameGraphEntityNode = m_renderer->renderNodesManager()->lookupHandle(fg->parentNode()->asEntity()->uuid());
    //    createFrameGraph(fg);
    if (fg->activeFrameGraph() != Q_NULLPTR) {
        buildFrameGraph(fg->activeFrameGraph());
        FrameGraphNode **fgRootNode = m_renderer->frameGraphManager()->lookupResource(fg->activeFrameGraph()->uuid());
        Q_ASSERT(fgRootNode);
        m_renderer->setFrameGraphRoot(*fgRootNode);
    }
    else
        qWarning() << Q_FUNC_INFO << "ActiveFrameGraph not set";
}

void RenderSceneBuilder::buildFrameGraph(QFrameGraphItem *root)
{
    createRenderElement(root);
    Q_FOREACH (QFrameGraphItem *child, root->frameGraphChildren())
        buildFrameGraph(child);
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

void RenderSceneBuilder::createRenderElement(QNode *frontend)
{
    if (qobject_cast<QEntity *>(frontend)) {
        QEntity *entity = qobject_cast<QEntity *>(frontend);
        QEntity *parentEntity = entity->parentEntity();
        RenderEntity *rEntity = createRenderNode(entity);
        if (parentEntity)
            rEntity->setParentHandle(m_renderer->renderNodesManager()->lookupHandle(parentEntity->uuid()));
        if (m_rootNodeHandle.isNull())
            m_rootNodeHandle = rEntity->handle();
    }
    else if (qobject_cast<QTransform *>(frontend)) {
        createRenderElementHelper<QTransform, RenderTransform, TransformManager>(frontend,
                                                                                m_renderer->transformManager());
    }
    else if (qobject_cast<QAbstractMesh *>(frontend)) {
        createRenderElementHelper<QAbstractMesh, RenderMesh, MeshManager>(frontend,
                                                                          m_renderer->meshManager());
    }
    else if (qobject_cast<QCameraLens *>(frontend)) {
        createRenderElementHelper<QCameraLens, RenderCameraLens, CameraManager>(frontend,
                                                                                m_renderer->cameraManager());
    }
    else if (qobject_cast<QLayer *>(frontend)) {
        createRenderElementHelper<QLayer, RenderLayer, LayerManager>(frontend,
                                                                     m_renderer->layerManager());
    }
    else if (qobject_cast<QAbstractLight *>(frontend)) {
        createRenderElementHelper<QAbstractLight, RenderLight, LightManager>(frontend,
                                                                             m_renderer->lightManager());
    }
    else if (qobject_cast<QMaterial *>(frontend)) {
        createRenderElementHelper<QMaterial, RenderMaterial, MaterialManager>(frontend,
                                                                              m_renderer->materialManager());
    }
    else if (qobject_cast<QTechnique *>(frontend)) {
        createRenderElementHelper<QTechnique, RenderTechnique, TechniqueManager>(frontend,
                                                                                 m_renderer->techniqueManager());
    }
    else if (qobject_cast<QAbstractEffect *>(frontend)) {
        createRenderElementHelper<QAbstractEffect, RenderEffect, EffectManager>(frontend,
                                                                                m_renderer->effectManager());
    }
    else if (qobject_cast<QRenderPass *>(frontend)) {
        createRenderElementHelper<QRenderPass, RenderRenderPass, RenderPassManager>(frontend,
                                                                                    m_renderer->renderPassManager());
    }
    else if (qobject_cast<QAbstractShader *>(frontend)) {
        createRenderElementHelper<QShaderProgram, RenderShader, ShaderManager>(frontend,
                                                                               m_renderer->shaderManager());
    }
    else if (qobject_cast<QParameter *>(frontend)) {

    }
    else if (qobject_cast<QParameterMapper *>(frontend)) {

    }
    else if (qobject_cast<QCriterion *>(frontend)) {
        createRenderElementHelper<QCriterion, RenderCriterion, CriterionManager>(frontend,
                                                                                 m_renderer->criterionManager());
    }
    else if (qobject_cast<QFrameGraphItem *>(frontend)) {
        FrameGraphNode *fgNode = backendFrameGraphNode(frontend);
        if (qobject_cast<QFrameGraphItem *>(frontend->parentNode()))
            fgNode->setParentHandle(m_renderer->frameGraphManager()->lookupHandle(frontend->parentNode()->uuid()));
    }
}

void RenderSceneBuilder::releaseRenderElement(QNode *frontend)
{
    if (qobject_cast<QEntity *>(frontend))
        m_renderer->renderNodesManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QAbstractMesh *>(frontend))
        m_renderer->meshDataManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QCameraLens *>(frontend))
        m_renderer->cameraManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QLayer *>(frontend))
        m_renderer->layerManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QAbstractLight *>(frontend))
        m_renderer->lightManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QMaterial *>(frontend))
        m_renderer->materialManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QTechnique *>(frontend))
        m_renderer->techniqueManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QAbstractEffect *>(frontend))
        m_renderer->effectManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QAbstractRenderPass *>(frontend))
        m_renderer->renderPassManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QCriterion *>(frontend))
        m_renderer->criterionManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QFrameGraphItem *>(frontend))
        m_renderer->frameGraphManager()->releaseResource(frontend->uuid());
    else if (qobject_cast<QAbstractShader *>(frontend))
        m_renderer->shaderManager()->releaseResource(frontend->uuid());
}

RenderEntity* RenderSceneBuilder::createRenderNode(QEntity *entity)
{
    HRenderNode renderNodeHandle;
    renderNodeHandle = m_renderer->renderNodesManager()->getOrAcquireHandle(entity->uuid());
    RenderEntity *rEntity = m_renderer->renderNodesManager()->data(renderNodeHandle);
    rEntity->setHandle(renderNodeHandle);
    rEntity->setRenderer(m_renderer);
    rEntity->setPeer(entity);

    QList<QTransform *> transforms = entity->componentsOfType<QTransform>();
    QList<QMaterial *> materials = entity->componentsOfType<QMaterial>();
    QList<QAbstractMesh *> meshes = entity->componentsOfType<QAbstractMesh>();
    QList<QCameraLens *> lenses = entity->componentsOfType<QCameraLens>();
    QList<QLayer *> layers = entity->componentsOfType<QLayer>();
    QList<QAbstractLight *> lights = entity->componentsOfType<QAbstractLight>();

    // Retrieve Transform from Entity
    if (!transforms.isEmpty()) {
        createRenderElement(transforms.first());
        rEntity->addComponent(transforms.first());
    }
    // Retrieve Material from Entity
    if (!materials.isEmpty()) {
        createRenderElement(materials.first());
        rEntity->addComponent(materials.first());
    }
    // Retrieve Mesh from Entity
    if (!meshes.isEmpty()) {
        createRenderElement(meshes.first());
        rEntity->addComponent(meshes.first());
    }
    // Retrieve Camera from Entity
    if (!lenses.isEmpty()) {
        createRenderElement(lenses.first());
        rEntity->addComponent(lenses.first());
    }

    // Lights and Layers do not have to be limited to one per entity

    // Retrieve Layer from Entity
    if (!layers.isEmpty()) {
        createRenderElement(layers.first());
        rEntity->addComponent(layers.first());
    }
    // Retrieve Lights from Entity
    if (!lights.isEmpty()) {
        createRenderElement(lights.first());
        rEntity->addComponent(lights.first());
    }

    return rEntity;
}

void RenderSceneBuilder::visitNode(QNode *nd)
{
    // Each real frontend node is registered with the QChangeArbiter
    nd->registerObserver(m_renderer->rendererAspect()->aspectManager()->changeArbiter());
    createRenderElement(nd);
    NodeVisitor::visitNode(nd);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
