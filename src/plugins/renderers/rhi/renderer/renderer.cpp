/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "renderer_p.h"

#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qeffect.h>

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
#include <Qt3DRender/private/renderthread_p.h>
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
#include <Qt3DRender/private/loadbufferjob_p.h>
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
#include <renderqueue_p.h>
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

QT_BEGIN_NAMESPACE

// Crashes on AMD Radeon drivers on Windows. Disable for now.
//#define SHADER_LOADING_IN_COMMAND_THREAD
using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {
namespace Rhi {

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

        QVector<Entity *> selectedEntities = filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        QMutexLocker lock(m_cache->mutex());
        m_cache->renderableEntities = selectedEntities;
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

        QVector<Entity *> selectedEntities = filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        QMutexLocker lock(m_cache->mutex());
        m_cache->computeEntities = selectedEntities;
    }

private:
    RendererCache *m_cache;
};

int locationForAttribute(Attribute *attr, RHIShader *shader) noexcept
{
    const QVector<ShaderAttribute> attribInfo = shader->attributes();
    const auto it = std::find_if(
            attribInfo.begin(), attribInfo.end(),
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

Renderer::Renderer(QRenderAspect::RenderType type)
    : m_services(nullptr),
      m_aspect(nullptr),
      m_nodesManager(nullptr),
      m_renderSceneRoot(nullptr),
      m_defaultRenderStateSet(nullptr),
      m_submissionContext(nullptr),
      m_renderQueue(new RenderQueue()),
      m_renderThread(type == QRenderAspect::Threaded ? new RenderThread(this) : nullptr),
      m_vsyncFrameAdvanceService(new VSyncFrameAdvanceService(m_renderThread != nullptr)),
      m_waitForInitializationToBeCompleted(0),
      m_hasBeenInitializedMutex(),
      m_exposed(0),
      m_lastFrameCorrect(0),
      m_glContext(nullptr),
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
      m_offscreenHelper(nullptr),
      m_RHIResourceManagers(nullptr),
      m_commandExecuter(new Qt3DRender::Debug::CommandExecuter(this)),
      m_shouldSwapBuffers(true)
{
    std::fill_n(m_textureTransform, 4, 0.f);

    // Set renderer as running - it will wait in the context of the
    // RenderThread for RenderViews to be submitted
    m_running.fetchAndStoreOrdered(1);
    if (m_renderThread)
        m_renderThread->waitForStart();

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
    if (m_renderThread)
        Q_ASSERT(m_renderThread->isFinished());

    delete m_renderQueue;
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
    return API::OpenGL;
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

void Renderer::setServices(QServiceLocator *services)
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

// Executed in the reloadDirtyShader job
void Renderer::loadShader(Shader *shader, HShader shaderHandle)
{
    Q_UNUSED(shader);
    if (!m_dirtyShaders.contains(shaderHandle))
        m_dirtyShaders.push_back(shaderHandle);
}

void Renderer::setOpenGLContext(QOpenGLContext *context)
{
    m_glContext = context;
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
    QMutexLocker lockRenderQueue(m_renderQueue->mutex());
    qDeleteAll(m_renderQueue->nextFrameQueue());
    m_renderQueue->reset();
    lockRenderQueue.unlock();

    if (!m_renderThread) {
        releaseGraphicsResources();
    } else {
        // Wake up the render thread in case it is waiting for some renderviews
        // to be ready. The isReadyToSubmit() function checks for a shutdown
        // having been requested.
        m_submitRenderViewsSemaphore.release(1);
        m_renderThread->wait();
    }

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

    // Try to temporarily make the context current so we can free up any resources
    QMutexLocker locker(&m_offscreenSurfaceMutex);
    QOffscreenSurface *offscreenSurface = m_offscreenHelper->offscreenSurface();
    if (!offscreenSurface) {
        qWarning() << "Failed to make context current: OpenGL resources will not be destroyed";
        // We still need to delete the submission context
        m_submissionContext.reset(nullptr);
        return;
    }

    //* QOpenGLContext *context = m_submissionContext->openGLContext();
    //* Q_ASSERT(context);
    //*
    //* if (context->thread() == QThread::currentThread() && context->makeCurrent(offscreenSurface))
    //{
    //*
    //*     // Clean up the graphics context and any resources
    //*     const QVector<HRHITexture> activeTexturesHandles =
    //m_RHIResourceManagers->rhiTextureManager()->activeHandles();
    //*     for (const HRHITexture &textureHandle : activeTexturesHandles) {
    //*         RHITexture *tex = m_RHIResourceManagers->rhiTextureManager()->data(textureHandle);
    //*         tex->destroy();
    //*     }
    //*
    //*     // Do the same thing with buffers
    //*     const QVector<HRHIBuffer> activeBuffers =
    //m_RHIResourceManagers->rhiBufferManager()->activeHandles();
    //*     for (const HRHIBuffer &bufferHandle : activeBuffers) {
    //*         RHIBuffer *buffer = m_RHIResourceManagers->rhiBufferManager()->data(bufferHandle);
    //*         buffer->destroy(m_submissionContext.data());
    //*     }
    //*
    //*     // Do the same thing with shaders
    //*     const QVector<RHIShader *> shaders =
    //m_RHIResourceManagers->rhiShaderManager()->takeActiveResources();
    //*     qDeleteAll(shaders);
    //*
    //*
    //*     context->doneCurrent();
    //* } else {
    //*     qWarning() << "Failed to make context current: OpenGL resources will not be destroyed";
    //* }
    //*
    //* if (m_ownedContext)
    //*     delete context;

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

void Renderer::render()
{
    // Traversing the framegraph tree from root to lead node
    // Allows us to define the rendering set up
    // Camera, RenderTarget ...

    // Utimately the renderer should be a framework
    // For the processing of the list of renderviews

    // Matrice update, bounding volumes computation ...
    // Should be jobs

    // namespace Qt3DCore has 2 distincts node trees
    // One scene description
    // One framegraph description

    while (m_running.loadRelaxed() > 0) {
        doRender();
        // TO DO: Restore windows exposed detection
        // Probably needs to happens some place else though
    }
}

// Either called by render if Qt3D is in charge of the RenderThread
// or by QRenderAspectPrivate::renderSynchronous (for Scene3D)
void Renderer::doRender(bool swapBuffers)
{
    Renderer::ViewSubmissionResultData submissionData;
    bool hasCleanedQueueAndProceeded = false;
    bool preprocessingComplete = false;
    bool beganDrawing = false;

    // Blocking until RenderQueue is full
    const bool canSubmit = isReadyToSubmit();
    m_shouldSwapBuffers = swapBuffers;

    // Lock the mutex to protect access to the renderQueue while we look for its state
    QMutexLocker locker(m_renderQueue->mutex());
    const bool queueIsComplete = m_renderQueue->isFrameQueueComplete();
    const bool queueIsEmpty = m_renderQueue->targetRenderViewCount() == 0;

    bool mustCleanResources = false;

    // When using synchronous rendering (QtQuick)
    // We are not sure that the frame queue is actually complete
    // Since a call to render may not be synched with the completions
    // of the RenderViewJobs
    // In such a case we return early, waiting for a next call with
    // the frame queue complete at this point

    // RenderQueue is complete (but that means it may be of size 0)
    if (canSubmit && (queueIsComplete && !queueIsEmpty)) {
        const QVector<Render::Rhi::RenderView *> renderViews = m_renderQueue->nextFrameQueue();
        QTaskLogger submissionStatsPart1(m_services->systemInformation(),
                                         { JobTypes::FrameSubmissionPart1, 0 },
                                         QTaskLogger::Submission);
        QTaskLogger submissionStatsPart2(m_services->systemInformation(),
                                         { JobTypes::FrameSubmissionPart2, 0 },
                                         QTaskLogger::Submission);

        QVector<RHIPassInfo> rhiPassesInfo;

        if (canRender()) {
            QSurface *surface = nullptr;
            for (const RenderView *rv : renderViews) {
                surface = rv->surface();
                if (surface)
                    break;
            }

            // In case we did not draw because e.g. there wase no swapchain,
            // we keep the resource updates from the previous frame.
            if (!m_submissionContext->m_currentUpdates) {
                m_submissionContext->m_currentUpdates =
                        m_submissionContext->rhi()->nextResourceUpdateBatch();
            }

            // 1) Execute commands for buffer uploads, texture updates, shader loading first
            updateResources();

            rhiPassesInfo = prepareCommandsSubmission(renderViews);
            // 2) Update Pipelines and copy data into commands to allow concurrent submission
            preprocessingComplete = true;

            bool hasCommands = false;
            for (const RenderView *rv : renderViews) {
                const auto &commands = rv->commands();
                hasCommands = std::any_of(commands.begin(), commands.end(),
                                          [](const RenderCommand &cmd) { return cmd.isValid(); });
                if (hasCommands)
                    break;
            }

            if (hasCommands) {
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
            // 2) Proceed to next frame and start preparing frame n + 1
            m_renderQueue->reset();
            locker.unlock(); // Done protecting RenderQueue
            m_vsyncFrameAdvanceService->proceedToNextFrame();
            hasCleanedQueueAndProceeded = true;

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
        }

        // Execute the pending shell commands
        m_commandExecuter->performAsynchronousCommandExecution(renderViews);

        // Delete all the RenderViews which will clear the allocators
        // that were used for their allocation
        qDeleteAll(renderViews);
    }

    // If hasCleanedQueueAndProceeded isn't true this implies that something went wrong
    // with the rendering and/or the renderqueue is incomplete from some reason
    // or alternatively it could be complete but empty (RenderQueue of size 0)

    if (!hasCleanedQueueAndProceeded) {
        // RenderQueue was full but something bad happened when
        // trying to render it and therefore proceedToNextFrame was not called
        // Note: in this case the renderQueue mutex is still locked

        // Reset the m_renderQueue so that we won't try to render
        // with a queue used by a previous frame with corrupted content
        // if the current queue was correctly submitted
        m_renderQueue->reset();

        // We allow the RenderTickClock service to proceed to the next frame
        // In turn this will allow the aspect manager to request a new set of jobs
        // to be performed for each aspect
        m_vsyncFrameAdvanceService->proceedToNextFrame();
    }

    // Perform the last swapBuffers calls after the proceedToNextFrame
    // as this allows us to gain a bit of time for the preparation of the
    // next frame
    // Finish up with last surface used in the list of RenderViews
    if (beganDrawing) {
        SurfaceLocker surfaceLock(submissionData.surface);
        // Finish up with last surface used in the list of RenderViews
        const bool swapBuffers = submissionData.lastBoundFBOId == m_submissionContext->defaultFBO()
                && surfaceLock.isSurfaceValid() && m_shouldSwapBuffers;
        m_submissionContext->endDrawing(swapBuffers);

        if (mustCleanResources)
            cleanGraphicsResources();
    }
}

// Called by RenderViewJobs
// When the frameQueue is complete and we are using a renderThread
// we allow the render thread to proceed
void Renderer::enqueueRenderView(RenderView *renderView, int submitOrder)
{
    QMutexLocker locker(m_renderQueue->mutex()); // Prevent out of order execution
    // We cannot use a lock free primitive here because:
    // - QVector is not thread safe
    // - Even if the insert is made correctly, the isFrameComplete call
    //   could be invalid since depending on the order of execution
    //   the counter could be complete but the renderview not yet added to the
    //   buffer depending on whichever order the cpu decides to process this
    const bool isQueueComplete = m_renderQueue->queueRenderView(renderView, submitOrder);
    locker.unlock(); // We're done protecting the queue at this point
    if (isQueueComplete) {
        if (m_renderThread && m_running.loadRelaxed())
            Q_ASSERT(m_submitRenderViewsSemaphore.available() == 0);
        m_submitRenderViewsSemaphore.release(1);
    }
}

bool Renderer::canRender() const

{
    // Make sure that we've not been told to terminate
    if (m_renderThread && !m_running.loadRelaxed()) {
        qCDebug(Rendering) << "RenderThread termination requested whilst waiting";
        return false;
    }

    // TO DO: Check if all surfaces have been destroyed...
    // It may be better if the last window to be closed trigger a call to shutdown
    // Rather than having checks for the surface everywhere

    return true;
}

bool Renderer::isReadyToSubmit()
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
    Q_ASSERT(m_renderQueue->isFrameQueueComplete());
    return true;
}

// Main thread
QVariant Renderer::executeCommand(const QStringList &args)
{
    return m_commandExecuter->executeCommand(args);
}

/*!
    \internal
    Called in the context of the aspect thread from QRenderAspect::onRegistered
*/
void Renderer::setOffscreenSurfaceHelper(OffscreenSurfaceHelper *helper)
{
    QMutexLocker locker(&m_offscreenSurfaceMutex);
    m_offscreenHelper = helper;
}

QSurfaceFormat Renderer::format()
{
    return m_submissionContext->format();
}

void Renderer::updateGraphicsPipeline(RenderCommand &cmd, RenderView *rv, int renderViewIndex)
{
    if (!cmd.m_rhiShader) {
        qDebug() << "Warning: command has no shader";
        return;
    }

    // The Graphics Pipeline defines
    // - Render State (Depth, Culling, Stencil, Blending)
    // - Shader Resources Binding
    // - Shader Vertex Attribute Layout

    // This means we need to have one GraphicsPipeline per
    // - geometry
    // - material
    // - state (RV + RC)

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

    // Try to retrieve existing pipeline
    auto &pipelineManager = *m_RHIResourceManagers->rhiGraphicsPipelineManager();
    const GraphicsPipelineIdentifier pipelineKey { cmd.m_geometry, cmd.m_shaderId,
                                                   renderViewIndex };
    RHIGraphicsPipeline *graphicsPipeline = pipelineManager.getOrCreateResource(pipelineKey);
    // TO DO: Ensure we find a way to know when the state is dirty to trigger a rebuild

    if (!graphicsPipeline) {
        qDebug() << "Warning : could not create a graphics pipeline";
        return;
    }

    // Increase score so that we know the pipeline was used for this frame and shouldn't be
    // destroyed
    graphicsPipeline->increaseScore();

    // TO DO: Set to true if geometry, shader or render state dirty
    bool requiredRebuild = false;

    // Note: we can rebuild add/remove things from the QRhiShaderResourceBindings after having
    // created the pipeline and rebuild it. Changes should be picked up automatically

    // Create pipeline if it doesn't exist or needs to be updated
    if (graphicsPipeline->pipeline() == nullptr || requiredRebuild) {
        bool ok = true;

        const SubmissionContext::SwapChainInfo *swapchain =
                m_submissionContext->swapChainForSurface(rv->surface());
        if (!swapchain || !swapchain->swapChain || !swapchain->renderPassDescriptor)
            return;

        // TO DO: Find a way to recycle those
        // Create Per Command UBO
        QRhiBuffer *commandUBO = m_submissionContext->rhi()->newBuffer(
                QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(CommandUBO));
        QRhiBuffer *rvUBO = m_submissionContext->rhi()->newBuffer(
                QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(RenderViewUBO));
        commandUBO->build();
        rvUBO->build();
        graphicsPipeline->setCommandUBO(commandUBO);
        graphicsPipeline->setRenderViewUBO(rvUBO);

        QVector<QRhiShaderResourceBinding> uboBindings;
        uboBindings << QRhiShaderResourceBinding::uniformBuffer(
                0,
                QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage,
                rvUBO)
                    << QRhiShaderResourceBinding::uniformBuffer(
                               1,
                               QRhiShaderResourceBinding::VertexStage
                                       | QRhiShaderResourceBinding::FragmentStage,
                               commandUBO);

        // Create additional empty UBO Buffer for UBO with binding point > 1 (since we assume 0 and
        // 1 and for Qt3D standard values)
        const QVector<ShaderUniformBlock> uniformBlocks = cmd.m_rhiShader->uniformBlocks();
        QHash<int, RHIGraphicsPipeline::UBOBuffer> uboBuffers;
        for (const ShaderUniformBlock &block : uniformBlocks) {
            if (block.m_binding > 1) {
                auto handle = m_RHIResourceManagers->rhiBufferManager()->allocateResource();
                RHIBuffer *ubo = m_RHIResourceManagers->rhiBufferManager()->data(handle);
                Q_ASSERT(ubo);
                const QByteArray rawData(block.m_size, '\0');
                ubo->allocate(m_submissionContext.data(), rawData, true);
                ok = ubo->bind(m_submissionContext.data(), RHIBuffer::UniformBuffer);
                uboBuffers[block.m_binding] = { handle, ubo };
                uboBindings << QRhiShaderResourceBinding::uniformBuffer(
                        block.m_binding,
                        QRhiShaderResourceBinding::VertexStage
                                | QRhiShaderResourceBinding::FragmentStage,
                        ubo->rhiBuffer());
            }
        }
        graphicsPipeline->setUBOs(uboBuffers);

        // Samplers
        for (const auto &textureParameter : cmd.m_parameterPack.textures()) {
            const auto handle = m_RHIResourceManagers->rhiTextureManager()->getOrAcquireHandle(
                    textureParameter.nodeId);
            const auto textureData = m_RHIResourceManagers->rhiTextureManager()->data(handle);

            for (const ShaderAttribute &samplerAttribute : cmd.m_rhiShader->samplers()) {
                if (samplerAttribute.m_nameId == textureParameter.glslNameId) {
                    const auto rhiTexture = textureData->getRhiTexture();
                    const auto rhiSampler = textureData->getRhiSampler();
                    if (rhiTexture && rhiSampler) {
                        uboBindings.push_back(QRhiShaderResourceBinding::sampledTexture(
                                samplerAttribute.m_location,
                                QRhiShaderResourceBinding::FragmentStage, rhiTexture, rhiSampler));
                    }
                }
            }
        }

        QRhiShaderResourceBindings *shaderResourceBindings =
                m_submissionContext->rhi()->newShaderResourceBindings();
        assert(shaderResourceBindings);

        shaderResourceBindings->setBindings(uboBindings.cbegin(), uboBindings.cend());
        ok = shaderResourceBindings->build();
        assert(ok);

        // Create pipeline
        QRhiGraphicsPipeline *pipeline = m_submissionContext->rhi()->newGraphicsPipeline();
        graphicsPipeline->setShaderResourceBindings(shaderResourceBindings);
        graphicsPipeline->setPipeline(pipeline);
        assert(pipeline);

        const QShader vertexShader = cmd.m_rhiShader->shaderStage(QShader::VertexStage);
        const QShader fragmentShader = cmd.m_rhiShader->shaderStage(QShader::FragmentStage);

        assert(vertexShader.isValid());
        assert(fragmentShader.isValid());

        pipeline->setShaderStages({ { QRhiShaderStage::Vertex, vertexShader },
                                    { QRhiShaderStage::Fragment, fragmentShader } });

        QVarLengthArray<QRhiVertexInputBinding, 8> inputBindings;
        QVarLengthArray<QRhiVertexInputAttribute, 8> rhiAttributes;

        const auto geom = cmd.m_geometry;
        const auto &attributes = geom->attributes();

        struct BufferBinding
        {
            Qt3DCore::QNodeId bufferId;
            uint stride;
            QRhiVertexInputBinding::Classification classification;
            uint attributeDivisor;
        };
        QVector<BufferBinding> uniqueBindings;

        auto rhiAttributeType = [](Attribute *attr) {
            switch (attr->vertexBaseType()) {
            case QAttribute::Byte:
            case QAttribute::UnsignedByte: {
                if (attr->vertexSize() == 1)
                    return QRhiVertexInputAttribute::UNormByte;
                if (attr->vertexSize() == 2)
                    return QRhiVertexInputAttribute::UNormByte2;
                if (attr->vertexSize() == 4)
                    return QRhiVertexInputAttribute::UNormByte4;
                Q_FALLTHROUGH();
            }
            case QAttribute::Float: {
                if (attr->vertexSize() == 1)
                    return QRhiVertexInputAttribute::Float;
                if (attr->vertexSize() == 2)
                    return QRhiVertexInputAttribute::Float2;
                if (attr->vertexSize() == 3)
                    return QRhiVertexInputAttribute::Float3;
                if (attr->vertexSize() == 4)
                    return QRhiVertexInputAttribute::Float4;
                Q_FALLTHROUGH();
            }
            default:
                qWarning() << "Attribute type not handles by RHI";
                Q_UNREACHABLE();
            }
        };

        // QRhiVertexInputBinding -> specifies the stride of an attribute, whether it's per vertex
        // or per instance and the instance divisor QRhiVertexInputAttribute -> specifies the format
        // of the attribute (offset, type), the shader location and the index of the binding
        // QRhiCommandBuffer::VertexInput -> binds a buffer to a binding

        QHash<int, int> attributeNameToBinding;

        for (Qt3DCore::QNodeId attribute_id : attributes) {
            Attribute *attrib = m_nodesManager->attributeManager()->lookupResource(attribute_id);
            if (attrib->attributeType() == QAttribute::VertexAttribute) {
                const bool isPerInstanceAttr = attrib->divisor() != 0;
                const QRhiVertexInputBinding::Classification classification = isPerInstanceAttr
                        ? QRhiVertexInputBinding::PerInstance
                        : QRhiVertexInputBinding::PerVertex;
                const BufferBinding binding { attrib->bufferId(), attrib->byteStride(),
                                              classification,
                                              isPerInstanceAttr ? attrib->divisor() : 1U };

                const auto it =
                        std::find_if(uniqueBindings.begin(), uniqueBindings.end(),
                                     [binding](const BufferBinding &a) {
                                         return binding.bufferId == a.bufferId
                                                 && binding.stride == a.stride
                                                 && binding.classification == a.classification
                                                 && binding.attributeDivisor == a.attributeDivisor;
                                     });

                int bindingIndex = uniqueBindings.size();
                if (it == uniqueBindings.end())
                    uniqueBindings.push_back(binding);
                else
                    bindingIndex = std::distance(uniqueBindings.begin(), it);

                rhiAttributes.push_back({ bindingIndex,
                                          locationForAttribute(attrib, cmd.m_rhiShader),
                                          rhiAttributeType(attrib), attrib->byteOffset() });

                attributeNameToBinding.insert(attrib->nameId(), bindingIndex);
            }
        }

        inputBindings.resize(uniqueBindings.size());
        for (int i = 0, m = uniqueBindings.size(); i < m; ++i) {
            const BufferBinding binding = uniqueBindings.at(i);
            /*
            qDebug() << binding.bufferId
                     << binding.stride
                     << binding.classification
                     << binding.attributeDivisor;
            */
            inputBindings[i] = QRhiVertexInputBinding { binding.stride, binding.classification,
                                                        int(binding.attributeDivisor) };
        }

        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings(inputBindings.begin(), inputBindings.end());
        inputLayout.setAttributes(rhiAttributes.begin(), rhiAttributes.end());

        pipeline->setVertexInputLayout(inputLayout);
        pipeline->setShaderResourceBindings(shaderResourceBindings);

        pipeline->setRenderPassDescriptor(swapchain->renderPassDescriptor);

        graphicsPipeline->setAttributesToBindingHash(attributeNameToBinding);

        // Render States
        m_submissionContext->applyStateSet(renderState, pipeline);

        ok = pipeline->build();
        assert(ok);
    }

    // Record RHIGraphicsPipeline into command for later use
    if (graphicsPipeline && graphicsPipeline->pipeline())
        cmd.pipeline = graphicsPipeline;
}

// When this function is called, we must not be processing the commands for frame n+1
QVector<Renderer::RHIPassInfo>
Renderer::prepareCommandsSubmission(const QVector<RenderView *> &renderViews)
{
    // TO DO: Find a central place to initialize RHI resources
    const int renderViewCount = renderViews.size();

    // We need to have a single RHI RenderPass per RenderTarget
    // as creating the pass clears the buffers
    // 1) We need to find all adjacents RenderViews that have the same renderTarget
    // and submit all of these as part of the same RHI pass
    QVector<RHIPassInfo> rhiPassesInfo;

    for (int i = 0; i < renderViewCount;) {
        QVector<RenderView *> sameRenderTargetRVs;
        QVector<QRhiBuffer *> rvUbos;
        RenderView *refRV = renderViews.at(i);
        sameRenderTargetRVs.push_back(refRV);

        for (i = i + 1; i < renderViewCount; ++i) {
            RenderView *curRV = renderViews.at(i);
            if (refRV->renderTargetId() == curRV->renderTargetId()) {
                sameRenderTargetRVs.push_back(curRV);
            } else
                break;
        }

        RHIPassInfo bucket;
        bucket.rvs = std::move(sameRenderTargetRVs);
        bucket.surface = refRV->surface();
        bucket.renderTargetId = refRV->renderTargetId();
        bucket.attachmentPack = refRV->attachmentPack();
        rhiPassesInfo.push_back(bucket);
    }

    for (int i = 0; i < renderViewCount; ++i) {
        RenderView *rv = renderViews.at(i);
        QVector<RenderCommand> &commands = rv->commands();
        for (RenderCommand &command : commands) {
            // Update/Create GraphicsPipelines
            if (command.m_type == RenderCommand::Draw) {
                Geometry *rGeometry =
                        m_nodesManager->data<Geometry, GeometryManager>(command.m_geometry);
                GeometryRenderer *rGeometryRenderer =
                        m_nodesManager->data<GeometryRenderer, GeometryRendererManager>(
                                command.m_geometryRenderer);
                // By this time shaders should have been loaded
                RHIShader *shader = m_RHIResourceManagers->rhiShaderManager()->lookupResource(
                        command.m_shaderId);
                if (!shader) {
                    qDebug() << "Warning: could not find shader";
                    continue;
                }

                // We should never have inserted a command for which these are null
                // in the first place
                Q_ASSERT(rGeometry && rGeometryRenderer && shader);

                // Unset dirtiness on rGeometryRenderer only
                // The rGeometry may be shared by several rGeometryRenderer
                // so we cannot unset its dirtiness at this point
                if (rGeometryRenderer->isDirty())
                    rGeometryRenderer->unsetDirty();

                // Prepare the ShaderParameterPack based on the active uniforms of the shader
                //  shader->prepareUniforms(command.m_parameterPack);

                updateGraphicsPipeline(command, rv, i);

            } else if (command.m_type == RenderCommand::Compute) {
                RHI_UNIMPLEMENTED;
                // By this time shaders have been loaded
                RHIShader *shader = m_RHIResourceManagers->rhiShaderManager()->lookupResource(
                        command.m_shaderId);
                command.m_rhiShader = shader;
                Q_ASSERT(shader);

                // Prepare the ShaderParameterPack based on the active uniforms of the shader
                // shader->prepareUniforms(command.m_parameterPack);
            }
        }
    }

    // Unset dirtiness on Geometry and Attributes
    // Note: we cannot do it in the loop above as we want to be sure that all
    // the VAO which reference the geometry/attributes are properly updated
    RHI_UNIMPLEMENTED;
    for (Attribute *attribute : qAsConst(m_dirtyAttributes))
        attribute->unsetDirty();
    m_dirtyAttributes.clear();

    for (Geometry *geometry : qAsConst(m_dirtyGeometry))
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
        if (buffer->access() & QBuffer::Read)
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
        const QNodeIdVector imageIds = texture->textureImageIds();

        // Does the texture reference any of the dirty texture images?
        for (const QNodeId imageId : imageIds) {
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
            for (const QNodeId &passId : passIds) {
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
                            m_shaderBuilderUpdates.append(shaderBuilder->takePendingUpdates());
                        }

                        const auto code = shaderBuilder->shaderCode(shaderType);
                        shader->setShaderCode(shaderType, code);
                    }
                }

                if (shader != nullptr && shader->isDirty())
                    loadShader(shader, shaderHandle);
            }
        }
    }
}

// Executed in job (in main thread when jobs are done)
void Renderer::sendShaderChangesToFrontend(Qt3DCore::QAspectManager *manager)
{
    Q_ASSERT(isRunning());

    // Sync Shader
    const std::vector<HShader> &activeShaders = m_nodesManager->shaderManager()->activeHandles();
    for (const HShader &handle : activeShaders) {
        Shader *s = m_nodesManager->shaderManager()->data(handle);
        if (s->requiresFrontendSync()) {
            QShaderProgram *frontend =
                    static_cast<decltype(frontend)>(manager->lookupNode(s->peerId()));
            QShaderProgramPrivate *dFrontend =
                    static_cast<decltype(dFrontend)>(QNodePrivate::get(frontend));
            s->unsetRequiresFrontendSync();
            dFrontend->setStatus(s->status());
            dFrontend->setLog(s->log());
        }
    }

    // Sync ShaderBuilder
    const QVector<ShaderBuilderUpdate> shaderBuilderUpdates = std::move(m_shaderBuilderUpdates);
    for (const ShaderBuilderUpdate &update : shaderBuilderUpdates) {
        QShaderProgramBuilder *builder =
                static_cast<decltype(builder)>(manager->lookupNode(update.builderId));
        QShaderProgramBuilderPrivate *dBuilder =
                static_cast<decltype(dBuilder)>(QNodePrivate::get(builder));
        dBuilder->setShaderCode(update.shaderCode, update.shaderType);
    }
}

// Executed in a job (in main thread when jobs are done)
void Renderer::sendTextureChangesToFrontend(Qt3DCore::QAspectManager *manager)
{
    const QVector<QPair<Texture::TextureUpdateInfo, Qt3DCore::QNodeIdVector>>
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
                    static_cast<QAbstractTexturePrivate *>(QNodePrivate::get(texture));

            dTexture->setStatus(properties.status);
            dTexture->setHandleType(pair.first.handleType);
            dTexture->setHandle(pair.first.handle);
        }
    }
}

// Executed in a job (in main thread when jobs done)
void Renderer::sendDisablesToFrontend(Qt3DCore::QAspectManager *manager)
{
    // SubtreeEnabled
    const auto updatedDisables = std::move(m_updatedDisableSubtreeEnablers);
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
        const QVector<HBuffer> dirtyBufferHandles = std::move(m_dirtyBuffers);
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

#ifndef SHADER_LOADING_IN_COMMAND_THREAD
    {
        const QVector<HShader> dirtyShaderHandles = std::move(m_dirtyShaders);
        ShaderManager *shaderManager = m_nodesManager->shaderManager();
        for (const HShader &handle : dirtyShaderHandles) {
            Shader *shader = shaderManager->data(handle);

            // Can be null when using Scene3D rendering
            if (shader == nullptr)
                continue;

            // Compile shader
            m_submissionContext->loadShader(shader, shaderManager,
                                            m_RHIResourceManagers->rhiShaderManager());
        }
    }
#endif

    {
        const QVector<HTexture> activeTextureHandles = std::move(m_dirtyTextures);
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
            const std::vector<HRHITexture> &glTextureHandles = rhiTextureManager->activeHandles();
            // Upload texture data
            for (const HRHITexture &glTextureHandle : glTextureHandles) {
                RHI_UNIMPLEMENTED;
                RHITexture *glTexture = rhiTextureManager->data(glTextureHandle);

                // We create/update the actual GL texture using the GL context at this point
                const RHITexture::TextureUpdateInfo info =
                        glTexture->createOrUpdateRhiTexture(m_submissionContext.data());

                // RHITexture creation provides us width/height/format ... information
                // for textures which had not initially specified these information
                // (TargetAutomatic...) Gather these information and store them to be distributed by
                // a change next frame
                const QNodeIdVector referenceTextureIds = {
                    rhiTextureManager->texNodeIdForRHITexture.value(glTexture)
                };
                // Store properties and referenceTextureIds
                if (info.wasUpdated) {
                    Texture::TextureUpdateInfo updateInfo;
                    updateInfo.properties = info.properties;
                    updateInfo.handleType = QAbstractTexture::OpenGLTextureId;
                    //                    updateInfo.handle = info.texture ?
                    //                    QVariant(info.texture->textureId()) : QVariant();
                    m_updatedTextureProperties.push_back({ updateInfo, referenceTextureIds });
                }
            }
        }

        // Record ids of texture to cleanup while we are still blocking the aspect thread
        m_textureIdsToCleanup += m_nodesManager->textureManager()->takeTexturesIdsToCleanup();
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
        qWarning() << Q_FUNC_INFO << "QTexture referencing invalid QTextureImages";
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
        const QNodeIdVector textureImageIds = texture->textureImageIds();
        QVector<RHITexture::Image> images;
        images.reserve(textureImageIds.size());
        // TODO: Move this into RHITexture directly
        for (const QNodeId textureImageId : textureImageIds) {
            const TextureImage *img =
                    m_nodesManager->textureImageManager()->lookupResource(textureImageId);
            if (img == nullptr) {
                qWarning() << Q_FUNC_INFO << "invalid TextureImage handle";
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

// Called by SubmitRenderView
void Renderer::downloadGLBuffers()
{
    const QVector<Qt3DCore::QNodeId> downloadableHandles = std::move(m_downloadableBuffers);
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
Renderer::submitRenderViews(const QVector<RHIPassInfo> &rhiPassesInfo)
{
    QElapsedTimer timer;
    quint64 queueElapsed = 0;
    timer.start();

    quint64 frameElapsed = queueElapsed;
    m_lastFrameCorrect.storeRelaxed(1); // everything fine until now.....

    qCDebug(Memory) << Q_FUNC_INFO << "rendering frame ";

    // We might not want to render on the default FBO
    uint lastBoundFBOId = 0; // m_submissionContext->boundFrameBufferObject();
    QSurface *surface = nullptr;
    QSurface *previousSurface = nullptr;
    QSurface *lastUsedSurface = nullptr;

    const int rhiPassesCount = rhiPassesInfo.size();

    for (int i = 0; i < rhiPassesCount; ++i) {
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
            const bool swapBuffers = lastBoundFBOId == m_submissionContext->defaultFBO()
                    && surfaceLock.isSurfaceValid() && m_shouldSwapBuffers;
            // We only call swap buffer if we are sure the previous surface is still valid
            m_submissionContext->endDrawing(swapBuffers);
        }

        if (surfaceHasChanged) {
            // If we can't make the context current on the surface, skip to the
            // next RenderView. We won't get the full frame but we may get something
            if (!m_submissionContext->beginDrawing(surface)) {
                qWarning() << "Failed to make OpenGL context current on surface";
                m_lastFrameCorrect.storeRelaxed(0);
                continue;
            }

            previousSurface = surface;
            //            lastBoundFBOId = m_submissionContext->boundFrameBufferObject();
        }

        // Apply Memory Barrier if needed
        //        if (renderView->memoryBarrier() != QMemoryBarrier::None)
        //            qWarning() << "RHI Doesn't support MemoryBarrier";

        // Set RenderTarget ...
        // Activate RenderTarget
        {
            m_submissionContext->activateRenderTarget(rhiPassInfo.renderTargetId,
                                                      rhiPassInfo.attachmentPack, lastBoundFBOId);
        }

        // Execute the render commands
        if (!executeCommandsSubmission(rhiPassInfo))
            m_lastFrameCorrect.storeRelaxed(
                    0); // something went wrong; make sure to render the next frame!

        // executeCommandsSubmission takes care of restoring the stateset to the value
        // of gc->currentContext() at the moment it was called (either
        // renderViewStateSet or m_defaultRenderStateSet)
        //        if (!renderView->renderCaptureNodeId().isNull()) {
        //            RHI_UNIMPLEMENTED;
        //*            const QRenderCaptureRequest request = renderView->renderCaptureRequest();
        //*            const QSize size =
        //m_submissionContext->renderTargetSize(renderView->surfaceSize());
        //*            QRect rect(QPoint(0, 0), size);
        //*            if (!request.rect.isEmpty())
        //*                rect = rect.intersected(request.rect);
        //*            QImage image;
        //*            if (!rect.isEmpty()) {
        //*                // Bind fbo as read framebuffer
        //*                m_submissionContext->bindFramebuffer(m_submissionContext->activeFBO(),
        //GraphicsHelperInterface::FBORead);
        //*                image = m_submissionContext->readFramebuffer(rect);
        //*            } else {
        //*                qWarning() << "Requested capture rectangle is outside framebuffer";
        //*            }
        //*            Render::RenderCapture *renderCapture =
        //*
        //static_cast<Render::RenderCapture*>(m_nodesManager->frameGraphManager()->lookupNode(renderView->renderCaptureNodeId()));
        //*            renderCapture->addRenderCapture(request.captureId, image);
        //*            if
        //(!m_pendingRenderCaptureSendRequests.contains(renderView->renderCaptureNodeId()))
        //* m_pendingRenderCaptureSendRequests.push_back(renderView->renderCaptureNodeId());
        //        }

        //        if (renderView->isDownloadBuffersEnable())
        //        {
        //            RHI_UNIMPLEMENTED;
        ////*            downloadGLBuffers();
        //        }

        //        // Perform BlitFramebuffer operations
        //        if (renderView->hasBlitFramebufferInfo()) {
        //            RHI_UNIMPLEMENTED;
        ////*            const auto &blitFramebufferInfo = renderView->blitFrameBufferInfo();
        ////*            const QNodeId inputTargetId = blitFramebufferInfo.sourceRenderTargetId;
        ////*            const QNodeId outputTargetId =
        ///blitFramebufferInfo.destinationRenderTargetId;
        ////*            const QRect inputRect = blitFramebufferInfo.sourceRect;
        ////*            const QRect outputRect = blitFramebufferInfo.destinationRect;
        ////*            const QRenderTargetOutput::AttachmentPoint inputAttachmentPoint =
        ///blitFramebufferInfo.sourceAttachmentPoint;
        ////*            const QRenderTargetOutput::AttachmentPoint outputAttachmentPoint =
        ///blitFramebufferInfo.destinationAttachmentPoint;
        ////*            const QBlitFramebuffer::InterpolationMethod interpolationMethod =
        ///blitFramebufferInfo.interpolationMethod;
        ////*            m_submissionContext->blitFramebuffer(inputTargetId, outputTargetId,
        ///inputRect, outputRect, lastBoundFBOId,
        ////*                                                 inputAttachmentPoint,
        ///outputAttachmentPoint,
        ////*                                                 interpolationMethod);
        //        }

        frameElapsed = timer.elapsed() - frameElapsed;
        qCDebug(Rendering) << Q_FUNC_INFO << "Submitted RHI Passes " << i + 1 << "/"
                           << rhiPassesCount << "in " << frameElapsed << "ms";
        frameElapsed = timer.elapsed();
    }

    // Bind lastBoundFBOId back. Needed also in threaded mode.
    // lastBoundFBOId != m_graphicsContext->activeFBO() when the last FrameGraph leaf
    // node/renderView contains RenderTargetSelector/RenderTarget
    if (lastBoundFBOId != m_submissionContext->activeFBO()) {
        RHI_UNIMPLEMENTED;
        //         m_submissionContext->bindFramebuffer(lastBoundFBOId,
        //         GraphicsHelperInterface::FBOReadAndDraw);
    }

    // Reset state and call doneCurrent if the surface
    // is valid and was actually activated
    if (lastUsedSurface) {
        RHI_UNIMPLEMENTED;
        // Reset state to the default state if the last stateset is not the
        // defaultRenderStateSet
        //        if (m_submissionContext->currentStateSet() != m_defaultRenderStateSet)
        //            m_submissionContext->setCurrentStateSet(m_defaultRenderStateSet);
    }

    queueElapsed = timer.elapsed() - queueElapsed;
    qCDebug(Rendering) << Q_FUNC_INFO << "Submission Completed in " << timer.elapsed() << "ms";

    // Stores the necessary information to safely perform
    // the last swap buffer call
    ViewSubmissionResultData resultData;
    resultData.lastBoundFBOId = lastBoundFBOId;
    resultData.surface = lastUsedSurface;
    return resultData;
}

void Renderer::markDirty(BackendNodeDirtySet changes, BackendNode *node)
{
    Q_UNUSED(node)
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
    m_renderQueue->setNoRender();
    m_submitRenderViewsSemaphore.release(1);
}

void Renderer::jobsDone(Qt3DCore::QAspectManager *manager)
{
    // called in main thread once all jobs are done running

    // sync captured renders to frontend
    const QVector<Qt3DCore::QNodeId> pendingCaptureIds =
            std::move(m_pendingRenderCaptureSendRequests);
    for (const Qt3DCore::QNodeId &id : qAsConst(pendingCaptureIds)) {
        auto *backend = static_cast<Qt3DRender::Render::RenderCapture *>(
                m_nodesManager->frameGraphManager()->lookupNode(id));
        backend->syncRenderCapturesToFrontend(manager);
    }

    // Do we need to notify any texture about property changes?
    if (m_updatedTextureProperties.size() > 0)
        sendTextureChangesToFrontend(manager);

    sendDisablesToFrontend(manager);
}

void Renderer::setPendingEvents(const QList<QPair<QObject *, QMouseEvent>> &mouseEvents,
                                const QList<QKeyEvent> &keyEvents)
{
    QMutexLocker l(&m_frameEventsMutex);
    m_frameMouseEvents = mouseEvents;
    m_frameKeyEvents = keyEvents;
}

// Jobs we may have to run even if no rendering will happen
QVector<QAspectJobPtr> Renderer::preRenderingJobs()
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
QVector<Qt3DCore::QAspectJobPtr> Renderer::renderBinJobs()
{
    QVector<QAspectJobPtr> renderBinJobs;

    // Remove previous dependencies
    m_cleanupJob->removeDependency(QWeakPointer<QAspectJob>());

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

    QMutexLocker lock(m_renderQueue->mutex());
    if (m_renderQueue->wasReset()) { // Have we rendered yet? (Scene3D case)
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
                if (!m_frameGraphLeaves.contains(leafNode))
                    m_cache.leafNodeCache.remove(leafNode);
            }

            // Handle single shot subtree enablers
            const auto subtreeEnablers = visitor.takeEnablersToDisable();
            for (auto *node : subtreeEnablers)
                m_updatedDisableSubtreeEnablers.push_back(node->peerId());
        }

        const int fgBranchCount = m_frameGraphLeaves.size();
        for (int i = 0; i < fgBranchCount; ++i) {
            FrameGraphNode *leaf = m_frameGraphLeaves.at(i);
            RenderViewBuilder builder(leaf, i, this);
            // If we have a new RV (wasn't in the cache before, then it contains no cached data)
            const bool isNewRV = !m_cache.leafNodeCache.contains(leaf);
            builder.setLayerCacheNeedsToBeRebuilt(layersCacheNeedsToBeRebuilt || isNewRV);
            builder.setMaterialGathererCacheNeedsToBeRebuilt(materialCacheNeedsToBeRebuilt
                                                             || isNewRV);
            builder.setRenderCommandCacheNeedsToBeRebuilt(renderCommandsDirty || isNewRV);

            builder.prepareJobs();
            renderBinJobs.append(builder.buildJobHierachy());
        }

        // Set target number of RenderViews
        m_renderQueue->setTargetRenderViewCount(fgBranchCount);
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

    return renderBinJobs;
}

QAbstractFrameAdvanceService *Renderer::frameAdvanceService() const
{
    return static_cast<Qt3DCore::QAbstractFrameAdvanceService *>(m_vsyncFrameAdvanceService.data());
}

// Called by executeCommands
void Renderer::performDraw(RenderCommand *command)
{
    QRhiCommandBuffer *cb = m_submissionContext->currentFrameCommandBuffer();
    // Indirect Draw Calls
    if (command->m_drawIndirect) {
        RHI_UNIMPLEMENTED;
    } else { // Direct Draw Calls

        // TO DO: Add glMulti Draw variants
        if (command->m_primitiveType == QGeometryRenderer::Patches) {
            RHI_UNIMPLEMENTED;
            //* m_submissionContext->setVerticesPerPatch(command->m_verticesPerPatch);
        }

        if (command->m_primitiveRestartEnabled) {
            RHI_UNIMPLEMENTED;
            //* m_submissionContext->enablePrimitiveRestart(command->m_restartIndexValue);
        }

        // TO DO: Add glMulti Draw variants
        if (command->m_drawIndexed) {
            cb->drawIndexed(command->m_primitiveCount, command->m_instanceCount,
                            command->m_indexOffset, command->m_indexAttributeByteOffset,
                            command->m_firstInstance);
        } else {
            cb->draw(command->m_primitiveCount, command->m_instanceCount, command->m_firstVertex,
                     command->m_firstInstance);
        }
    }

#if defined(QT3D_RENDER_ASPECT_RHI_DEBUG)
    int err = m_submissionContext->openGLContext()->functions()->glGetError();
    if (err)
        qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);
#endif

    //    if (command->m_primitiveRestartEnabled)
    //        m_submissionContext->disablePrimitiveRestart();
}

void Renderer::performCompute(const RenderView *, RenderCommand *command)
{
    RHI_UNIMPLEMENTED;
    //* {
    //*     RHIShader *shader =
    //m_RHIResourceManagers->rhiShaderManager()->lookupResource(command->m_shaderId);
    //*     m_submissionContext->activateShader(shader);
    //* }
    //* {
    //*     m_submissionContext->setParameters(command->m_parameterPack);
    //* }
    //* {
    //*     m_submissionContext->dispatchCompute(command->m_workGroups[0],
    //*             command->m_workGroups[1],
    //*             command->m_workGroups[2]);
    //* }
    //* // HACK: Reset the compute flag to dirty
    //* m_dirtyBits.marked |= AbstractRenderer::ComputeDirty;

    //* #if defined(QT3D_RENDER_ASPECT_RHI_DEBUG)
    //*     int err = m_submissionContext->openGLContext()->functions()->glGetError();
    //*     if (err)
    //*         qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);
    //* #endif
}

static auto rhiIndexFormat(QAttribute::VertexBaseType type)
{
    switch (type) {
    case QAttribute::VertexBaseType ::UnsignedShort:
        return QRhiCommandBuffer::IndexUInt16;
    case QAttribute::VertexBaseType ::UnsignedInt:
        return QRhiCommandBuffer::IndexUInt32;
    default:
        std::abort();
    }
}

bool Renderer::uploadBuffersForCommand(QRhiCommandBuffer *cb, const RenderView *rv,
                                       RenderCommand &command)
{
    RHIGraphicsPipeline *graphicsPipeline = command.pipeline;
    if (!graphicsPipeline)
        return true;

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
        RHIBuffer *hbuf =
                m_RHIResourceManagers->rhiBufferManager()->lookupResource(buffer->peerId());
        switch (attrib->attributeType()) {
        case QAttribute::VertexAttribute: {
            hbuf->bind(&*m_submissionContext, RHIBuffer::Type::ArrayBuffer);
            assert(hbuf->rhiBuffer());
            // Find Binding for Attribute
            const int bindingIndex = graphicsPipeline->bindingIndexForAttribute(attrib->nameId());
            // We need to reference a binding, a buffer and an offset which is always = 0
            // as Qt3D only assumes interleaved or continuous data but not
            // buffer where first half of it is attribute1 and second half attribute2
            command.vertex_input[bindingIndex] = { hbuf->rhiBuffer(), 0 };
            break;
        }
        case QAttribute::IndexAttribute: {
            hbuf->bind(&*m_submissionContext, RHIBuffer::Type::IndexBuffer);
            assert(hbuf->rhiBuffer());
            assert(command.indexBuffer == nullptr);

            command.indexBuffer = hbuf->rhiBuffer();
            command.indexAttribute = attrib;
            break;
        }
        case QAttribute::DrawIndirectAttribute:
            RHI_UNIMPLEMENTED;
            break;
        }
    }

    for (const BlockToUBO &pack : command.m_parameterPack.uniformBuffers()) {
        Buffer *cpuBuffer = nodeManagers()->bufferManager()->lookupResource(pack.m_bufferID);
        RHIBuffer *ubo = m_submissionContext->rhiBufferForRenderBuffer(cpuBuffer);
        ubo->bind(&*m_submissionContext, RHIBuffer::UniformBuffer);
    }

    return true;
}

namespace {
void printUpload(const UniformValue &value, const QShaderDescription::BlockVariable &member)
{
    switch (member.type) {
    case QShaderDescription::VariableType::Int:
        qDebug() << "Updating" << member.name << "with int data: " << *value.constData<int>()
                 << " (offset: " << member.offset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Float:
        qDebug() << "Updating" << member.name << "with float data: " << *value.constData<float>()
                 << " (offset: " << member.offset << ", size: " << member.size << ")";
        break;
    case QShaderDescription::VariableType::Vec2:
        qDebug() << "Updating" << member.name << "with vec2 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << " (offset: " << member.offset
                 << ", size: " << member.size << ")";
        ;
        break;
    case QShaderDescription::VariableType::Vec3:
        qDebug() << "Updating" << member.name << "with vec3 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << ", " << value.constData<float>()[2]
                 << " (offset: " << member.offset << ", size: " << member.size << ")";
        ;
        break;
    case QShaderDescription::VariableType::Vec4:
        qDebug() << "Updating" << member.name << "with vec4 data: " << value.constData<float>()[0]
                 << ", " << value.constData<float>()[1] << ", " << value.constData<float>()[2]
                 << ", " << value.constData<float>()[3] << " (offset: " << member.offset
                 << ", size: " << member.size << ")";
        ;
        break;
    default:
        qDebug() << "Updating" << member.name << "with data: " << value.constData<char>();
        break;
    }
}

void uploadUniform(SubmissionContext &submissionContext, const PackUniformHash &uniforms,
                   const RHIShader::UBO_Member &uboMember,
                   const QHash<int, RHIGraphicsPipeline::UBOBuffer> &uboBuffers,
                   const QString &uniformName, const QShaderDescription::BlockVariable &member,
                   int arrayOffset = 0)
{
    const int uniformNameId = StringToInt::lookupId(uniformName);

    if (!uniforms.contains(uniformNameId))
        return;

    const UniformValue value = uniforms.value(uniformNameId);
    const ShaderUniformBlock block = uboMember.block;

    // Update UBO with uniform value
    Q_ASSERT(uboBuffers.contains(block.m_binding));
    const RHIGraphicsPipeline::UBOBuffer &ubo = uboBuffers[block.m_binding];
    RHIBuffer *buffer = ubo.buffer;

    // TODO we should maybe have this thread_local to not reallocate memory every time
    QByteArray rawData;
    rawData.resize(member.size);
    memcpy(rawData.data(), value.constData<char>(), std::min(value.byteSize(), member.size));
    buffer->update(&submissionContext, rawData, member.offset + arrayOffset);

    // printUpload(value, member);
}
}

bool Renderer::uploadUBOsForCommand(QRhiCommandBuffer *cb, const RenderView *rv,
                                    const RenderCommand &command)
{
    RHIGraphicsPipeline *pipeline = command.pipeline;
    if (!pipeline)
        return true;

    // Upload UBO data for the Command
    QRhiBuffer *commandUBO = pipeline->commandUBO();
    m_submissionContext->m_currentUpdates->updateDynamicBuffer(commandUBO, 0, sizeof(CommandUBO),
                                                               &command.m_commandUBO);

    // We have to update the RV UBO once per graphics pipeline
    QRhiBuffer *rvUBO = pipeline->renderViewUBO();
    m_submissionContext->m_currentUpdates->updateDynamicBuffer(rvUBO, 0, sizeof(RenderViewUBO),
                                                               rv->renderViewUBO());

    // Upload UBO for custom parameters
    {
        RHIShader *shader =
                m_RHIResourceManagers->rhiShaderManager()->lookupResource(command.m_shaderId);
        if (!shader)
            return true;

        const QVector<RHIShader::UBO_Member> &uboMembers = shader->uboMembers();
        const QHash<int, RHIGraphicsPipeline::UBOBuffer> &uboBuffers = pipeline->ubos();
        const ShaderParameterPack &parameterPack = command.m_parameterPack;
        const PackUniformHash &uniforms = parameterPack.uniforms();

        // Update Buffer CPU side data based on uniforms being set
        for (const RHIShader::UBO_Member &uboMember : uboMembers) {
            for (const QShaderDescription::BlockVariable &member : qAsConst(uboMember.members)) {

                if (!member.arrayDims.empty()) {
                    if (!member.structMembers.empty()) {
                        const int arr0 = member.arrayDims[0];
                        for (int i = 0; i < arr0; i++) {
                            for (const QShaderDescription::BlockVariable &structMember :
                                 member.structMembers) {
                                const QString processedName = member.name + "[" + QString::number(i)
                                        + "]." + structMember.name;
                                uploadUniform(*m_submissionContext, uniforms, uboMember, uboBuffers,
                                              processedName, structMember, i * member.size / arr0);
                            }
                        }
                    } else {
                        uploadUniform(*m_submissionContext, uniforms, uboMember, uboBuffers,
                                      member.name, member);
                    }
                } else {
                    uploadUniform(*m_submissionContext, uniforms, uboMember, uboBuffers,
                                  member.name, member);
                }
            }
        }
        // Upload changes to GPU Buffer
        for (const RHIGraphicsPipeline::UBOBuffer &ubo : uboBuffers) {
            // Binding triggers the upload
            ubo.buffer->bind(m_submissionContext.data(), RHIBuffer::UniformBuffer);
        }
    }
    return true;
}

bool Renderer::performDraw(QRhiCommandBuffer *cb, const QRhiViewport &vp,
                           const QRhiScissor *scissor, const RenderCommand &command)
{
    RHIGraphicsPipeline *pipeline = command.pipeline;
    if (!pipeline)
        return true;

    // Setup the rendering pass
    cb->setGraphicsPipeline(pipeline->pipeline());
    cb->setViewport(vp);
    if (scissor)
        cb->setScissor(*scissor);
    cb->setShaderResources(pipeline->pipeline()->shaderResourceBindings());

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

// Called by RenderView->submit() in RenderThread context
// Returns true, if all RenderCommands were sent to the GPU
bool Renderer::executeCommandsSubmission(const RHIPassInfo &passInfo)
{
    bool allCommandsIssued = true;

    const QVector<RenderView *> &renderViews = passInfo.rvs;
    QColor clearColor;
    QRhiDepthStencilClearValue clearDepthStencil;

    // Submit the commands to the underlying graphics API (RHI)
    QRhiCommandBuffer *cb = m_submissionContext->currentFrameCommandBuffer();

    // Upload data for all RenderCommands
    for (RenderView *rv : renderViews) {
        // Render drawing commands

        QVector<RenderCommand> &commands = rv->commands();

        // Upload all the required data to rhi...
        for (RenderCommand &command : commands) {
            if (command.m_type == RenderCommand::Draw) {
                uploadBuffersForCommand(cb, rv, command);
                uploadUBOsForCommand(cb, rv, command);
            }
        }

        // Record clear information
        if (rv->clearTypes() != QClearBuffers::None) {
            clearColor = [=] {
                auto col = rv->globalClearColorBufferInfo().clearColor;
                return QColor::fromRgbF(col.x(), col.y(), col.z(), col.w());
            }();
            clearDepthStencil = { rv->clearDepthValue(), (quint32)rv->clearStencilValue() };
        }
    }
    // TO DO: should be moved elsewhere
    // Perform compute actions
    //        cb->beginComputePass(m_submissionContext->m_currentUpdates);
    //        for (RenderCommand &command : commands) {
    //            if (command.m_type == RenderCommand::Compute) {
    //                performCompute(rv, &command);
    //            }
    //        }
    //        cb->endComputePass();
    //    m_submissionContext->m_currentUpdates =
    //    m_submissionContext->rhi()->nextResourceUpdateBatch();

    // Draw the commands

    // TO DO: Retrieve real renderTarget for RHIPassInfo
    QRhiRenderTarget *renderTarget = m_submissionContext->currentFrameRenderTarget();

    // Begin pass
    cb->beginPass(renderTarget, clearColor, clearDepthStencil,
                  m_submissionContext->m_currentUpdates);

    // Per Pass Global States
    for (RenderView *rv : renderViews) {
        // Viewport
        QRhiViewport vp;
        QRhiScissor scissor;
        bool hasScissor = false;
        {
            const float x = rv->viewport().x() * rv->surfaceSize().width();
            const float y = (1. - rv->viewport().y() - rv->viewport().height())
                    * rv->surfaceSize().height();
            const float w = rv->viewport().width() * rv->surfaceSize().width();
            const float h = rv->viewport().height() * rv->surfaceSize().height();
            //            qDebug() << x << y << w << h;
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
        const QVector<RenderCommand> &commands = rv->commands();

        for (const RenderCommand &command : commands) {
            if (command.m_type == RenderCommand::Draw) {
                performDraw(cb, vp, hasScissor ? &scissor : nullptr, command);
            }
        }
    }

    cb->endPass();
    m_submissionContext->m_currentUpdates = m_submissionContext->rhi()->nextResourceUpdateBatch();

    return allCommandsIssued;
}

// Erase graphics related resources that may become unused after a frame
void Renderer::cleanGraphicsResources()
{
    // Remove unused GraphicsPipeline
    RHIGraphicsPipelineManager *pipelineManager =
            m_RHIResourceManagers->rhiGraphicsPipelineManager();
    const std::vector<HRHIGraphicsPipeline> &graphicsPipelinesHandles = pipelineManager->activeHandles();
    for (HRHIGraphicsPipeline pipelineHandle : graphicsPipelinesHandles) {
        RHIGraphicsPipeline *pipeline = pipelineManager->data(pipelineHandle);
        pipeline->decreaseScore();
        // Pipeline wasn't used recently, let's destroy it
        if (pipeline->score() < 0) {
            pipeline->cleanup();
        }
    }

    // Clean buffers
    const QVector<Qt3DCore::QNodeId> buffersToRelease =
            m_nodesManager->bufferManager()->takeBuffersToRelease();
    for (Qt3DCore::QNodeId bufferId : buffersToRelease)
        m_submissionContext->releaseBuffer(bufferId);

    // When Textures are cleaned up, their id is saved so that they can be
    // cleaned up in the render thread
    const QVector<Qt3DCore::QNodeId> cleanedUpTextureIds = std::move(m_textureIdsToCleanup);
    for (const Qt3DCore::QNodeId textureCleanedUpId : cleanedUpTextureIds)
        cleanupTexture(textureCleanedUpId);

    // Abandon GL shaders when a Shader node is destroyed Note: We are sure
    // that when this gets executed, all scene changes have been received and
    // shader nodes updated
    const QVector<Qt3DCore::QNodeId> cleanedUpShaderIds =
            m_nodesManager->shaderManager()->takeShaderIdsToCleanup();
    for (const Qt3DCore::QNodeId shaderCleanedUpId : cleanedUpShaderIds) {
        cleanupShader(m_nodesManager->shaderManager()->lookupResource(shaderCleanedUpId));
        // We can really release the texture at this point
        m_nodesManager->shaderManager()->releaseResource(shaderCleanedUpId);
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
