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

#include "rendereraspect.h"

#include <Qt3DRenderer/private/rendermesh_p.h>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/scenemanager_p.h>

#include <Qt3DRenderer/qabstractlight.h>
#include <Qt3DRenderer/qabstractsceneloader.h>
#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qlayer.h>
#include <Qt3DRenderer/qlayerfilter.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qparametermapper.h>
#include <Qt3DRenderer/qrenderpassfilter.h>
#include <Qt3DRenderer/qrendertargetselector.h>
#include <Qt3DRenderer/qtechniquefilter.h>
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DRenderer/renderlogging.h>
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DRenderer/qclearbuffer.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DCore/qcameralens.h>

#include <Qt3DRenderer/private/cameraselectornode_p.h>
#include <Qt3DRenderer/private/layerfilternode_p.h>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/renderannotation_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/renderpassfilternode_p.h>
#include <Qt3DRenderer/private/rendertargetselectornode_p.h>
#include <Qt3DRenderer/private/techniquefilternode_p.h>
#include <Qt3DRenderer/private/viewportnode_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/scenemanager_p.h>
#include <Qt3DRenderer/private/clearbuffer_p.h>
#include <Qt3DRenderer/private/sortmethod_p.h>
#include <Qt3DRenderer/private/sortcriterion_p.h>
#include <Qt3DRenderer/private/rendernodefunctor_p.h>
#include <Qt3DRenderer/private/framegraphnode_p.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/nodevisitor.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <loadmeshdatajob.h>
#include <updateworldtransformjob.h>
#include <updateboundingvolumejob.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qaspectmanager_p.h>

#include <QDebug>
#include <QThread>
#include <QWindow>

QT_BEGIN_NAMESPACE

namespace Qt3D {

RendererAspect::RendererAspect(QObject *parent)
    : QAbstractAspect(QAbstractAspect::AspectRenderer, parent)
    , m_renderer(new Render::Renderer())
{
    // Won't return until the private RenderThread in Renderer has been created
    // The Renderer is set to wait the surface with a wait condition
    // Threads modifying the Renderer should be synchronized using the Renderer's mutex
    registerBackendType<QEntity>(QBackendNodeFunctorPtr(new Render::RenderEntityFunctor(m_renderer)));
    registerBackendType<QTransform>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTransform, Render::TransformManager>(m_renderer->transformManager())));
    registerBackendType<QMaterial>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderMaterial, Render::MaterialManager>(m_renderer->materialManager())));
    registerBackendType<QTechnique>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTechnique, Render::TechniqueManager>(m_renderer->techniqueManager())));
    registerBackendType<QTexture>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTexture, Render::TextureManager>(m_renderer->textureManager())));
    registerBackendType<QShaderProgram>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderShader, Render::ShaderManager>(m_renderer->shaderManager())));
    registerBackendType<QEffect>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderEffect, Render::EffectManager>(m_renderer->effectManager())));
    registerBackendType<QAnnotation>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderAnnotation, Render::CriterionManager>(m_renderer->criterionManager())));
    registerBackendType<QCameraLens>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderCameraLens, Render::CameraManager>(m_renderer->cameraManager())));
    registerBackendType<QAbstractLight>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderLight, Render::LightManager>(m_renderer->lightManager())));
    registerBackendType<QLayer>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderLayer, Render::LayerManager>(m_renderer->layerManager())));
    registerBackendType<QAbstractMesh>(QBackendNodeFunctorPtr(new Render::RenderMeshCreatorFunctor(m_renderer->meshManager(), m_renderer->meshDataManager())));
    registerBackendType<QRenderPass>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderRenderPass, Render::RenderPassManager>(m_renderer->renderPassManager())));
    registerBackendType<Render::QAbstractSceneLoader>(QBackendNodeFunctorPtr(new Render::RenderSceneFunctor(m_renderer->sceneManager())));
    registerBackendType<QRenderTarget>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderTarget, Render::RenderTargetManager>(m_renderer->renderTargetManager())));
    registerBackendType<QRenderAttachment>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderAttachment, Render::AttachmentManager>(m_renderer->attachmentManager())));
    registerBackendType<QSortCriterion>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::SortCriterion, Render::SortCriterionManager>(m_renderer->sortCriterionManager())));
    registerBackendType<QClearBuffer>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::ClearBuffer, QClearBuffer>(m_renderer->frameGraphManager())));
    registerBackendType<QTechniqueFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::TechniqueFilter, QTechniqueFilter>(m_renderer->frameGraphManager())));
    registerBackendType<QViewport>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::ViewportNode, QViewport>(m_renderer->frameGraphManager())));
    registerBackendType<QRenderPassFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::RenderPassFilter, QRenderPassFilter>(m_renderer->frameGraphManager())));
    registerBackendType<QCameraSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::CameraSelector, QCameraSelector>(m_renderer->frameGraphManager())));
    registerBackendType<QRenderTargetSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::RenderTargetSelector, QRenderTargetSelector>(m_renderer->frameGraphManager())));
    registerBackendType<QLayerFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::LayerFilterNode, QLayerFilter>(m_renderer->frameGraphManager())));
    registerBackendType<QSortMethod>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::SortMethod, QSortMethod>(m_renderer->frameGraphManager())));
    registerBackendType<QFrameGraph>(QBackendNodeFunctorPtr(new Render::FrameGraphComponentFunctor(m_renderer)));
    registerBackendType<QParameter>(QBackendNodeFunctorPtr(new Render::RenderNodeFunctor<Render::RenderParameter, Render::ParameterManager>(m_renderer->parameterManager())));
}

QVector<QJobPtr> RendererAspect::jobsToExecute()
{
    // Create jobs that will get exectued by the threadpool
    QVector<QJobPtr> jobs;

    // Create jobs to load in any meshes that are pending
    if (m_renderer != Q_NULLPTR) {
        QHash<QUuid, QAbstractMeshFunctorPtr> meshSources = m_renderer->meshDataManager()->meshesPending();
        Q_FOREACH (const QUuid &meshId, meshSources.keys()) {
            Render::LoadMeshDataJobPtr loadMeshJob(new Render::LoadMeshDataJob(meshSources[meshId], meshId));
            loadMeshJob->setRenderer(m_renderer);
            jobs.append(loadMeshJob);
        }

        // TO DO: Have 2 jobs queue
        // One for urgent jobs that are mandatory for the rendering of a frame
        // Another for jobs that can span across multiple frames (Scene/Mesh loading)
        QVector<Render::LoadSceneJobPtr> sceneJobs = m_renderer->sceneManager()->pendingSceneLoaderJobs();
        Q_FOREACH (Render::LoadSceneJobPtr job, sceneJobs) {
            job->setRenderer(m_renderer);
            jobs.append(job);
        }

        // Create jobs to update transforms and bounding volumes
        Render::UpdateWorldTransformJobPtr worldTransformJob(new Render::UpdateWorldTransformJob(m_renderer->renderSceneRoot()));
        Render::UpdateBoundingVolumeJobPtr boundingVolumeJob(new Render::UpdateBoundingVolumeJob(m_renderer->renderSceneRoot()));

        // We can only update bounding volumes once all world transforms are known
        boundingVolumeJob->addDependency(worldTransformJob);

        // Add all jobs to queue
        jobs.append(worldTransformJob);
        jobs.append(boundingVolumeJob);

        // Traverse the current framegraph and create jobs to populate
        // RenderBins with RenderCommands
        QVector<QJobPtr> renderBinJobs = m_renderer->createRenderBinJobs();
        // TODO: Add wrapper around ThreadWeaver::Collection
        for (int i = 0; i < renderBinJobs.size(); ++i) {
            QJobPtr renderBinJob = renderBinJobs.at(i);
            renderBinJob->addDependency(boundingVolumeJob);
            jobs.append(renderBinJob);
        }
    }
    return jobs;
}

void RendererAspect::sceneNodeAdded(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNode *n = propertyChange->value().value<QNodePtr>().data();
    NodeVisitor visitor;
    visitor.traverse(n, this, &RendererAspect::visitNode, &RendererAspect::visitNode);
}

void RendererAspect::sceneNodeRemoved(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNode *n = propertyChange->value().value<QNodePtr>().data();
    QAbstractAspect::clearBackendNode(n);
}

void RendererAspect::setRootEntity(QEntity *rootObject)
{
    // setSceneGraphRoot is synchronized using the Renderer's mutex
    NodeVisitor visitor;
    visitor.traverse(rootObject, this, &RendererAspect::visitNode, &RendererAspect::visitNode);
    m_renderer->setSceneGraphRoot(m_renderer->renderNodesManager()->lookupResource(rootObject->uuid()));
}

void RendererAspect::onInitialize(QSurface *surface)
{
    m_renderer->setRendererAspect(this);
    m_renderer->createAllocators();
    // setSurface is synchronized using the Renderer's mutex
    m_renderer->setSurface(surface);
}

void RendererAspect::onCleanup()
{
    delete m_renderer;
    //Render::Renderer *renderer = m_renderThread->renderer();
    //QMetaObject::invokeMethod(renderer, "cleanup");
}

void RendererAspect::visitNode(QNode *node)
{
    QAbstractAspect::createBackendNode(node);
}

} // Qt3D

QT_END_NAMESPACE
