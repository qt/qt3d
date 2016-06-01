/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
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

#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qrenderstateset.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qrendersettings.h>

#include <Qt3DRender/private/qboundingvolumedebug_p.h>
#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/rendertargetselectornode_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/clearbuffers_p.h>
#include <Qt3DRender/private/sortpolicy_p.h>
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
#include <Qt3DRender/private/qsceneiofactory_p.h>
#include <Qt3DRender/private/frustumculling_p.h>
#include <Qt3DRender/private/light_p.h>
#include <Qt3DRender/private/dispatchcompute_p.h>
#include <Qt3DRender/private/computecommand_p.h>
#include <Qt3DRender/private/rendersurfaceselector_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/backendnode_p.h>

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

/*!
 * \class Qt3DRender::QRenderAspect
 * \brief The QRenderAspect class
 * \since 5.7
 * \inmodule Qt3DRender
 */

/*! \internal */
QRenderAspectPrivate::QRenderAspectPrivate(QRenderAspect::RenderType type)
    : QAbstractAspectPrivate()
    , m_nodeManagers(new Render::NodeManagers())
    , m_renderer(nullptr)
    , m_initialized(false)
    , m_framePreparationJob(Render::FramePreparationJobPtr::create(m_nodeManagers))
    , m_cleanupJob(Render::FrameCleanupJobPtr::create(m_nodeManagers))
    , m_worldTransformJob(Render::UpdateWorldTransformJobPtr::create())
    , m_updateBoundingVolumeJob(Render::UpdateBoundingVolumeJobPtr::create())
    , m_calculateBoundingVolumeJob(Render::CalculateBoundingVolumeJobPtr::create(m_nodeManagers))
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

    // Create a renderer implementation given
    // a specific rendering API -> only OpenGL for now
    m_renderer = new Render::Renderer(type);
    m_renderer->setNodeManagers(m_nodeManagers);
}

/*! \internal */
QRenderAspectPrivate::~QRenderAspectPrivate()
{
    // The renderer should have been shutdown as part of onUnregistered().
    // If it still exists then this aspect is being deleted before the aspect
    // engine is finished with it.
    Q_ASSERT(m_renderer == nullptr);
    delete m_nodeManagers;
}

/*! \internal */
void QRenderAspectPrivate::registerBackendTypes()
{
    Q_Q(QRenderAspect);

    q->registerBackendType<Qt3DCore::QEntity>(QSharedPointer<Render::RenderEntityFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<Qt3DCore::QTransform>(QSharedPointer<Render::NodeFunctor<Render::Transform, Render::TransformManager> >::create(m_renderer, m_nodeManagers->transformManager()));

    q->registerBackendType<Qt3DRender::QCameraLens>(QSharedPointer<Render::NodeFunctor<Render::CameraLens, Render::CameraManager> >::create(m_renderer, m_nodeManagers->cameraManager()));
    q->registerBackendType<QLayer>(QSharedPointer<Render::NodeFunctor<Render::Layer, Render::LayerManager> >::create(m_renderer, m_nodeManagers->layerManager()));
    q->registerBackendType<QSceneLoader>(QSharedPointer<Render::RenderSceneFunctor>::create(m_renderer, m_nodeManagers->sceneManager()));
    q->registerBackendType<QRenderTarget>(QSharedPointer<Render::NodeFunctor<Render::RenderTarget, Render::RenderTargetManager> >::create(m_renderer, m_nodeManagers->renderTargetManager()));
    q->registerBackendType<QRenderTargetOutput>(QSharedPointer<Render::NodeFunctor<Render::RenderTargetOutput, Render::AttachmentManager> >::create(m_renderer, m_nodeManagers->attachmentManager()));
    q->registerBackendType<QRenderSettings>(QSharedPointer<Render::RenderSettingsFunctor>::create(m_renderer));
    q->registerBackendType<QRenderState>(QSharedPointer<Render::NodeFunctor<Render::RenderStateNode, Render::RenderStateManager> >::create(m_renderer, m_nodeManagers->renderStateManager()));

    // Geometry + Compute
    q->registerBackendType<QAttribute>(QSharedPointer<Render::NodeFunctor<Render::Attribute, Render::AttributeManager> >::create(m_renderer, m_nodeManagers->attributeManager()));
    q->registerBackendType<QBuffer>(QSharedPointer<Render::BufferFunctor>::create(m_renderer, m_nodeManagers->bufferManager()));
    q->registerBackendType<QComputeCommand>(QSharedPointer<Render::NodeFunctor<Render::ComputeCommand, Render::ComputeCommandManager> >::create(m_renderer, m_nodeManagers->computeJobManager()));
    q->registerBackendType<QGeometry>(QSharedPointer<Render::NodeFunctor<Render::Geometry, Render::GeometryManager> >::create(m_renderer, m_nodeManagers->geometryManager()));
    q->registerBackendType<QGeometryRenderer>(QSharedPointer<Render::GeometryRendererFunctor>::create(m_renderer, m_nodeManagers->geometryRendererManager()));

    // Textures
    q->registerBackendType<QAbstractTexture>(QSharedPointer<Render::TextureFunctor>::create(m_renderer, m_nodeManagers->textureManager(), m_nodeManagers->textureImageManager(), m_nodeManagers->textureDataManager()));
    q->registerBackendType<QAbstractTextureImage>(QSharedPointer<Render::TextureImageFunctor>::create(m_renderer, m_nodeManagers->textureManager(), m_nodeManagers->textureImageManager(), m_nodeManagers->textureDataManager()));

    // Material system
    q->registerBackendType<QEffect>(QSharedPointer<Render::NodeFunctor<Render::Effect, Render::EffectManager> >::create(m_renderer, m_nodeManagers->effectManager()));
    q->registerBackendType<QFilterKey>(QSharedPointer<Render::NodeFunctor<Render::FilterKey, Render::FilterKeyManager> >::create(m_renderer, m_nodeManagers->filterKeyManager()));
    q->registerBackendType<QAbstractLight>(QSharedPointer<Render::RenderLightFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QMaterial>(QSharedPointer<Render::NodeFunctor<Render::Material, Render::MaterialManager> >::create(m_renderer, m_nodeManagers->materialManager()));
    q->registerBackendType<QParameter>(QSharedPointer<Render::NodeFunctor<Render::Parameter, Render::ParameterManager> >::create(m_renderer, m_nodeManagers->parameterManager()));
    q->registerBackendType<QRenderPass>(QSharedPointer<Render::NodeFunctor<Render::RenderPass, Render::RenderPassManager> >::create(m_renderer, m_nodeManagers->renderPassManager()));
    q->registerBackendType<QShaderData>(QSharedPointer<Render::RenderShaderDataFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QShaderProgram>(QSharedPointer<Render::NodeFunctor<Render::Shader, Render::ShaderManager> >::create(m_renderer, m_nodeManagers->shaderManager()));
    q->registerBackendType<QTechnique>(QSharedPointer<Render::NodeFunctor<Render::Technique, Render::TechniqueManager> >::create(m_renderer, m_nodeManagers->techniqueManager()));

    // Framegraph
    q->registerBackendType<QCameraSelector>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::CameraSelector, QCameraSelector> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QClearBuffers>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ClearBuffers, QClearBuffers> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QDispatchCompute>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::DispatchCompute, QDispatchCompute> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QFrustumCulling>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::FrustumCulling, QFrustumCulling> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QLayerFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::LayerFilterNode, QLayerFilter> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QNoDraw>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::NoDraw, QNoDraw> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QRenderPassFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderPassFilter, QRenderPassFilter> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QRenderStateSet>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::StateSetNode, QRenderStateSet> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QRenderSurfaceSelector>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderSurfaceSelector, QRenderSurfaceSelector> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QRenderTargetSelector>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderTargetSelector, QRenderTargetSelector> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QSortPolicy>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SortPolicy, QSortPolicy> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QTechniqueFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::TechniqueFilter, QTechniqueFilter> >::create(m_renderer, m_nodeManagers->frameGraphManager()));
    q->registerBackendType<QViewport>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ViewportNode, QViewport> >::create(m_renderer, m_nodeManagers->frameGraphManager()));

    // Picking
    // q->registerBackendType<QBoundingVolumeDebug>(QSharedPointer<Render::NodeFunctor<Render::BoundingVolumeDebug, Render::BoundingVolumeDebugManager> >::create(m_renderer, m_nodeManagers->boundingVolumeDebugManager()));
    q->registerBackendType<QObjectPicker>(QSharedPointer<Render::NodeFunctor<Render::ObjectPicker, Render::ObjectPickerManager> >::create(m_renderer, m_nodeManagers->objectPickerManager()));
}

/*!
 * The constructor creates a new QRenderAspect::QRenderAspect instance with the
 * specified \a parent.
 * \param parent
 */
QRenderAspect::QRenderAspect(QObject *parent)
    : QRenderAspect(Threaded, parent) {}

/*!
 * The constructor creates a new QRenderAspect::QRenderAspect instance with the
 * specified \a type and \a parent.
 * \param type
 * \param parent
 */
QRenderAspect::QRenderAspect(QRenderAspect::RenderType type, QObject *parent)
    : QRenderAspect(*new QRenderAspectPrivate(type), parent) {}

/*! \internal */
QRenderAspect::QRenderAspect(QRenderAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{
    // Won't return until the private RenderThread in Renderer has been created
    // The Renderer is set to wait the surface with a wait condition
    // Threads modifying the Renderer should be synchronized using the Renderer's mutex
    setObjectName(QStringLiteral("Render Aspect"));
    Q_D(QRenderAspect);
    d->registerBackendTypes();
}

/*! \internal */
QRenderAspect::~QRenderAspect()
{
}

void QRenderAspectPrivate::renderInitialize(QOpenGLContext *context)
{
    if (m_renderer->api() == Render::AbstractRenderer::OpenGL)
        static_cast<Render::Renderer *>(m_renderer)->setOpenGLContext(context);
    m_renderer->initialize();
}

/*! \internal */
void QRenderAspectPrivate::renderSynchronous()
{
    m_renderer->doRender();
}

/*!
 * Only called when rendering with QtQuick 2 and a Scene3D item
 */
void QRenderAspectPrivate::renderShutdown()
{
    m_renderer->shutdown();
}

QVector<Qt3DCore::QAspectJobPtr> QRenderAspect::jobsToExecute(qint64 time)
{
    Q_D(QRenderAspect);
    d->m_renderer->setTime(time);

#if defined(QT3D_RENDER_DUMP_BACKEND_NODES)
    d->m_renderer->dumpInfo();
#endif

    // Create jobs that will get exectued by the threadpool
    QVector<QAspectJobPtr> jobs;

    // 1 LoadBufferJobs, GeometryJobs, SceneLoaderJobs
    // 2 CalculateBoundingVolumeJob (depends on LoadBuffer)
    // 3 WorldTransformJob
    // 4 UpdateBoundingVolume, FramePreparationJob (depend on WorlTransformJob)
    // 5 CalcGeometryTriangleVolumes (frame preparation job), RenderViewJobs
    // 6 PickBoundingVolumeJob
    // 7 Cleanup Job (depends on RV)

    // Ensure we have a settings object. It may get deleted by the call to
    // QChangeArbiter::syncChanges() that happens just before the render aspect is
    // asked for jobs to execute (this function). If that is the case, the RenderSettings will
    // be null and we should not generate any jobs.
    if (d->m_renderer != nullptr && d->m_renderer->isRunning() && d->m_renderer->settings()) {
        // don't spawn any jobs, if the renderer decides to skip this frame
        if (!d->m_renderer->shouldRender()) {
            d->m_renderer->skipNextFrame();
            return jobs;
        }

        Render::NodeManagers *manager = d->m_renderer->nodeManagers();
        QAspectJobPtr pickBoundingVolumeJob = d->m_renderer->pickBoundingVolumeJob();

        // Create the jobs to build the frame
        d->m_framePreparationJob->setRoot(d->m_renderer->sceneRoot());
        d->m_worldTransformJob->setRoot(d->m_renderer->sceneRoot());
        d->m_updateBoundingVolumeJob->setRoot(d->m_renderer->sceneRoot());
        d->m_calculateBoundingVolumeJob->setRoot(d->m_renderer->sceneRoot());
        d->m_cleanupJob->setRoot(d->m_renderer->sceneRoot());

        const QVector<QNodeId> texturesPending = manager->textureDataManager()->texturesPending();
        for (const QNodeId textureId : texturesPending) {
            auto loadTextureJob = Render::LoadTextureDataJobPtr::create(textureId);
            loadTextureJob->setNodeManagers(manager);
            jobs.append(loadTextureJob);
        }
        // TO DO: Have 2 jobs queue
        // One for urgent jobs that are mandatory for the rendering of a frame
        // Another for jobs that can span across multiple frames (Scene/Mesh loading)
        const QVector<Render::LoadSceneJobPtr> sceneJobs = manager->sceneManager()->pendingSceneLoaderJobs();
        for (const Render::LoadSceneJobPtr &job : sceneJobs) {
            job->setNodeManagers(d->m_nodeManagers);
            job->setSceneIOHandlers(d->m_sceneIOHandler);
            jobs.append(job);
        }

        // Clear any previous temporary dependency
        d->m_calculateBoundingVolumeJob->removeDependency(QWeakPointer<QAspectJob>());
        const QVector<QAspectJobPtr> bufferJobs = d->createRenderBufferJobs();
        for (const QAspectJobPtr &bufferJob : bufferJobs)
            d->m_calculateBoundingVolumeJob->addDependency(bufferJob);
        jobs.append(bufferJobs);

        const QVector<QAspectJobPtr> geometryJobs = d->createGeometryRendererJobs();
        jobs.append(geometryJobs);

        // Only add dependency if not already present
        const QVector<QWeakPointer<QAspectJob> > dependencies = pickBoundingVolumeJob->dependencies();
        if (std::find(dependencies.begin(), dependencies.end(), d->m_framePreparationJob) == dependencies.end())
            pickBoundingVolumeJob->addDependency(d->m_framePreparationJob);

        // Add all jobs to queue
        jobs.append(d->m_calculateBoundingVolumeJob);
        jobs.append(d->m_worldTransformJob);
        jobs.append(d->m_updateBoundingVolumeJob);
        jobs.append(d->m_framePreparationJob);
        jobs.append(pickBoundingVolumeJob);

        // Clear any old dependencies from previous frames
        d->m_cleanupJob->removeDependency(QWeakPointer<QAspectJob>());

        // Note: We need the RenderBinJobs to set the surface
        // so we must create the RenderViews in all cases

        // Traverse the current framegraph and create jobs to populate
        // RenderBins with RenderCommands
        QVector<QAspectJobPtr> renderBinJobs = d->m_renderer->renderBinJobs();
        for (int i = 0; i < renderBinJobs.size(); ++i) {
            QAspectJobPtr renderBinJob = renderBinJobs.at(i);
            renderBinJob->addDependency(d->m_updateBoundingVolumeJob);
            jobs.append(renderBinJob);
            d->m_cleanupJob->addDependency(renderBinJob);
        }
        jobs.append(d->m_cleanupJob);
    }
    return jobs;
}

void QRenderAspect::onEngineStartup()
{
    Q_D(QRenderAspect);
    Render::NodeManagers *managers = d->m_renderer->nodeManagers();
    Render::Entity *rootEntity = managers->lookupResource<Render::Entity, Render::EntityManager>(rootEntityId());
    Q_ASSERT(rootEntity);
    d->m_renderer->setSceneRoot(d, rootEntity);
}

void QRenderAspect::onRegistered()
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

    //    QSurface *surface = nullptr;
    //    const QVariant &v = data.value(QStringLiteral("surface"));
    //    if (v.isValid())
    //        surface = v.value<QSurface *>();

    //    if (surface)
    //        d->setSurface(surface);

    if (d->m_aspectManager)
        d->m_renderer->registerEventFilter(d->services()->eventFilterService());
}

void QRenderAspect::onUnregistered()
{
    Q_D(QRenderAspect);
    if (d->m_renderer) {
        // Request the renderer shuts down. In the threaded renderer case, the
        // Renderer destructor is the synchronization point where we wait for the
        // thread to join (see below).
        d->m_renderer->shutdown();

        // Free the per-thread threadpool allocators
        d->m_renderer->destroyAllocators(d->jobManager());
    }

    // Waits for the render thread to join (if using threaded renderer)
    delete d->m_renderer;
    d->m_renderer = nullptr;
}

// Returns a vector of jobs to be performed for dirty buffers
// 1 dirty buffer == 1 job, all job can be performed in parallel
QVector<Qt3DCore::QAspectJobPtr> QRenderAspectPrivate::createRenderBufferJobs()
{
    const QVector<QNodeId> dirtyBuffers = m_nodeManagers->bufferManager()->dirtyBuffers();
    QVector<QAspectJobPtr> dirtyBuffersJobs;

    for (const QNodeId bufId : dirtyBuffers) {
        Render::HBuffer bufferHandle = m_nodeManagers->lookupHandle<Render::Buffer, Render::BufferManager, Render::HBuffer>(bufId);
        if (!bufferHandle.isNull()) {
            // Create new buffer job
            auto job = Render::LoadBufferJobPtr::create(bufferHandle);
            job->setNodeManager(m_nodeManagers);
            dirtyBuffersJobs.push_back(job);
        }
    }

    return dirtyBuffersJobs;
}

QVector<Qt3DCore::QAspectJobPtr> QRenderAspectPrivate::createGeometryRendererJobs()
{
    Render::GeometryRendererManager *geomRendererManager = m_nodeManagers->geometryRendererManager();
    const QVector<QNodeId> dirtyGeometryRenderers = geomRendererManager->dirtyGeometryRenderers();
    QVector<QAspectJobPtr> dirtyGeometryRendererJobs;

    for (const QNodeId geoRendererId : dirtyGeometryRenderers) {
        Render::HGeometryRenderer geometryRendererHandle = geomRendererManager->lookupHandle(geoRendererId);
        if (!geometryRendererHandle.isNull()) {
            auto job = Render::LoadGeometryJobPtr::create(geometryRendererHandle);
            job->setNodeManagers(m_nodeManagers);
            dirtyGeometryRendererJobs.push_back(job);
        }
    }

    return dirtyGeometryRendererJobs;
}

void QRenderAspectPrivate::loadSceneParsers()
{
    const QStringList keys = QSceneIOFactory::keys();
    for (const QString &key : keys) {
        QSceneIOHandler *sceneIOHandler = QSceneIOFactory::create(key, QStringList());
        if (sceneIOHandler != nullptr)
            m_sceneIOHandler.append(sceneIOHandler);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("render", QT_PREPEND_NAMESPACE(Qt3DRender), QRenderAspect)
