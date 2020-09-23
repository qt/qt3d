/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRender/private/qrenderaspect_p.h>
#ifndef Q_OS_INTEGRITY
#include <imguirenderer_p.h>
#endif

#include <Qt3DRender/qcameralens.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p_p.h>
#include <Qt3DRender/private/resourceaccessor_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/setfence_p.h>

#include <glbuffer_p.h>
#include <graphicscontext_p.h>
#include <rendercommand_p.h>
#include <renderqueue_p.h>
#include <renderview_p.h>
#include <gltexture_p.h>
#include <openglvertexarrayobject_p.h>
#include <renderviewbuilder_p.h>
#include <glresourcemanagers_p.h>
#include <commandexecuter_p.h>

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
#include "frameprofiler_p.h"

QT_BEGIN_NAMESPACE

// Crashes on AMD Radeon drivers on Windows. Disable for now.
//#define SHADER_LOADING_IN_COMMAND_THREAD

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

namespace {

class CachingLightGatherer : public LightGatherer {
public:
    CachingLightGatherer(RendererCache *cache)
        : LightGatherer()
        , m_cache(cache)
    {
    }

    void run() override
    {
        LightGatherer::run();

        m_cache->gatheredLights = lights();
        m_cache->environmentLight = environmentLight();
    }

private:
    RendererCache *m_cache;
};

class CachingRenderableEntityFilter : public RenderableEntityFilter {
public:
    CachingRenderableEntityFilter(RendererCache *cache)
        : RenderableEntityFilter()
        , m_cache(cache)
    {

    }

    void run() override
    {
        RenderableEntityFilter::run();

        QVector<Entity *> selectedEntities = filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        m_cache->renderableEntities = selectedEntities;
    }

private:
    RendererCache *m_cache;
};

class CachingComputableEntityFilter : public ComputableEntityFilter {
public:
    CachingComputableEntityFilter(RendererCache *cache)
        : ComputableEntityFilter()
        , m_cache(cache)
    {

    }

    void run() override
    {
        ComputableEntityFilter::run();

        QVector<Entity *> selectedEntities = filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        m_cache->computeEntities = selectedEntities;
    }

private:
    RendererCache *m_cache;
};

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
    : m_services(nullptr)
    , m_aspect(nullptr)
    , m_nodesManager(nullptr)
    , m_renderSceneRoot(nullptr)
    , m_defaultRenderStateSet(nullptr)
    , m_submissionContext(nullptr)
    , m_renderQueue(new RenderQueue())
    , m_renderThread(type == QRenderAspect::Threaded ? new RenderThread(this) : nullptr)
    , m_vsyncFrameAdvanceService(new VSyncFrameAdvanceService(m_renderThread != nullptr))
    , m_waitForInitializationToBeCompleted(0)
    , m_hasBeenInitializedMutex()
    , m_exposed(0)
    , m_lastFrameCorrect(0)
    , m_glContext(nullptr)
    , m_shareContext(nullptr)
    , m_time(0)
    , m_settings(nullptr)
    , m_updateShaderDataTransformJob(Render::UpdateShaderDataTransformJobPtr::create())
    , m_cleanupJob(Render::FrameCleanupJobPtr::create())
    , m_sendBufferCaptureJob(Render::SendBufferCaptureJobPtr::create())
    , m_filterCompatibleTechniqueJob(FilterCompatibleTechniqueJobPtr::create())
    , m_lightGathererJob(new CachingLightGatherer(&m_cache))
    , m_renderableEntityFilterJob(new CachingRenderableEntityFilter(&m_cache))
    , m_computableEntityFilterJob(new CachingComputableEntityFilter(&m_cache))
    , m_bufferGathererJob(CreateSynchronizerJobPtr([this] { lookForDirtyBuffers(); }, JobTypes::DirtyBufferGathering, 0))
    , m_vaoGathererJob(CreateSynchronizerJobPtr([this] { lookForAbandonedVaos(); }, JobTypes::DirtyVaoGathering, 0))
    , m_textureGathererJob(CreateSynchronizerJobPtr([this] { lookForDirtyTextures(); }, JobTypes::DirtyTextureGathering, 0))
    , m_sendSetFenceHandlesToFrontendJob(CreateSynchronizerJobPtr([this] { sendSetFenceHandlesToFrontend(); }, JobTypes::SendSetFenceHandlesToFrontend, 0))
    , m_introspectShaderJob(CreateSynchronizerPostFramePtr([this] { reloadDirtyShaders(); },
                                                           [this] (Qt3DCore::QAspectManager *m) { sendShaderChangesToFrontend(m); },
                                                           JobTypes::DirtyShaderGathering))
    , m_ownedContext(false)
    , m_offscreenHelper(nullptr)
    , m_glResourceManagers(nullptr)
    , m_commandExecuter(new Qt3DRender::Debug::CommandExecuter(this))
    , m_shouldSwapBuffers(true)
    , m_imGuiRenderer(nullptr)
    , m_jobsInLastFrame(0)
{
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
    delete m_glResourceManagers;

    if (!m_ownedContext)
        QObject::disconnect(m_contextConnection);

#ifndef Q_OS_INTEGRITY
    delete  m_imGuiRenderer;
#endif
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
    m_updateShaderDataTransformJob->addDependency(QRenderAspectPrivate::get(aspect)->m_worldTransformJob);
}

void Renderer::setNodeManagers(NodeManagers *managers)
{
    m_nodesManager = managers;
    m_glResourceManagers = new GLResourceManagers();
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
    QMutexLocker lock(&m_shareContextMutex);
    return m_shareContext ? m_shareContext
                          : (m_submissionContext->openGLContext()
                             ? m_submissionContext->openGLContext()->shareContext()
                             : nullptr);
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

bool Renderer::accessOpenGLTexture(Qt3DCore::QNodeId nodeId,
                                   QOpenGLTexture **texture,
                                   QMutex **lock,
                                   bool readonly)
{
    Texture *tex = m_nodesManager->textureManager()->lookupResource(nodeId);
    if (!tex)
        return false;

    GLTexture *glTex = m_glResourceManagers->glTextureManager()->lookupResource(tex->peerId());
    if (!glTex)
        return false;

    if (glTex->isDirty())
        return false;

    if (!readonly)
        glTex->setExternalRenderingEnabled(true);

    GLTexture::TextureUpdateInfo texInfo = glTex->createOrUpdateGLTexture();
    *texture = texInfo.texture;

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

    {
        QMutexLocker lock(&m_shareContextMutex);
        // If we are using our own context (not provided by QtQuick),
        // we need to create it
        if (!m_glContext) {
            m_glContext = new QOpenGLContext;
            if (m_screen)
                m_glContext->setScreen(m_screen);
            m_glContext->setShareContext(qt_gl_global_share_context());

            // TO DO: Shouldn't we use the highest context available and trust
            // QOpenGLContext to fall back on the best lowest supported ?
            const QByteArray debugLoggingMode = qgetenv("QT3DRENDER_DEBUG_LOGGING");

            if (!debugLoggingMode.isEmpty()) {
                QSurfaceFormat sf = m_glContext->format();
                sf.setOption(QSurfaceFormat::DebugContext);
                m_glContext->setFormat(sf);
            }

            // Create OpenGL context

            if (m_glContext->create())
                qCDebug(Backend) << "OpenGL context created with actual format" << m_glContext->format();
            else
                qCWarning(Backend) << Q_FUNC_INFO << "OpenGL context creation failed";
            m_ownedContext = true;

            QObject::connect(m_glContext, &QOpenGLContext::aboutToBeDestroyed,
                             [this] { m_frameProfiler.reset(); });
        } else {
            // Context is not owned by us, so we need to know if it gets destroyed
            m_contextConnection = QObject::connect(m_glContext, &QOpenGLContext::aboutToBeDestroyed,
                                                   [this] { releaseGraphicsResources(); });
        }

        qCDebug(Backend) << "Qt3D shared context:" << m_glContext->shareContext();
        qCDebug(Backend) << "Qt global shared context:" << qt_gl_global_share_context();

        if (!m_glContext->shareContext()) {
            m_shareContext = new QOpenGLContext;
            if (m_glContext->screen())
                m_shareContext->setScreen(m_glContext->screen());
            m_shareContext->setFormat(m_glContext->format());
            m_shareContext->setShareContext(m_glContext);
            m_shareContext->create();
        }

        // Note: we don't have a surface at this point
        // The context will be made current later on (at render time)
        m_submissionContext->setOpenGLContext(m_glContext);

        // Store the format used by the context and queue up creating an
        // offscreen surface in the main thread so that it is available
        // for use when we want to shutdown the renderer. We need to create
        // the offscreen surface on the main thread because on some platforms
        // (MS Windows), an offscreen surface is just a hidden QWindow.
        m_format = m_glContext->format();
        QMetaObject::invokeMethod(m_offscreenHelper, "createOffscreenSurface");
    }

    // Awake setScenegraphRoot in case it was waiting
    m_waitForInitializationToBeCompleted.release(1);
    // Allow the aspect manager to proceed
    m_vsyncFrameAdvanceService->proceedToNextFrame();

    // Force initial refresh
    markDirty(AllDirty, nullptr);
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
    const bool wasRunning = m_running.testAndSetRelaxed(1, 0);

    // We might have already been shutdown
    if (!wasRunning)
        return;

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
    delete m_glResourceManagers;
    m_glResourceManagers = nullptr;
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

    QOpenGLContext *context = m_submissionContext->openGLContext();
    Q_ASSERT(context);

    if (context->thread() == QThread::currentThread() && context->makeCurrent(offscreenSurface)) {

        // Clean up the graphics context and any resources
        const std::vector<HGLTexture> &activeTexturesHandles = m_glResourceManagers->glTextureManager()->activeHandles();
        for (const HGLTexture &textureHandle : activeTexturesHandles) {
            GLTexture *tex = m_glResourceManagers->glTextureManager()->data(textureHandle);
            tex->destroy();
        }

        // Do the same thing with buffers
        const std::vector<HGLBuffer> &activeBuffers = m_glResourceManagers->glBufferManager()->activeHandles();
        for (const HGLBuffer &bufferHandle : activeBuffers) {
            GLBuffer *buffer = m_glResourceManagers->glBufferManager()->data(bufferHandle);
            buffer->destroy(m_submissionContext.data());
        }

        // Do the same thing with shaders
        const QVector<GLShader *> shaders = m_glResourceManagers->glShaderManager()->takeActiveResources();
        qDeleteAll(shaders);

        // Do the same thing with VAOs
        const std::vector<HVao> &activeVaos = m_glResourceManagers->vaoManager()->activeHandles();
        for (const HVao &vaoHandle : activeVaos) {
            OpenGLVertexArrayObject *vao = m_glResourceManagers->vaoManager()->data(vaoHandle);
            vao->destroy();
        }

        m_submissionContext->releaseRenderTargets();

        m_frameProfiler.reset();
        if (m_ownedContext)
            context->doneCurrent();
    } else {
        qWarning() << "Failed to make context current: OpenGL resources will not be destroyed";
    }

    if (m_ownedContext)
        delete context;
    if (m_shareContext)
        delete m_shareContext;

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

    // When using synchronous rendering (QtQuick)
    // We are not sure that the frame queue is actually complete
    // Since a call to render may not be synched with the completions
    // of the RenderViewJobs
    // In such a case we return early, waiting for a next call with
    // the frame queue complete at this point

    // RenderQueue is complete (but that means it may be of size 0)
    if (canSubmit && (queueIsComplete && !queueIsEmpty)) {
        const QVector<Render::OpenGL::RenderView *> renderViews = m_renderQueue->nextFrameQueue();
        QTaskLogger submissionStatsPart1(m_services->systemInformation(),
                                         {JobTypes::FrameSubmissionPart1, 0},
                                         QTaskLogger::Submission);
        QTaskLogger submissionStatsPart2(m_services->systemInformation(),
                                         {JobTypes::FrameSubmissionPart2, 0},
                                         QTaskLogger::Submission);
        if (canRender()) {
            { // Scoped to destroy surfaceLock
                QSurface *surface = nullptr;
                for (const RenderView *rv: renderViews) {
                    surface = rv->surface();
                    if (surface)
                        break;
                }

                SurfaceLocker surfaceLock(surface);
                const bool surfaceIsValid = (surface && surfaceLock.isSurfaceValid());
                if (surfaceIsValid) {
                    // Reset state for each draw if we don't have complete control of the context
                    if (!m_ownedContext)
                        m_submissionContext->setCurrentStateSet(nullptr);
                    beganDrawing = m_submissionContext->beginDrawing(surface);
                    if (beganDrawing) {
                        // 1) Execute commands for buffer uploads, texture updates, shader loading first
                        updateGLResources();
                        // 2) Update VAO and copy data into commands to allow concurrent submission
                        prepareCommandsSubmission(renderViews);
                        preprocessingComplete = true;

                        // Purge shader which aren't used any longer
                        static int callCount = 0;
                        ++callCount;
                        const int shaderPurgePeriod = 600;
                        if (callCount % shaderPurgePeriod == 0)
                            m_glResourceManagers->glShaderManager()->purge();
                    }
                }
            }
            // 2) Proceed to next frame and start preparing frame n + 1
            m_renderQueue->reset();
            locker.unlock(); // Done protecting RenderQueue
            m_vsyncFrameAdvanceService->proceedToNextFrame();
            hasCleanedQueueAndProceeded = true;

            // Only try to submit the RenderViews if the preprocessing was successful
            // This part of the submission is happening in parallel to the RV building for the next frame
            if (preprocessingComplete) {
                submissionStatsPart1.end(submissionStatsPart2.restart());

                // 3) Submit the render commands for frame n (making sure we never reference something that could be changing)
                // Render using current device state and renderer configuration
                submissionData = submitRenderViews(renderViews);

                // Perform any required cleanup of the Graphics resources (Buffers deleted, Shader deleted...)
                cleanGraphicsResources();
            }
        }

        // Execute the pending shell commands
        m_commandExecuter->performAsynchronousCommandExecution(renderViews);

        // Delete all the RenderViews which will clear the allocators
        // that were used for their allocation
        qDeleteAll(renderViews);

        if (preprocessingComplete && activeProfiler())
            m_frameProfiler->writeResults();
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
                && surfaceLock.isSurfaceValid()
                && m_shouldSwapBuffers;
        m_submissionContext->endDrawing(swapBuffers);
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

Profiling::FrameProfiler *Renderer::activeProfiler() const
{
    if (m_services && m_services->systemInformation()->isGraphicsTraceEnabled()) {
        if (m_frameProfiler.isNull())
            m_frameProfiler.reset(new Profiling::FrameProfiler(m_services->systemInformation()));

        return m_frameProfiler.data();
    }

    return nullptr;
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
    return m_format;
}

// When this function is called, we must not be processing the commands for frame n+1
void Renderer::prepareCommandsSubmission(const QVector<RenderView *> &renderViews)
{
    OpenGLVertexArrayObject *vao = nullptr;
    QHash<HVao, bool> updatedTable;

    for (RenderView *rv: renderViews) {
        rv->forEachCommand([&] (RenderCommand &command) {
            // Update/Create VAO
            if (command.m_type == RenderCommand::Draw) {
                Geometry *rGeometry = m_nodesManager->data<Geometry, GeometryManager>(command.m_geometry);
                GeometryRenderer *rGeometryRenderer = m_nodesManager->data<GeometryRenderer, GeometryRendererManager>(command.m_geometryRenderer);
                GLShader *shader = command.m_glShader;

                // We should never have inserted a command for which these are null
                // in the first place
                Q_ASSERT(rGeometry && rGeometryRenderer && shader);

                // The VAO should be created only once for a QGeometry and a ShaderProgram
                // Manager should have a VAO Manager that are indexed by QMeshData and Shader
                // RenderCommand should have a handle to the corresponding VAO for the Mesh and Shader
                HVao vaoHandle;

                // Create VAO or return already created instance associated with command shader/geometry
                // (VAO is emulated if not supported)
                createOrUpdateVAO(&command, &vaoHandle, &vao);
                command.m_vao = vaoHandle;

                // Avoids redoing the same thing for the same VAO
                if (!updatedTable.contains(vaoHandle)) {
                    updatedTable.insert(vaoHandle, true);

                    // Do we have any attributes that are dirty ?
                    const bool requiresPartialVAOUpdate = requiresVAOAttributeUpdate(rGeometry, &command);

                    // If true, we need to reupload all attributes to set the VAO
                    // Otherwise only dirty attributes will be updates
                    const bool requiresFullVAOUpdate = (!vao->isSpecified()) || (rGeometry->isDirty() || rGeometryRenderer->isDirty());

                    // Append dirty Geometry to temporary vector
                    // so that its dirtiness can be unset later
                    if (rGeometry->isDirty())
                        m_dirtyGeometry.push_back(rGeometry);

                    if (!command.m_activeAttributes.isEmpty() && (requiresFullVAOUpdate || requiresPartialVAOUpdate)) {
                        Profiling::GLTimeRecorder recorder(Profiling::VAOUpload, activeProfiler());
                        // Activate shader
                        m_submissionContext->activateShader(shader);
                        // Bind VAO
                        vao->bind();
                        // Update or set Attributes and Buffers for the given rGeometry and Command
                        // Note: this fills m_dirtyAttributes as well
                        if (updateVAOWithAttributes(rGeometry, &command, shader, requiresFullVAOUpdate))
                            vao->setSpecified(true);
                    }
                }

                // Unset dirtiness on rGeometryRenderer only
                // The rGeometry may be shared by several rGeometryRenderer
                // so we cannot unset its dirtiness at this point
                if (rGeometryRenderer->isDirty())
                    rGeometryRenderer->unsetDirty();
            } else if (command.m_type == RenderCommand::Compute) {
                GLShader *shader = command.m_glShader;
                Q_ASSERT(shader);
            }
        });
    }

    // Make sure we leave nothing bound
    if (vao)
        vao->release();

    // Unset dirtiness on Geometry and Attributes
    // Note: we cannot do it in the loop above as we want to be sure that all
    // the VAO which reference the geometry/attributes are properly updated
    for (Attribute *attribute : qAsConst(m_dirtyAttributes))
        attribute->unsetDirty();
    m_dirtyAttributes.clear();

    for (Geometry *geometry : qAsConst(m_dirtyGeometry))
        geometry->unsetDirty();
    m_dirtyGeometry.clear();
}

// Executed in a job
void Renderer::lookForAbandonedVaos()
{
    const std::vector<HVao> &activeVaos = m_glResourceManagers->vaoManager()->activeHandles();
    for (HVao handle : activeVaos) {
        OpenGLVertexArrayObject *vao = m_glResourceManagers->vaoManager()->data(handle);

        // Make sure to only mark VAOs for deletion that were already created
        // (ignore those that might be currently under construction in the render thread)
        if (vao && vao->isAbandoned(m_nodesManager->geometryManager(), m_glResourceManagers->glShaderManager())) {
            m_abandonedVaosMutex.lock();
            m_abandonedVaos.push_back(handle);
            m_abandonedVaosMutex.unlock();
        }
    }
}

// Executed in a job
void Renderer::lookForDirtyBuffers()
{
    const std::vector<HBuffer> &activeBufferHandles = m_nodesManager->bufferManager()->activeHandles();
    for (const HBuffer &handle: activeBufferHandles) {
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
    for (const HTextureImage &handle: activeTextureImageHandles) {
        TextureImage *image = imageManager->data(handle);
        if (image->isDirty()) {
            dirtyImageIds.push_back(image->peerId());
            image->unsetDirty();
        }
    }

    TextureManager *textureManager = m_nodesManager->textureManager();
    const std::vector<HTexture> &activeTextureHandles = textureManager->activeHandles();
    for (const HTexture &handle: activeTextureHandles) {
        Texture *texture = textureManager->data(handle);
        const QNodeIdVector imageIds = texture->textureImageIds();

        // Does the texture reference any of the dirty texture images?
        for (const QNodeId imageId: imageIds) {
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
    const std::vector<HTechnique> &activeTechniques = m_nodesManager->techniqueManager()->activeHandles();
    const std::vector<HShaderBuilder> &activeBuilders = m_nodesManager->shaderBuilderManager()->activeHandles();
    for (const HTechnique &techniqueHandle : activeTechniques) {
        Technique *technique = m_nodesManager->techniqueManager()->data(techniqueHandle);
        // If api of the renderer matches the one from the technique
        if (technique->isCompatibleWithRenderer()) {
            const auto passIds = technique->renderPasses();
            for (const QNodeId &passId : passIds) {
                RenderPass *renderPass = m_nodesManager->renderPassManager()->lookupResource(passId);
                HShader shaderHandle = m_nodesManager->shaderManager()->lookupHandle(renderPass->shaderProgram());
                Shader *shader = m_nodesManager->shaderManager()->data(shaderHandle);

                // Shader could be null if the pass doesn't reference one yet
                if (!shader)
                    continue;

                ShaderBuilder *shaderBuilder = nullptr;
                for (const HShaderBuilder &builderHandle : activeBuilders) {
                    ShaderBuilder *builder = m_nodesManager->shaderBuilderManager()->data(builderHandle);
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

                if (shader->isDirty())
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
    for (const HShader &handle :activeShaders) {
        Shader *s = m_nodesManager->shaderManager()->data(handle);
        if (s->requiresFrontendSync()) {
            QShaderProgram *frontend = static_cast<decltype(frontend)>(manager->lookupNode(s->peerId()));
            // Could happen as a backend shader might live beyong the frontend
            // the time needed to destroy the GLShader assoicated with it.
            if (!frontend)
                continue;
            QShaderProgramPrivate *dFrontend = static_cast<decltype(dFrontend)>(QNodePrivate::get(frontend));
            s->unsetRequiresFrontendSync();
            dFrontend->setStatus(s->status());
            dFrontend->setLog(s->log());
        }
    }

    // Sync ShaderBuilder
    const QVector<ShaderBuilderUpdate> shaderBuilderUpdates = std::move(m_shaderBuilderUpdates);
    for (const ShaderBuilderUpdate &update : shaderBuilderUpdates) {
        QShaderProgramBuilder *builder = static_cast<decltype(builder)>(manager->lookupNode(update.builderId));
        QShaderProgramBuilderPrivate *dBuilder = static_cast<decltype(dBuilder)>(QNodePrivate::get(builder));
        dBuilder->setShaderCode(update.shaderCode, update.shaderType);
    }
}

// Executed in a job (in main thread when jobs are done)
void Renderer::sendTextureChangesToFrontend(Qt3DCore::QAspectManager *manager)
{
    const QVector<QPair<Texture::TextureUpdateInfo, Qt3DCore::QNodeIdVector>> updateTextureProperties = std::move(m_updatedTextureProperties);
    for (const auto &pair : updateTextureProperties) {
        const Qt3DCore::QNodeIdVector targetIds = pair.second;
        for (const Qt3DCore::QNodeId &targetId: targetIds) {
            // Lookup texture
            Texture *t = m_nodesManager->textureManager()->lookupResource(targetId);
            // If backend texture is Dirty, some property has changed and the properties we are
            // about to send are already outdate
            if (t == nullptr || t->dirtyFlags() != Texture::NotDirty)
                continue;

            QAbstractTexture *texture = static_cast<QAbstractTexture *>(manager->lookupNode(targetId));
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

           QAbstractTexturePrivate *dTexture = static_cast<QAbstractTexturePrivate *>(QNodePrivate::get(texture));

           dTexture->setStatus(properties.status);
           dTexture->setHandleType(pair.first.handleType);
           dTexture->setHandle(pair.first.handle);
        }
    }
}

// Executed in a job
void Renderer::sendSetFenceHandlesToFrontend()
{
    const QVector<QPair<Qt3DCore::QNodeId, GLFence>> updatedSetFence = std::move(m_updatedSetFences);
    FrameGraphManager *fgManager = m_nodesManager->frameGraphManager();
    for (const auto &pair : updatedSetFence) {
        FrameGraphNode *fgNode = fgManager->lookupNode(pair.first);
        if (fgNode != nullptr) { // Node could have been deleted before we got a chance to notify it
            Q_ASSERT(fgNode->nodeType() == FrameGraphNode::SetFence);
            SetFence *setFenceNode = static_cast<SetFence *>(fgNode);
            setFenceNode->setHandleType(QSetFence::OpenGLFenceId);
            setFenceNode->setHandle(QVariant::fromValue(pair.second));
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
    const std::vector<HComputeCommand> &activeCommands = m_nodesManager->computeJobManager()->activeHandles();
    for (const HComputeCommand &handle :activeCommands) {
        ComputeCommand *c = m_nodesManager->computeJobManager()->data(handle);
        if (c->hasReachedFrameCount()) {
            QComputeCommand *frontend = static_cast<decltype(frontend)>(manager->lookupNode(c->peerId()));
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
void Renderer::updateGLResources()
{
    {
        // Update active fence objects:
        // - Destroy fences that have reached their signaled state
        GLFenceManager *fenceManager = m_glResourceManagers->glFenceManager();
        const auto end = fenceManager->end();
        auto it = fenceManager->begin();
        while (it != end) {
            const GLFence fence = it.value();
            if (m_submissionContext->wasSyncSignaled(fence)) {
                // Fence was signaled, we delete it
                // before removing the entry from the manager
                m_submissionContext->deleteSync(fence);
                it = fenceManager->erase(it);
            } else {
                ++it;
            }
        }
    }

    {
        Profiling::GLTimeRecorder recorder(Profiling::BufferUpload, activeProfiler());
        const QVector<HBuffer> dirtyBufferHandles = std::move(m_dirtyBuffers);
        for (const HBuffer &handle: dirtyBufferHandles) {
            Buffer *buffer = m_nodesManager->bufferManager()->data(handle);

            // Can be null when using Scene3D rendering
            if (buffer == nullptr)
                continue;

            // Forces creation if it doesn't exit
            // Also note the binding point doesn't really matter here, we just upload data
            if (!m_submissionContext->hasGLBufferForBuffer(buffer))
                m_submissionContext->glBufferForRenderBuffer(buffer);
            // Update the glBuffer data
            m_submissionContext->updateBuffer(buffer);
            buffer->unsetDirty();
        }
    }

#ifndef SHADER_LOADING_IN_COMMAND_THREAD
    {
        Profiling::GLTimeRecorder recorder(Profiling::ShaderUpload, activeProfiler());
        const QVector<HShader> dirtyShaderHandles = std::move(m_dirtyShaders);
        ShaderManager *shaderManager = m_nodesManager->shaderManager();
        for (const HShader &handle: dirtyShaderHandles) {
            Shader *shader = shaderManager->data(handle);

            // Can be null when using Scene3D rendering
            if (shader == nullptr)
                continue;

            // Compile shader
            m_submissionContext->loadShader(shader, shaderManager, m_glResourceManagers->glShaderManager());
        }
    }
#endif

    {
        Profiling::GLTimeRecorder recorder(Profiling::TextureUpload, activeProfiler());
        const QVector<HTexture> activeTextureHandles = std::move(m_dirtyTextures);
        for (const HTexture &handle: activeTextureHandles) {
            Texture *texture = m_nodesManager->textureManager()->data(handle);

            // Can be null when using Scene3D rendering
            if (texture ==  nullptr)
                continue;

            // Create or Update GLTexture (the GLTexture instance is created
            // (not the underlying GL instance) if required and all things that
            // can take place without a GL context are done here)
            updateTexture(texture);
        }
        // We want to upload textures data at this point as the SubmissionThread and
        // AspectThread are locked ensuring no races between Texture/TextureImage and
        // GLTexture
        QNodeIdVector updatedTexturesForFrame;
        if (m_submissionContext != nullptr) {
            GLTextureManager *glTextureManager = m_glResourceManagers->glTextureManager();
            const std::vector<HGLTexture> &glTextureHandles = glTextureManager->activeHandles();
            // Upload texture data
            for (const HGLTexture &glTextureHandle : glTextureHandles) {
                GLTexture *glTexture = glTextureManager->data(glTextureHandle);

                // We create/update the actual GL texture using the GL context at this point
                const GLTexture::TextureUpdateInfo info = glTexture->createOrUpdateGLTexture();

                // GLTexture creation provides us width/height/format ... information
                // for textures which had not initially specified these information (TargetAutomatic...)
                // Gather these information and store them to be distributed by a change next frame
                const QNodeIdVector referenceTextureIds = { glTextureManager->texNodeIdForGLTexture.value(glTexture) };
                // Store properties and referenceTextureIds
                if (info.wasUpdated) {
                    Texture::TextureUpdateInfo updateInfo;
                    updateInfo.properties = info.properties;
                    updateInfo.handleType = QAbstractTexture::OpenGLTextureId;
                    updateInfo.handle = info.texture ? QVariant(info.texture->textureId()) : QVariant();
                    m_updatedTextureProperties.push_back({updateInfo, referenceTextureIds});
                    updatedTexturesForFrame += referenceTextureIds;
                }
            }
        }

        // If the underlying GL Texture was for whatever reason recreated, we need to make sure
        // that if it is used as a color attachment, we rebuild the FBO next time it is used
        m_submissionContext->setUpdatedTexture(std::move(updatedTexturesForFrame));

        // Record ids of texture to cleanup while we are still blocking the aspect thread
        m_textureIdsToCleanup += m_nodesManager->textureManager()->takeTexturesIdsToCleanup();
    }

    // Record list of buffer that might need uploading
    lookForDownloadableBuffers();

    // Remove destroyed FBOs
    {
        const QNodeIdVector destroyedRenderTargetIds = m_nodesManager->renderTargetManager()->takeRenderTargetIdsToCleanup();
        for (const Qt3DCore::QNodeId &renderTargetId : destroyedRenderTargetIds)
            m_submissionContext->releaseRenderTarget(renderTargetId);
    }
}

// Render Thread
void Renderer::updateTexture(Texture *texture)
{
    // Check that the current texture images are still in place, if not, do not update
    const bool isValid = texture->isValid(m_nodesManager->textureImageManager());
    if (!isValid) {
        qWarning() << Q_FUNC_INFO << "QTexture referencing invalid QTextureImages";
        return;
    }

    // All textures are unique, if you instanciate twice the exact same texture
    // this will create 2 identical GLTextures, no sharing will take place

    // Try to find the associated GLTexture for the backend Texture
    GLTextureManager *glTextureManager = m_glResourceManagers->glTextureManager();
    GLTexture *glTexture = glTextureManager->lookupResource(texture->peerId());

    // No GLTexture associated yet -> create it
    if (glTexture == nullptr) {
        glTexture = glTextureManager->getOrCreateResource(texture->peerId());
        glTextureManager->texNodeIdForGLTexture.insert(glTexture, texture->peerId());
    }

    // Update GLTexture to match Texture instance
    const Texture::DirtyFlags dirtyFlags = texture->dirtyFlags();
    if (dirtyFlags.testFlag(Texture::DirtySharedTextureId))
        glTexture->setSharedTextureId(texture->sharedTextureId());

    if (dirtyFlags.testFlag(Texture::DirtyProperties))
        glTexture->setProperties(texture->properties());

    if (dirtyFlags.testFlag(Texture::DirtyParameters))
        glTexture->setParameters(texture->parameters());

    // Will make the texture requestUpload
    if (dirtyFlags.testFlag(Texture::DirtyImageGenerators)) {
        const QNodeIdVector textureImageIds = texture->textureImageIds();
        QVector<GLTexture::Image> images;
        images.reserve(textureImageIds.size());
        // TODO: Move this into GLTexture directly
        for (const QNodeId textureImageId : textureImageIds) {
            const TextureImage *img = m_nodesManager->textureImageManager()->lookupResource(textureImageId);
            if (img == nullptr) {
                qWarning() << Q_FUNC_INFO << "invalid TextureImage handle";
            } else {
                GLTexture::Image glImg {img->dataGenerator(), img->layer(), img->mipLevel(), img->face()};
                images.push_back(glImg);
            }
        }
        glTexture->setImages(images);
    }

    // Will make the texture requestUpload
    if (dirtyFlags.testFlag(Texture::DirtyDataGenerator))
        glTexture->setGenerator(texture->dataGenerator());

    // Will make the texture requestUpload
    if (dirtyFlags.testFlag(Texture::DirtyPendingDataUpdates))
        glTexture->addTextureDataUpdates(texture->takePendingTextureDataUpdates());

    // Unset the dirty flag on the texture
    texture->unsetDirty();
}

// Render Thread
void Renderer::cleanupTexture(Qt3DCore::QNodeId cleanedUpTextureId)
{
    GLTextureManager *glTextureManager = m_glResourceManagers->glTextureManager();
    GLTexture *glTexture = glTextureManager->lookupResource(cleanedUpTextureId);

    // Destroying the GLTexture implicitely also destroy the GL resources
    if (glTexture != nullptr) {
        glTextureManager->releaseResource(cleanedUpTextureId);
        glTextureManager->texNodeIdForGLTexture.remove(glTexture);
    }
}

// Render Thread
void Renderer::cleanupShader(const Shader *shader)
{
    GLShaderManager *glShaderManager = m_glResourceManagers->glShaderManager();
    GLShader *glShader = glShaderManager->lookupResource(shader->peerId());

    if (glShader != nullptr)
        glShaderManager->abandon(glShader, shader);
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
Renderer::ViewSubmissionResultData Renderer::submitRenderViews(const QVector<RenderView *> &renderViews)
{
    QElapsedTimer timer;
    quint64 queueElapsed = 0;
    timer.start();

    const int renderViewsCount = renderViews.size();
    quint64 frameElapsed = queueElapsed;
    m_lastFrameCorrect.storeRelaxed(1);    // everything fine until now.....

    qCDebug(Memory) << Q_FUNC_INFO << "rendering frame ";

    // We might not want to render on the default FBO
    uint lastBoundFBOId = m_submissionContext->boundFrameBufferObject();
    QSurface *surface = nullptr;
    QSurface *previousSurface = nullptr;
    for (const RenderView *rv: renderViews) {
        previousSurface = rv->surface();
        if (previousSurface)
            break;
    }
    QSurface *lastUsedSurface = nullptr;

    bool imGuiOverlayShown = false;
    for (int i = 0; i < renderViewsCount; ++i) {
        // Initialize GraphicsContext for drawing
        // If the RenderView has a RenderStateSet defined
        RenderView *renderView = renderViews.at(i);

        if (renderView->shouldSkipSubmission())
            continue;

        // Check if using the same surface as the previous RenderView.
        // If not, we have to free up the context from the previous surface
        // and make the context current on the new surface
        surface = renderView->surface();
        SurfaceLocker surfaceLock(surface);

        // TO DO: Make sure that the surface we are rendering too has not been unset

        // For now, if we do not have a surface, skip this renderview
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
                    && surfaceLock.isSurfaceValid()
                    && m_shouldSwapBuffers;
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
            lastBoundFBOId = m_submissionContext->boundFrameBufferObject();
        }

        // Apply Memory Barrier if needed
        if (renderView->memoryBarrier() != QMemoryBarrier::None)
            m_submissionContext->memoryBarrier(renderView->memoryBarrier());


        // Insert Fence into command stream if needed
        const Qt3DCore::QNodeIdVector insertFenceIds = renderView->insertFenceIds();
        GLFenceManager *fenceManager = m_glResourceManagers->glFenceManager();
        for (const Qt3DCore::QNodeId insertFenceId : insertFenceIds) {
            // If the fence is not in the manager, then it hasn't been inserted
            // into the command stream yet.
            if (fenceManager->find(insertFenceId) == fenceManager->end()) {
                // Insert fence into command stream
                GLFence glFence = m_submissionContext->fenceSync();
                // Record glFence
                fenceManager->insert(insertFenceId, glFence);
                // Add entry for notification changes to be sent
                m_updatedSetFences.push_back({insertFenceId, glFence});
            }
            // If it is in the manager, then it hasn't been signaled yet,
            // nothing we can do but try at the next frame
        }

        // Wait for fences if needed
        const QVector<QWaitFenceData> waitFences = renderView->waitFences();
        for (const QWaitFenceData &waitFence : waitFences) {
            // TO DO
            if (waitFence.handleType != QWaitFence::OpenGLFenceId) {
                qWarning() << "WaitFence handleType should be OpenGLFenceId when using the Qt 3D OpenGL renderer";
                continue;
            }
            GLFence fence = reinterpret_cast<GLFence>(waitFence.handle.value<qintptr>());
            if (fence == nullptr)
                continue;

            if (waitFence.waitOnCPU) {
                m_submissionContext->clientWaitSync(fence,
                                                    waitFence.timeout);
            } else {
                m_submissionContext->waitSync(fence);
            }
        }

        // Note: the RenderStateSet is allocated once per RV if needed
        // and it contains a list of StateVariant value types
        RenderStateSet *renderViewStateSet = renderView->stateSet();

        {
            Profiling::GLTimeRecorder recorder(Profiling::StateUpdate, activeProfiler());
            // Set the RV state if not null,
            if (renderViewStateSet != nullptr)
                m_submissionContext->setCurrentStateSet(renderViewStateSet);
            else
                m_submissionContext->setCurrentStateSet(m_defaultRenderStateSet);
        }

        // Set RenderTarget ...
        // Activate RenderTarget
        {
            Profiling::GLTimeRecorder recorder(Profiling::RenderTargetUpdate, activeProfiler());
            m_submissionContext->activateRenderTarget(renderView->renderTargetId(),
                                                    renderView->attachmentPack(),
                                                    lastBoundFBOId);
        }

        {
            Profiling::GLTimeRecorder recorder(Profiling::ClearBuffer, activeProfiler());
            // set color, depth, stencil clear values (only if needed)
            auto clearBufferTypes = renderView->clearTypes();
            if (clearBufferTypes & QClearBuffers::ColorBuffer) {
                const QVector4D vCol = renderView->globalClearColorBufferInfo().clearColor;
                m_submissionContext->clearColor(QColor::fromRgbF(vCol.x(), vCol.y(), vCol.z(), vCol.w()));
            }
            if (clearBufferTypes & QClearBuffers::DepthBuffer)
                m_submissionContext->clearDepthValue(renderView->clearDepthValue());
            if (clearBufferTypes & QClearBuffers::StencilBuffer)
                m_submissionContext->clearStencilValue(renderView->clearStencilValue());

            // Clear BackBuffer
            m_submissionContext->clearBackBuffer(clearBufferTypes);

            // if there are ClearColors set for different draw buffers,
            // clear each of these draw buffers individually now
            const QVector<ClearBufferInfo> clearDrawBuffers = renderView->specificClearColorBufferInfo();
            for (const ClearBufferInfo &clearBuffer : clearDrawBuffers)
                m_submissionContext->clearBufferf(clearBuffer.drawBufferIndex, clearBuffer.clearColor);
        }

        // Set the Viewport
        m_submissionContext->setViewport(renderView->viewport(), renderView->surfaceSize());

        // Execute the render commands
        if (!executeCommandsSubmission(renderView))
            m_lastFrameCorrect.storeRelaxed(0);    // something went wrong; make sure to render the next frame!

        // executeCommandsSubmission takes care of restoring the stateset to the value
        // of gc->currentContext() at the moment it was called (either
        // renderViewStateSet or m_defaultRenderStateSet)
        if (!renderView->renderCaptureNodeId().isNull()) {
            const QRenderCaptureRequest request = renderView->renderCaptureRequest();
            const QSize size = m_submissionContext->renderTargetSize(renderView->surfaceSize());
            QRect rect(QPoint(0, 0), size);
            if (!request.rect.isEmpty())
                rect = rect.intersected(request.rect);
            QImage image;
            if (!rect.isEmpty()) {
                // Bind fbo as read framebuffer
                m_submissionContext->bindFramebuffer(m_submissionContext->activeFBO(), GraphicsHelperInterface::FBORead);
                image = m_submissionContext->readFramebuffer(rect);
            } else {
                qWarning() << "Requested capture rectangle is outside framebuffer";
            }
            Render::RenderCapture *renderCapture =
                    static_cast<Render::RenderCapture*>(m_nodesManager->frameGraphManager()->lookupNode(renderView->renderCaptureNodeId()));
            renderCapture->addRenderCapture(request.captureId, image);
            if (!m_pendingRenderCaptureSendRequests.contains(renderView->renderCaptureNodeId()))
                m_pendingRenderCaptureSendRequests.push_back(renderView->renderCaptureNodeId());
        }

        if (renderView->isDownloadBuffersEnable())
            downloadGLBuffers();

        // Perform BlitFramebuffer operations
        if (renderView->hasBlitFramebufferInfo()) {
            const auto &blitFramebufferInfo = renderView->blitFrameBufferInfo();
            const QNodeId inputTargetId = blitFramebufferInfo.sourceRenderTargetId;
            const QNodeId outputTargetId = blitFramebufferInfo.destinationRenderTargetId;
            const QRect inputRect = blitFramebufferInfo.sourceRect;
            const QRect outputRect = blitFramebufferInfo.destinationRect;
            const QRenderTargetOutput::AttachmentPoint inputAttachmentPoint = blitFramebufferInfo.sourceAttachmentPoint;
            const QRenderTargetOutput::AttachmentPoint outputAttachmentPoint = blitFramebufferInfo.destinationAttachmentPoint;
            const QBlitFramebuffer::InterpolationMethod interpolationMethod = blitFramebufferInfo.interpolationMethod;
            m_submissionContext->blitFramebuffer(inputTargetId, outputTargetId, inputRect, outputRect, lastBoundFBOId,
                                                 inputAttachmentPoint, outputAttachmentPoint,
                                                 interpolationMethod);
        }

#ifndef Q_OS_INTEGRITY
        if (!imGuiOverlayShown && renderView->showDebugOverlay()) {
            imGuiOverlayShown = true;
            if (!m_imGuiRenderer) {
                m_imGuiRenderer = new Debug::ImGuiRenderer(this);
                if (m_settings)
                    m_imGuiRenderer->setCapabilities(m_settings->capabilities());
            }

            {
                QMutexLocker l(&m_frameEventsMutex);
                for (auto &keyEvent: m_frameKeyEvents)
                    m_imGuiRenderer->processEvent(&keyEvent);
                for (auto &mouseEvent: m_frameMouseEvents)
                    m_imGuiRenderer->processEvent(&mouseEvent.second);
            }
            m_imGuiRenderer->renderDebugOverlay(renderViews, renderView, m_jobsInLastFrame);
        }
#endif

        frameElapsed = timer.elapsed() - frameElapsed;
        qCDebug(Rendering) << Q_FUNC_INFO << "Submitted Renderview " << i + 1 << "/" << renderViewsCount  << "in " << frameElapsed << "ms";
        frameElapsed = timer.elapsed();
    }

    // Bind lastBoundFBOId back. Needed also in threaded mode.
    // lastBoundFBOId != m_graphicsContext->activeFBO() when the last FrameGraph leaf node/renderView
    // contains RenderTargetSelector/RenderTarget
    if (lastBoundFBOId != m_submissionContext->activeFBO())
        m_submissionContext->bindFramebuffer(lastBoundFBOId, GraphicsHelperInterface::FBOReadAndDraw);

    // Reset state and call doneCurrent if the surface
    // is valid and was actually activated
    if (lastUsedSurface && m_submissionContext->hasValidGLHelper()) {
        // Reset state to the default state if the last stateset is not the
        // defaultRenderStateSet
        if (m_submissionContext->currentStateSet() != m_defaultRenderStateSet)
            m_submissionContext->setCurrentStateSet(m_defaultRenderStateSet);
    }

    queueElapsed = timer.elapsed() - queueElapsed;
    qCDebug(Rendering) << Q_FUNC_INFO << "Submission of Queue in " << queueElapsed << "ms <=> " << queueElapsed / renderViewsCount << "ms per RenderView <=> Avg " << 1000.0f / (queueElapsed * 1.0f/ renderViewsCount * 1.0f) << " RenderView/s";
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
    return ((m_settings && m_settings->renderPolicy() == QRenderSettings::Always)
            || m_dirtyBits.marked != 0
            || m_dirtyBits.remaining != 0
            || !m_lastFrameCorrect.loadRelaxed());
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
    const QVector<Qt3DCore::QNodeId> pendingCaptureIds = std::move(m_pendingRenderCaptureSendRequests);
    for (const Qt3DCore::QNodeId &id : qAsConst(pendingCaptureIds)) {
        auto *backend = static_cast<Qt3DRender::Render::RenderCapture *>
            (m_nodesManager->frameGraphManager()->lookupNode(id));
        backend->syncRenderCapturesToFrontend(manager);
    }

    // Do we need to notify any texture about property changes?
    if (m_updatedTextureProperties.size() > 0)
        sendTextureChangesToFrontend(manager);

    sendDisablesToFrontend(manager);
}

void Renderer::setPendingEvents(const QList<QPair<QObject *, QMouseEvent> > &mouseEvents, const QList<QKeyEvent> &keyEvents)
{
    QMutexLocker l(&m_frameEventsMutex);
    m_frameMouseEvents = mouseEvents;
    m_frameKeyEvents = keyEvents;
}

// Jobs we may have to run even if no rendering will happen
QVector<QAspectJobPtr> Renderer::preRenderingJobs()
{
    QVector<QAspectJobPtr> jobs;

    // Do we need to notify frontend about fence change?
    if (m_updatedSetFences.size() > 0)
        jobs.push_back(m_sendSetFenceHandlesToFrontendJob);

    if (m_sendBufferCaptureJob->hasRequests())
        jobs.push_back(m_sendBufferCaptureJob);

    return jobs;
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
    if (dirtyBitsForFrame & AbstractRenderer::TransformDirty)
        renderBinJobs.push_back(m_updateShaderDataTransformJob);

    // TO DO: Conditionally add if skeletons dirty
    renderBinJobs.push_back(m_cleanupJob);

    // Jobs to prepare GL Resource upload
    renderBinJobs.push_back(m_vaoGathererJob);

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
    const bool renderCommandsDirty = materialCacheNeedsToBeRebuilt || renderableDirty || computeableDirty;

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

        int idealThreadCount = QThreadPooler::maxThreadCount();

        const int fgBranchCount = m_frameGraphLeaves.size();
        if (fgBranchCount > 1) {
            int workBranches = fgBranchCount;
            for (auto leaf: qAsConst(m_frameGraphLeaves))
                if (leaf->nodeType() == FrameGraphNode::NoDraw)
                    --workBranches;

            if (idealThreadCount > 4 && workBranches)
                idealThreadCount = qMax(4, idealThreadCount / workBranches);
        }

        for (int i = 0; i < fgBranchCount; ++i) {
            FrameGraphNode *leaf = m_frameGraphLeaves.at(i);
            RenderViewBuilder builder(leaf, i, this);
            builder.setOptimalJobCount(leaf->nodeType() == FrameGraphNode::NoDraw ? 1 : idealThreadCount);

            // If we have a new RV (wasn't in the cache before, then it contains no cached data)
            const bool isNewRV = !m_cache.leafNodeCache.contains(leaf);
            builder.setLayerCacheNeedsToBeRebuilt(layersCacheNeedsToBeRebuilt || isNewRV);
            builder.setMaterialGathererCacheNeedsToBeRebuilt(materialCacheNeedsToBeRebuilt || isNewRV);
            builder.setRenderCommandCacheNeedsToBeRebuilt(renderCommandsDirty || isNewRV);
            builder.setLightCacheNeedsToBeRebuilt(lightsDirty);

            // Insert leaf into cache
            if (isNewRV) {
                m_cache.leafNodeCache[leaf] = {};
            }

            builder.prepareJobs();
            renderBinJobs.append(builder.buildJobHierachy());
        }

        // Set target number of RenderViews
        m_renderQueue->setTargetRenderViewCount(fgBranchCount);
    } else {
        // FilterLayerEntityJob is part of the RenderViewBuilder jobs and must be run later
        // if none of those jobs are started this frame
        notCleared |= AbstractRenderer::EntityEnabledDirty;
        notCleared |= AbstractRenderer::LayersDirty;
        notCleared |= AbstractRenderer::FrameGraphDirty;
    }

    if (isRunning() && m_submissionContext->isInitialized()) {
        if (dirtyBitsForFrame & AbstractRenderer::TechniquesDirty )
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
void Renderer::performDraw(const RenderCommand *command)
{
    // Indirect Draw Calls
    if (command->m_drawIndirect) {

        // Bind the indirect draw buffer
        Buffer *indirectDrawBuffer = m_nodesManager->bufferManager()->data(command->m_indirectDrawBuffer);
        if (Q_UNLIKELY(indirectDrawBuffer == nullptr)) {
            qWarning() << "Invalid Indirect Draw Buffer - failed to retrieve Buffer";
            return;
        }

        // Get GLBuffer from Buffer;
        GLBuffer *indirectDrawGLBuffer = m_submissionContext->glBufferForRenderBuffer(indirectDrawBuffer);
        if (Q_UNLIKELY(indirectDrawGLBuffer == nullptr)) {
            qWarning() << "Invalid Indirect Draw Buffer - failed to retrieve GLBuffer";
            return;
        }

        // Bind GLBuffer
        const bool successfullyBound = indirectDrawGLBuffer->bind(m_submissionContext.data(), GLBuffer::DrawIndirectBuffer);

        if (Q_LIKELY(successfullyBound)) {
            // TO DO: Handle multi draw variants if attribute count > 1
            if (command->m_drawIndexed) {
                m_submissionContext->drawElementsIndirect(command->m_primitiveType,
                                                        command->m_indexAttributeDataType,
                                                        reinterpret_cast<void*>(quintptr(command->m_indirectAttributeByteOffset)));
            } else {
                m_submissionContext->drawArraysIndirect(command->m_primitiveType,
                                                      reinterpret_cast<void*>(quintptr(command->m_indirectAttributeByteOffset)));
            }
        } else {
            qWarning() << "Failed to bind IndirectDrawBuffer";
        }

    } else { // Direct Draw Calls

        // TO DO: Add glMulti Draw variants
        if (command->m_primitiveType == QGeometryRenderer::Patches)
            m_submissionContext->setVerticesPerPatch(command->m_verticesPerPatch);

        if (command->m_primitiveRestartEnabled)
            m_submissionContext->enablePrimitiveRestart(command->m_restartIndexValue);

        // TO DO: Add glMulti Draw variants
        if (command->m_drawIndexed) {
            Profiling::GLTimeRecorder recorder(Profiling::DrawElement, activeProfiler());
            m_submissionContext->drawElementsInstancedBaseVertexBaseInstance(command->m_primitiveType,
                                                                           command->m_primitiveCount,
                                                                           command->m_indexAttributeDataType,
                                                                           reinterpret_cast<void*>(quintptr(command->m_indexAttributeByteOffset)),
                                                                           command->m_instanceCount,
                                                                           command->m_indexOffset,
                                                                           command->m_firstInstance);
        } else {
            Profiling::GLTimeRecorder recorder(Profiling::DrawArray, activeProfiler());
            m_submissionContext->drawArraysInstancedBaseInstance(command->m_primitiveType,
                                                               command->m_firstVertex,
                                                               command->m_primitiveCount,
                                                               command->m_instanceCount,
                                                               command->m_firstInstance);
        }
    }

#if defined(QT3D_RENDER_ASPECT_OPENGL_DEBUG)
    int err = m_submissionContext->openGLContext()->functions()->glGetError();
    if (err)
        qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);
#endif

    if (command->m_primitiveRestartEnabled)
        m_submissionContext->disablePrimitiveRestart();
}

void Renderer::performCompute(const RenderView *, RenderCommand *command)
{
    {
        Profiling::GLTimeRecorder recorder(Profiling::ShaderUpdate, activeProfiler());
        GLShader *shader = m_glResourceManagers->glShaderManager()->lookupResource(command->m_shaderId);
        m_submissionContext->activateShader(shader);
    }
    {
        Profiling::GLTimeRecorder recorder(Profiling::UniformUpdate, activeProfiler());
        m_submissionContext->setParameters(command->m_parameterPack, command->m_glShader);
    }
    {
        Profiling::GLTimeRecorder recorder(Profiling::DispatchCompute, activeProfiler());
        m_submissionContext->dispatchCompute(command->m_workGroups[0],
                command->m_workGroups[1],
                command->m_workGroups[2]);
    }
    // HACK: Reset the compute flag to dirty
    m_dirtyBits.marked |= AbstractRenderer::ComputeDirty;

#if defined(QT3D_RENDER_ASPECT_OPENGL_DEBUG)
    int err = m_submissionContext->openGLContext()->functions()->glGetError();
    if (err)
        qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);
#endif
}

void Renderer::createOrUpdateVAO(RenderCommand *command,
                                 HVao *previousVaoHandle,
                                 OpenGLVertexArrayObject **vao)
{
    const VAOIdentifier vaoKey(command->m_geometry, command->m_shaderId);

    VAOManager *vaoManager = m_glResourceManagers->vaoManager();
    command->m_vao = vaoManager->lookupHandle(vaoKey);

    if (command->m_vao.isNull()) {
        qCDebug(Rendering) << Q_FUNC_INFO << "Allocating new VAO";
        command->m_vao = vaoManager->getOrAcquireHandle(vaoKey);
        vaoManager->data(command->m_vao)->create(m_submissionContext.data(), vaoKey);
    }

    if (*previousVaoHandle != command->m_vao) {
        *previousVaoHandle = command->m_vao;
        *vao = vaoManager->data(command->m_vao);
    }
    Q_ASSERT(*vao);
}

// Called by RenderView->submit() in RenderThread context
// Returns true, if all RenderCommands were sent to the GPU
bool Renderer::executeCommandsSubmission(RenderView *rv)
{
    bool allCommandsIssued = true;

    // Render drawing commands

    // Use the graphicscontext to submit the commands to the underlying
    // graphics API (OpenGL)

    // Save the RenderView base stateset
    RenderStateSet *globalState = m_submissionContext->currentStateSet();
    OpenGLVertexArrayObject *vao = nullptr;

    rv->forEachCommand([&] (RenderCommand &command) {

        if (command.m_type == RenderCommand::Compute) { // Compute Call
            performCompute(rv, &command);
        } else { // Draw Command
            // Check if we have a valid command that can be drawn
            if (!command.m_isValid) {
                allCommandsIssued = false;
                return;
            }

            vao = m_glResourceManagers->vaoManager()->data(command.m_vao);

            // something may have went wrong when initializing the VAO
            if (!vao->isSpecified()) {
                allCommandsIssued = false;
                return;
            }

            {
                Profiling::GLTimeRecorder recorder(Profiling::ShaderUpdate, activeProfiler());
                //// We activate the shader here
                GLShader *shader = command.m_glShader;
                if (!m_submissionContext->activateShader(shader)) {
                    allCommandsIssued = false;
                    return;
                }
            }

            {
                Profiling::GLTimeRecorder recorder(Profiling::VAOUpdate, activeProfiler());
                // Bind VAO
                vao->bind();
            }

            {
                Profiling::GLTimeRecorder recorder(Profiling::UniformUpdate, activeProfiler());
                //// Update program uniforms
                if (!m_submissionContext->setParameters(command.m_parameterPack, command.m_glShader)) {
                    allCommandsIssued = false;
                    // If we have failed to set uniform (e.g unable to bind a texture)
                    // we won't perform the draw call which could show invalid content
                    return;
                }
            }

            //// OpenGL State
            // TO DO: Make states not dependendent on their backend node for this step
            // Set state
            RenderStateSet *localState = command.m_stateSet.data();


            {
                Profiling::GLTimeRecorder recorder(Profiling::StateUpdate, activeProfiler());
                // Merge the RenderCommand state with the globalState of the RenderView
                // Or restore the globalState if no stateSet for the RenderCommand
                if (localState != nullptr) {
                    command.m_stateSet->merge(globalState);
                    m_submissionContext->setCurrentStateSet(localState);
                } else {
                    m_submissionContext->setCurrentStateSet(globalState);
                }
            }
            // All Uniforms for a pass are stored in the QUniformPack of the command
            // Uniforms for Effect, Material and Technique should already have been correctly resolved
            // at that point

            //// Draw Calls
            performDraw(&command);
        }
    }); // end of RenderCommands loop

    // We cache the VAO and release it only at the end of the exectute frame
    // We try to minimize VAO binding between RenderCommands
    if (vao)
        vao->release();

    // Reset to the state we were in before executing the render commands
    m_submissionContext->setCurrentStateSet(globalState);

    return allCommandsIssued;
}

bool Renderer::updateVAOWithAttributes(Geometry *geometry,
                                       const RenderCommand *command,
                                       GLShader *shader,
                                       bool forceUpdate)
{
    m_dirtyAttributes.reserve(m_dirtyAttributes.size() + geometry->attributes().size());
    const auto attributeIds = geometry->attributes();

    for (QNodeId attributeId : attributeIds) {
        // TO DO: Improvement we could store handles and use the non locking policy on the attributeManager
        Attribute *attribute = m_nodesManager->attributeManager()->lookupResource(attributeId);

        if (attribute == nullptr)
            return false;

        Buffer *buffer = m_nodesManager->bufferManager()->lookupResource(attribute->bufferId());

        // Buffer update was already performed at this point
        // Just make sure the attribute reference a valid buffer
        if (buffer == nullptr)
            return false;

        // Index Attribute
        bool attributeWasDirty = false;
        if (attribute->attributeType() == QAttribute::IndexAttribute) {
            if ((attributeWasDirty = attribute->isDirty()) == true || forceUpdate)
                m_submissionContext->specifyIndices(buffer);
            // Vertex Attribute
        } else if (command->m_activeAttributes.contains(attribute->nameId())) {
            if ((attributeWasDirty = attribute->isDirty()) == true || forceUpdate) {
                // Find the location for the attribute
                const QVector<ShaderAttribute> shaderAttributes = shader->attributes();
                const ShaderAttribute *attributeDescription = nullptr;
                for (const ShaderAttribute &shaderAttribute : shaderAttributes) {
                    if (shaderAttribute.m_nameId == attribute->nameId()) {
                        attributeDescription = &shaderAttribute;
                        break;
                    }
                }
                if (!attributeDescription || attributeDescription->m_location < 0)
                    return false;
                m_submissionContext->specifyAttribute(attribute, buffer, attributeDescription);
            }
        }

        // Append attribute to temporary vector so that its dirtiness
        // can be cleared at the end of the frame
        if (attributeWasDirty)
            m_dirtyAttributes.push_back(attribute);

        // Note: We cannot call unsertDirty on the Attribute at this
        // point as we don't know if the attributes are being shared
        // with other geometry / geometryRenderer in which case they still
        // should remain dirty so that VAO for these commands are properly
        // updated
    }

    return true;
}

bool Renderer::requiresVAOAttributeUpdate(Geometry *geometry,
                                          const RenderCommand *command) const
{
    const auto attributeIds = geometry->attributes();

    for (QNodeId attributeId : attributeIds) {
        // TO DO: Improvement we could store handles and use the non locking policy on the attributeManager
        Attribute *attribute = m_nodesManager->attributeManager()->lookupResource(attributeId);

        if (attribute == nullptr)
            continue;

        if ((attribute->attributeType() == QAttribute::IndexAttribute && attribute->isDirty()) ||
                (command->m_activeAttributes.contains(attribute->nameId()) && attribute->isDirty()))
            return true;
    }
    return false;
}

// Erase graphics related resources that may become unused after a frame
void Renderer::cleanGraphicsResources()
{
    // Clean buffers
    const QVector<Qt3DCore::QNodeId> buffersToRelease = m_nodesManager->bufferManager()->takeBuffersToRelease();
    for (Qt3DCore::QNodeId bufferId : buffersToRelease)
        m_submissionContext->releaseBuffer(bufferId);

    // When Textures are cleaned up, their id is saved so that they can be
    // cleaned up in the render thread
    const QVector<Qt3DCore::QNodeId> cleanedUpTextureIds = std::move(m_textureIdsToCleanup);
    for (const Qt3DCore::QNodeId textureCleanedUpId: cleanedUpTextureIds)
        cleanupTexture(textureCleanedUpId);

    // Delete abandoned VAOs
    m_abandonedVaosMutex.lock();
    const QVector<HVao> abandonedVaos = std::move(m_abandonedVaos);
    m_abandonedVaosMutex.unlock();
    for (const HVao &vaoHandle : abandonedVaos) {
        // might have already been destroyed last frame, but added by the cleanup job before, so
        // check if the VAO is really still existent
        OpenGLVertexArrayObject *vao = m_glResourceManagers->vaoManager()->data(vaoHandle);
        if (vao) {
            vao->destroy();
            // We remove VAO from manager using its VAOIdentifier
            m_glResourceManagers->vaoManager()->release(vaoHandle);
        }
    }

    // Abandon GL shaders when a Shader node is destroyed Note: We are sure
    // that when this gets executed, all scene changes have been received and
    // shader nodes updated
    const QVector<Qt3DCore::QNodeId> cleanedUpShaderIds = m_nodesManager->shaderManager()->takeShaderIdsToCleanup();
    for (const Qt3DCore::QNodeId shaderCleanedUpId: cleanedUpShaderIds) {
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

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
