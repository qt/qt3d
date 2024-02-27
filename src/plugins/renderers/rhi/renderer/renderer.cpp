// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderer_p.h"
#include "rhirendertarget_p.h"

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/vector_helper_p.h>

#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qrendercapture.h>

#include <Qt3DRender/private/qsceneimporter_p.h>
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/framegraphvisitor_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/material_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/technique_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/vsyncframeadvanceservice_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/platformsurfacefilter_p.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DRender/private/updatelevelofdetailjob_p.h>
#include <Qt3DRender/private/buffercapture_p.h>
#include <Qt3DRender/private/offscreensurfacehelper_p.h>
#include <Qt3DRender/private/subtreeenabler_p.h>
#include <Qt3DRender/private/qshaderprogrambuilder_p.h>
#include <Qt3DRender/private/qshaderprogram_p.h>

#include <Qt3DRender/qcameralens.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p_p.h>
#include <Qt3DRender/private/resourceaccessor_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/setfence_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>

#include <rhibuffer_p.h>
#include <rhigraphicspipeline_p.h>

#include <rendercommand_p.h>
#include <renderview_p.h>
#include <texture_p.h>
#include <renderviewbuilder_p.h>
#include <rhiresourcemanagers_p.h>
#include <commandexecuter_p.h>
#include <submissioncontext_p.h>

#include <QStack>
#include <QOffscreenSurface>
#include <QSurface>
#include <QElapsedTimer>
#include <QLibraryInfo>
#include <QMutexLocker>
#include <QPluginLoader>
#include <QDir>
#include <QUrl>
#include <QOffscreenSurface>
#include <QWindow>
#include <QThread>
#include <QKeyEvent>
#include <QMouseEvent>

#include <QtGui/private/qopenglcontext_p.h>
#include <QGuiApplication>
#include <Qt3DCore/private/qthreadpooler_p.h>

#include <optional>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Rhi {

using RendererCache = Render::RendererCache<RenderCommand>;

namespace {

class CachingLightGatherer : public LightGatherer
{
public:
    CachingLightGatherer(RendererCache *cache) : LightGatherer(), m_cache(cache) { }

    void run() override
    {
        LightGatherer::run();

        QMutexLocker lock(m_cache->mutex());
        m_cache->gatheredLights = lights();
        m_cache->environmentLight = environmentLight();
    }

private:
    RendererCache *m_cache;
};

class CachingRenderableEntityFilter : public RenderableEntityFilter
{
public:
    CachingRenderableEntityFilter(RendererCache *cache) : RenderableEntityFilter(), m_cache(cache)
    {
    }

    void run() override
    {
        RenderableEntityFilter::run();

        std::vector<Entity *> selectedEntities = filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        QMutexLocker lock(m_cache->mutex());
        m_cache->renderableEntities = std::move(selectedEntities);
    }

private:
    RendererCache *m_cache;
};

class CachingComputableEntityFilter : public ComputableEntityFilter
{
public:
    CachingComputableEntityFilter(RendererCache *cache) : ComputableEntityFilter(), m_cache(cache)
    {
    }

    void run() override
    {
        ComputableEntityFilter::run();

        std::vector<Entity *> selectedEntities = filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        QMutexLocker lock(m_cache->mutex());
        m_cache->computeEntities = std::move(selectedEntities);
    }

private:
    RendererCache *m_cache;
};

int locationForAttribute(Attribute *attr, const RHIShader *shader) noexcept
{
    const std::vector<ShaderAttribute> &attribInfo = shader->attributes();
    const auto it = std::find_if(attribInfo.begin(), attribInfo.end(),
                                 [attr](const ShaderAttribute &sAttr) { return attr->nameId() == sAttr.m_nameId; });
    if (it != attribInfo.end())
        return it->m_location;
    return -1;
}

} // anonymous

/*!
    \internal

    Renderer shutdown procedure:

    Since the renderer relies on the surface and OpenGLContext to perform its cleanup,
    it is shutdown when the surface is set to nullptr

    When the surface is set to nullptr this will request the RenderThread to terminate
    and will prevent createRenderBinJobs from returning a set of jobs as there is nothing
    more to be rendered.

    In turn, this will call shutdown which will make the OpenGL context current one last time
    to allow cleanups requiring a call to QOpenGLContext::currentContext to execute properly.
    At the end of that function, the GraphicsContext is set to null.

    At this point though, the QAspectThread is still running its event loop and will only stop
    a short while after.
 */

Renderer::Renderer()
    : m_services(nullptr),
      m_aspect(nullptr),
      m_nodesManager(nullptr),
      m_renderSceneRoot(nullptr),
      m_defaultRenderStateSet(nullptr),
      m_submissionContext(nullptr),
      m_vsyncFrameAdvanceService(new VSyncFrameAdvanceService(false)),
      m_waitForInitializationToBeCompleted(0),
      m_hasBeenInitializedMutex(),
      m_exposed(0),
      m_lastFrameCorrect(0),
      m_glContext(nullptr),
      m_rhiContext(nullptr),
      m_time(0),
      m_settings(nullptr),
      m_updateShaderDataTransformJob(Render::UpdateShaderDataTransformJobPtr::create()),
      m_cleanupJob(Render::FrameCleanupJobPtr::create()),
      m_sendBufferCaptureJob(Render::SendBufferCaptureJobPtr::create()),
      m_filterCompatibleTechniqueJob(FilterCompatibleTechniqueJobPtr::create()),
      m_lightGathererJob(new CachingLightGatherer(&m_cache)),
      m_renderableEntityFilterJob(new CachingRenderableEntityFilter(&m_cache)),
      m_computableEntityFilterJob(new CachingComputableEntityFilter(&m_cache)),
      m_bufferGathererJob(SynchronizerJobPtr::create([this] { lookForDirtyBuffers(); },
                                                     JobTypes::DirtyBufferGathering)),
      m_textureGathererJob(SynchronizerJobPtr::create([this] { lookForDirtyTextures(); },
                                                      JobTypes::DirtyTextureGathering)),
      m_introspectShaderJob(SynchronizerPostFramePtr::create(
              [this] { reloadDirtyShaders(); },
              [this](Qt3DCore::QAspectManager *m) { sendShaderChangesToFrontend(m); },
              JobTypes::DirtyShaderGathering)),
      m_ownedContext(false),
      m_RHIResourceManagers(nullptr),
      m_commandExecuter(new Qt3DRender::DebugRhi::CommandExecuter(this)),
      m_shouldSwapBuffers(true)
{
    std::fill_n(m_textureTransform, 4, 0.f);

    // Set renderer as running - it will wait in the context of the
    // RenderThread for RenderViews to be submitted
    m_running.fetchAndStoreOrdered(1);

    m_introspectShaderJob->addDependency(m_filterCompatibleTechniqueJob);

    m_filterCompatibleTechniqueJob->setRenderer(this);

    m_defaultRenderStateSet = new RenderStateSet;
    m_defaultRenderStateSet->addState(StateVariant::createState<DepthTest>(GL_LESS));
    m_defaultRenderStateSet->addState(StateVariant::createState<CullFace>(GL_BACK));
    m_defaultRenderStateSet->addState(StateVariant::createState<ColorMask>(true, true, true, true));
}

Renderer::~Renderer()
{
    Q_ASSERT(m_running.fetchAndStoreOrdered(0) == 0);

    delete m_defaultRenderStateSet;
    delete m_RHIResourceManagers;

    if (!m_ownedContext)
        QObject::disconnect(m_contextConnection);
}

void Renderer::dumpInfo() const
{
    qDebug() << Q_FUNC_INFO << "t =" << m_time;

    const ShaderManager *shaderManager = m_nodesManager->shaderManager();
    qDebug() << "=== Shader Manager ===";
    qDebug() << *shaderManager;

    const TextureManager *textureManager = m_nodesManager->textureManager();
    qDebug() << "=== Texture Manager ===";
    qDebug() << *textureManager;

    const TextureImageManager *textureImageManager = m_nodesManager->textureImageManager();
    qDebug() << "=== Texture Image Manager ===";
    qDebug() << *textureImageManager;
}

API Renderer::api() const
{
    return API::RHI;
}

// When Scene3D is driving rendering:
// - Don't create SwapChains
// - Use the defaultRenderTarget if no renderTarget specified instead of the
//   swapChain
// - Use the provided commandBuffer
void Renderer::setRenderDriver(AbstractRenderer::RenderDriver driver)
{
    // This must be called before initialize which creates the submission context
    Q_ASSERT(!m_submissionContext);
    m_driver = driver;
}

AbstractRenderer::RenderDriver Renderer::renderDriver() const
{
    return m_driver;
}

qint64 Renderer::time() const
{
    return m_time;
}

void Renderer::setTime(qint64 time)
{
    m_time = time;
}

void Renderer::setJobsInLastFrame(int jobsInLastFrame)
{
    m_jobsInLastFrame = jobsInLastFrame;
}

void Renderer::setAspect(QRenderAspect *aspect)
{
    m_aspect = aspect;
    m_updateShaderDataTransformJob->addDependency(
            QRenderAspectPrivate::get(aspect)->m_worldTransformJob);
}

void Renderer::setNodeManagers(NodeManagers *managers)
{
    m_nodesManager = managers;
    m_RHIResourceManagers = new RHIResourceManagers();
    m_scene2DResourceAccessor.reset(new ResourceAccessor(this, m_nodesManager));

    m_updateShaderDataTransformJob->setManagers(m_nodesManager);
    m_cleanupJob->setManagers(m_nodesManager);
    m_filterCompatibleTechniqueJob->setManager(m_nodesManager->techniqueManager());
    m_sendBufferCaptureJob->setManagers(m_nodesManager);
    m_lightGathererJob->setManager(m_nodesManager->renderNodesManager());
    m_renderableEntityFilterJob->setManager(m_nodesManager->renderNodesManager());
    m_computableEntityFilterJob->setManager(m_nodesManager->renderNodesManager());
}

void Renderer::setServices(Qt3DCore::QServiceLocator *services)
{
    m_services = services;

    m_nodesManager->sceneManager()->setDownloadService(m_services->downloadHelperService());
}

QRenderAspect *Renderer::aspect() const
{
    return m_aspect;
}

NodeManagers *Renderer::nodeManagers() const
{
    return m_nodesManager;
}

/*!
    \internal

    Return context which can be used to share resources safely
    with qt3d main render context.
*/
QOpenGLContext *Renderer::shareContext() const
{
    return nullptr;
}

void Renderer::setOpenGLContext(QOpenGLContext *context)
{
    m_glContext = context;
}

void Renderer::setRHIContext(QRhi *ctx)
{
    m_rhiContext = ctx;
}

// Called by Scene3D
void Renderer::setDefaultRHIRenderTarget(QRhiRenderTarget *defaultTarget)
{
    Q_ASSERT(m_submissionContext);
    m_submissionContext->setDefaultRenderTarget(defaultTarget);

    // When the defaultTarget changes, we have to recreate all QRhiGraphicsPipelines
    // to ensure they match new DefaultRenderTargetLayout. This is potentially expensive
    RHIGraphicsPipelineManager *pipelineManager = m_RHIResourceManagers->rhiGraphicsPipelineManager();
    pipelineManager->releaseAllResources();
}

// Called by Scene3D
void Renderer::setRHICommandBuffer(QRhiCommandBuffer *commandBuffer)
{
    Q_ASSERT(m_submissionContext);
    m_submissionContext->setCommandBuffer(commandBuffer);
}

void Renderer::setScreen(QScreen *scr)
{
    m_screen = scr;
}

QScreen *Renderer::screen() const
{
    return m_screen;
}

bool Renderer::accessOpenGLTexture(Qt3DCore::QNodeId nodeId, QOpenGLTexture **texture,
                                   QMutex **lock, bool readonly)
{
    RHI_UNIMPLEMENTED;
    Q_UNUSED(texture);

    Texture *tex = m_nodesManager->textureManager()->lookupResource(nodeId);
    if (!tex)
        return false;

    RHITexture *glTex = m_RHIResourceManagers->rhiTextureManager()->lookupResource(tex->peerId());
    if (!glTex)
        return false;

    if (glTex->isDirty())
        return false;

    if (!readonly)
        glTex->setExternalRenderingEnabled(true);

    //    RHITexture::TextureUpdateInfo texInfo =
    //    glTex->createOrUpdateRhiTexture(m_submissionContext.data()); *texture = texInfo.texture;

    if (!readonly)
        *lock = glTex->externalRenderingLock();

    return true;
}

QSharedPointer<RenderBackendResourceAccessor> Renderer::resourceAccessor() const
{
    return m_scene2DResourceAccessor;
}

// Called in RenderThread context by the run method of RenderThread
// RenderThread has locked the mutex already and unlocks it when this
// method termintates
void Renderer::initialize()
{
    QMutexLocker lock(&m_hasBeenInitializedMutex);
    m_submissionContext.reset(new SubmissionContext);
    m_submissionContext->setRenderer(this);

    if (m_driver == AbstractRenderer::Scene3D) {
        m_submissionContext->setRHIContext(m_rhiContext);
        m_submissionContext->setDrivenExternally(true);
    }

    // RHI initialization
    {
        qCDebug(Backend) << Q_FUNC_INFO << "Requesting renderer initialize";
        m_submissionContext->initialize();

        // We need to adapt texture coordinates
        // m_textureTransform is (a;b) in texCoord = a * texCoord + b
        if (m_submissionContext->rhi()->isYUpInFramebuffer()) {
            // OpenGL case - that is what we assume to be the default so we do not change
            // anything
            m_textureTransform[0] = 1.f;
            m_textureTransform[1] = 1.f;
            m_textureTransform[2] = 0.f;
            m_textureTransform[3] = 0.f;
        } else {
            // Other cases : y = 1 - y
            m_textureTransform[0] = 1.f;
            m_textureTransform[1] = -1.f;
            m_textureTransform[2] = 0.f;
            m_textureTransform[3] = 1.f;
        }

        // Awake setScenegraphRoot in case it was waiting
        m_waitForInitializationToBeCompleted.release(1);

        // Allow the aspect manager to proceed
        m_vsyncFrameAdvanceService->proceedToNextFrame();

        // Force initial refresh
        markDirty(AllDirty, nullptr);
        return;
    }
}

/*!
 * \internal
 *
 * Signals for the renderer to stop rendering. If a threaded renderer is in use,
 * the render thread will call releaseGraphicsResources() just before the thread exits.
 * If rendering synchronously, this function will call releaseGraphicsResources().
 */
void Renderer::shutdown()
{
    // Ensure we have waited to be fully initialized before trying to shut down
    // (in case initialization is taking place at the same time)
    QMutexLocker lock(&m_hasBeenInitializedMutex);

    qCDebug(Backend) << Q_FUNC_INFO << "Requesting renderer shutdown";
    m_running.storeRelaxed(0);

    // We delete any renderqueue that we may not have had time to render
    // before the surface was destroyed
    QMutexLocker lockRenderQueue(m_renderQueue.mutex());
    m_renderQueue.reset();
    lockRenderQueue.unlock();

    releaseGraphicsResources();

    // Destroy internal managers
    // This needs to be done before the nodeManager is destroy
    // as the internal resources might somehow rely on nodeManager resources
    delete m_RHIResourceManagers;
    m_RHIResourceManagers = nullptr;
}

/*!
    \internal

    When using a threaded renderer this function is called in the context of the
    RenderThread to do any shutdown and cleanup that needs to be performed in the
    thread where the OpenGL context lives.

    When using Scene3D or anything that provides a custom QOpenGLContext (not
    owned by Qt3D) this function is called whenever the signal
    QOpenGLContext::aboutToBeDestroyed is emitted. In that case this function
    is called in the context of the emitter's thread.
*/
void Renderer::releaseGraphicsResources()
{
    // We may get called twice when running inside of a Scene3D. Once when Qt Quick
    // wants to shutdown, and again when the render aspect gets unregistered. So
    // check that we haven't already cleaned up before going any further.
    if (!m_submissionContext)
        return;
    m_submissionContext.reset(nullptr);

    qCDebug(Backend) << Q_FUNC_INFO << "Renderer properly shutdown";
}

void Renderer::setSurfaceExposed(bool exposed)
{
    qCDebug(Backend) << "Window exposed: " << exposed;
    m_exposed.fetchAndStoreOrdered(exposed);
}

Render::FrameGraphNode *Renderer::frameGraphRoot() const
{
    Q_ASSERT(m_settings);
    if (m_nodesManager && m_nodesManager->frameGraphManager() && m_settings)
        return m_nodesManager->frameGraphManager()->lookupNode(m_settings->activeFrameGraphID());
    return nullptr;
}

// QAspectThread context
// Order of execution :
// 1) RenderThread is created -> release 1 of m_waitForInitializationToBeCompleted when started
// 2) setSceneRoot waits to acquire initialization
// 3) submitRenderView -> check for surface
//    -> make surface current + create proper glHelper if needed
void Renderer::setSceneRoot(Entity *sgRoot)
{
    Q_ASSERT(sgRoot);

    // If initialization hasn't been completed we must wait
    m_waitForInitializationToBeCompleted.acquire();

    m_renderSceneRoot = sgRoot;
    if (!m_renderSceneRoot)
        qCWarning(Backend) << "Failed to build render scene";
    m_renderSceneRoot->dump();
    qCDebug(Backend) << Q_FUNC_INFO << "DUMPING SCENE";

    // Set the scene root on the jobs
    m_cleanupJob->setRoot(m_renderSceneRoot);

    // Set all flags to dirty
    m_dirtyBits.marked |= AbstractRenderer::AllDirty;
}

void Renderer::setSettings(RenderSettings *settings)
{
    m_settings = settings;
}

RenderSettings *Renderer::settings() const
{
    return m_settings;
}

// Either called by render if Qt3D is in charge of rendering (in the mainthread)
// or by QRenderAspectPrivate::render (for Scene3D, potentially from a RenderThread)
// This will wait until renderQueue is ready or shutdown was requested
void Renderer::render(bool swapBuffers)
{
    Renderer::ViewSubmissionResultData submissionData;
    bool beganDrawing = false;

    // Blocking until RenderQueue is full
    const bool canSubmit = waitUntilReadyToSubmit();

    // If it returns false -> we are shutting down
    if (!canSubmit)
        return;

    m_shouldSwapBuffers = swapBuffers;
    const std::vector<Render::Rhi::RenderView *> &renderViews = m_renderQueue.nextFrameQueue();
    const bool queueIsEmpty = m_renderQueue.targetRenderViewCount() == 0;

    bool mustCleanResources = false;

    // RenderQueue is complete (but that means it may be of size 0)
    if (!queueIsEmpty) {
        Qt3DCore::QTaskLogger submissionStatsPart1(m_services->systemInformation(),
                                                   { JobTypes::FrameSubmissionPart1, 0 },
                                                   Qt3DCore::QTaskLogger::Submission);
        Qt3DCore::QTaskLogger submissionStatsPart2(m_services->systemInformation(),
                                                   { JobTypes::FrameSubmissionPart2, 0 },
                                                   Qt3DCore::QTaskLogger::Submission);

        std::vector<RHIPassInfo> rhiPassesInfo;

        QSurface *surface = nullptr;
        for (const RenderView *rv : renderViews) {
            surface = rv->surface();
            if (surface)
                break;
        }

        // In case we did not draw because e.g. there was no swapchain,
        // we keep the resource updates from the previous frame.
        if (!m_submissionContext->m_currentUpdates) {
            m_submissionContext->m_currentUpdates =
                    m_submissionContext->rhi()->nextResourceUpdateBatch();
        }

        // 1) Execute commands for buffer uploads, texture updates, shader loading first
        updateResources();

        rhiPassesInfo = prepareCommandsSubmission(renderViews);
        // 2) Update Pipelines and copy data into commands to allow concurrent submission

        {
            // Scoped to destroy surfaceLock
            SurfaceLocker surfaceLock(surface);
            const bool surfaceIsValid = (surface && surfaceLock.isSurfaceValid());
            if (surfaceIsValid) {
                beganDrawing = m_submissionContext->beginDrawing(surface);
                if (beganDrawing) {
                    // Purge shader which aren't used any longer
                    static int callCount = 0;
                    ++callCount;
                    const int shaderPurgePeriod = 600;
                    if (callCount % shaderPurgePeriod == 0)
                        m_RHIResourceManagers->rhiShaderManager()->purge();
                }
            }
        }

        // Only try to submit the RenderViews if the preprocessing was successful
        // This part of the submission is happening in parallel to the RV building for the next
        // frame
        if (beganDrawing) {
            submissionStatsPart1.end(submissionStatsPart2.restart());

            // 3) Submit the render commands for frame n (making sure we never reference
            // something that could be changing) Render using current device state and renderer
            // configuration
            submissionData = submitRenderViews(rhiPassesInfo);

            // Perform any required cleanup of the Graphics resources (Buffers deleted, Shader
            // deleted...)
            mustCleanResources = true;
        }

        // Execute the pending shell commands
        m_commandExecuter->performAsynchronousCommandExecution(renderViews);

        // Delete all the RenderViews which will clear the allocators
        // that were used for their allocation
    }

    // Perform the last swapBuffers calls after the proceedToNextFrame
    // as this allows us to gain a bit of time for the preparation of the
    // next frame
    // Finish up with last surface used in the list of RenderViews
    if (beganDrawing) {
        SurfaceLocker surfaceLock(submissionData.surface);
        // Finish up with last surface used in the list of RenderViews
        const bool swapBuffers =
                true // submissionData.lastBoundFBOId == m_submissionContext->defaultFBO()
                && surfaceLock.isSurfaceValid()
                && m_shouldSwapBuffers;
        m_submissionContext->endDrawing(swapBuffers);

        if (mustCleanResources)
            cleanGraphicsResources();
    }

    // Reset RenderQueue and destroy the renderViews
    m_renderQueue.reset();

    // We allow the RenderTickClock service to proceed to the next frame
    // In turn this will allow the aspect manager to request a new set of jobs
    // to be performed for each aspect
    m_vsyncFrameAdvanceService->proceedToNextFrame();
}

// Called by RenderViewJobs
// When the frameQueue is complete and we are using a renderThread
// we allow the render thread to proceed
void Renderer::enqueueRenderView(RenderView *renderView, int submitOrder)
{
    QMutexLocker locker(m_renderQueue.mutex()); // Prevent out of order execution
    // We cannot use a lock free primitive here because:
    // - std::vector is not thread safe
    // - Even if the insert is made correctly, the isFrameComplete call
    //   could be invalid since depending on the order of execution
    //   the counter could be complete but the renderview not yet added to the
    //   buffer depending on whichever order the cpu decides to process this
    const bool isQueueComplete = m_renderQueue.queueRenderView(renderView, submitOrder);
    locker.unlock(); // We're done protecting the queue at this point
    if (isQueueComplete) {
        m_submitRenderViewsSemaphore.release(1);
    }
}

bool Renderer::waitUntilReadyToSubmit()
{
    // Make sure that we've been told to render before rendering
    // Prevent ouf of order execution
    m_submitRenderViewsSemaphore.acquire(1);

    // Check if shutdown has been requested
    if (m_running.loadRelaxed() == 0)
        return false;

    // The semaphore should only
    // be released when the frame queue is complete and there's
    // something to render
    // The case of shutdown should have been handled just before
    Q_ASSERT(m_renderQueue.isFrameQueueComplete());
    return true;
}

// Main thread
QVariant Renderer::executeCommand(const QStringList &args)
{
    return m_commandExecuter->executeCommand(args);
}

QSurfaceFormat Renderer::format()
{
    return m_submissionContext->format();
}

namespace {
std::optional<QRhiVertexInputAttribute::Format> rhiAttributeType(Attribute *attr) {
    switch (attr->vertexBaseType()) {
    case Qt3DCore::QAttribute::Byte:
    case Qt3DCore::QAttribute::UnsignedByte: {
        if (attr->vertexSize() == 1)
            return QRhiVertexInputAttribute::UNormByte;
        if (attr->vertexSize() == 2)
            return QRhiVertexInputAttribute::UNormByte2;
        if (attr->vertexSize() == 4)
            return QRhiVertexInputAttribute::UNormByte4;
        break;
    }
    case Qt3DCore::QAttribute::Int: {
        if (attr->vertexSize() == 1)
            return QRhiVertexInputAttribute::SInt;
        if (attr->vertexSize() == 2)
            return QRhiVertexInputAttribute::SInt2;
        if (attr->vertexSize() == 3)
            return QRhiVertexInputAttribute::SInt3;
        if (attr->vertexSize() == 4)
            return QRhiVertexInputAttribute::SInt4;
        break;
    }
    case Qt3DCore::QAttribute::UnsignedInt: {
        if (attr->vertexSize() == 1)
            return QRhiVertexInputAttribute::UInt;
        if (attr->vertexSize() == 2)
            return QRhiVertexInputAttribute::UInt2;
        if (attr->vertexSize() == 3)
            return QRhiVertexInputAttribute::UInt3;
        if (attr->vertexSize() == 4)
            return QRhiVertexInputAttribute::UInt4;
        break;
    }
    case Qt3DCore::QAttribute::HalfFloat: {
        if (attr->vertexSize() == 1)
            return QRhiVertexInputAttribute::Half;
        if (attr->vertexSize() == 2)
            return QRhiVertexInputAttribute::Half2;
        if (attr->vertexSize() == 3)
            return QRhiVertexInputAttribute::Half3;
        if (attr->vertexSize() >= 4)
            return QRhiVertexInputAttribute::Half4;
        break;
    }
    case Qt3DCore::QAttribute::Float: {
        if (attr->vertexSize() == 1)
            return QRhiVertexInputAttribute::Float;
        if (attr->vertexSize() == 2)
            return QRhiVertexInputAttribute::Float2;
        if (attr->vertexSize() == 3)
            return QRhiVertexInputAttribute::Float3;
        if (attr->vertexSize() >= 4)
            return QRhiVertexInputAttribute::Float4;
        break;
    }
    default:
        break;
    }
    return std::nullopt;
}
}

void Renderer::updateGraphicsPipeline(RenderCommand &cmd, RenderView *rv)
{
    if (!cmd.m_rhiShader) {
        qCWarning(Backend) << "Command has no shader";
        return;
    }

    // The Graphics Pipeline defines
    // - Render State (Depth, Culling, Stencil, Blending)
    // - Shader Resources Binding
    // - Shader Vertex Attribute Layout

    // This means we need to have one GraphicsPipeline per
    // - geometry layout
    // - material
    // - state (RV + RC)

    // Try to retrieve existing pipeline
    // TO DO: Make RenderState part of the Key
    // as it is likely many geometrys will have the same layout
    RHIGraphicsPipelineManager *pipelineManager = m_RHIResourceManagers->rhiGraphicsPipelineManager();
    const int geometryLayoutId = pipelineManager->getIdForAttributeVec(cmd.m_attributeInfo);
    const int renderStatesKey = pipelineManager->getIdForRenderStates(cmd.m_stateSet);
    const GraphicsPipelineIdentifier pipelineKey { geometryLayoutId, cmd.m_shaderId, rv->renderTargetId(), cmd.m_primitiveType, renderStatesKey };
    RHIGraphicsPipeline *graphicsPipeline = pipelineManager->lookupResource(pipelineKey);
    if (graphicsPipeline == nullptr) {
        // Init UBOSet the first time we allocate a new pipeline
        graphicsPipeline = pipelineManager->getOrCreateResource(pipelineKey);
        graphicsPipeline->setKey(pipelineKey);
        graphicsPipeline->uboSet()->setResourceManager(m_RHIResourceManagers);
        graphicsPipeline->uboSet()->setNodeManagers(m_nodesManager);
        graphicsPipeline->uboSet()->initializeLayout(m_submissionContext.data(), cmd.m_rhiShader);
    }

    // Increase score so that we know the pipeline was used for this frame and shouldn't be
    // destroyed
    graphicsPipeline->increaseScore();

    // Record command reference in UBOSet
    graphicsPipeline->uboSet()->addRenderCommand(cmd);

    // Store association between RV and pipeline
    if (auto& pipelines = m_rvToGraphicsPipelines[rv]; !Qt3DCore::contains(pipelines, graphicsPipeline))
        pipelines.push_back(graphicsPipeline);

    // Record RHIGraphicsPipeline into command for later use
    cmd.pipeline = graphicsPipeline;

    // TO DO: Set to true if geometry, shader or render state dirty
    bool requiresRebuild = false;

    // Build/Rebuild actual RHI pipeline if required
    // TO DO: Ensure we find a way to know when the state is dirty to trigger a rebuild
    if (graphicsPipeline->pipeline() == nullptr || requiresRebuild)
        buildGraphicsPipelines(graphicsPipeline, rv, cmd);
}

void Renderer::buildGraphicsPipelines(RHIGraphicsPipeline *graphicsPipeline,
                                      RenderView *rv,
                                      const RenderCommand &cmd)
{
    // Note: This is completely stupid but RHI doesn't allow you to build
    // a QRhiShaderResourceBindings if you don't already have buffers/textures
    // at hand (where it should only need to know the type/formats ... of the resources)
    // For that reason we need to provide a RenderCommand

    // Note: we can rebuild add/remove things from the QRhiShaderResourceBindings after having
    // created the pipeline and rebuild it. Changes should be picked up automatically

    // If a defaultRenderTarget was set (Scene3D) we don't bother retrieving the swapchain
    // as we have to use the one provided by Scene3D
    QRhiSwapChain *rhiSwapChain = nullptr;
    if (!m_submissionContext->defaultRenderTarget()) {
        const SubmissionContext::SwapChainInfo *swapchain = m_submissionContext->swapChainForSurface(rv->surface());
        if (!swapchain || !swapchain->swapChain || !swapchain->renderPassDescriptor) {
            qCWarning(Backend) << "Can't create pipeline, incomplete SwapChain and no default Render Target";
            return;
        }
        rhiSwapChain = swapchain->swapChain;
    }

    auto onFailure = [&](const char* msg) {
        qCWarning(Backend) << "Failed to build graphics pipeline:" << msg;
    };

    PipelineUBOSet *uboSet = graphicsPipeline->uboSet();
    RHIShader *shader = cmd.m_rhiShader;

    // Setup shaders
    const QShader& vertexShader = shader->shaderStage(QShader::VertexStage);
    if (!vertexShader.isValid())
        return onFailure("Invalid vertex shader");

    const QShader& fragmentShader = shader->shaderStage(QShader::FragmentStage);
    if (!fragmentShader.isValid())
        return onFailure("Invalid fragment shader");

    // Set Resource Bindings
    const std::vector<QRhiShaderResourceBinding> resourceBindings = uboSet->resourceLayout(shader);
    QRhiShaderResourceBindings *shaderResourceBindings =
            m_submissionContext->rhi()->newShaderResourceBindings();
    graphicsPipeline->setShaderResourceBindings(shaderResourceBindings);

    shaderResourceBindings->setBindings(resourceBindings.cbegin(), resourceBindings.cend());
    if (!shaderResourceBindings->create())
        return onFailure("Unable to create resource bindings");

    // Setup attributes
    const Geometry *geom = cmd.m_geometry.data();
    QVarLengthArray<QRhiVertexInputBinding, 8> inputBindings;
    QVarLengthArray<QRhiVertexInputAttribute, 8> rhiAttributes;
    QHash<int, int> attributeNameToBinding;

    if (!prepareGeometryInputBindings(geom, cmd.m_rhiShader,
                                      inputBindings, rhiAttributes,
                                      attributeNameToBinding))
        return onFailure("Geometry doesn't match expected layout");

    // Create pipeline
    QRhiGraphicsPipeline *pipeline = m_submissionContext->rhi()->newGraphicsPipeline();
    graphicsPipeline->setPipeline(pipeline);

    pipeline->setShaderStages({ { QRhiShaderStage::Vertex, vertexShader },
                                { QRhiShaderStage::Fragment, fragmentShader } });

    pipeline->setShaderResourceBindings(shaderResourceBindings);

    auto rhiTopologyFromQt3DTopology = [] (const Qt3DRender::QGeometryRenderer::PrimitiveType t) {
        switch (t) {
        case Qt3DRender::QGeometryRenderer::Points:
        return QRhiGraphicsPipeline::Points;

        case Qt3DRender::QGeometryRenderer::Lines:
        return QRhiGraphicsPipeline::Lines;

        case Qt3DRender::QGeometryRenderer::LineStrip:
            return QRhiGraphicsPipeline::LineStrip;

        case Qt3DRender::QGeometryRenderer::Triangles:
        return QRhiGraphicsPipeline::Triangles;

        case Qt3DRender::QGeometryRenderer::TriangleStrip:
            return QRhiGraphicsPipeline::TriangleStrip;

        case Qt3DRender::QGeometryRenderer::TriangleFan:
            return QRhiGraphicsPipeline::TriangleFan;

        case Qt3DRender::QGeometryRenderer::LineLoop: {
            qWarning(Backend) << "LineLoop primitive type is not handled by RHI";
            return QRhiGraphicsPipeline::Lines;
        }

        case Qt3DRender::QGeometryRenderer::Patches: {
            qWarning(Backend) << "Patches primitive type is not handled by RHI";
            return QRhiGraphicsPipeline::Points;
        }

        case Qt3DRender::QGeometryRenderer::LinesAdjacency:
        case Qt3DRender::QGeometryRenderer::TrianglesAdjacency:
        case Qt3DRender::QGeometryRenderer::LineStripAdjacency:
        case Qt3DRender::QGeometryRenderer::TriangleStripAdjacency: {
            qWarning(Backend) << "Adjancency primitive types are not handled by RHI";
            return QRhiGraphicsPipeline::Points;
        }
        }
        return QRhiGraphicsPipeline::Points;
    };

    pipeline->setTopology(rhiTopologyFromQt3DTopology(cmd.m_primitiveType));

    QRhiVertexInputLayout inputLayout;
    inputLayout.setBindings(inputBindings.begin(), inputBindings.end());
    inputLayout.setAttributes(rhiAttributes.begin(), rhiAttributes.end());
    pipeline->setVertexInputLayout(inputLayout);
    graphicsPipeline->setAttributesToBindingHash(attributeNameToBinding);

    // Render States
    RenderStateSet *renderState = nullptr;
    {
        RenderStateSet *globalState =
                (rv->stateSet() != nullptr) ? rv->stateSet() : m_defaultRenderStateSet;

        // Merge global state into local state
        RenderStateSet *localState = cmd.m_stateSet.data();
        if (localState != nullptr) {
            localState->merge(globalState);
            renderState = localState;
        } else {
            renderState = globalState;
        }
    }
    m_submissionContext->applyStateSet(renderState, pipeline);

    // Setup potential texture render target
    const bool renderTargetIsSet = setupRenderTarget(rv, graphicsPipeline, rhiSwapChain);
    if (!renderTargetIsSet)
        return onFailure("No Render Target Set");

    if (!pipeline->create())
        return onFailure("Creation Failed");

    graphicsPipeline->markComplete();
}

void Renderer::updateComputePipeline(RenderCommand &cmd, RenderView *rv, int renderViewIndex)
{
    if (!cmd.m_rhiShader) {
        qCWarning(Backend) << "Command has no shader";
        return;
    }

    // Try to retrieve existing pipeline
    RHIComputePipelineManager *pipelineManager = m_RHIResourceManagers->rhiComputePipelineManager();
    const ComputePipelineIdentifier pipelineKey { cmd.m_shaderId, renderViewIndex };
    RHIComputePipeline *computePipeline = pipelineManager->lookupResource(pipelineKey);
    if (computePipeline == nullptr) {
        // Init UBOSet the first time we allocate a new pipeline
        computePipeline = pipelineManager->getOrCreateResource(pipelineKey);
        computePipeline->setKey(pipelineKey);
        computePipeline->uboSet()->setResourceManager(m_RHIResourceManagers);
        computePipeline->uboSet()->setNodeManagers(m_nodesManager);
        computePipeline->uboSet()->initializeLayout(m_submissionContext.data(), cmd.m_rhiShader);
    }

    if (!computePipeline) {
        qCWarning(Backend) << "Could not create a compute pipeline";
        return;
    }

    // Increase score so that we know the pipeline was used for this frame and shouldn't be
    // destroyed
    computePipeline->increaseScore();

    // Record command reference in UBOSet
    computePipeline->uboSet()->addRenderCommand(cmd);

    // Store association between RV and pipeline
    if (auto& pipelines = m_rvToComputePipelines[rv]; !Qt3DCore::contains(pipelines, computePipeline))
        pipelines.push_back(computePipeline);

    // Record RHIGraphicsPipeline into command for later use
    cmd.pipeline = computePipeline;

    // TO DO: Set to true if geometry, shader or render state dirty
    bool requiredRebuild = false;

    // Note: we can rebuild add/remove things from the QRhiShaderResourceBindings after having
    // created the pipeline and rebuild it. Changes should be picked up automatically

    // Create pipeline if it doesn't exist or needs to be updated
    if (computePipeline->pipeline() == nullptr || requiredRebuild)
        buildComputePipelines(computePipeline, rv, cmd);
}

void Renderer::buildComputePipelines(RHIComputePipeline *computePipeline,
                                     RenderView *rv,
                                     const RenderCommand &cmd)
{
    Q_UNUSED(rv);
    auto onFailure = [&] {
        qCWarning(Backend) << "Failed to build compute pipeline";
    };

    PipelineUBOSet *uboSet = computePipeline->uboSet();
    RHIShader *shader = cmd.m_rhiShader;

    // Setup shaders
    const QShader& computeShader = cmd.m_rhiShader->shaderStage(QShader::ComputeStage);
    if (!computeShader.isValid())
        return onFailure();

    // Set Resource Bindings
    const std::vector<QRhiShaderResourceBinding> resourceBindings = uboSet->resourceLayout(shader);
    QRhiShaderResourceBindings *shaderResourceBindings =
            m_submissionContext->rhi()->newShaderResourceBindings();
    computePipeline->setShaderResourceBindings(shaderResourceBindings);

    shaderResourceBindings->setBindings(resourceBindings.cbegin(), resourceBindings.cend());
    if (!shaderResourceBindings->create()) {
        return onFailure();
    }

    // Create pipeline
    QRhiComputePipeline *pipeline = m_submissionContext->rhi()->newComputePipeline();
    computePipeline->setPipeline(pipeline);

    pipeline->setShaderStage(QRhiShaderStage{QRhiShaderStage::Compute, computeShader});
    pipeline->setShaderResourceBindings(shaderResourceBindings);

    // QRhiComputePiple has no render states

    if (!pipeline->create())
        return onFailure();
}

void Renderer::createRenderTarget(RenderTarget *target)
{
    const Qt3DCore::QNodeId &renderTargetId = target->peerId();
    RHIRenderTargetManager *rhiRenderTargetManager = m_RHIResourceManagers->rhiRenderTargetManager();

    Q_ASSERT(!m_RHIResourceManagers->rhiRenderTargetManager()->contains(renderTargetId));
    RHIRenderTarget *rhiTarget = rhiRenderTargetManager->getOrCreateResource(renderTargetId);

    RHITextureManager *texman = rhiResourceManagers()->rhiTextureManager();
    // TO DO: We use all render targets and ignore the fact that
    // QRenderTargetSelector can specify a subset of outputs
    // -> We should propably remove that from the frontend API
    // as it's hard to handle for us and very unlikely anyone uses that
    const AttachmentPack pack = AttachmentPack(target, m_nodesManager->attachmentManager());
    QRhiTextureRenderTargetDescription desc;

    QSize targetSize{};
    int targetSamples{1};
    QVarLengthArray<QRhiColorAttachment, 8> rhiAttachments;

    bool hasDepthTexture = false;

    // Used in case of failure
    QVarLengthArray<QRhiResource*> resourcesToClean;
    auto cleanAllocatedResources = [&] {
        QStringList descDetails;
        auto texDetails = [](QRhiTexture *tex) {
            return QString("Texture format: %1; flags: %2; samples: %3").arg(tex->format()).arg(tex->flags()).arg(tex->sampleCount());
        };
        auto bufferDetails = [](QRhiRenderBuffer* buffer) {
            return QString("Buffer Type: %1; flags: %2; samples: %3").arg(buffer->type()).arg(buffer->flags()).arg(buffer->sampleCount());
        };
        const auto itEnd = desc.cendColorAttachments();
        for (auto it = desc.cbeginColorAttachments(); it != itEnd; ++it) {
            QString attDetails = QString("Layer: %1; Level: %2; ").arg(it->layer()).arg(it->level());
            if (it->texture())
                attDetails += texDetails(it->texture());
            descDetails << attDetails;
        }
        if (desc.depthTexture())
            descDetails << QString("Depth Texture: %1").arg(texDetails(desc.depthTexture()));
        if (desc.depthStencilBuffer())
            descDetails << QString("Depth Buffer: %1").arg(bufferDetails(desc.depthStencilBuffer()));
        qCWarning(Backend) << "Failed to create RenderTarget" << renderTargetId << "\n" << descDetails;
        for (auto res : resourcesToClean) {
            res->destroy();
            delete res;
        }
    };

    RHIRenderTarget::BackBuffer backBuffer = RHIRenderTarget::BackBuffer::None;

    // Look up attachments to populate the RT description
    // Attachments are sorted by attachment point (Color0 is first)
    for (const Attachment &attachment : pack.attachments()) {

        if (attachment.m_point == QRenderTargetOutput::Left || attachment.m_point == QRenderTargetOutput::Right) {
            backBuffer = attachment.m_point == QRenderTargetOutput::Left ? RHIRenderTarget::BackBuffer::Left : RHIRenderTarget::BackBuffer::Right;
            break;
        }

        RHITexture *tex = texman->lookupResource(attachment.m_textureUuid);
        if (tex && tex->getRhiTexture()) {
            auto rhiTex = tex->getRhiTexture();
            if (!rhiTex->flags().testFlag(QRhiTexture::RenderTarget) ||
                !rhiTex->flags().testFlag(QRhiTexture::UsedAsTransferSource)) {
                // UsedAsTransferSource is required if we ever want to read back from the texture
                rhiTex->destroy();
                rhiTex->setFlags(rhiTex->flags() | QRhiTexture::RenderTarget|QRhiTexture::UsedAsTransferSource);
                rhiTex->create();
            }
            switch (rhiTex->format()) {
            case QRhiTexture::Format::D16:
            case QRhiTexture::Format::D24:
            case QRhiTexture::Format::D24S8:
            case QRhiTexture::Format::D32F: {
                desc.setDepthTexture(rhiTex);
                targetSize = tex->size();
                hasDepthTexture = true;
                break;
            }
            default: {
                QRhiColorAttachment rhiAtt{rhiTex};
                // TODO handle cubemap face
                targetSize = tex->size();
                targetSamples = tex->properties().samples;

                rhiAtt.setLayer(attachment.m_layer);
                rhiAtt.setLevel(attachment.m_mipLevel);
                rhiAttachments.push_back(rhiAtt);

                break;
            }
            }
        } else {
            cleanAllocatedResources();
            return;
        }
    }

    rhiTarget->backBuffer = backBuffer;
    // If we are targeting one of the back buffers directly, don't create an offscreen RT
    if (backBuffer != RHIRenderTarget::BackBuffer::None)
        return;

    // Otherwise, create QRhiRenderBuffer and associated resources
    if (targetSize.width() <= 0 || targetSize.height() <= 0) {
        cleanAllocatedResources();
        return;
    }

    desc.setColorAttachments(rhiAttachments.begin(), rhiAttachments.end());

    // Potentially create a depth & stencil renderbuffer
    QRhiRenderBuffer *ds{};
    if (!hasDepthTexture) {
        ds = m_submissionContext->rhi()->newRenderBuffer(QRhiRenderBuffer::DepthStencil, targetSize, targetSamples);
        resourcesToClean << ds;

        if (!ds->create()) {
            cleanAllocatedResources();
            return;
        }
        desc.setDepthStencilBuffer(ds);
    }

    // Create the render target
    auto rt = m_submissionContext->rhi()->newTextureRenderTarget(desc);
    resourcesToClean << rt;

    auto rp = rt->newCompatibleRenderPassDescriptor();
    resourcesToClean << rp;

    rt->setRenderPassDescriptor(rp);

    if (!rt->create()) {
        cleanAllocatedResources();
        rhiRenderTargetManager->releaseResource(renderTargetId);
        return;
    }

    rhiTarget->renderTarget = rt;
    rhiTarget->renderPassDescriptor = rp;
    rhiTarget->depthStencilBuffer = ds;
}

bool Renderer::setupRenderTarget(RenderView *rv,
                                 RHIGraphicsPipeline *graphicsPipeline,
                                 QRhiSwapChain *swapchain)
{
    QRhiGraphicsPipeline *rhiPipeline = graphicsPipeline->pipeline();

    const auto &managers = *nodeManagers();
    auto &renderTargetManager = *managers.renderTargetManager();

    auto useSwapchainForPipeline = [&]() {
        Q_ASSERT(swapchain);
        rhiPipeline->setRenderPassDescriptor(swapchain->renderPassDescriptor());
        rhiPipeline->setSampleCount(swapchain->sampleCount());
    };

    auto *renderTarget = renderTargetManager.lookupResource(rv->renderTargetId());
    if (renderTarget) {
        // Render to texture
        const Qt3DCore::QNodeId &renderTargetId = renderTarget->peerId();
        RHIRenderTargetManager *rhiRenderTargetManager = m_RHIResourceManagers->rhiRenderTargetManager();
        RHIRenderTarget *rhiTarget = rhiRenderTargetManager->lookupResource(renderTargetId);

        if (!rhiTarget) {
            qWarning(Backend) << "Invalid RenderTarget " << renderTargetId << " for Pipeline";
            return false;
        }

        // The RenderTarget we reference might actually be referencing a swapchain back buffer
        if (rhiTarget->backBuffer != RHIRenderTarget::BackBuffer::None) {
            // Render to the default framebuffer on our swapchain
            useSwapchainForPipeline();
        } else {
            if (!rhiTarget->renderTarget) {
                qWarning(Backend) << "Invalid RenderTarget " << renderTargetId << " for Pipeline";
                return false;
            }
            rhiPipeline->setRenderPassDescriptor(rhiTarget->renderPassDescriptor);
            rhiPipeline->setSampleCount(rhiTarget->renderTarget->sampleCount());
        }
        return true;

    } else if (m_submissionContext->defaultRenderTarget()) {
        // Use default RenderTarget if set Default FBO set by Scene3D
        QRhiRenderTarget *defaultTarget = m_submissionContext->defaultRenderTarget();;
        rhiPipeline->setRenderPassDescriptor(defaultTarget->renderPassDescriptor());
        rhiPipeline->setSampleCount(defaultTarget->sampleCount());
        return true;
    } else {
        // Render to the default framebuffer on our swapchain
        useSwapchainForPipeline();
        return true;
    }
}

// When this function is called, we must not be processing the commands for frame n+1
std::vector<Renderer::RHIPassInfo>
Renderer::prepareCommandsSubmission(const std::vector<RenderView *> &renderViews)
{
    const size_t renderViewCount = renderViews.size();

    // Gather all distinct RHIGraphicsPipeline we will use
    // For each RenderView, we will need to gather
    // -> The RHIGraphicsPipelines being used
    // -> The number of RenderCommands used by each pipeline

    // This will allows us to generate UBOs based on the number of commands/rv we have
    RHIGraphicsPipelineManager *graphicsPipelineManager = m_RHIResourceManagers->rhiGraphicsPipelineManager();
    const std::vector<HRHIGraphicsPipeline> &graphicsPipelinesHandles = graphicsPipelineManager->activeHandles();
    for (HRHIGraphicsPipeline pipelineHandle : graphicsPipelinesHandles) {
        RHIGraphicsPipeline *pipeline = graphicsPipelineManager->data(pipelineHandle);
        // Reset PipelineUBOSet
        pipeline->uboSet()->clear();
    }
    RHIComputePipelineManager *computePipelineManager = m_RHIResourceManagers->rhiComputePipelineManager();
    const std::vector<HRHIComputePipeline> &computePipelinesHandles = computePipelineManager->activeHandles();
    for (HRHIComputePipeline pipelineHandle : computePipelinesHandles) {
        RHIComputePipeline *pipeline = computePipelineManager->data(pipelineHandle);
        // Reset PipelineUBOSet
        pipeline->uboSet()->clear();
    }

    // Clear any reference between RV and Pipelines we had
    // as we are about to rebuild these
    m_rvToGraphicsPipelines.clear();
    m_rvToComputePipelines.clear();

    // We need to have a single RHI RenderPass per RenderTarget
    // as creating the pass clears the buffers
    // 1) We need to find all adjacents RenderViews that have the same renderTarget
    // and submit all of these as part of the same RHI pass
    std::vector<RHIPassInfo> rhiPassesInfo;

    for (size_t i = 0; i < renderViewCount;) {
        std::vector<RenderView *> sameRenderTargetRVs;
        RenderView *refRV = renderViews[i];
        sameRenderTargetRVs.push_back(refRV);

        for (i = i + 1; i < renderViewCount; ++i) {
            RenderView *curRV = renderViews[i];
            if (refRV->renderTargetId() == curRV->renderTargetId()) {
                sameRenderTargetRVs.push_back(curRV);
            } else
                break;
        }

        RHIPassInfo bucket;
        bucket.rvs = std::move(sameRenderTargetRVs);
        bucket.surface = refRV->surface();
        bucket.renderTargetId = refRV->renderTargetId();
        rhiPassesInfo.push_back(bucket);
    }

    RHIShaderManager *rhiShaderManager = m_RHIResourceManagers->rhiShaderManager();
    // Assign a Graphics Pipeline to each RenderCommand
    for (size_t i = 0; i < renderViewCount; ++i) {
        RenderView *rv = renderViews[i];

        // Handle BlitFrameBufferCase
        if (rv->hasBlitFramebufferInfo())
            qWarning(Backend) << "The RHI backend doesn't support Blit operations. Instead, we recommend drawing a full screen quad with a custom shader and resolving manually.";

        rv->forEachCommand([&] (RenderCommand &command) {
            // Update/Create GraphicsPipelines
            if (command.m_type == RenderCommand::Draw) {
                Geometry *rGeometry =
                        m_nodesManager->data<Geometry, GeometryManager>(command.m_geometry);
                GeometryRenderer *rGeometryRenderer =
                        m_nodesManager->data<GeometryRenderer, GeometryRendererManager>(
                                command.m_geometryRenderer);

                command.m_rhiShader = rhiShaderManager->lookupResource(command.m_shaderId);
                // By this time shaders should have been loaded
                RHIShader *shader = command.m_rhiShader;
                if (!shader)
                    return;

                // We should never have inserted a command for which these are null
                // in the first place
                Q_ASSERT(rGeometry && rGeometryRenderer && shader);

                // Unset dirtiness on rGeometryRenderer only
                // The rGeometry may be shared by several rGeometryRenderer
                // so we cannot unset its dirtiness at this point
                if (rGeometryRenderer->isDirty())
                    rGeometryRenderer->unsetDirty();

                updateGraphicsPipeline(command, rv);

            } else if (command.m_type == RenderCommand::Compute) {
                // By this time shaders have been loaded
                RHIShader *shader = command.m_rhiShader;
                if (!shader)
                    return;

                updateComputePipeline(command, rv, int(i));
            }
        });
    }

    // Now that we know how many pipelines we have and how many RC each pipeline
    // has, we can allocate/reallocate UBOs with correct size for each pipelines
    for (RenderView *rv : renderViews) {
        // Allocate UBOs for pipelines used in current RV
        const std::vector<RHIGraphicsPipeline *> &rvGraphicsPipelines = m_rvToGraphicsPipelines[rv];
        for (RHIGraphicsPipeline *pipeline : rvGraphicsPipelines)
            pipeline->uboSet()->allocateUBOs(m_submissionContext.data());
        // Allocate UBOs for pipelines used in current RV
        const std::vector<RHIComputePipeline *> &rvComputePipelines = m_rvToComputePipelines[rv];
        for (RHIComputePipeline *pipeline : rvComputePipelines)
            pipeline->uboSet()->allocateUBOs(m_submissionContext.data());
    }

    // Unset dirtiness on Geometry and Attributes
    // Note: we cannot do it in the loop above as we want to be sure that all
    // the VAO which reference the geometry/attributes are properly updated
    RHI_UNIMPLEMENTED;
    for (Attribute *attribute : std::as_const(m_dirtyAttributes))
        attribute->unsetDirty();
    m_dirtyAttributes.clear();

    for (Geometry *geometry : std::as_const(m_dirtyGeometry))
        geometry->unsetDirty();
    m_dirtyGeometry.clear();

    return rhiPassesInfo;
}

// Executed in a job
void Renderer::lookForDirtyBuffers()
{
    const std::vector<HBuffer> &activeBufferHandles = m_nodesManager->bufferManager()->activeHandles();
    for (const HBuffer &handle : activeBufferHandles) {
        Buffer *buffer = m_nodesManager->bufferManager()->data(handle);
        if (buffer->isDirty())
            m_dirtyBuffers.push_back(handle);
    }
}

// Called in prepareSubmission
void Renderer::lookForDownloadableBuffers()
{
    m_downloadableBuffers.clear();
    const std::vector<HBuffer> &activeBufferHandles = m_nodesManager->bufferManager()->activeHandles();
    for (const HBuffer &handle : activeBufferHandles) {
        Buffer *buffer = m_nodesManager->bufferManager()->data(handle);
        if (buffer->access() & Qt3DCore::QBuffer::Read)
            m_downloadableBuffers.push_back(buffer->peerId());
    }
}

// Executed in a job
void Renderer::lookForDirtyTextures()
{
    // To avoid having Texture or TextureImage maintain relationships between
    // one another, we instead perform a lookup here to check if a texture
    // image has been updated to then notify textures referencing the image
    // that they need to be updated
    TextureImageManager *imageManager = m_nodesManager->textureImageManager();
    const std::vector<HTextureImage> &activeTextureImageHandles = imageManager->activeHandles();
    Qt3DCore::QNodeIdVector dirtyImageIds;
    for (const HTextureImage &handle : activeTextureImageHandles) {
        TextureImage *image = imageManager->data(handle);
        if (image->isDirty()) {
            dirtyImageIds.push_back(image->peerId());
            image->unsetDirty();
        }
    }

    TextureManager *textureManager = m_nodesManager->textureManager();
    const std::vector<HTexture> &activeTextureHandles = textureManager->activeHandles();
    for (const HTexture &handle : activeTextureHandles) {
        Texture *texture = textureManager->data(handle);
        const Qt3DCore::QNodeIdVector imageIds = texture->textureImageIds();

        // Does the texture reference any of the dirty texture images?
        for (const Qt3DCore::QNodeId &imageId : imageIds) {
            if (dirtyImageIds.contains(imageId)) {
                texture->addDirtyFlag(Texture::DirtyImageGenerators);
                break;
            }
        }

        // Dirty meaning that something has changed on the texture
        // either properties, parameters, shared texture id, generator or a texture image
        if (texture->dirtyFlags() != Texture::NotDirty)
            m_dirtyTextures.push_back(handle);
        // Note: texture dirty flags are reset when actually updating the
        // textures in updateGLResources() as resetting flags here would make
        // us lose information about what was dirty exactly.
    }
}

// Executed in a job
void Renderer::reloadDirtyShaders()
{
    Q_ASSERT(isRunning());
    const std::vector<HTechnique> &activeTechniques =
            m_nodesManager->techniqueManager()->activeHandles();
    const std::vector<HShaderBuilder> &activeBuilders =
            m_nodesManager->shaderBuilderManager()->activeHandles();
    for (const HTechnique &techniqueHandle : activeTechniques) {
        Technique *technique = m_nodesManager->techniqueManager()->data(techniqueHandle);
        // If api of the renderer matches the one from the technique
        if (technique->isCompatibleWithRenderer()) {
            const auto passIds = technique->renderPasses();
            for (const Qt3DCore::QNodeId &passId : passIds) {
                RenderPass *renderPass =
                        m_nodesManager->renderPassManager()->lookupResource(passId);
                HShader shaderHandle =
                        m_nodesManager->shaderManager()->lookupHandle(renderPass->shaderProgram());
                Shader *shader = m_nodesManager->shaderManager()->data(shaderHandle);

                ShaderBuilder *shaderBuilder = nullptr;
                for (const HShaderBuilder &builderHandle : activeBuilders) {
                    ShaderBuilder *builder =
                            m_nodesManager->shaderBuilderManager()->data(builderHandle);
                    if (builder->shaderProgramId() == shader->peerId()) {
                        shaderBuilder = builder;
                        break;
                    }
                }

                if (shaderBuilder) {
                    shaderBuilder->setGraphicsApi(*technique->graphicsApiFilter());

                    for (int i = 0; i <= QShaderProgram::Compute; i++) {
                        const auto shaderType = static_cast<QShaderProgram::ShaderType>(i);
                        if (!shaderBuilder->shaderGraph(shaderType).isValid())
                            continue;

                        if (shaderBuilder->isShaderCodeDirty(shaderType)) {
                            shaderBuilder->generateCode(shaderType);
                            Qt3DCore::moveAtEnd(m_shaderBuilderUpdates, shaderBuilder->takePendingUpdates());
                        }

                        const auto code = shaderBuilder->shaderCode(shaderType);
                        shader->setShaderCode(shaderType, code);
                    }
                }

                if (shader != nullptr && shader->isDirty()) {
                    if (!Qt3DCore::contains(m_dirtyShaders, shaderHandle))
                        m_dirtyShaders.push_back(shaderHandle);
                }
            }
        }
    }
}

// Executed in job postFrame (in main thread when jobs are done)
void Renderer::sendShaderChangesToFrontend(Qt3DCore::QAspectManager *manager)
{
    Q_ASSERT(isRunning());

    // Sync Shader
    const std::vector<HShader> &activeShaders = m_nodesManager->shaderManager()->activeHandles();
    for (const HShader &handle : activeShaders) {
        Shader *s = m_nodesManager->shaderManager()->data(handle);
        if (!s)
            continue;

        if (s->requiresFrontendSync()) {
            QShaderProgram *frontend =
                    static_cast<decltype(frontend)>(manager->lookupNode(s->peerId()));
            if (frontend) {
                QShaderProgramPrivate *dFrontend =
                        static_cast<decltype(dFrontend)>(Qt3DCore::QNodePrivate::get(frontend));
                dFrontend->setStatus(s->status());
                dFrontend->setLog(s->log());
                s->unsetRequiresFrontendSync();
            }
        }
    }

    // Sync ShaderBuilder
    for (const ShaderBuilderUpdate &update : m_shaderBuilderUpdates) {
        QShaderProgramBuilder *builder =
                static_cast<decltype(builder)>(manager->lookupNode(update.builderId));
        if (!builder)
            continue;

        QShaderProgramBuilderPrivate *dBuilder =
                static_cast<decltype(dBuilder)>(Qt3DCore::QNodePrivate::get(builder));
        dBuilder->setShaderCode(update.shaderCode, update.shaderType);
    }
    m_shaderBuilderUpdates.clear();
}

// Executed in a job postFrame (in main thread when jobs are done)
void Renderer::sendTextureChangesToFrontend(Qt3DCore::QAspectManager *manager)
{
    const std::vector<QPair<Texture::TextureUpdateInfo, Qt3DCore::QNodeIdVector>>
            updateTextureProperties = std::move(m_updatedTextureProperties);
    for (const auto &pair : updateTextureProperties) {
        const Qt3DCore::QNodeIdVector targetIds = pair.second;
        for (const Qt3DCore::QNodeId &targetId : targetIds) {
            // Lookup texture
            Texture *t = m_nodesManager->textureManager()->lookupResource(targetId);
            // If backend texture is Dirty, some property has changed and the properties we are
            // about to send are already outdate
            if (t == nullptr || t->dirtyFlags() != Texture::NotDirty)
                continue;

            QAbstractTexture *texture =
                    static_cast<QAbstractTexture *>(manager->lookupNode(targetId));
            if (!texture)
                continue;
            const TextureProperties &properties = pair.first.properties;

            const bool blocked = texture->blockNotifications(true);
            texture->setWidth(properties.width);
            texture->setHeight(properties.height);
            texture->setDepth(properties.depth);
            texture->setLayers(properties.layers);
            texture->setFormat(properties.format);
            texture->blockNotifications(blocked);

            QAbstractTexturePrivate *dTexture =
                    static_cast<QAbstractTexturePrivate *>(Qt3DCore::QNodePrivate::get(texture));
            dTexture->setStatus(properties.status);
            dTexture->setHandleType(pair.first.handleType);
            dTexture->setHandle(pair.first.handle);
        }
    }
}

// Executed in a job postFrame (in main thread when jobs done)
void Renderer::sendDisablesToFrontend(Qt3DCore::QAspectManager *manager)
{
    // SubtreeEnabled
    const auto updatedDisables = Qt3DCore::moveAndClear(m_updatedDisableSubtreeEnablers);
    for (const auto &nodeId : updatedDisables) {
        QSubtreeEnabler *frontend = static_cast<decltype(frontend)>(manager->lookupNode(nodeId));
        frontend->setEnabled(false);
    }

    // Compute Commands
    const std::vector<HComputeCommand> &activeCommands =
            m_nodesManager->computeJobManager()->activeHandles();
    for (const HComputeCommand &handle : activeCommands) {
        ComputeCommand *c = m_nodesManager->computeJobManager()->data(handle);
        if (c->hasReachedFrameCount()) {
            QComputeCommand *frontend =
                    static_cast<decltype(frontend)>(manager->lookupNode(c->peerId()));
            frontend->setEnabled(false);
            c->resetHasReachedFrameCount();
        }
    }
}

bool Renderer::prepareGeometryInputBindings(const Geometry *geometry, const RHIShader *shader,
                                            QVarLengthArray<QRhiVertexInputBinding, 8> &inputBindings,
                                            QVarLengthArray<QRhiVertexInputAttribute, 8> &rhiAttributes,
                                            QHash<int, int> &attributeNameToBinding)
{
    // shader requires no attributes
    if (shader->attributes().empty())
        return true;

    // QRhiVertexInputBinding -> specifies the stride of an attribute,
    // whether it's per vertex or per instance and the instance divisor

    // QRhiVertexInputAttribute -> specifies the format of the attribute
    // (offset, type), the shader location and the index of the binding
    // QRhiCommandBuffer::VertexInput -> binds a buffer to a binding
    struct BufferBinding
    {
        Qt3DCore::QNodeId bufferId;
        uint stride;
        QRhiVertexInputBinding::Classification classification;
        uint attributeDivisor;
    };
    std::vector<BufferBinding> uniqueBindings;

    const auto &attributesIds = geometry->attributes();

    for (Qt3DCore::QNodeId attribute_id : attributesIds) {
        Attribute *attrib = m_nodesManager->attributeManager()->lookupResource(attribute_id);
        if (attrib->attributeType() != Qt3DCore::QAttribute::VertexAttribute)
            continue;
        const int location = locationForAttribute(attrib, shader);
        // In case the shader doesn't use the attribute, we would get no
        // location. This is not a failure, just that we provide more attributes
        // than required.
        if (location == -1)
            continue;

        const bool isPerInstanceAttr = attrib->divisor() != 0;
        const QRhiVertexInputBinding::Classification classification = isPerInstanceAttr
                ? QRhiVertexInputBinding::PerInstance
                : QRhiVertexInputBinding::PerVertex;

        auto getAttributeByteSize = [](const Qt3DCore::QAttribute::VertexBaseType type) {
            switch (type) {
            case Qt3DCore::QAttribute::Byte:
            case Qt3DCore::QAttribute::UnsignedByte:
                return 1;
            case Qt3DCore::QAttribute::Short:
            case Qt3DCore::QAttribute::UnsignedShort:
            case Qt3DCore::QAttribute::HalfFloat:
                return 2;
            case Qt3DCore::QAttribute::Int:
            case Qt3DCore::QAttribute::UnsignedInt:
            case Qt3DCore::QAttribute::Float:
                return 4;
            case Qt3DCore::QAttribute::Double:
                return 8;
            }
            return 0;
        };

        uint byteStride = attrib->byteStride();
        const uint vertexTypeByteSize = getAttributeByteSize(attrib->vertexBaseType());
        // in GL 0 means tighly packed, we therefore assume a tighly packed
        // attribute and compute the stride if that happens
        if (byteStride == 0)
            byteStride = attrib->vertexSize() * vertexTypeByteSize;

        const BufferBinding binding = { attrib->bufferId(), byteStride,
                                        classification,
                                        isPerInstanceAttr ? attrib->divisor() : 1U };


        const auto it = std::find_if(uniqueBindings.begin(), uniqueBindings.end(),
                                     [binding](const BufferBinding &a) {
            return binding.bufferId == a.bufferId
                    && binding.stride == a.stride
                    && binding.classification == a.classification
                    && binding.attributeDivisor == a.attributeDivisor;
        });

        int bindingIndex = int(uniqueBindings.size());
        if (it == uniqueBindings.end())
            uniqueBindings.push_back(binding);
        else
            bindingIndex = std::distance(uniqueBindings.begin(), it);

        const auto attributeType = rhiAttributeType(attrib);
        if (!attributeType) {
            qCWarning(Backend) << "An attribute type is not supported" << attrib->name() << attrib->vertexBaseType();
            return false;
        }

        // Special Handling for Matrix as Vertex Attributes
        // If an attribute has a size > 4 it can only be a matrix based type
        // for which we will actually upload several attributes at contiguous
        // locations
        const uint elementsPerColumn = 4;
        const int attributeSpan = std::ceil(float(attrib->vertexSize()) / elementsPerColumn);
        for (int i = 0; i < attributeSpan; ++i) {
            rhiAttributes.push_back({ bindingIndex,
                                      location + i,
                                      *attributeType,
                                      attrib->byteOffset() + (i * elementsPerColumn * vertexTypeByteSize)});
        }

        attributeNameToBinding.insert(attrib->nameId(), bindingIndex);
    }

    inputBindings.resize(uniqueBindings.size());
    for (int i = 0, m = int(uniqueBindings.size()); i < m; ++i) {
        const BufferBinding binding = uniqueBindings.at(i);

        /*
        qDebug() << "binding"
                 << binding.bufferId
                 << binding.stride
                 << binding.classification
                 << binding.attributeDivisor;
        //*/

        inputBindings[i] = QRhiVertexInputBinding{ binding.stride, binding.classification,
                                                   binding.attributeDivisor };
    }

    return true;
}

// Render Thread (or QtQuick RenderThread when using Scene3D)
// Scene3D: When using Scene3D rendering, we can't assume that when
// updateGLResources is called, the resource handles points to still existing
// objects. This is because Scene3D calls doRender independently of whether all
// jobs have completed or not which in turn calls proceedToNextFrame under some
// conditions. Such conditions are usually met on startup to avoid deadlocks.
// proceedToNextFrame triggers the syncChanges calls for the next frame, which
// may contain destruction changes targeting resources. When the above
// happens, this can result in the dirtyResource vectors containing handles of
// objects that may already have been destroyed
void Renderer::updateResources()
{
    {
        const std::vector<HBuffer> dirtyBufferHandles = Qt3DCore::moveAndClear(m_dirtyBuffers);
        for (const HBuffer &handle : dirtyBufferHandles) {
            Buffer *buffer = m_nodesManager->bufferManager()->data(handle);

            // Can be null when using Scene3D rendering
            if (buffer == nullptr)
                continue;

            // Forces creation if it doesn't exit
            // Also note the binding point doesn't really matter here, we just upload data
            if (!m_submissionContext->hasRHIBufferForBuffer(buffer))
                m_submissionContext->rhiBufferForRenderBuffer(buffer);
            // Update the RHIBuffer data
            m_submissionContext->updateBuffer(buffer);
            buffer->unsetDirty();
        }
    }

    RHIGraphicsPipelineManager *graphicsPipelineManager = m_RHIResourceManagers->rhiGraphicsPipelineManager();
    RHIComputePipelineManager *computePipelineManager = m_RHIResourceManagers->rhiComputePipelineManager();

    {
        const std::vector<HShader> dirtyShaderHandles = Qt3DCore::moveAndClear(m_dirtyShaders);
        ShaderManager *shaderManager = m_nodesManager->shaderManager();
        for (const HShader &handle : dirtyShaderHandles) {
            Shader *shader = shaderManager->data(handle);

            // Can be null when using Scene3D rendering
            if (shader == nullptr)
                continue;

            // Compile shader
            m_submissionContext->loadShader(shader, shaderManager,
                                            m_RHIResourceManagers->rhiShaderManager());

            // Release pipelines that reference the shaderId
            // to ensure they get rebuilt with updated shader
            graphicsPipelineManager->releasePipelinesReferencingShader(shader->peerId());
            computePipelineManager->releasePipelinesReferencingShader(shader->peerId());
        }
    }

    std::vector<RHITexture *> updatedRHITextures;

    // Create/Update textures. We record the update info to later fill
    // m_updatedTextureProperties once we are use the RHITextures have been
    // fully created (as creating the RenderTargets below could change existing
    // RHITextures)
    {
        const std::vector<HTexture> activeTextureHandles = Qt3DCore::moveAndClear(m_dirtyTextures);
        for (const HTexture &handle : activeTextureHandles) {
            Texture *texture = m_nodesManager->textureManager()->data(handle);

            // Can be null when using Scene3D rendering
            if (texture == nullptr)
                continue;

            // Create or Update RHITexture (the RHITexture instance is created if required
            // and all things that can take place without a GL context are done here)
            updateTexture(texture);
        }
        // We want to upload textures data at this point as the SubmissionThread and
        // AspectThread are locked ensuring no races between Texture/TextureImage and
        // RHITexture
        if (m_submissionContext != nullptr) {
            RHITextureManager *rhiTextureManager = m_RHIResourceManagers->rhiTextureManager();
            const std::vector<HRHITexture> &rhiTextureHandles = rhiTextureManager->activeHandles();
            // Upload texture data
            for (const HRHITexture &rhiTextureHandle : rhiTextureHandles) {
                RHI_UNIMPLEMENTED;
                RHITexture *rhiTexture = rhiTextureManager->data(rhiTextureHandle);

                // We create/update the actual RHI texture using the RHI context at this point
                const RHITexture::TextureUpdateInfo info =
                        rhiTexture->createOrUpdateRhiTexture(m_submissionContext.data());

                if (info.wasUpdated) {
                    // RHITexture creation provides us width/height/format ... information
                    // for textures which had not initially specified these information
                    // (TargetAutomatic...) Gather these information and store them to be distributed by
                    // a change next frame
                    const Qt3DCore::QNodeIdVector referenceTextureIds = { rhiTextureManager->texNodeIdForRHITexture.value(rhiTexture) };
                    // Store properties and referenceTextureIds
                    Texture::TextureUpdateInfo updateInfo;
                    updateInfo.properties = info.properties;
                    // Record texture updates to notify frontend (we are sure at this stage
                    // that the internal QRHITexture won't be updated further for this frame
                    m_updatedTextureProperties.push_back({ updateInfo, referenceTextureIds });
                    updatedRHITextures.push_back(rhiTexture);
                }
            }
        }

        // Record ids of texture to cleanup while we are still blocking the aspect thread
        m_textureIdsToCleanup += m_nodesManager->textureManager()->takeTexturesIdsToCleanup();
    }


    // Find dirty renderTargets
    // -> attachments added/removed
    // -> attachments textures updated (new dimensions, format ...)
    // -> destroy pipelines associated with dirty renderTargets

    // Note: we might end up recreating some of the internal textures when
    // creating the RenderTarget as those might have been created above without
    // the proper RenderTarget/TransformSource flags
    {
        RHIRenderTargetManager *rhiRenderTargetManager = m_RHIResourceManagers->rhiRenderTargetManager();
        RenderTargetManager *renderTargetManager = m_nodesManager->renderTargetManager();
        AttachmentManager *attachmentManager = m_nodesManager->attachmentManager();
        const std::vector<HTarget> &activeHandles = renderTargetManager->activeHandles();
        for (const HTarget &hTarget : activeHandles) {
            const Qt3DCore::QNodeId renderTargetId = hTarget->peerId();
            bool isDirty = hTarget->isDirty() || !rhiRenderTargetManager->contains(renderTargetId);

            // Check dirtiness of attachments if RenderTarget is not dirty
            if (!isDirty) {
                const Qt3DCore::QNodeIdVector &attachmentIds = hTarget->renderOutputs();
                for (const Qt3DCore::QNodeId &attachmentId : attachmentIds) {
                    RenderTargetOutput *output = attachmentManager->lookupResource(attachmentId);

                    auto it = std::find_if(m_updatedTextureProperties.begin(),
                                           m_updatedTextureProperties.end(),
                                           [&output] (const QPair<Texture::TextureUpdateInfo, Qt3DCore::QNodeIdVector> &updateData) {
                        const Qt3DCore::QNodeIdVector &referencedTextureIds = updateData.second;
                        return referencedTextureIds.contains(output->textureUuid());
                    });
                    // Attachment references a texture which was updated
                    isDirty = (it != m_updatedTextureProperties.end());
                }
            }

            if (isDirty) {
                hTarget->unsetDirty();
                // We need to destroy the render target and the pipelines associated with it
                // so that they can be recreated
                // If the RT was never created, the 2 lines below are noop
                graphicsPipelineManager->releasePipelinesReferencingRenderTarget(renderTargetId);
                rhiRenderTargetManager->releaseResource(renderTargetId);

                // Create RenderTarget
                createRenderTarget(hTarget.data());
            }
        }
    }


    // Note: we can only retrieve the internal QRhiResource handle to set on
    // the frontend nodes after we are sure we are no going to modify the
    // QRhiTextures (which happens when we create the Textures or the
    // RenderTargets)
    {
        for (size_t i = 0, m = m_updatedTextureProperties.size(); i < m; ++i) {
            auto &updateInfoPair = m_updatedTextureProperties[i];
            RHITexture *rhiTexture = updatedRHITextures[i];
            QRhiTexture *qRhiTexture = rhiTexture->getRhiTexture();
            Texture::TextureUpdateInfo &updateInfo = updateInfoPair.first;
            updateInfo.handleType = QAbstractTexture::RHITextureId;
            updateInfo.handle = qRhiTexture ? QVariant(qRhiTexture->nativeTexture().object) : QVariant();
        }
    }

    // Record list of buffer that might need uploading
    lookForDownloadableBuffers();
}

// Render Thread
void Renderer::updateTexture(Texture *texture)
{
    RHI_UNIMPLEMENTED;
    // Check that the current texture images are still in place, if not, do not update
    const bool isValid = texture->isValid(m_nodesManager->textureImageManager());
    if (!isValid) {
        qCWarning(Backend) << "QTexture referencing invalid QTextureImages";
        return;
    }

    // All textures are unique, if you instanciate twice the exact same texture
    // this will create 2 identical GLTextures, no sharing will take place

    // Try to find the associated RHITexture for the backend Texture
    RHITextureManager *rhiTextureManager = m_RHIResourceManagers->rhiTextureManager();
    RHITexture *rhiTexture = rhiTextureManager->lookupResource(texture->peerId());

    // No RHITexture associated yet -> create it
    if (rhiTexture == nullptr) {
        rhiTexture = rhiTextureManager->getOrCreateResource(texture->peerId());
        rhiTextureManager->texNodeIdForRHITexture.insert(rhiTexture, texture->peerId());
    }

    // Update RHITexture to match Texture instance
    const Texture::DirtyFlags dirtyFlags = texture->dirtyFlags();
    if (dirtyFlags.testFlag(Texture::DirtySharedTextureId))
        rhiTexture->setSharedTextureId(texture->sharedTextureId());

    if (dirtyFlags.testFlag(Texture::DirtyProperties))
        rhiTexture->setProperties(texture->properties());

    if (dirtyFlags.testFlag(Texture::DirtyParameters))
        rhiTexture->setParameters(texture->parameters());

    // Will make the texture requestUpload
    if (dirtyFlags.testFlag(Texture::DirtyImageGenerators)) {
        const Qt3DCore::QNodeIdVector textureImageIds = texture->textureImageIds();
        std::vector<RHITexture::Image> images;
        images.reserve(textureImageIds.size());
        // TODO: Move this into RHITexture directly
        for (const Qt3DCore::QNodeId &textureImageId : textureImageIds) {
            const TextureImage *img =
                    m_nodesManager->textureImageManager()->lookupResource(textureImageId);
            if (img == nullptr) {
                qCWarning(Backend) << "invalid TextureImage handle";
            } else {
                RHITexture::Image glImg { img->dataGenerator(), img->layer(), img->mipLevel(),
                                          img->face() };
                images.push_back(glImg);
            }
        }
        rhiTexture->setImages(images);
    }

    // Will make the texture requestUpload
    if (dirtyFlags.testFlag(Texture::DirtyDataGenerator))
        rhiTexture->setGenerator(texture->dataGenerator());

    // Will make the texture requestUpload
    if (dirtyFlags.testFlag(Texture::DirtyPendingDataUpdates))
        rhiTexture->addTextureDataUpdates(texture->takePendingTextureDataUpdates());

    // Unset the dirty flag on the texture
    texture->unsetDirty();
}

// Render Thread
void Renderer::cleanupTexture(Qt3DCore::QNodeId cleanedUpTextureId)
{
    RHITextureManager *rhiTextureManager = m_RHIResourceManagers->rhiTextureManager();
    RHITexture *glTexture = rhiTextureManager->lookupResource(cleanedUpTextureId);

    // Destroying the RHITexture implicitely also destroy the GL resources
    if (glTexture != nullptr) {
        rhiTextureManager->releaseResource(cleanedUpTextureId);
        rhiTextureManager->texNodeIdForRHITexture.remove(glTexture);
    }
}

// Render Thread
void Renderer::cleanupShader(const Shader *shader)
{
    RHIShaderManager *rhiShaderManager = m_RHIResourceManagers->rhiShaderManager();
    RHIShader *glShader = rhiShaderManager->lookupResource(shader->peerId());

    if (glShader != nullptr)
        rhiShaderManager->abandon(glShader, shader);
}

void Renderer::cleanupRenderTarget(const Qt3DCore::QNodeId &renderTargetId)
{
    RHIRenderTargetManager *rhiRenderTargetManager = m_RHIResourceManagers->rhiRenderTargetManager();

    rhiRenderTargetManager->releaseResource(renderTargetId);
}

// Called by SubmitRenderView
void Renderer::downloadRHIBuffers()
{
    const std::vector<Qt3DCore::QNodeId> downloadableHandles = Qt3DCore::moveAndClear(m_downloadableBuffers);
    for (const Qt3DCore::QNodeId &bufferId : downloadableHandles) {
        BufferManager *bufferManager = m_nodesManager->bufferManager();
        BufferManager::ReadLocker locker(const_cast<const BufferManager *>(bufferManager));
        Buffer *buffer = bufferManager->lookupResource(bufferId);
        // Buffer could have been destroyed at this point
        if (!buffer)
            continue;
        // locker is protecting us from the buffer being destroy while we're looking
        // up its content
        const QByteArray content = m_submissionContext->downloadBufferContent(buffer);
        m_sendBufferCaptureJob->addRequest(QPair<Qt3DCore::QNodeId, QByteArray>(bufferId, content));
    }
}

// Happens in RenderThread context when all RenderViewJobs are done
// Returns the id of the last bound FBO
Renderer::ViewSubmissionResultData
Renderer::submitRenderViews(const std::vector<RHIPassInfo> &rhiPassesInfo)
{
    QElapsedTimer timer;
    quint64 queueElapsed = 0;
    timer.start();

    quint64 frameElapsed = queueElapsed;
    m_lastFrameCorrect.storeRelaxed(1); // everything fine until now.....

    qCDebug(Memory) << Q_FUNC_INFO << "rendering frame ";

    // We might not want to render on the default FBO
    QSurface *surface = nullptr;
    QSurface *previousSurface = nullptr;
    QSurface *lastUsedSurface = nullptr;

    const size_t rhiPassesCount = rhiPassesInfo.size();

    for (size_t i = 0; i < rhiPassesCount; ++i) {
        // Initialize GraphicsContext for drawing
        const RHIPassInfo &rhiPassInfo = rhiPassesInfo.at(i);

        // Initialize Previous surface the first time we enter this loop
        if (i == 0) {
            for (const RenderView *rv : rhiPassInfo.rvs) {
                previousSurface = rv->surface();
                if (previousSurface)
                    break;
            }
        }

        // Check if using the same surface as the previous RHIPassInfo.
        // If not, we have to free up the context from the previous surface
        // and make the context current on the new surface
        surface = rhiPassInfo.surface;
        SurfaceLocker surfaceLock(surface);

        // TO DO: Make sure that the surface we are rendering too has not been unset

        // For now, if we do not have a surface, skip this rhipassinfo
        // TODO: Investigate if it's worth providing a fallback offscreen surface
        //       to use when surface is null. Or if we should instead expose an
        //       offscreensurface to Qt3D.
        if (!surface || !surfaceLock.isSurfaceValid()) {
            m_lastFrameCorrect.storeRelaxed(0);
            continue;
        }

        lastUsedSurface = surface;
        const bool surfaceHasChanged = surface != previousSurface;

        if (surfaceHasChanged && previousSurface) {
            // TO DO: Warn that this likely won't work with Scene3D

            // TODO what should be the swapBuffers condition for RHI ?
            // lastRenderTarget == swapChain->renderTarget or something like that ?
            const bool swapBuffers = surfaceLock.isSurfaceValid() && m_shouldSwapBuffers;
            // We only call swap buffer if we are sure the previous surface is still valid
            m_submissionContext->endDrawing(swapBuffers);
        }

        if (surfaceHasChanged) {
            // TO DO: Warn that this likely won't work with Scene3D

            // If we can't make the context current on the surface, skip to the
            // next RenderView. We won't get the full frame but we may get something
            if (!m_submissionContext->beginDrawing(surface)) {
                qCWarning(Backend) << "Failed to make RHI context current on surface";
                m_lastFrameCorrect.storeRelaxed(0);
                continue;
            }

            previousSurface = surface;
        }

        // Execute the render commands
        if (!executeCommandsSubmission(rhiPassInfo))
            m_lastFrameCorrect.storeRelaxed(
                    0); // something went wrong; make sure to render the next frame!

        frameElapsed = timer.elapsed() - frameElapsed;
        qCDebug(Rendering) << Q_FUNC_INFO << "Submitted RHI Passes " << i + 1 << "/"
                           << rhiPassesCount << "in " << frameElapsed << "ms";
        frameElapsed = timer.elapsed();
    }

    //* TODO: Shouldn't be needed with RHI ? as FBOs, etc.. are per-pipeline
    //* // Bind lastBoundFBOId back. Needed also in threaded mode.
    //* // lastBoundFBOId != m_graphicsContext->activeFBO() when the last FrameGraph leaf
    //* // node/renderView contains RenderTargetSelector/RenderTarget
    //* if (lastBoundFBOId != m_submissionContext->activeFBO()) {
    //*     RHI_UNIMPLEMENTED;
    //*     //         m_submissionContext->bindFramebuffer(lastBoundFBOId,
    //*     //         GraphicsHelperInterface::FBOReadAndDraw);
    //* }

    queueElapsed = timer.elapsed() - queueElapsed;
    qCDebug(Rendering) << Q_FUNC_INFO << "Submission Completed in " << timer.elapsed() << "ms";

    // Stores the necessary information to safely perform
    // the last swap buffer call
    ViewSubmissionResultData resultData;
    resultData.surface = lastUsedSurface;
    return resultData;
}

void Renderer::markDirty(BackendNodeDirtySet changes, BackendNode *node)
{
    Q_UNUSED(node);
    m_dirtyBits.marked |= changes;
}

Renderer::BackendNodeDirtySet Renderer::dirtyBits()
{
    return m_dirtyBits.marked;
}

#if defined(QT_BUILD_INTERNAL)
void Renderer::clearDirtyBits(BackendNodeDirtySet changes)
{
    m_dirtyBits.remaining &= ~changes;
    m_dirtyBits.marked &= ~changes;
}
#endif

bool Renderer::shouldRender() const
{
    // Only render if something changed during the last frame, or the last frame
    // was not rendered successfully (or render-on-demand is disabled)
    return (m_settings->renderPolicy() == QRenderSettings::Always || m_dirtyBits.marked != 0
            || m_dirtyBits.remaining != 0 || !m_lastFrameCorrect.loadRelaxed());
}

void Renderer::skipNextFrame()
{
    Q_ASSERT(m_settings->renderPolicy() != QRenderSettings::Always);

    // make submitRenderViews() actually run
    m_renderQueue.setNoRender();
    m_submitRenderViewsSemaphore.release(1);
}

void Renderer::jobsDone(Qt3DCore::QAspectManager *manager)
{
    // called in main thread once all jobs are done running

    // sync captured renders to frontend
    QMutexLocker lock(&m_pendingRenderCaptureSendRequestsMutex);
    const std::vector<Qt3DCore::QNodeId> pendingCaptureIds =
            Qt3DCore::moveAndClear(m_pendingRenderCaptureSendRequests);
    lock.unlock();
    for (const Qt3DCore::QNodeId &id : std::as_const(pendingCaptureIds)) {
        auto *backend = static_cast<Qt3DRender::Render::RenderCapture *>(
                m_nodesManager->frameGraphManager()->lookupNode(id));
        backend->syncRenderCapturesToFrontend(manager);
    }

    // Do we need to notify any texture about property changes?
    if (!m_updatedTextureProperties.empty())
        sendTextureChangesToFrontend(manager);

    sendDisablesToFrontend(manager);
}

bool Renderer::processMouseEvent(QObject *object, QMouseEvent *event)
{
    Q_UNUSED(object);
    Q_UNUSED(event);
    return false;
}

bool Renderer::processKeyEvent(QObject *object, QKeyEvent *event)
{
    Q_UNUSED(object);
    Q_UNUSED(event);
    return false;
}

// Jobs we may have to run even if no rendering will happen
std::vector<Qt3DCore::QAspectJobPtr> Renderer::preRenderingJobs()
{
    if (m_sendBufferCaptureJob->hasRequests())
        return { m_sendBufferCaptureJob };
    else
        return {};
}

// Waits to be told to create jobs for the next frame
// Called by QRenderAspect jobsToExecute context of QAspectThread
// Returns all the jobs (and with proper dependency chain) required
// for the rendering of the scene
std::vector<Qt3DCore::QAspectJobPtr> Renderer::renderBinJobs()
{
    std::vector<Qt3DCore::QAspectJobPtr> renderBinJobs;

    // Remove previous dependencies
    m_cleanupJob->removeDependency(QWeakPointer<Qt3DCore::QAspectJob>());

    const bool dirtyParametersForCurrentFrame = m_dirtyBits.marked & AbstractRenderer::ParameterDirty;
    const BackendNodeDirtySet dirtyBitsForFrame = m_dirtyBits.marked | m_dirtyBits.remaining;
    m_dirtyBits.marked = {};
    m_dirtyBits.remaining = {};
    BackendNodeDirtySet notCleared = {};

    // Add jobs
    if (dirtyBitsForFrame & AbstractRenderer::TransformDirty) {
        renderBinJobs.push_back(m_updateShaderDataTransformJob);
    }

    // TO DO: Conditionally add if skeletons dirty
    renderBinJobs.push_back(m_cleanupJob);

    // Jobs to prepare RHI Resource upload
    if (dirtyBitsForFrame & AbstractRenderer::BuffersDirty)
        renderBinJobs.push_back(m_bufferGathererJob);

    if (dirtyBitsForFrame & AbstractRenderer::TexturesDirty)
        renderBinJobs.push_back(m_textureGathererJob);

    // Layer cache is dependent on layers, layer filters (hence FG structure
    // changes) and the enabled flag on entities
    const bool entitiesEnabledDirty = dirtyBitsForFrame & AbstractRenderer::EntityEnabledDirty;
    const bool frameGraphDirty = dirtyBitsForFrame & AbstractRenderer::FrameGraphDirty;
    const bool layersDirty = dirtyBitsForFrame & AbstractRenderer::LayersDirty;
    const bool layersCacheNeedsToBeRebuilt = layersDirty || entitiesEnabledDirty || frameGraphDirty;
    const bool shadersDirty = dirtyBitsForFrame & AbstractRenderer::ShadersDirty;
    const bool materialDirty = dirtyBitsForFrame & AbstractRenderer::MaterialDirty;
    const bool lightsDirty = dirtyBitsForFrame & AbstractRenderer::LightsDirty;
    const bool computeableDirty = dirtyBitsForFrame & AbstractRenderer::ComputeDirty;
    const bool renderableDirty = dirtyBitsForFrame & AbstractRenderer::GeometryDirty;
    const bool materialCacheNeedsToBeRebuilt = shadersDirty || materialDirty || frameGraphDirty;
    const bool renderCommandsDirty =
            materialCacheNeedsToBeRebuilt || renderableDirty || computeableDirty;

    // Rebuild Entity Layers list if layers are dirty

    if (renderableDirty)
        renderBinJobs.push_back(m_renderableEntityFilterJob);

    if (computeableDirty)
        renderBinJobs.push_back(m_computableEntityFilterJob);

    if (lightsDirty)
        renderBinJobs.push_back(m_lightGathererJob);

    QMutexLocker lock(m_renderQueue.mutex());
    if (m_renderQueue.wasReset()) { // Have we rendered yet? (Scene3D case)
        // Traverse the current framegraph. For each leaf node create a
        // RenderView and set its configuration then create a job to
        // populate the RenderView with a set of RenderCommands that get
        // their details from the RenderNodes that are visible to the
        // Camera selected by the framegraph configuration
        if (frameGraphDirty) {
            FrameGraphVisitor visitor(m_nodesManager->frameGraphManager());
            m_frameGraphLeaves = visitor.traverse(frameGraphRoot());
            // Remove leaf nodes that no longer exist from cache
            const QList<FrameGraphNode *> keys = m_cache.leafNodeCache.keys();
            for (FrameGraphNode *leafNode : keys) {
                if (std::find(m_frameGraphLeaves.begin(),
                              m_frameGraphLeaves.end(),
                              leafNode) == m_frameGraphLeaves.end())
                    m_cache.leafNodeCache.remove(leafNode);
            }

            // Handle single shot subtree enablers
            const auto subtreeEnablers = visitor.takeEnablersToDisable();
            for (auto *node : subtreeEnablers)
                m_updatedDisableSubtreeEnablers.push_back(node->peerId());
        }

        int idealThreadCount = Qt3DCore::QAspectJobManager::idealThreadCount();

        const size_t fgBranchCount = m_frameGraphLeaves.size();
        if (fgBranchCount > 1) {
            int workBranches = int(fgBranchCount);
            for (auto leaf: m_frameGraphLeaves)
                if (leaf->nodeType() == FrameGraphNode::NoDraw)
                    --workBranches;

            if (idealThreadCount > 4 && workBranches)
                idealThreadCount = qMax(4, idealThreadCount / workBranches);
        }

        for (size_t i = 0; i < fgBranchCount; ++i) {
            FrameGraphNode *leaf = m_frameGraphLeaves.at(i);
            RenderViewBuilder builder(leaf, int(i), this);
            builder.setOptimalJobCount(leaf->nodeType() == FrameGraphNode::NoDraw ? 1 : idealThreadCount);

            // If we have a new RV (wasn't in the cache before, then it contains no cached data)
            const bool isNewRV = !m_cache.leafNodeCache.contains(leaf);
            builder.setLayerCacheNeedsToBeRebuilt(layersCacheNeedsToBeRebuilt || isNewRV);
            builder.setMaterialGathererCacheNeedsToBeRebuilt(materialCacheNeedsToBeRebuilt
                                                             || isNewRV);
            builder.setRenderCommandCacheNeedsToBeRebuilt(renderCommandsDirty || isNewRV);
            builder.setLightCacheNeedsToBeRebuilt(lightsDirty);

            // Insert leaf into cache
            if (isNewRV) {
                m_cache.leafNodeCache[leaf] = {};
            }

            builder.prepareJobs();
            Qt3DCore::moveAtEnd(renderBinJobs, builder.buildJobHierachy());
        }

        // Set target number of RenderViews
        m_renderQueue.setTargetRenderViewCount(int(fgBranchCount));
    } else {
        // FilterLayerEntityJob is part of the RenderViewBuilder jobs and must be run later
        // if none of those jobs are started this frame
        notCleared |= AbstractRenderer::EntityEnabledDirty;
        notCleared |= AbstractRenderer::FrameGraphDirty;
        notCleared |= AbstractRenderer::LayersDirty;
    }

    if (isRunning() && m_submissionContext->isInitialized()) {
        if (dirtyBitsForFrame & AbstractRenderer::TechniquesDirty)
            renderBinJobs.push_back(m_filterCompatibleTechniqueJob);
        if (dirtyBitsForFrame & AbstractRenderer::ShadersDirty)
            renderBinJobs.push_back(m_introspectShaderJob);
    } else {
        notCleared |= AbstractRenderer::TechniquesDirty;
        notCleared |= AbstractRenderer::ShadersDirty;
    }

    m_dirtyBits.remaining = dirtyBitsForFrame & notCleared;

    // Dirty Parameters might need 2 frames to react if the parameter references a texture
    if (dirtyParametersForCurrentFrame)
        m_dirtyBits.remaining |= AbstractRenderer::ParameterDirty;

    return renderBinJobs;
}

Qt3DCore::QAbstractFrameAdvanceService *Renderer::frameAdvanceService() const
{
    return static_cast<Qt3DCore::QAbstractFrameAdvanceService *>(m_vsyncFrameAdvanceService.data());
}

bool Renderer::performCompute(QRhiCommandBuffer *cb, RenderCommand &command)
{
    RHIComputePipeline *pipeline = command.pipeline.compute();
    if (!pipeline)
        return true;
    cb->setComputePipeline(pipeline->pipeline());

    if (!setBindingAndShaderResourcesForCommand(cb, command, pipeline->uboSet()))
        return false;

    const std::vector<QRhiCommandBuffer::DynamicOffset> offsets = pipeline->uboSet()->offsets(command);
    cb->setShaderResources(command.shaderResourceBindings,
                           int(offsets.size()),
                           offsets.data());

    cb->dispatch(command.m_workGroups[0], command.m_workGroups[1], command.m_workGroups[2]);
    m_dirtyBits.marked |= AbstractRenderer::ComputeDirty;
    return true;
}

static auto rhiIndexFormat(Qt3DCore::QAttribute::VertexBaseType type)
{
    switch (type) {
    case Qt3DCore::QAttribute::VertexBaseType ::UnsignedShort:
        return QRhiCommandBuffer::IndexUInt16;
    case Qt3DCore::QAttribute::VertexBaseType ::UnsignedInt:
        return QRhiCommandBuffer::IndexUInt32;
    default:
        std::abort();
    }
}

bool Renderer::uploadBuffersForCommand(QRhiCommandBuffer *cb, const RenderView *rv,
                                       RenderCommand &command)
{
    Q_UNUSED(cb);
    Q_UNUSED(rv);

    struct
    {
      Renderer &self;
      RenderCommand &command;
       bool operator()(RHIGraphicsPipeline* pipeline) const noexcept {
           if (!pipeline)
               return true;

           return self.uploadBuffersForCommand(pipeline, command);
       }
       bool operator()(RHIComputePipeline* pipeline) const noexcept {
           if (!pipeline)
               return true;

           return self.uploadBuffersForCommand(pipeline, command);
       }
       bool operator()(std::monostate) {
           return false;
       }
    } vis{*this, command};

    if (!command.pipeline.visit(vis))
        return false;

    for (const BlockToUBO &pack : command.m_parameterPack.uniformBuffers()) {
        Buffer *cpuBuffer = nodeManagers()->bufferManager()->lookupResource(pack.m_bufferID);
        RHIBuffer *ubo = m_submissionContext->rhiBufferForRenderBuffer(cpuBuffer);
        if (!ubo->bind(&*m_submissionContext, RHIBuffer::UniformBuffer))
            return false;
    }
    for (const BlockToSSBO &pack : command.m_parameterPack.shaderStorageBuffers()) {
        Buffer *cpuBuffer = nodeManagers()->bufferManager()->lookupResource(pack.m_bufferID);
        RHIBuffer *ubo = m_submissionContext->rhiBufferForRenderBuffer(cpuBuffer);
        if (!ubo->bind(&*m_submissionContext, RHIBuffer::ShaderStorageBuffer))
            return false;
    }

    return true;
}

bool Renderer::uploadBuffersForCommand(RHIGraphicsPipeline* graphicsPipeline, RenderCommand &command)
{
    // Create the vertex input description

    // Note: we have to bind the buffers here -> which will trigger the actual
    // upload, as this is the only place where we know about the usage type of the buffers

    const auto geom = command.m_geometry;
    const auto &attributes = geom->attributes();
    const QRhiVertexInputLayout layout = graphicsPipeline->pipeline()->vertexInputLayout();
    const int bindingAttributeCount = std::distance(layout.cbeginBindings(), layout.cendBindings());
    command.vertex_input.resize(bindingAttributeCount);

    for (Qt3DCore::QNodeId attribute_id : attributes) {
        // TODO isn't there a more efficient way than doing three hash lookups ?
        Attribute *attrib = m_nodesManager->attributeManager()->lookupResource(attribute_id);
        Buffer *buffer = m_nodesManager->bufferManager()->lookupResource(attrib->bufferId());
        RHIBuffer *hbuf = m_RHIResourceManagers->rhiBufferManager()->lookupResource(buffer->peerId());
        switch (attrib->attributeType()) {
        case Qt3DCore::QAttribute::VertexAttribute: {
            if (!hbuf->bind(&*m_submissionContext, RHIBuffer::Type((int)RHIBuffer::Type::ArrayBuffer | (int)RHIBuffer::Type::ShaderStorageBuffer)))
                return false;
            assert(hbuf->rhiBuffer());
            // Find Binding for Attribute
            const int bindingIndex = graphicsPipeline->bindingIndexForAttribute(attrib->nameId());
            // We need to reference a binding, a buffer and an offset which is always = 0
            // as Qt3D only assumes interleaved or continuous data but not
            // buffer where first half of it is attribute1 and second half attribute2
            if (bindingIndex != -1)
                command.vertex_input[bindingIndex] = { hbuf->rhiBuffer(), 0 };
            break;
        }
        case Qt3DCore::QAttribute::IndexAttribute: {
            if (!hbuf->bind(&*m_submissionContext, RHIBuffer::Type::IndexBuffer))
                return false;
            assert(hbuf->rhiBuffer());

            command.indexBuffer = hbuf->rhiBuffer();
            command.indexAttribute = attrib;
            break;
        }
        case Qt3DCore::QAttribute::DrawIndirectAttribute:
            RHI_UNIMPLEMENTED;
            break;
        }
    }

    return true;
}

bool Renderer::uploadBuffersForCommand(RHIComputePipeline* computePipeline, RenderCommand &command)
{
    Q_UNUSED(computePipeline);
    Q_UNUSED(command);
    return true;
}

bool Renderer::performDraw(QRhiCommandBuffer *cb, const QRhiViewport &vp,
                           const QRhiScissor *scissor, RenderCommand &command)
{
    RHIGraphicsPipeline *pipeline = command.pipeline.graphics();
    if (!pipeline || !pipeline->isComplete())
        return true;

    // Setup the rendering pass
    cb->setGraphicsPipeline(pipeline->pipeline());
    cb->setViewport(vp);
    if (scissor)
        cb->setScissor(*scissor);

    if (!setBindingAndShaderResourcesForCommand(cb, command, pipeline->uboSet()))
        return false;

    // Send the draw command
    if (Q_UNLIKELY(!command.indexBuffer)) {
        cb->setVertexInput(0, command.vertex_input.size(), command.vertex_input.data());
        cb->draw(command.m_primitiveCount, command.m_instanceCount, command.m_firstVertex,
                 command.m_firstInstance);
    } else {
        auto indexFormat = rhiIndexFormat(command.indexAttribute->vertexBaseType());
        auto indexOffset = command.indexAttribute->byteOffset();
        cb->setVertexInput(0, command.vertex_input.size(), command.vertex_input.data(),
                           command.indexBuffer, indexOffset, indexFormat);
        cb->drawIndexed(command.m_primitiveCount, command.m_instanceCount, command.m_indexOffset,
                        command.m_indexAttributeByteOffset, command.m_firstInstance);
    }
    return true;
}

bool Renderer::setBindingAndShaderResourcesForCommand(QRhiCommandBuffer *cb,
                                                      RenderCommand &command,
                                                      PipelineUBOSet *uboSet)
{
    // We need to create new resource bindings for each RC as each RC might potentially
    // have different textures or reference custom UBOs (if using Parameters with UBOs directly).
    // TO DO: We could propably check for texture and use the UBO set default ShaderResourceBindings
    // if we only have UBOs with offsets
    bool needsRecreate = false;
    if (command.shaderResourceBindings == nullptr) {
        command.shaderResourceBindings = m_submissionContext->rhi()->newShaderResourceBindings();
        needsRecreate = true;
    }

    // TO DO: Improve this to only perform when required
    const std::vector<QRhiShaderResourceBinding> &resourcesBindings = uboSet->resourceBindings(command);
    if (command.resourcesBindings != resourcesBindings) {
        command.resourcesBindings = std::move(resourcesBindings);
        command.shaderResourceBindings->setBindings(command.resourcesBindings.cbegin(), command.resourcesBindings.cend());
        needsRecreate = true;
    }

    if (needsRecreate && !command.shaderResourceBindings->create()) {
        qCWarning(Backend) << "Failed to create ShaderResourceBindings";
        return false;
    }
    const std::vector<QRhiCommandBuffer::DynamicOffset> offsets = uboSet->offsets(command);

    cb->setShaderResources(command.shaderResourceBindings,
                           int(offsets.size()),
                           offsets.data());
    return true;
}

// Called by RenderView->submit() in RenderThread context
// Returns true, if all RenderCommands were sent to the GPU
bool Renderer::executeCommandsSubmission(const RHIPassInfo &passInfo)
{
    bool allCommandsIssued = true;

    const std::vector<RenderView *> &renderViews = passInfo.rvs;
    QColor clearColor;
    QRhiDepthStencilClearValue clearDepthStencil;

    // Submit the commands to the underlying graphics API (RHI)
    QRhiCommandBuffer *cb = m_submissionContext->currentFrameCommandBuffer();

    // Upload data for all RenderCommands
    for (RenderView *rv : renderViews) {
        // Render drawing commands

        // Upload UBOs for pipelines used in current RV
        const std::vector<RHIGraphicsPipeline *> &rvGraphicsPipelines = m_rvToGraphicsPipelines[rv];
        for (RHIGraphicsPipeline *pipeline : rvGraphicsPipelines)
            pipeline->uboSet()->uploadUBOs(m_submissionContext.data(), rv);

        const std::vector<RHIComputePipeline *> &rvComputePipelines = m_rvToComputePipelines[rv];
        for (RHIComputePipeline *pipeline : rvComputePipelines)
            pipeline->uboSet()->uploadUBOs(m_submissionContext.data(), rv);

        // Upload Buffers for Commands
        rv->forEachCommand([&] (RenderCommand &command) {
            if (Q_UNLIKELY(!command.isValid()))
                return;

            if (!uploadBuffersForCommand(cb, rv, command)) {
                // Something went wrong trying to upload buffers
                // -> likely that frontend buffer has no initial data
                qCWarning(Backend) << "Failed to upload buffers";
                // Prevents further processing which could be catastrophic
                command.m_isValid = false;
            }
        });

        // Record clear information
        if (rv->clearTypes() != QClearBuffers::None) {
            clearColor = [=] {
                auto col = rv->globalClearColorBufferInfo().clearColor;
                return QColor::fromRgbF(col.x(), col.y(), col.z(), col.w());
            }();
            clearDepthStencil = { rv->clearDepthValue(), (quint32)rv->clearStencilValue() };
        }
    }

    // Lookup the render target
    QRhiRenderTarget *rhiRenderTarget{};
    {
        const auto &managers = *m_RHIResourceManagers;
        auto &renderTargetManager = *managers.rhiRenderTargetManager();
        auto *renderTarget = renderTargetManager.lookupResource(passInfo.renderTargetId);

        if (renderTarget) {
            // Is our RenderTarget targeting offscreen attachments?
            if (renderTarget->backBuffer == RHIRenderTarget::BackBuffer::None)
                rhiRenderTarget = renderTarget->renderTarget;
            else // Or one of the back buffers?
                rhiRenderTarget = m_submissionContext->currentSwapChain()->currentFrameRenderTarget(renderTarget->backBuffer == RHIRenderTarget::BackBuffer::Left
                                                                                                            ? QRhiSwapChain::LeftBuffer
                                                                                                            : QRhiSwapChain::RightBuffer);
        } else if (m_submissionContext->defaultRenderTarget()) {
            rhiRenderTarget = m_submissionContext->defaultRenderTarget();
        } else {
            rhiRenderTarget = m_submissionContext->currentSwapChain()->currentFrameRenderTarget();
        }
    }

    auto executeDrawRenderView = [&] (RenderView* rv) {
        // Viewport
        QRhiViewport vp;
        QRhiScissor scissor;
        bool hasScissor = false;
        {
            const QSize surfaceSize = rhiRenderTarget->pixelSize();

            const float x = rv->viewport().x() * surfaceSize.width();
            const float y = (1. - rv->viewport().y() - rv->viewport().height()) * surfaceSize.height();
            const float w = rv->viewport().width() * surfaceSize.width();
            const float h = rv->viewport().height() * surfaceSize.height();
            // qDebug() << surfaceSize << x << y << w << h;
            vp = { x, y, w, h };
        }
        // Scissoring
        {
            RenderStateSet *ss = rv->stateSet();
            if (ss == nullptr)
                ss = m_defaultRenderStateSet;
            StateVariant *scissorTestSVariant =
                    m_submissionContext->getState(ss, StateMask::ScissorStateMask);
            if (scissorTestSVariant) {
                const ScissorTest *scissorTest =
                        static_cast<const ScissorTest *>(scissorTestSVariant->constState());
                const auto &scissorValues = scissorTest->values();
                scissor = { std::get<0>(scissorValues), std::get<1>(scissorValues),
                            std::get<2>(scissorValues), std::get<3>(scissorValues) };
                hasScissor = true;
            }
        }

        // Render drawing commands
        rv->forEachCommand([&] (RenderCommand &command) {
            if (Q_UNLIKELY(!command.isValid()))
                return;

            Q_ASSERT (command.m_type == RenderCommand::Draw);
            performDraw(cb, vp, hasScissor ? &scissor : nullptr, command);
        });
    };

    auto executeComputeRenderView = [&] (RenderView* rv) {
        rv->forEachCommand([&] (RenderCommand &command) {
            if (Q_UNLIKELY(!command.isValid()))
                return;

            Q_ASSERT (command.m_type == RenderCommand::Compute);
            performCompute(cb, command);
        });
    };

    bool inCompute = false;
    bool inDraw = false;

    // All the RVs in the current passinfo target the same RenderTarget
    // A single beginPass should take place, unless Computes RVs are intermingled
    static const bool supportsCompute = m_submissionContext->rhi()->isFeatureSupported(QRhi::Compute);

    // Per Pass Global States
    for (RenderView *rv : renderViews) {
        if (rv->isCompute()) {
            // If we were running draw calls we stop the draw pass
            if (inDraw) {
                cb->endPass(m_submissionContext->m_currentUpdates);
                m_submissionContext->m_currentUpdates = m_submissionContext->rhi()->nextResourceUpdateBatch();
                inDraw = false;
            }

            // There is also the possibility where we weren't either in a compute or draw pass (for the first RV)
            // hence why these conditions are like this
            if (supportsCompute) {
                if (!inCompute) {
                    cb->beginComputePass(m_submissionContext->m_currentUpdates);
                    m_submissionContext->m_currentUpdates = m_submissionContext->rhi()->nextResourceUpdateBatch();
                    inCompute = true;
                }
                executeComputeRenderView(rv);
            } else {
                qWarning() << "RHI backend doesn't support Compute";
            }
        } else {
            // Same logic than above but reversed
            if (inCompute) {
                cb->endComputePass(m_submissionContext->m_currentUpdates);
                m_submissionContext->m_currentUpdates = m_submissionContext->rhi()->nextResourceUpdateBatch();
                inCompute = false;
            }

            if (!inDraw) {
                if (rhiRenderTarget == nullptr) {
                    qWarning(Backend) << "Invalid Render Target for pass, skipping";
                    inDraw = false;
                    continue;
                }
                cb->beginPass(rhiRenderTarget, clearColor, clearDepthStencil, m_submissionContext->m_currentUpdates);
                m_submissionContext->m_currentUpdates = m_submissionContext->rhi()->nextResourceUpdateBatch();
                inDraw = true;
            }

            executeDrawRenderView(rv);

            const Qt3DCore::QNodeId renderCaptureId = rv->renderCaptureNodeId();
            if (!renderCaptureId.isNull()) {
                const QRenderCaptureRequest request = rv->renderCaptureRequest();
                QRhiRenderTarget *rhiTarget = m_submissionContext->defaultRenderTarget();
                RHIRenderTarget *target = nullptr;

                if (rv->renderTargetId()) {
                    RHIRenderTargetManager *targetManager = m_RHIResourceManagers->rhiRenderTargetManager();
                    target = targetManager->lookupResource(rv->renderTargetId());
                    if (target != nullptr)
                        rhiTarget = target->renderTarget;
                }
                // Use FBO size if RV has one
                const QSize size = rhiTarget ? rhiTarget->pixelSize() : rv->surfaceSize();

                QRect rect(QPoint(0, 0), size);
                if (!request.rect.isEmpty())
                    rect = rect.intersected(request.rect);
                if (!rect.isEmpty()) {
                    // Bind fbo as read framebuffer
                    QRhiReadbackResult *readBackResult = new QRhiReadbackResult;
                    readBackResult->completed = [this, readBackResult, renderCaptureId, request] () {
                        const QImage::Format fmt = QImage::Format_RGBA8888_Premultiplied; // fits QRhiTexture::RGBA8
                        const uchar *p = reinterpret_cast<const uchar *>(readBackResult->data.constData());
                        const QImage image(p, readBackResult->pixelSize.width(), readBackResult->pixelSize.height(), fmt, [] (void *ptr) {
                            delete static_cast<QRhiReadbackResult *>(ptr);
                        }, readBackResult);

                        Render::RenderCapture *renderCapture = static_cast<Render::RenderCapture*>(m_nodesManager->frameGraphManager()->lookupNode(renderCaptureId));
                        renderCapture->addRenderCapture(request.captureId, image);
                        QMutexLocker lock(&m_pendingRenderCaptureSendRequestsMutex);
                        if (!Qt3DCore::contains(m_pendingRenderCaptureSendRequests, renderCaptureId))
                            m_pendingRenderCaptureSendRequests.push_back(renderCaptureId);
                    };

                    QRhiReadbackDescription readbackDesc;
                    if (rhiTarget) {
                        // First texture should be Attachment 0
                        QRhiTextureRenderTarget *textureRenderTarget = static_cast<QRhiTextureRenderTarget *>(rhiTarget);
                        const QRhiTextureRenderTargetDescription &desc = textureRenderTarget->description();
                        const QRhiColorAttachment *color0Att = desc.colorAttachmentAt(0);
                        readbackDesc.setTexture(color0Att->texture());
                    }
                    m_submissionContext->m_currentUpdates->readBackTexture(readbackDesc, readBackResult);
                } else {
                    qCWarning(Backend) << "Requested capture rectangle is outside framebuffer";
                }
            }
        }

        if (rv->isDownloadBuffersEnable())
            downloadRHIBuffers();
    }

    if (Q_LIKELY(inDraw))
        cb->endPass(m_submissionContext->m_currentUpdates);
    else if (inCompute)
        cb->endComputePass(m_submissionContext->m_currentUpdates);

    m_submissionContext->m_currentUpdates = m_submissionContext->rhi()->nextResourceUpdateBatch();

    return allCommandsIssued;
}

namespace {

template<typename Manager>
void removeUnusedPipelines(Manager *manager)
{
    const auto &pipelinesHandles = manager->activeHandles();
    std::vector<typename Manager::Handle> pipelinesToCleanup;
    // Store pipelines to cleanup in a temporary vector so that we can use a
    // ref on the activeHandles vector. Calling releaseResources modifies the
    // activeHandles vector which we want to avoid while iterating over it.
    for (const auto &pipelineHandle : pipelinesHandles) {
        auto *pipeline = pipelineHandle.data();
        Q_ASSERT(pipeline);
        pipeline->decreaseScore();
        // Pipeline wasn't used recently, let's destroy it
        if (pipeline->score() < 0) {
            pipelinesToCleanup.push_back(pipelineHandle);
        }
    }

    // Release Pipelines marked for cleanup
    for (const auto &pipelineHandle : pipelinesToCleanup) {
        manager->releaseResource(pipelineHandle->key());
    }
}

} // anonymous

// Erase graphics related resources that may become unused after a frame
void Renderer::cleanGraphicsResources()
{
    // Remove unused Pipeline
    RHIGraphicsPipelineManager *graphicsPipelineManager = m_RHIResourceManagers->rhiGraphicsPipelineManager();
    RHIComputePipelineManager *computePipelineManager = m_RHIResourceManagers->rhiComputePipelineManager();

    removeUnusedPipelines(graphicsPipelineManager);
    removeUnusedPipelines(computePipelineManager);

    // Clean buffers
    const QList<Qt3DCore::QNodeId> buffersToRelease =
            m_nodesManager->bufferManager()->takeBuffersToRelease();
    for (Qt3DCore::QNodeId bufferId : buffersToRelease)
        m_submissionContext->releaseBuffer(bufferId);

    const RHIBufferManager *bufferManager = m_RHIResourceManagers->rhiBufferManager();
    const std::vector<HRHIBuffer> &activeBufferHandles = bufferManager->activeHandles();
    // Release internal QRhiBuffer that might have been orphaned
    for (const HRHIBuffer &bufferH : activeBufferHandles)
        bufferH->destroyOrphaned();

    // When Textures are cleaned up, their id is saved so that they can be
    // cleaned up in the render thread
    const QList<Qt3DCore::QNodeId> cleanedUpTextureIds = Qt3DCore::moveAndClear(m_textureIdsToCleanup);
    for (const Qt3DCore::QNodeId &textureCleanedUpId : cleanedUpTextureIds)
        cleanupTexture(textureCleanedUpId);

    // Abandon GL shaders when a Shader node is destroyed Note: We are sure
    // that when this gets executed, all scene changes have been received and
    // shader nodes updated
    const QList<Qt3DCore::QNodeId> cleanedUpShaderIds =
            m_nodesManager->shaderManager()->takeShaderIdsToCleanup();
    for (const Qt3DCore::QNodeId &shaderCleanedUpId : cleanedUpShaderIds) {
        cleanupShader(m_nodesManager->shaderManager()->lookupResource(shaderCleanedUpId));
        // We can really release the texture at this point
        m_nodesManager->shaderManager()->releaseResource(shaderCleanedUpId);

        // Release pipelines that were referencing shader
        graphicsPipelineManager->releasePipelinesReferencingShader(shaderCleanedUpId);
        computePipelineManager->releasePipelinesReferencingShader(shaderCleanedUpId);
    }

    const QList<Qt3DCore::QNodeId> cleanedUpRenderTargetIds =
            m_nodesManager->renderTargetManager()->takeRenderTargetIdsToCleanup();
    for (const Qt3DCore::QNodeId &renderTargetCleanedUpId : cleanedUpRenderTargetIds) {
        cleanupRenderTarget(renderTargetCleanedUpId);
        m_nodesManager->renderTargetManager()->releaseResource(renderTargetCleanedUpId);
        // Release pipelines that were referencing renderTarget
        graphicsPipelineManager->releasePipelinesReferencingRenderTarget(renderTargetCleanedUpId);
    }
}

const GraphicsApiFilterData *Renderer::contextInfo() const
{
    return m_submissionContext->contextInfo();
}

SubmissionContext *Renderer::submissionContext() const
{
    return m_submissionContext.data();
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
