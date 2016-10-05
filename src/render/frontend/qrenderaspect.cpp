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

#include "qrenderaspect.h"
#include "qrenderaspect_p.h"

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>

#include <Qt3DRender/qabstractsceneloader.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qframegraph.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qparametermapping.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qclearbuffer.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qstateset.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qboundingvolumedebug.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qlight.h>
#include <Qt3DRender/qlighting.h>
#include <Qt3DRender/qdispatchcompute.h>

#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/annotation_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/rendertargetselectornode_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/clearbuffer_p.h>
#include <Qt3DRender/private/sortmethod_p.h>
#include <Qt3DRender/private/sortcriterion_p.h>
#include <Qt3DRender/private/framegraphsubtreeselector_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/nodefunctor_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/loadtexturedatajob_p.h>
#include <Qt3DRender/private/textureimage_p.h>
#include <Qt3DRender/private/statesetnode_p.h>
#include <Qt3DRender/private/nodraw_p.h>
#include <Qt3DRender/private/vsyncframeadvanceservice_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/boundingvolumedebug_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/calcgeometrytrianglevolumes_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/loadbufferjob_p.h>
#include <Qt3DRender/private/loadgeometryjob_p.h>
#include <Qt3DRender/private/qsceneparserfactory_p.h>
#include <Qt3DRender/private/frustumculling_p.h>
#include <Qt3DRender/private/light_p.h>
#include <Qt3DRender/private/lighting_p.h>
#include <Qt3DRender/private/dispatchcompute_p.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qservicelocator_p.h>

#include <QDebug>
#include <QOffscreenSurface>
#include <QThread>
#include <QWindow>

static void initResources()
{
    Q_INIT_RESOURCE(render);
}

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QRenderAspectPrivate::QRenderAspectPrivate(QRenderAspect::RenderType type)
    : QAbstractAspectPrivate()
    , m_nodeManagers(new Render::NodeManagers())
    , m_renderer(Q_NULLPTR)
    , m_surfaceEventFilter(new Render::PlatformSurfaceFilter())
    , m_surface(Q_NULLPTR)
    , m_initialized(false)
    , m_framePreparationJob(new Render::FramePreparationJob(m_nodeManagers))
    , m_cleanupJob(new Render::FrameCleanupJob(m_nodeManagers))
    , m_worldTransformJob(new Render::UpdateWorldTransformJob())
    , m_updateBoundingVolumeJob(new Render::UpdateBoundingVolumeJob())
    , m_calculateBoundingVolumeJob(new Render::CalculateBoundingVolumeJob(m_nodeManagers))
{
    initResources();

    // Load the scene parsers
    loadSceneParsers();

    // Create jobs to update transforms and bounding volumes
    // We can only update bounding volumes once all world transforms are known
    m_updateBoundingVolumeJob->addDependency(m_worldTransformJob);
    m_framePreparationJob->addDependency(m_worldTransformJob);

    // All world stuff depends on the RenderEntity's localBoundingVolume
    m_worldTransformJob->addDependency(m_calculateBoundingVolumeJob);

    // Create property renderer implementation given
    // a targeted rendering API -> only OpenGL for now
    m_renderer = new Render::Renderer(type);
    m_renderer->setNodeManagers(m_nodeManagers);
    m_surfaceEventFilter->setRenderer(m_renderer);
}

void QRenderAspectPrivate::setSurface(QSurface *surface)
{
    if (m_surface == surface)
        return;

    m_surface = surface;

    // If we have a new surface, install the platform surface event filter onto it
    // so that we get informed when the underlying platform surface is about to be
    // deleted and we can tell the renderer about it before it's too late.
    if (m_surface) {
        bool hasPlatformSurface = false;
        switch (m_surface->surfaceClass()) {
        case QSurface::Window: {
            QWindow *window = static_cast<QWindow *>(m_surface);
            m_surfaceEventFilter->setWindow(window);
            hasPlatformSurface = (window->handle() != Q_NULLPTR);
            break;
        }

        case QSurface::Offscreen: {
            QOffscreenSurface *offscreen = static_cast<QOffscreenSurface *>(m_surface);
            m_surfaceEventFilter->setOffscreenSurface(offscreen);
            hasPlatformSurface = (offscreen->handle() != Q_NULLPTR);
            break;
        }
        }

        // If the window/offscreen surface has a native surface, tell the renderer
        if (hasPlatformSurface)
            m_renderer->setSurface(surface);
    }
}

QRenderAspect::QRenderAspect(QObject *parent)
    : QAbstractAspect(*new QRenderAspectPrivate(Threaded), parent)
{
    // Won't return until the private RenderThread in Renderer has been created
    // The Renderer is set to wait the surface with a wait condition
    // Threads modifying the Renderer should be synchronized using the Renderer's mutex
    registerBackendTypes();
}

QRenderAspect::QRenderAspect(QRenderAspect::RenderType type, QObject *parent)
    : QAbstractAspect(*new QRenderAspectPrivate(type), parent)
{
    registerBackendTypes();
}

/*! \internal */
QRenderAspect::QRenderAspect(QRenderAspectPrivate &dd, QObject *parent)

    : QAbstractAspect(dd, parent)
{
    registerBackendTypes();
}

void QRenderAspect::registerBackendTypes()
{
    Q_D(QRenderAspect);

    registerBackendType<Qt3DCore::QEntity>(QBackendNodeFunctorPtr(new Render::RenderEntityFunctor(d->m_nodeManagers)));
    registerBackendType<Qt3DCore::QTransform>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Transform, Render::TransformManager>(d->m_nodeManagers->transformManager())));
    registerBackendType<QMaterial>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Material, Render::MaterialManager>(d->m_nodeManagers->materialManager())));
    registerBackendType<QTechnique>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Technique, Render::TechniqueManager>(d->m_nodeManagers->techniqueManager())));
    registerBackendType<QAbstractTextureProvider>(QBackendNodeFunctorPtr(new Render::TextureFunctor(d->m_nodeManagers->textureManager(), d->m_nodeManagers->textureImageManager(), d->m_nodeManagers->textureDataManager())));
    registerBackendType<QShaderProgram>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Shader, Render::ShaderManager>(d->m_nodeManagers->shaderManager())));
    registerBackendType<QEffect>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Effect, Render::EffectManager>(d->m_nodeManagers->effectManager())));
    registerBackendType<QAnnotation>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Annotation, Render::CriterionManager>(d->m_nodeManagers->criterionManager())));
    registerBackendType<Qt3DCore::QCameraLens>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::CameraLens, Render::CameraManager>(d->m_nodeManagers->cameraManager())));
    registerBackendType<QLayer>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Layer, Render::LayerManager>(d->m_nodeManagers->layerManager())));
    registerBackendType<QRenderPass>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::RenderPass, Render::RenderPassManager>(d->m_nodeManagers->renderPassManager())));
    registerBackendType<QAbstractSceneLoader>(QBackendNodeFunctorPtr(new Render::RenderSceneFunctor(d->m_nodeManagers->sceneManager())));
    registerBackendType<QRenderTarget>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::RenderTarget, Render::RenderTargetManager>(d->m_nodeManagers->renderTargetManager())));
    registerBackendType<QRenderAttachment>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::RenderAttachment, Render::AttachmentManager>(d->m_nodeManagers->attachmentManager())));
    registerBackendType<QSortCriterion>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::SortCriterion, Render::SortCriterionManager>(d->m_nodeManagers->sortCriterionManager())));
    registerBackendType<QClearBuffer>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::ClearBuffer, QClearBuffer>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QTechniqueFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::TechniqueFilter, QTechniqueFilter>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QViewport>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::ViewportNode, QViewport>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QRenderPassFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::RenderPassFilter, QRenderPassFilter>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QCameraSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::CameraSelector, QCameraSelector>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QRenderTargetSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::RenderTargetSelector, QRenderTargetSelector>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QLayerFilter>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::LayerFilterNode, QLayerFilter>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QSortMethod>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::SortMethod, QSortMethod>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QFrameGraphSelector>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::FrameGraphSubtreeSelector, QFrameGraphSelector>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QFrameGraph>(QBackendNodeFunctorPtr(new Render::FrameGraphComponentFunctor(d->m_renderer)));
    registerBackendType<QParameter>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Parameter, Render::ParameterManager>(d->m_nodeManagers->parameterManager())));
    registerBackendType<QShaderData>(QBackendNodeFunctorPtr(new Render::RenderShaderDataFunctor(d->m_nodeManagers)));
    registerBackendType<QAbstractTextureImage>(QBackendNodeFunctorPtr(new Render::TextureImageFunctor(d->m_nodeManagers->textureManager(), d->m_nodeManagers->textureImageManager(), d->m_nodeManagers->textureDataManager())));
    registerBackendType<QStateSet>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::StateSetNode, QStateSet>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QNoDraw>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::NoDraw, QNoDraw>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QBuffer>(QBackendNodeFunctorPtr(new Render::BufferFunctor(d->m_nodeManagers->bufferManager())));
    registerBackendType<QAttribute>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Attribute, Render::AttributeManager>(d->m_nodeManagers->attributeManager())));
    registerBackendType<QGeometry>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::Geometry, Render::GeometryManager>(d->m_nodeManagers->geometryManager())));
    registerBackendType<QGeometryRenderer>(QBackendNodeFunctorPtr(new Render::GeometryRendererFunctor(d->m_nodeManagers->geometryRendererManager())));
    registerBackendType<QObjectPicker>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::ObjectPicker, Render::ObjectPickerManager>(d->m_nodeManagers->objectPickerManager())));
    registerBackendType<QBoundingVolumeDebug>(QBackendNodeFunctorPtr(new Render::NodeFunctor<Render::BoundingVolumeDebug, Render::BoundingVolumeDebugManager>(d->m_nodeManagers->boundingVolumeDebugManager())));
    registerBackendType<QFrustumCulling>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::FrustumCulling, QFrustumCulling>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QLight>(QBackendNodeFunctorPtr(new Render::RenderLightFunctor(d->m_nodeManagers)));
    registerBackendType<QLighting>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::Lighting, QLighting>(d->m_nodeManagers->frameGraphManager())));
    registerBackendType<QDispatchCompute>(QBackendNodeFunctorPtr(new Render::FrameGraphNodeFunctor<Render::DispatchCompute, QDispatchCompute>(d->m_nodeManagers->frameGraphManager())));
}

void QRenderAspect::renderInitialize(QOpenGLContext *context)
{
    Q_D(QRenderAspect);
    if (d->m_renderer->api() == Render::AbstractRenderer::OpenGL)
        static_cast<Render::Renderer *>(d->m_renderer)->setOpenGLContext(context);
    d->m_renderer->initialize();
}

void QRenderAspect::renderSynchronous()
{
    Q_D(QRenderAspect);
    d->m_renderer->doRender();
}

/*!
 * Only called when rendering with QtQuick 2 and a Scene3D item
 */
void QRenderAspect::renderShutdown()
{
    Q_D(QRenderAspect);
    d->m_renderer->shutdown();
}

QVector<Qt3DCore::QAspectJobPtr> QRenderAspect::jobsToExecute(qint64 time)
{
    Q_D(QRenderAspect);
    d->m_renderer->setTime(time);

    // Create jobs that will get exectued by the threadpool
    QVector<QAspectJobPtr> jobs;

    // 1 LoadBufferJobs, GeometryJobs, SceneLoaderJobs
    // 2 CalculateBoundingVolumeJob (depends on LoadBuffer)
    // 3 WorldTransformJob
    // 4 UpdateBoundingVolume, FramePreparationJob (depend on WorlTransformJob)
    // 5 CalcGeometryTriangleVolumes (frame preparation job), RenderViewJobs
    // 6 PickBoundingVolumeJob
    // 7 Cleanup Job (depends on RV)

    // Create jobs to load in any meshes that are pending
    if (d->m_renderer->isRunning()) {

        Render::NodeManagers *manager = d->m_renderer->nodeManagers();
        QAspectJobPtr pickBoundingVolumeJob = d->m_renderer->pickBoundingVolumeJob();

        // Create the jobs to build the frame
        d->m_framePreparationJob->setRoot(d->m_renderer->sceneRoot());
        d->m_worldTransformJob->setRoot(d->m_renderer->sceneRoot());
        d->m_updateBoundingVolumeJob->setRoot(d->m_renderer->sceneRoot());
        d->m_calculateBoundingVolumeJob->setRoot(d->m_renderer->sceneRoot());
        d->m_cleanupJob->setRoot(d->m_renderer->sceneRoot());

        const QVector<QNodeId> texturesPending = manager->textureDataManager()->texturesPending();
        Q_FOREACH (const QNodeId &textureId, texturesPending) {
            Render::LoadTextureDataJobPtr loadTextureJob(new Render::LoadTextureDataJob(textureId));
            loadTextureJob->setNodeManagers(manager);
            jobs.append(loadTextureJob);
        }
        // TO DO: Have 2 jobs queue
        // One for urgent jobs that are mandatory for the rendering of a frame
        // Another for jobs that can span across multiple frames (Scene/Mesh loading)
        const QVector<Render::LoadSceneJobPtr> sceneJobs = manager->sceneManager()->pendingSceneLoaderJobs();
        Q_FOREACH (Render::LoadSceneJobPtr job, sceneJobs) {
            job->setNodeManagers(d->m_nodeManagers);
            job->setSceneParsers(d->m_sceneParsers);
            jobs.append(job);
        }

        // Clear any previous temporary dependency
        d->m_calculateBoundingVolumeJob->removeDependency(QWeakPointer<QAspectJob>());
        const QVector<QAspectJobPtr> bufferJobs = createRenderBufferJobs();
        Q_FOREACH (const QAspectJobPtr bufferJob, bufferJobs)
            d->m_calculateBoundingVolumeJob->addDependency(bufferJob);
        jobs.append(bufferJobs);

        const QVector<QAspectJobPtr> geometryJobs = createGeometryRendererJobs();
        jobs.append(geometryJobs);


        const QVector<QNodeId> geometryRendererTriangleUpdates = manager->geometryRendererManager()->geometryRenderersRequiringTriangleDataRefresh();
        Q_FOREACH (const QNodeId geomRendererId, geometryRendererTriangleUpdates) {
            Render::CalcGeometryTriangleVolumesPtr triangleComputeJob(new Render::CalcGeometryTriangleVolumes(geomRendererId, manager));
            triangleComputeJob->addDependency(d->m_framePreparationJob);
            pickBoundingVolumeJob->addDependency(triangleComputeJob);
            jobs.append(triangleComputeJob);
        }

        // Only add dependency if not already present
        const QVector<QWeakPointer<QAspectJob> > dependencies = pickBoundingVolumeJob->dependencies();
        if (std::find(dependencies.begin(), dependencies.end(), d->m_updateBoundingVolumeJob) == dependencies.end())
            pickBoundingVolumeJob->addDependency(d->m_updateBoundingVolumeJob);

        // Add all jobs to queue
        jobs.append(d->m_calculateBoundingVolumeJob);
        jobs.append(d->m_worldTransformJob);
        jobs.append(d->m_updateBoundingVolumeJob);
        jobs.append(d->m_framePreparationJob);
        jobs.append(pickBoundingVolumeJob);

        // Clear any old dependencies from previous frames
        d->m_cleanupJob->removeDependency(QWeakPointer<QAspectJob>());

        // Do not create any more RenderView jobs when the platform surface is gone.
        if (d->m_renderer->surface()) {
            // Traverse the current framegraph and create jobs to populate
            // RenderBins with RenderCommands
            QVector<QAspectJobPtr> renderBinJobs = d->m_renderer->renderBinJobs();
            // TODO: Add wrapper around ThreadWeaver::Collection
            for (int i = 0; i < renderBinJobs.size(); ++i) {
                QAspectJobPtr renderBinJob = renderBinJobs.at(i);
                renderBinJob->addDependency(d->m_updateBoundingVolumeJob);
                jobs.append(renderBinJob);
                d->m_cleanupJob->addDependency(renderBinJob);
            }
        }
        jobs.append(d->m_cleanupJob);
    }
    return jobs;
}

void QRenderAspect::onRootEntityChanged(Qt3DCore::QEntity *rootEntity)
{
    Q_D(QRenderAspect);
    d->m_renderer->setSceneRoot(d, d->m_renderer->nodeManagers()->lookupResource<Render::Entity, Render::EntityManager>(rootEntity->id()));
}

void QRenderAspect::onInitialize(const QVariantMap &data)
{
    // TODO: Remove the m_initialized variable and split out onInitialize()
    // and setting a resource (the QSurface) on the aspects.
    Q_D(QRenderAspect);
    if (!d->m_initialized) {

        // Register the VSyncFrameAdvanceService to drive the aspect manager loop
        // depending on the vsync
        if (d->m_aspectManager) {
            QAbstractFrameAdvanceService *advanceService = d->m_renderer->frameAdvanceService();
            if (advanceService)
                d->services()->registerServiceProvider(Qt3DCore::QServiceLocator::FrameAdvanceService,
                                                       advanceService);
        }

        d->m_renderer->setServices(d->services());
        d->m_renderer->createAllocators(d->jobManager());
        d->m_initialized = true;
    }

    QSurface *surface = Q_NULLPTR;
    const QVariant &v = data.value(QStringLiteral("surface"));
    if (v.isValid())
        surface = v.value<QSurface *>();

    if (surface)
        d->setSurface(surface);

    if (d->m_aspectManager)
        d->m_renderer->registerEventFilter(d->services()->eventFilterService());
}

void QRenderAspect::onCleanup()
{
    Q_D(QRenderAspect);
    if (d->m_renderer)
        d->m_renderer->destroyAllocators(d->jobManager());
    delete d->m_renderer;
    d->m_renderer = Q_NULLPTR;
}

// Returns a vector of jobs to be performed for dirty buffers
// 1 dirty buffer == 1 job, all job can be performed in parallel
QVector<Qt3DCore::QAspectJobPtr> QRenderAspect::createRenderBufferJobs()
{
    Q_D(QRenderAspect);
    const QVector<QNodeId> dirtyBuffers = d->m_nodeManagers->bufferManager()->dirtyBuffers();
    QVector<QAspectJobPtr> dirtyBuffersJobs;

    Q_FOREACH (const QNodeId &bufId, dirtyBuffers) {
        Render::HBuffer bufferHandle = d->m_nodeManagers->lookupHandle<Render::Buffer, Render::BufferManager, Render::HBuffer>(bufId);
        if (!bufferHandle.isNull()) {
            // Create new buffer job
            Render::LoadBufferJobPtr job(new Render::LoadBufferJob(bufferHandle));
            job->setNodeManager(d->m_nodeManagers);
            dirtyBuffersJobs.push_back(job);
        }
    }

    return dirtyBuffersJobs;
}

QVector<Qt3DCore::QAspectJobPtr> QRenderAspect::createGeometryRendererJobs()
{
    Q_D(QRenderAspect);
    Render::GeometryRendererManager *geomRendererManager = d->m_nodeManagers->geometryRendererManager();
    const QVector<QNodeId> dirtyGeometryRenderers = geomRendererManager->dirtyGeometryRenderers();
    QVector<QAspectJobPtr> dirtyGeometryRendererJobs;

    Q_FOREACH (const QNodeId &geoRendererId, dirtyGeometryRenderers) {
        Render::HGeometryRenderer geometryRendererHandle = geomRendererManager->lookupHandle(geoRendererId);
        if (!geometryRendererHandle.isNull()) {
            Render::LoadGeometryJobPtr job(new Render::LoadGeometryJob(geometryRendererHandle));
            job->setNodeManagers(d->m_nodeManagers);
            dirtyGeometryRendererJobs.push_back(job);
        }
    }

    return dirtyGeometryRendererJobs;
}

void QRenderAspectPrivate::loadSceneParsers()
{
    QStringList keys = QSceneParserFactory::keys();
    Q_FOREACH (QString key, keys) {
        QAbstractSceneParser *sceneParser = QSceneParserFactory::create(key, QStringList());
        if (sceneParser != Q_NULLPTR)
            m_sceneParsers.append(sceneParser);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("render", QT_PREPEND_NAMESPACE(Qt3DRender), QRenderAspect)
