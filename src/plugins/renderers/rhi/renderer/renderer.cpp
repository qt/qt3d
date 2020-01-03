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
#include <Qt3DRender/private/pickeventfilter_p.h>
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
#include <Qt3DRender/private/filterentitybycomponentjob_p.h>

#include <Qt3DRender/qcameralens.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p_p.h>
#include <Qt3DRender/private/resourceaccessor_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/setfence_p.h>

#include <QtGui/private/qrhid3d11_p.h>
#include <QtGui/private/qrhimetal_p.h>
#include <QtGui/private/qrhivulkan_p.h>
#include <QtGui/private/qrhigles2_p.h>

#include <buffer_p.h>
#include <graphicscontext_p.h>
#include <rendercommand_p.h>
#include <renderqueue_p.h>
#include <renderview_p.h>
#include <texture_p.h>
#include <openglvertexarrayobject_p.h>
#include <renderviewbuilder_p.h>
#include <resourcemanagers_p.h>
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

#include <QtGui/private/qopenglcontext_p.h>
#include <Qt3DRender/private/frameprofiler_p.h>
#include <QGuiApplication>
#include <QShaderBaker>

QT_BEGIN_NAMESPACE

// Crashes on AMD Radeon drivers on Windows. Disable for now.
//#define SHADER_LOADING_IN_COMMAND_THREAD
using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {
namespace Rhi {

namespace {

class SyncLightsGatherer
{
public:
    explicit SyncLightsGatherer(LightGathererPtr gatherJob,
                                RendererCache *cache)
        : m_gatherJob(gatherJob)
        , m_cache(cache)
    {
    }

    void operator()()
    {
        QMutexLocker lock(m_cache->mutex());
        m_cache->gatheredLights = m_gatherJob->lights();
        m_cache->environmentLight = m_gatherJob->takeEnvironmentLight();
    }

private:
    LightGathererPtr m_gatherJob;
    RendererCache *m_cache;
};

class SyncRenderableEntities
{
public:
    explicit SyncRenderableEntities(RenderableEntityFilterPtr gatherJob,
                                    RendererCache *cache)
        : m_gatherJob(gatherJob)
        , m_cache(cache)
    {
    }

    void operator()()
    {
        QVector<Entity *> selectedEntities = m_gatherJob->filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        QMutexLocker lock(m_cache->mutex());
        m_cache->renderableEntities = selectedEntities;
    }

private:
    RenderableEntityFilterPtr m_gatherJob;
    RendererCache *m_cache;
};

class SyncComputableEntities
{
public:
    explicit SyncComputableEntities(ComputableEntityFilterPtr gatherJob,
                                    RendererCache *cache)
        : m_gatherJob(gatherJob)
        , m_cache(cache)
    {
    }

    void operator()()
    {
        QVector<Entity *> selectedEntities = m_gatherJob->filteredEntities();
        std::sort(selectedEntities.begin(), selectedEntities.end());

        QMutexLocker lock(m_cache->mutex());
        m_cache->computeEntities = selectedEntities;
    }

private:
    ComputableEntityFilterPtr m_gatherJob;
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
    , m_nodesManager(nullptr)
    , m_renderSceneRoot(nullptr)
    , m_defaultRenderStateSet(nullptr)
    , m_submissionContext(nullptr)
    , m_renderQueue(new RenderQueue())
    , m_renderThread(type == QRenderAspect::Threaded ? new RenderThread(this) : nullptr)
    , m_vsyncFrameAdvanceService(new VSyncFrameAdvanceService(m_renderThread != nullptr))
    , m_waitForInitializationToBeCompleted(0)
    , m_hasBeenInitializedMutex()
    , m_pickEventFilter(new PickEventFilter())
    , m_exposed(0)
    , m_lastFrameCorrect(0)
    , m_glContext(nullptr)
    , m_pickBoundingVolumeJob(PickBoundingVolumeJobPtr::create())
    , m_rayCastingJob(RayCastingJobPtr::create())
    , m_time(0)
    , m_settings(nullptr)
    , m_updateShaderDataTransformJob(Render::UpdateShaderDataTransformJobPtr::create())
    , m_cleanupJob(Render::FrameCleanupJobPtr::create())
    , m_worldTransformJob(Render::UpdateWorldTransformJobPtr::create())
    , m_expandBoundingVolumeJob(Render::ExpandBoundingVolumeJobPtr::create())
    , m_calculateBoundingVolumeJob(Render::CalculateBoundingVolumeJobPtr::create())
    , m_updateWorldBoundingVolumeJob(Render::UpdateWorldBoundingVolumeJobPtr::create())
    , m_updateTreeEnabledJob(Render::UpdateTreeEnabledJobPtr::create())
    , m_sendBufferCaptureJob(Render::SendBufferCaptureJobPtr::create())
    , m_updateSkinningPaletteJob(Render::UpdateSkinningPaletteJobPtr::create())
    , m_updateLevelOfDetailJob(Render::UpdateLevelOfDetailJobPtr::create())
    , m_updateMeshTriangleListJob(Render::UpdateMeshTriangleListJobPtr::create())
    , m_filterCompatibleTechniqueJob(FilterCompatibleTechniqueJobPtr::create())
    , m_updateEntityLayersJob(Render::UpdateEntityLayersJobPtr::create())
    , m_lightGathererJob(Render::LightGathererPtr::create())
    , m_renderableEntityFilterJob(Render::RenderableEntityFilterPtr::create())
    , m_computableEntityFilterJob(Render::ComputableEntityFilterPtr::create())
    , m_bufferGathererJob(SynchronizerJobPtr::create([this] { lookForDirtyBuffers(); }, JobTypes::DirtyBufferGathering))
    , m_vaoGathererJob(SynchronizerJobPtr::create([this] { lookForAbandonedVaos(); }, JobTypes::DirtyVaoGathering))
    , m_textureGathererJob(SynchronizerJobPtr::create([this] { lookForDirtyTextures(); }, JobTypes::DirtyTextureGathering))
    , m_sendSetFenceHandlesToFrontendJob(SynchronizerJobPtr::create([this] { sendSetFenceHandlesToFrontend(); }, JobTypes::SendSetFenceHandlesToFrontend))
    , m_introspectShaderJob(SynchronizerPostFramePtr::create([this] { reloadDirtyShaders(); },
                                                             [this] (Qt3DCore::QAspectManager *m) { sendShaderChangesToFrontend(m); },
                                                             JobTypes::DirtyShaderGathering))
    , m_syncLoadingJobs(SynchronizerJobPtr::create([] {}, JobTypes::SyncLoadingJobs))
    , m_cacheRenderableEntitiesJob(SynchronizerJobPtr::create(SyncRenderableEntities(m_renderableEntityFilterJob, &m_cache),
                                                              JobTypes::EntityComponentTypeFiltering))
    , m_cacheComputableEntitiesJob(SynchronizerJobPtr::create(SyncComputableEntities(m_computableEntityFilterJob, &m_cache),
                                                              JobTypes::EntityComponentTypeFiltering))
    , m_cacheLightsJob(SynchronizerJobPtr::create(SyncLightsGatherer(m_lightGathererJob, &m_cache),
                                                  JobTypes::EntityComponentTypeFiltering))
    , m_ownedContext(false)
    , m_offscreenHelper(nullptr)
    , m_RHIResourceManagers(nullptr)
    , m_commandExecuter(new Qt3DRender::Debug::CommandExecuter(this))
    , m_shouldSwapBuffers(true)
{
    // Set renderer as running - it will wait in the context of the
    // RenderThread for RenderViews to be submitted
    m_running.fetchAndStoreOrdered(1);
    if (m_renderThread)
        m_renderThread->waitForStart();

    // Create jobs to update transforms and bounding volumes
    // We can only update bounding volumes once all world transforms are known
    m_updateWorldBoundingVolumeJob->addDependency(m_worldTransformJob);
    m_updateWorldBoundingVolumeJob->addDependency(m_calculateBoundingVolumeJob);
    m_expandBoundingVolumeJob->addDependency(m_updateWorldBoundingVolumeJob);
    m_updateShaderDataTransformJob->addDependency(m_worldTransformJob);
    m_pickBoundingVolumeJob->addDependency(m_expandBoundingVolumeJob);
    m_rayCastingJob->addDependency(m_expandBoundingVolumeJob);
    // m_calculateBoundingVolumeJob's dependency on m_updateTreeEnabledJob is set in renderBinJobs

    // Ensures all skeletons are loaded before we try to update them
    m_updateSkinningPaletteJob->addDependency(m_syncLoadingJobs);

    // All world stuff depends on the RenderEntity's localBoundingVolume
    m_updateLevelOfDetailJob->addDependency(m_updateMeshTriangleListJob);
    m_pickBoundingVolumeJob->addDependency(m_updateMeshTriangleListJob);
    m_rayCastingJob->addDependency(m_updateMeshTriangleListJob);

    m_introspectShaderJob->addDependency(m_filterCompatibleTechniqueJob);

    m_cacheLightsJob->addDependency(m_lightGathererJob);
    m_cacheRenderableEntitiesJob->addDependency(m_renderableEntityFilterJob);
    m_cacheComputableEntitiesJob->addDependency(m_computableEntityFilterJob);

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

qint64 Renderer::time() const
{
    return m_time;
}

void Renderer::setTime(qint64 time)
{
    m_time = time;
}

void Renderer::setNodeManagers(NodeManagers *managers)
{
    m_nodesManager = managers;
    m_RHIResourceManagers = new RHIResourceManagers();
    m_scene2DResourceAccessor.reset(new ResourceAccessor(this, m_nodesManager));

    m_updateShaderDataTransformJob->setManagers(m_nodesManager);
    m_cleanupJob->setManagers(m_nodesManager);
    m_calculateBoundingVolumeJob->setManagers(m_nodesManager);
    m_expandBoundingVolumeJob->setManagers(m_nodesManager);
    m_worldTransformJob->setManagers(m_nodesManager);
    m_pickBoundingVolumeJob->setManagers(m_nodesManager);
    m_rayCastingJob->setManagers(m_nodesManager);
    m_updateWorldBoundingVolumeJob->setManager(m_nodesManager->renderNodesManager());
    m_updateLevelOfDetailJob->setManagers(m_nodesManager);
    m_updateSkinningPaletteJob->setManagers(m_nodesManager);
    m_updateMeshTriangleListJob->setManagers(m_nodesManager);
    m_filterCompatibleTechniqueJob->setManager(m_nodesManager->techniqueManager());
    m_updateEntityLayersJob->setManager(m_nodesManager);
    m_updateTreeEnabledJob->setManagers(m_nodesManager);
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

bool Renderer::accessOpenGLTexture(Qt3DCore::QNodeId nodeId,
                                   QOpenGLTexture **texture,
                                   QMutex **lock,
                                   bool readonly)
{
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

    RHITexture::TextureUpdateInfo texInfo = glTex->createOrUpdateGLTexture();
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


    // RHI initialization
    {
        qCDebug(Backend) << Q_FUNC_INFO << "Requesting renderer initialize";

        {
            auto windows = qApp->allWindows();
            Q_ASSERT(!windows.empty());
            auto window = windows.front();
//            m_r = QRhi::create(QRhi::Null, nullptr);

            switch(window->surfaceType())
            {
            case QSurface::VulkanSurface:
            {
                QRhiVulkanInitParams params;
                params.window = window;
                params.inst = window->vulkanInstance();
                m_r = QRhi::create(QRhi::Vulkan, &params, QRhi::EnableDebugMarkers);
                break;
            }
            case QSurface::OpenGLSurface:
            {
                m_fallbackSurface = QRhiGles2InitParams::newFallbackSurface();
                QRhiGles2InitParams params;
                params.window = window;
                params.format = this->m_format;
                params.fallbackSurface = m_fallbackSurface;
                m_r = QRhi::create(QRhi::OpenGLES2, &params, QRhi::EnableDebugMarkers);
                break;
            }
            case QSurface::MetalSurface:
            {
                QRhiMetalInitParams params;
                m_r = QRhi::create(QRhi::Metal, &params, QRhi::EnableDebugMarkers);
                break;
            }
            }
            Q_ASSERT(m_r);

            const int samples = this->format().samples();
            m_sc = m_r->newSwapChain();
            // allow depth-stencil, although we do not actually enable depth test/write for the triangle
            m_ds = m_r->newRenderBuffer(QRhiRenderBuffer::DepthStencil,
                                        QSize(), // no need to set the size yet
                                        samples,
                                        QRhiRenderBuffer::UsedWithSwapChainOnly);
            m_sc->setWindow(window);
            m_sc->setDepthStencil(m_ds);
            m_sc->setSampleCount(samples);
            m_sc->setFlags(QRhiSwapChain::Flags {});
            m_rp = m_sc->newCompatibleRenderPassDescriptor();
            m_sc->setRenderPassDescriptor(m_rp);
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
    //* if (context->thread() == QThread::currentThread() && context->makeCurrent(offscreenSurface)) {
    //*
    //*     // Clean up the graphics context and any resources
    //*     const QVector<HRHITexture> activeTexturesHandles = m_RHIResourceManagers->rhiTextureManager()->activeHandles();
    //*     for (const HRHITexture &textureHandle : activeTexturesHandles) {
    //*         RHITexture *tex = m_RHIResourceManagers->rhiTextureManager()->data(textureHandle);
    //*         tex->destroy();
    //*     }
    //*
    //*     // Do the same thing with buffers
    //*     const QVector<HRHIBuffer> activeBuffers = m_RHIResourceManagers->rhiBufferManager()->activeHandles();
    //*     for (const HRHIBuffer &bufferHandle : activeBuffers) {
    //*         RHIBuffer *buffer = m_RHIResourceManagers->rhiBufferManager()->data(bufferHandle);
    //*         buffer->destroy(m_submissionContext.data());
    //*     }
    //*
    //*     // Do the same thing with shaders
    //*     const QVector<RHIShader *> shaders = m_RHIResourceManagers->rhiShaderManager()->takeActiveResources();
    //*     qDeleteAll(shaders);
    //*
    //*     // Do the same thing with VAOs
    //*     const QVector<HVao> activeVaos = m_RHIResourceManagers->vaoManager()->activeHandles();
    //*     for (const HVao &vaoHandle : activeVaos) {
    //*         RHIVertexArrayObject *vao = m_RHIResourceManagers->vaoManager()->data(vaoHandle);
    //*         vao->destroy();
    //*     }
    //*
    //*     context->doneCurrent();
    //* } else {
    //*     qWarning() << "Failed to make context current: OpenGL resources will not be destroyed";
    //* }
    //*
    //* if (m_ownedContext)
    //*     delete context;

    m_submissionContext.reset(nullptr);

    // Free RHI resources
    {
        qCDebug(Backend) << Q_FUNC_INFO;
        delete m_rp;
        m_rp = nullptr;

        delete m_ds;
        m_ds = nullptr;

        delete m_sc;
        m_sc = nullptr;

        delete m_r;
        m_r = nullptr;

        // delete m_fallbackSurface;
        // m_fallbackSurface = nullptr;
    }

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
    m_worldTransformJob->setRoot(m_renderSceneRoot);
    m_expandBoundingVolumeJob->setRoot(m_renderSceneRoot);
    m_calculateBoundingVolumeJob->setRoot(m_renderSceneRoot);
    m_cleanupJob->setRoot(m_renderSceneRoot);
    m_pickBoundingVolumeJob->setRoot(m_renderSceneRoot);
    m_rayCastingJob->setRoot(m_renderSceneRoot);
    m_updateLevelOfDetailJob->setRoot(m_renderSceneRoot);
    m_updateSkinningPaletteJob->setRoot(m_renderSceneRoot);
    m_updateTreeEnabledJob->setRoot(m_renderSceneRoot);

    // Set all flags to dirty
    m_dirtyBits.marked |= AbstractRenderer::AllDirty;
}

void Renderer::registerEventFilter(QEventFilterService *service)
{
    qCDebug(Backend) << Q_FUNC_INFO << QThread::currentThread();
    service->registerEventFilter(m_pickEventFilter.data(), 1024);
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

QShader getShader(const QString &name)
{
    QFile f(name);
    if (f.open(QIODevice::ReadOnly))
        return QShader::fromSerialized(f.readAll());

    return QShader();
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
        const QVector<Render::Rhi::RenderView *> renderViews = m_renderQueue->nextFrameQueue();
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

                    const QSize outputSize = m_sc->surfacePixelSize();

                    m_ds->setPixelSize(outputSize);
                    m_ds->build(); // == m_ds->release(); m_ds->build();

                    m_hasSwapChain = m_sc->buildOrResize();
                    m_submissionContext->m_rhi = this->m_r;
                    m_submissionContext->m_sc = this->m_sc;
                    m_submissionContext->m_currentUpdates = this->m_r->nextResourceUpdateBatch();
                    beganDrawing = m_submissionContext->beginDrawing(surface);
                    if (beganDrawing) {
                        // 1) Execute commands for buffer uploads, texture updates, shader loading first
                        updateResources();
                        // 2) Update VAO and copy data into commands to allow concurrent submission
                        prepareCommandsSubmission(renderViews);
                        preprocessingComplete = true;

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
void Renderer::setupDrawCommand(RenderCommand& cmd)
{
    // Create UBOs
    auto standard_ubuf = m_r->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(float) * 16);
    standard_ubuf->build();
    QMatrix4x4 ident;
    this->m_submissionContext->m_currentUpdates->updateDynamicBuffer(standard_ubuf, 0, sizeof(float) * 16, ident.data());

    auto custom_ubuf = m_r->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(float) * 1);
    custom_ubuf->build();
    static float f{0.5};
    this->m_submissionContext->m_currentUpdates->updateDynamicBuffer(custom_ubuf, 0, sizeof(float), &f);


    bool ok = true;

    cmd.shaderResourceBindings = m_r->newShaderResourceBindings();
    assert(cmd.shaderResourceBindings);

    cmd.shaderResourceBindings->setBindings({
        QRhiShaderResourceBinding::uniformBuffer(
                           0,
                           QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage,
                           standard_ubuf),
        QRhiShaderResourceBinding::uniformBuffer(
                           1,
                           QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage,
                           custom_ubuf),
    });
    ok = cmd.shaderResourceBindings->build();
    assert(ok);

    // Create pipeline
    cmd.graphicsPipeline = m_r->newGraphicsPipeline();
    assert(cmd.graphicsPipeline);

    cmd.graphicsPipeline->setShaderStages({
        { QRhiShaderStage::Vertex, cmd.m_glShader->shaderStage(QShader::VertexStage) },
        { QRhiShaderStage::Fragment, cmd.m_glShader->shaderStage(QShader::FragmentStage) }
    });

    QRhiVertexInputLayout inputLayout = cmd.m_glShader->inputLayout();
    QVarLengthArray<QRhiVertexInputBinding, 8> inputBindings;

    const auto geom = cmd.m_geometry;
    const auto& attributes = geom->attributes();
    for(Qt3DCore::QNodeId attribute_id : attributes)
    {
        Attribute* attrib = m_nodesManager->attributeManager()->lookupResource(attribute_id);
        if(attrib->attributeType() == QAttribute::VertexAttribute)
        {
            inputBindings.resize(std::max((std::size_t)inputBindings.size(), (std::size_t)attrib->location() + 1));
            // TODO handle the other arguments to QRhiVertexInputBinding
            inputBindings[attrib->location()] = QRhiVertexInputBinding{attrib->byteStride()};
        }
    }

    inputLayout.setBindings(inputBindings.begin(), inputBindings.end());

    cmd.graphicsPipeline->setVertexInputLayout(inputLayout);
    cmd.graphicsPipeline->setShaderResourceBindings(cmd.shaderResourceBindings);
    cmd.graphicsPipeline->setRenderPassDescriptor(m_rp);

    ok = cmd.graphicsPipeline->build();
    assert(ok);
}

void Renderer::setupComputeCommand(RenderCommand &command)
{

}

// When this function is called, we must not be processing the commands for frame n+1
void Renderer::prepareCommandsSubmission(const QVector<RenderView *> &renderViews)
{
    RHIVertexArrayObject *vao = nullptr;
    QHash<HVao, bool> updatedTable;

    for (RenderView *rv: renderViews) {
        QVector<RenderCommand> &commands = rv->commands();
        for (RenderCommand &command : commands) {
            // Update/Create VAO
            if (command.m_type == RenderCommand::Draw) {
                Geometry *rGeometry = m_nodesManager->data<Geometry, GeometryManager>(command.m_geometry);
                GeometryRenderer *rGeometryRenderer = m_nodesManager->data<GeometryRenderer, GeometryRendererManager>(command.m_geometryRenderer);
                RHIShader *shader = command.m_glShader;
                Q_ASSERT(shader);

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
                        Profiling::GLTimeRecorder recorder(Profiling::VAOUpload);
                        // Activate shader
                        // m_submissionContext->activateShader(shader);
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

                // Prepare the ShaderParameterPack based on the active uniforms of the shader
                shader->prepareUniforms(command.m_parameterPack);

                setupDrawCommand(command);

            } else if (command.m_type == RenderCommand::Compute) {
                RHI_UNIMPLEMENTED;
                RHIShader *shader = command.m_glShader;
                Q_ASSERT(shader);

                // Prepare the ShaderParameterPack based on the active uniforms of the shader
                shader->prepareUniforms(command.m_parameterPack);
            }
        }
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
    const QVector<HVao> activeVaos = m_RHIResourceManagers->vaoManager()->activeHandles();
    for (HVao handle : activeVaos) {
        RHIVertexArrayObject *vao = m_RHIResourceManagers->vaoManager()->data(handle);

        // Make sure to only mark VAOs for deletion that were already created
        // (ignore those that might be currently under construction in the render thread)
        if (vao && vao->isAbandoned(m_nodesManager->geometryManager(), m_RHIResourceManagers->rhiShaderManager())) {
            m_abandonedVaosMutex.lock();
            m_abandonedVaos.push_back(handle);
            m_abandonedVaosMutex.unlock();
        }
    }
}

// Executed in a job
void Renderer::lookForDirtyBuffers()
{
    const QVector<HBuffer> activeBufferHandles = m_nodesManager->bufferManager()->activeHandles();
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
    const QVector<HBuffer> activeBufferHandles = m_nodesManager->bufferManager()->activeHandles();
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
    const QVector<HTextureImage> activeTextureImageHandles = imageManager->activeHandles();
    Qt3DCore::QNodeIdVector dirtyImageIds;
    for (const HTextureImage &handle: activeTextureImageHandles) {
        TextureImage *image = imageManager->data(handle);
        if (image->isDirty()) {
            dirtyImageIds.push_back(image->peerId());
            image->unsetDirty();
        }
    }

    TextureManager *textureManager = m_nodesManager->textureManager();
    const QVector<HTexture> activeTextureHandles = textureManager->activeHandles();
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
    const QVector<HTechnique> activeTechniques = m_nodesManager->techniqueManager()->activeHandles();
    const QVector<HShaderBuilder> activeBuilders = m_nodesManager->shaderBuilderManager()->activeHandles();
    for (const HTechnique &techniqueHandle : activeTechniques) {
        Technique *technique = m_nodesManager->techniqueManager()->data(techniqueHandle);
        // If api of the renderer matches the one from the technique
        if (technique->isCompatibleWithRenderer()) {
            const auto passIds = technique->renderPasses();
            for (const QNodeId &passId : passIds) {
                RenderPass *renderPass = m_nodesManager->renderPassManager()->lookupResource(passId);
                HShader shaderHandle = m_nodesManager->shaderManager()->lookupHandle(renderPass->shaderProgram());
                Shader *shader = m_nodesManager->shaderManager()->data(shaderHandle);

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
    const QVector<HShader> activeShaders = m_nodesManager->shaderManager()->activeHandles();
    for (const HShader &handle :activeShaders) {
        Shader *s = m_nodesManager->shaderManager()->data(handle);
        if (s->requiresFrontendSync()) {
            QShaderProgram *frontend = static_cast<decltype(frontend)>(manager->lookupNode(s->peerId()));
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
    const QVector<QPair<Qt3DCore::QNodeId, RHIFence>> updatedSetFence = std::move(m_updatedSetFences);
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
    const QVector<HComputeCommand> activeCommands = m_nodesManager->computeJobManager()->activeHandles();
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
void Renderer::updateResources()
{
    {
        // Update active fence objects:
        // - Destroy fences that have reached their signaled state
        RHIFenceManager *fenceManager = m_RHIResourceManagers->rhiFenceManager();
        const auto end = fenceManager->end();
        auto it = fenceManager->begin();
        while (it != end) {
            const RHIFence fence = it.value();
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
        Profiling::GLTimeRecorder recorder(Profiling::BufferUpload);
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
        Profiling::GLTimeRecorder recorder(Profiling::ShaderUpload);
        const QVector<HShader> dirtyShaderHandles = std::move(m_dirtyShaders);
        ShaderManager *shaderManager = m_nodesManager->shaderManager();
        for (const HShader &handle: dirtyShaderHandles) {
            Shader *shader = shaderManager->data(handle);

            // Can be null when using Scene3D rendering
            if (shader == nullptr)
                continue;

            // Compile shader
            m_submissionContext->loadShader(shader, shaderManager, m_RHIResourceManagers->rhiShaderManager());
        }
    }
#endif

    {
        Profiling::GLTimeRecorder recorder(Profiling::TextureUpload);
        const QVector<HTexture> activeTextureHandles = std::move(m_dirtyTextures);
        for (const HTexture &handle: activeTextureHandles) {
            RHI_UNIMPLEMENTED;
            Texture *texture = m_nodesManager->textureManager()->data(handle);

            // Can be null when using Scene3D rendering
            if (texture ==  nullptr)
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
            const QVector<HRHITexture> glTextureHandles = rhiTextureManager->activeHandles();
            // Upload texture data
            for (const HRHITexture &glTextureHandle : glTextureHandles) {
                RHI_UNIMPLEMENTED;
                RHITexture *glTexture = rhiTextureManager->data(glTextureHandle);

                // We create/update the actual GL texture using the GL context at this point
                const RHITexture::TextureUpdateInfo info = glTexture->createOrUpdateGLTexture();

                // RHITexture creation provides us width/height/format ... information
                // for textures which had not initially specified these information (TargetAutomatic...)
                // Gather these information and store them to be distributed by a change next frame
                const QNodeIdVector referenceTextureIds = { rhiTextureManager->texNodeIdForRHITexture.value(glTexture) };
                // Store properties and referenceTextureIds
                if (info.wasUpdated) {
                    Texture::TextureUpdateInfo updateInfo;
                    updateInfo.properties = info.properties;
                    updateInfo.handleType = QAbstractTexture::OpenGLTextureId;
                    updateInfo.handle = info.texture ? QVariant(info.texture->textureId()) : QVariant();
                    m_updatedTextureProperties.push_back({updateInfo, referenceTextureIds});
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
    RHITexture *glTexture = rhiTextureManager->lookupResource(texture->peerId());

    // No RHITexture associated yet -> create it
    if (glTexture == nullptr) {
        glTexture = rhiTextureManager->getOrCreateResource(texture->peerId());
        rhiTextureManager->texNodeIdForRHITexture.insert(glTexture, texture->peerId());
    }

    // Update RHITexture to match Texture instance
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
        QVector<RHITexture::Image> images;
        images.reserve(textureImageIds.size());
        // TODO: Move this into RHITexture directly
        for (const QNodeId textureImageId : textureImageIds) {
            const TextureImage *img = m_nodesManager->textureImageManager()->lookupResource(textureImageId);
            if (img == nullptr) {
                qWarning() << Q_FUNC_INFO << "invalid TextureImage handle";
            } else {
                RHITexture::Image glImg {img->dataGenerator(), img->layer(), img->mipLevel(), img->face()};
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

    for (int i = 0; i < renderViewsCount; ++i) {
        // Initialize GraphicsContext for drawing
        // If the RenderView has a RenderStateSet defined
        const RenderView *renderView = renderViews.at(i);

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
        RHIFenceManager *fenceManager = m_RHIResourceManagers->rhiFenceManager();
        for (const Qt3DCore::QNodeId insertFenceId : insertFenceIds) {
            RHI_UNIMPLEMENTED;
            // If the fence is not in the manager, then it hasn't been inserted
            // into the command stream yet.
            if (fenceManager->find(insertFenceId) == fenceManager->end()) {
                // Insert fence into command stream
                RHIFence glFence = m_submissionContext->fenceSync();
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
            RHI_UNIMPLEMENTED;
            // TO DO
            if (waitFence.handleType != QWaitFence::OpenGLFenceId) {
                qWarning() << "WaitFence handleType should be OpenGLFenceId when using the Qt 3D OpenGL renderer";
                continue;
            }
            RHIFence fence = reinterpret_cast<RHIFence>(waitFence.handle.value<qintptr>());
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
            Profiling::GLTimeRecorder recorder(Profiling::StateUpdate);
            // Set the RV state if not null,
            if (renderViewStateSet != nullptr)
                m_submissionContext->setCurrentStateSet(renderViewStateSet);
            else
                m_submissionContext->setCurrentStateSet(m_defaultRenderStateSet);
        }

        // Set RenderTarget ...
        // Activate RenderTarget
        {
            Profiling::GLTimeRecorder recorder(Profiling::RenderTargetUpdate);
            m_submissionContext->activateRenderTarget(renderView->renderTargetId(),
                                                    renderView->attachmentPack(),
                                                    lastBoundFBOId);
        }


        {
            Profiling::GLTimeRecorder recorder(Profiling::ClearBuffer);
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
            RHI_UNIMPLEMENTED;
//*            const QRenderCaptureRequest request = renderView->renderCaptureRequest();
//*            const QSize size = m_submissionContext->renderTargetSize(renderView->surfaceSize());
//*            QRect rect(QPoint(0, 0), size);
//*            if (!request.rect.isEmpty())
//*                rect = rect.intersected(request.rect);
//*            QImage image;
//*            if (!rect.isEmpty()) {
//*                // Bind fbo as read framebuffer
//*                m_submissionContext->bindFramebuffer(m_submissionContext->activeFBO(), GraphicsHelperInterface::FBORead);
//*                image = m_submissionContext->readFramebuffer(rect);
//*            } else {
//*                qWarning() << "Requested capture rectangle is outside framebuffer";
//*            }
//*            Render::RenderCapture *renderCapture =
//*                    static_cast<Render::RenderCapture*>(m_nodesManager->frameGraphManager()->lookupNode(renderView->renderCaptureNodeId()));
//*            renderCapture->addRenderCapture(request.captureId, image);
//*            if (!m_pendingRenderCaptureSendRequests.contains(renderView->renderCaptureNodeId()))
//*                m_pendingRenderCaptureSendRequests.push_back(renderView->renderCaptureNodeId());
        }

        if (renderView->isDownloadBuffersEnable())
        {
            RHI_UNIMPLEMENTED;
//*            downloadGLBuffers();
        }

        // Perform BlitFramebuffer operations
        if (renderView->hasBlitFramebufferInfo()) {
            RHI_UNIMPLEMENTED;
//*            const auto &blitFramebufferInfo = renderView->blitFrameBufferInfo();
//*            const QNodeId inputTargetId = blitFramebufferInfo.sourceRenderTargetId;
//*            const QNodeId outputTargetId = blitFramebufferInfo.destinationRenderTargetId;
//*            const QRect inputRect = blitFramebufferInfo.sourceRect;
//*            const QRect outputRect = blitFramebufferInfo.destinationRect;
//*            const QRenderTargetOutput::AttachmentPoint inputAttachmentPoint = blitFramebufferInfo.sourceAttachmentPoint;
//*            const QRenderTargetOutput::AttachmentPoint outputAttachmentPoint = blitFramebufferInfo.destinationAttachmentPoint;
//*            const QBlitFramebuffer::InterpolationMethod interpolationMethod = blitFramebufferInfo.interpolationMethod;
//*            m_submissionContext->blitFramebuffer(inputTargetId, outputTargetId, inputRect, outputRect, lastBoundFBOId,
//*                                                 inputAttachmentPoint, outputAttachmentPoint,
//*                                                 interpolationMethod);
        }


        frameElapsed = timer.elapsed() - frameElapsed;
        qCDebug(Rendering) << Q_FUNC_INFO << "Submitted Renderview " << i + 1 << "/" << renderViewsCount  << "in " << frameElapsed << "ms";
        frameElapsed = timer.elapsed();
    }

    // Bind lastBoundFBOId back. Needed also in threaded mode.
    // lastBoundFBOId != m_graphicsContext->activeFBO() when the last FrameGraph leaf node/renderView
    // contains RenderTargetSelector/RenderTarget
    if (lastBoundFBOId != m_submissionContext->activeFBO())
    {
        RHI_UNIMPLEMENTED;
//*         m_submissionContext->bindFramebuffer(lastBoundFBOId, GraphicsHelperInterface::FBOReadAndDraw);
    }

    // Reset state and call doneCurrent if the surface
    // is valid and was actually activated
    if (lastUsedSurface) {
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
    return (m_settings->renderPolicy() == QRenderSettings::Always
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

// Jobs we may have to run even if no rendering will happen
QVector<QAspectJobPtr> Renderer::preRenderingJobs()
{
    QVector<QAspectJobPtr> jobs;

    // Do we need to notify frontend about fence change?
    if (m_updatedSetFences.size() > 0)
        jobs.push_back(m_sendSetFenceHandlesToFrontendJob);

    if (m_sendBufferCaptureJob->hasRequests())
        jobs.push_back(m_sendBufferCaptureJob);

    jobs.append(pickBoundingVolumeJob());
    jobs.append(rayCastingJob());

    return jobs;
}

// Waits to be told to create jobs for the next frame
// Called by QRenderAspect jobsToExecute context of QAspectThread
// Returns all the jobs (and with proper dependency chain) required
// for the rendering of the scene
QVector<Qt3DCore::QAspectJobPtr> Renderer::renderBinJobs()
{
    QVector<QAspectJobPtr> renderBinJobs;
    // Create the jobs to build the frame
    const QVector<QAspectJobPtr> bufferJobs = createRenderBufferJobs();

    // Remove previous dependencies
    m_calculateBoundingVolumeJob->removeDependency(QWeakPointer<QAspectJob>());
    m_cleanupJob->removeDependency(QWeakPointer<QAspectJob>());

    // Set dependencies
    for (const QAspectJobPtr &bufferJob : bufferJobs)
        m_calculateBoundingVolumeJob->addDependency(bufferJob);

    m_updateLevelOfDetailJob->setFrameGraphRoot(frameGraphRoot());

    const BackendNodeDirtySet dirtyBitsForFrame = m_dirtyBits.marked | m_dirtyBits.remaining;
    m_dirtyBits.marked = {};
    m_dirtyBits.remaining = {};
    BackendNodeDirtySet notCleared = {};

    // Add jobs
    const bool entitiesEnabledDirty = dirtyBitsForFrame & AbstractRenderer::EntityEnabledDirty;
    if (entitiesEnabledDirty) {
        renderBinJobs.push_back(m_updateTreeEnabledJob);
        // This dependency is added here because we clear all dependencies
        // at the start of this function.
        m_calculateBoundingVolumeJob->addDependency(m_updateTreeEnabledJob);
    }

    if (dirtyBitsForFrame & AbstractRenderer::TransformDirty) {
        renderBinJobs.push_back(m_worldTransformJob);
        renderBinJobs.push_back(m_updateWorldBoundingVolumeJob);
        renderBinJobs.push_back(m_updateShaderDataTransformJob);
    }

    if (dirtyBitsForFrame & AbstractRenderer::GeometryDirty ||
        dirtyBitsForFrame & AbstractRenderer::BuffersDirty) {
        renderBinJobs.push_back(m_calculateBoundingVolumeJob);
        renderBinJobs.push_back(m_updateMeshTriangleListJob);
    }

    if (dirtyBitsForFrame & AbstractRenderer::GeometryDirty ||
        dirtyBitsForFrame & AbstractRenderer::TransformDirty) {
        renderBinJobs.push_back(m_expandBoundingVolumeJob);
    }

    // TO DO: Conditionally add if skeletons dirty
    renderBinJobs.push_back(m_syncLoadingJobs);
    m_updateSkinningPaletteJob->setDirtyJoints(m_nodesManager->jointManager()->dirtyJoints());
    renderBinJobs.push_back(m_updateSkinningPaletteJob);
    renderBinJobs.push_back(m_updateLevelOfDetailJob);
    renderBinJobs.push_back(m_cleanupJob);
    renderBinJobs.append(bufferJobs);

    // Jobs to prepare GL Resource upload
    renderBinJobs.push_back(m_vaoGathererJob);

    if (dirtyBitsForFrame & AbstractRenderer::BuffersDirty)
        renderBinJobs.push_back(m_bufferGathererJob);

    if (dirtyBitsForFrame & AbstractRenderer::TexturesDirty)
        renderBinJobs.push_back(m_textureGathererJob);


    // Layer cache is dependent on layers, layer filters (hence FG structure
    // changes) and the enabled flag on entities
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

    // Rebuild Entity Layers list if layers are dirty
    if (layersDirty)
        renderBinJobs.push_back(m_updateEntityLayersJob);

    if (renderableDirty) {
        renderBinJobs.push_back(m_renderableEntityFilterJob);
        renderBinJobs.push_back(m_cacheRenderableEntitiesJob);
    }

    if (computeableDirty) {
        renderBinJobs.push_back(m_computableEntityFilterJob);
        renderBinJobs.push_back(m_cacheComputableEntitiesJob);
    }

    if (lightsDirty) {
        renderBinJobs.push_back(m_lightGathererJob);
        renderBinJobs.push_back(m_cacheLightsJob);
    }

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
            builder.setMaterialGathererCacheNeedsToBeRebuilt(materialCacheNeedsToBeRebuilt || isNewRV);
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

QAspectJobPtr Renderer::pickBoundingVolumeJob()
{
    // Set values on pickBoundingVolumeJob
    RenderSettings *renderSetting = settings();
    if (renderSetting != nullptr) {
        m_pickBoundingVolumeJob->setRenderSettings(renderSetting);
        m_pickBoundingVolumeJob->setFrameGraphRoot(frameGraphRoot());
        m_pickBoundingVolumeJob->setMouseEvents(pendingPickingEvents());
        m_pickBoundingVolumeJob->setKeyEvents(pendingKeyEvents());
    }

    return m_pickBoundingVolumeJob;
}

QAspectJobPtr Renderer::rayCastingJob()
{
    // Set values on rayCastingJob
    RenderSettings *renderSetting = settings();
    if (renderSetting != nullptr) {
        m_rayCastingJob->setRenderSettings(renderSetting);
        m_rayCastingJob->setFrameGraphRoot(frameGraphRoot());
    }

    return m_rayCastingJob;
}

QAspectJobPtr Renderer::syncLoadingJobs()
{
    return m_syncLoadingJobs;
}

QAspectJobPtr Renderer::expandBoundingVolumeJob()
{
    return m_expandBoundingVolumeJob;
}

QAbstractFrameAdvanceService *Renderer::frameAdvanceService() const
{
    return static_cast<Qt3DCore::QAbstractFrameAdvanceService *>(m_vsyncFrameAdvanceService.data());
}

// Called by executeCommands
void Renderer::performDraw(RenderCommand *command)
{
    QRhiCommandBuffer *cb = m_sc->currentFrameCommandBuffer();
    // Indirect Draw Calls
    if (command->m_drawIndirect) {
        RHI_UNIMPLEMENTED;
        //* // Bind the indirect draw buffer
        //* Buffer *indirectDrawBuffer = m_nodesManager->bufferManager()->data(command->m_indirectDrawBuffer);
        //* if (Q_UNLIKELY(indirectDrawBuffer == nullptr)) {
        //*     qWarning() << "Invalid Indirect Draw Buffer - failed to retrieve Buffer";
        //*     return;
        //* }
        //*
        //* // Get RHIBuffer from Buffer;
        //* RHIBuffer *indirectDrawGLBuffer = m_submissionContext->glBufferForRenderBuffer(indirectDrawBuffer);
        //* if (Q_UNLIKELY(indirectDrawGLBuffer == nullptr)) {
        //*     qWarning() << "Invalid Indirect Draw Buffer - failed to retrieve RHIBuffer";
        //*     return;
        //* }
        //*
        //* // Bind RHIBuffer
        //* const bool successfullyBound = indirectDrawGLBuffer->bind(m_submissionContext.data(), RHIBuffer::DrawIndirectBuffer);
        //*
        //* if (Q_LIKELY(successfullyBound)) {
        //*     // TO DO: Handle multi draw variants if attribute count > 1
        //*     if (command->m_drawIndexed) {
        //*         m_submissionContext->drawElementsIndirect(command->m_primitiveType,
        //*                                                 command->m_indexAttributeDataType,
        //*                                                 reinterpret_cast<void*>(quintptr(command->m_indirectAttributeByteOffset)));
        //*     } else {
        //*         m_submissionContext->drawArraysIndirect(command->m_primitiveType,
        //*                                               reinterpret_cast<void*>(quintptr(command->m_indirectAttributeByteOffset)));
        //*     }
        //* } else {
        //*     qWarning() << "Failed to bind IndirectDrawBuffer";
        //* }

    } else { // Direct Draw Calls

        // TO DO: Add glMulti Draw variants
        if (command->m_primitiveType == QGeometryRenderer::Patches)
        {
            RHI_UNIMPLEMENTED;
            //* m_submissionContext->setVerticesPerPatch(command->m_verticesPerPatch);
        }

        if (command->m_primitiveRestartEnabled)
        {
            RHI_UNIMPLEMENTED;
            //* m_submissionContext->enablePrimitiveRestart(command->m_restartIndexValue);
        }

        // TO DO: Add glMulti Draw variants
        if (command->m_drawIndexed) {
            Profiling::GLTimeRecorder recorder(Profiling::DrawElement);
            cb->drawIndexed(command->m_primitiveCount,
                            command->m_instanceCount,
                            command->m_indexOffset,
                            command->m_indexAttributeByteOffset,
                            command->m_firstInstance);
        } else {
            Profiling::GLTimeRecorder recorder(Profiling::DrawArray);
            cb->draw(command->m_primitiveCount,
                     command->m_instanceCount,
                     command->m_firstVertex,
                     command->m_firstInstance);
        }
    }

#if defined(QT3D_RENDER_ASPECT_RHI_DEBUG)
    int err = m_submissionContext->openGLContext()->functions()->glGetError();
    if (err)
        qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);
#endif

    if (command->m_primitiveRestartEnabled)
        m_submissionContext->disablePrimitiveRestart();
}

void Renderer::performCompute(const RenderView *, RenderCommand *command)
{
    RHI_UNIMPLEMENTED;
//* {
//*     Profiling::GLTimeRecorder recorder(Profiling::ShaderUpdate);
//*     RHIShader *shader = m_RHIResourceManagers->rhiShaderManager()->lookupResource(command->m_shaderId);
//*     m_submissionContext->activateShader(shader);
//* }
//* {
//*     Profiling::GLTimeRecorder recorder(Profiling::UniformUpdate);
//*     m_submissionContext->setParameters(command->m_parameterPack);
//* }
//* {
//*     Profiling::GLTimeRecorder recorder(Profiling::DispatchCompute);
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

void Renderer::createOrUpdateVAO(RenderCommand *command,
                                 HVao *previousVaoHandle,
                                 RHIVertexArrayObject **vao)
{
    const VAOIdentifier vaoKey(command->m_geometry, command->m_shaderId);

    VAOManager *vaoManager = m_RHIResourceManagers->vaoManager();
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

static auto rhiIndexFormat(QAttribute::VertexBaseType type)
{
    switch(type) {
    case QAttribute::VertexBaseType ::UnsignedShort: return QRhiCommandBuffer::IndexUInt16;
    case QAttribute::VertexBaseType ::UnsignedInt: return QRhiCommandBuffer::IndexUInt32;
    default: std::abort();
    }
}

bool Renderer::prepareDraw(QRhiCommandBuffer *cb, const RenderView *rv, RenderCommand& command)
{
    // Create the vertex input description
    const auto geom = command.m_geometry;
    const auto& attributes = geom->attributes();
    for(Qt3DCore::QNodeId attribute_id : attributes)
    {
        // TODO isn't there a more efficient way than doing three hash lookups ?
        Attribute* attrib = m_nodesManager->attributeManager()->lookupResource(attribute_id);
        Buffer *buffer = m_nodesManager->bufferManager()->lookupResource(attrib->bufferId());
        RHIBuffer* hbuf = m_RHIResourceManagers->rhiBufferManager()->lookupResource(buffer->peerId());
        switch(attrib->attributeType())
        {
        case QAttribute::VertexAttribute:
            hbuf->bind(&*m_submissionContext, RHIBuffer::Type::ArrayBuffer);
            assert(hbuf->rhiBuffer());
            command.vertex_input.resize(std::max((std::size_t)command.vertex_input.size(), (std::size_t)attrib->location() + 1));
            command.vertex_input[attrib->location()] = {hbuf->rhiBuffer(), attrib->byteOffset()};
            break;
        case QAttribute::IndexAttribute:
            hbuf->bind(&*m_submissionContext, RHIBuffer::Type::IndexBuffer);
            assert(hbuf->rhiBuffer());
            assert(command.indexBuffer == nullptr);

            command.indexBuffer = hbuf->rhiBuffer();
            command.indexAttribute = attrib;
            break;
        case QAttribute::DrawIndirectAttribute:
            RHI_UNIMPLEMENTED;
            break;
        }
    }

    for(const BlockToUBO& pack : command.m_parameterPack.uniformBuffers())
    {
        qDebug() << pack.m_bufferID;
        Buffer *cpuBuffer = nodeManagers()->bufferManager()->lookupResource(pack.m_bufferID);
        RHIBuffer *ubo = m_submissionContext->glBufferForRenderBuffer(cpuBuffer);
        ubo->bind(&*m_submissionContext, RHIBuffer::UniformBuffer);
    }
    return true;
}

bool Renderer::performDraw(QRhiCommandBuffer *cb, const RenderView *rv, RenderCommand& command)
{
    // Setup the rendering pass
    cb->setGraphicsPipeline(command.graphicsPipeline);
    cb->setShaderResources(command.graphicsPipeline->shaderResourceBindings());

    // Send the draw command
    if (Q_UNLIKELY(!command.indexBuffer))
    {
        Profiling::GLTimeRecorder recorder(Profiling::DrawArray);
        cb->setVertexInput(0, command.vertex_input.size(), command.vertex_input.data());
        cb->draw(command.m_primitiveCount,
                 command.m_instanceCount,
                 command.m_firstVertex,
                 command.m_firstInstance);
    }
    else
    {
        Profiling::GLTimeRecorder recorder(Profiling::DrawElement);
        auto indexFormat = rhiIndexFormat(command.indexAttribute->vertexBaseType());
        auto indexOffset = command.indexAttribute->byteOffset();
        cb->setVertexInput(0, command.vertex_input.size(), command.vertex_input.data(), command.indexBuffer, indexOffset, indexFormat);
        cb->drawIndexed(command.m_primitiveCount,
                        command.m_instanceCount,
                        command.m_indexOffset,
                        command.m_indexAttributeByteOffset,
                        command.m_firstInstance);
    }

    return true;
}
// Called by RenderView->submit() in RenderThread context
// Returns true, if all RenderCommands were sent to the GPU
bool Renderer::executeCommandsSubmission(const RenderView *rv)
{
    bool allCommandsIssued = true;

    // Render drawing commands
    QVector<RenderCommand> commands = rv->commands();

    // Submit the commands to the underlying graphics API (RHI)

    // Save the RenderView base stateset
    RenderStateSet *globalState = m_submissionContext->currentStateSet();

    QRhiCommandBuffer *cb = m_sc->currentFrameCommandBuffer();

    // Upload all the required data to rhi...
    for (RenderCommand &command : commands) {
        if (command.m_type == RenderCommand::Draw) {
            prepareDraw(cb, rv, command);
            qDebug() << command.vertex_input.size() << command.vertex_input[0];
        }
    }

    // Perform compute actions
    cb->beginComputePass(m_submissionContext->m_currentUpdates);
    for (RenderCommand &command : commands) {
        if (command.m_type == RenderCommand::Compute) {
            performCompute(rv, &command);
        }
    }
    cb->endComputePass();
    m_submissionContext->m_currentUpdates = m_r->nextResourceUpdateBatch();

    // Draw the commands
    QRhiRenderTarget *rt = m_sc->currentFrameRenderTarget();

    const QColor clearColor = [=] {
        auto col = rv->globalClearColorBufferInfo().clearColor;
        return QColor::fromRgbF(col.x(), col.y(), col.z(), col.w());
    }();

    const QRhiDepthStencilClearValue clearDepthStencil{ rv->clearDepthValue(), (quint32)rv->clearStencilValue() };

    cb->beginPass(rt, clearColor, clearDepthStencil, m_submissionContext->m_currentUpdates);
    {
        const float x = rv->viewport().x() * rv->surfaceSize().width();
        const float y = (1. - rv->viewport().y() - rv->viewport().height()) * rv->surfaceSize().height();
        const float w = rv->viewport().width() * rv->surfaceSize().width();
        const float h = rv->viewport().height() * rv->surfaceSize().height();

        cb->setViewport(QRhiViewport{x, y, w, h});
    }

    for (RenderCommand &command : commands) {
        if (command.m_type == RenderCommand::Draw) {
            performDraw(cb, rv, command);
        }
    }
    cb->endPass();
    m_submissionContext->m_currentUpdates = m_r->nextResourceUpdateBatch();
//*    for (RenderCommand &command : commands) {
//*
//*            // Check if we have a valid command that can be drawn
//*            if (!command.m_isValid) {
//*                allCommandsIssued = false;
//*                continue;
//*            }
//*
//*            vao = m_RHIResourceManagers->vaoManager()->data(command.m_vao);
//*
//*            // something may have went wrong when initializing the VAO
//*            if (!vao->isSpecified()) {
//*                allCommandsIssued = false;
//*                continue;
//*            }
//*
//*            {
//*                Profiling::GLTimeRecorder recorder(Profiling::ShaderUpdate);
//*                //// We activate the shader here
//*                //* RHIShader *shader = command.m_glShader;
//*                //* if (!m_submissionContext->activateShader(shader)) {
//*                //*     allCommandsIssued = false;
//*                //*     continue;
//*                //* }
//*            }
//*
//*            {
//*                Profiling::GLTimeRecorder recorder(Profiling::VAOUpdate);
//*                // Bind VAO
//*                vao->bind();
//*            }
//*
//*            {
//*                Profiling::GLTimeRecorder recorder(Profiling::UniformUpdate);
//*                //// Update program uniforms
//*                if (!m_submissionContext->setParameters(command.m_parameterPack)) {
//*                    allCommandsIssued = false;
//*                    // If we have failed to set uniform (e.g unable to bind a texture)
//*                    // we won't perform the draw call which could show invalid content
//*                    continue;
//*                }
//*            }
//*
//*            //// OpenGL State
//*            // TO DO: Make states not dependendent on their backend node for this step
//*            // Set state
//*            RenderStateSet *localState = command.m_stateSet.data();
//*
//*
//*            {
//*                Profiling::GLTimeRecorder recorder(Profiling::StateUpdate);
//*                // Merge the RenderCommand state with the globalState of the RenderView
//*                // Or restore the globalState if no stateSet for the RenderCommand
//*                if (localState != nullptr) {
//*                    command.m_stateSet->merge(globalState);
//*                    m_submissionContext->setCurrentStateSet(localState);
//*                } else {
//*                    m_submissionContext->setCurrentStateSet(globalState);
//*                }
//*            }
//*            // All Uniforms for a pass are stored in the QUniformPack of the command
//*            // Uniforms for Effect, Material and Technique should already have been correctly resolved
//*            // at that point
//*
//*            //// Draw Calls
//*            performDraw(&command);
//*        }
//*    } // end of RenderCommands loop


    // Reset to the state we were in before executing the render commands
    m_submissionContext->setCurrentStateSet(globalState);

    return allCommandsIssued;
}

bool Renderer::updateVAOWithAttributes(Geometry *geometry,
                                       const RenderCommand *command,
                                       RHIShader *shader,
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
        RHIVertexArrayObject *vao = m_RHIResourceManagers->vaoManager()->data(vaoHandle);
        if (vao) {
            vao->destroy();
            // We remove VAO from manager using its VAOIdentifier
            m_RHIResourceManagers->vaoManager()->release(vaoHandle);
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

QList<QPair<QObject *, QMouseEvent>> Renderer::pendingPickingEvents() const
{
    return m_pickEventFilter->pendingMouseEvents();
}

QList<QKeyEvent> Renderer::pendingKeyEvents() const
{
    return m_pickEventFilter->pendingKeyEvents();
}

const GraphicsApiFilterData *Renderer::contextInfo() const
{
    return m_submissionContext->contextInfo();
}

SubmissionContext *Renderer::submissionContext() const
{
    return m_submissionContext.data();
}

// Returns a vector of jobs to be performed for dirty buffers
// 1 dirty buffer == 1 job, all job can be performed in parallel
QVector<Qt3DCore::QAspectJobPtr> Renderer::createRenderBufferJobs() const
{
    const QVector<QNodeId> dirtyBuffers = m_nodesManager->bufferManager()->takeDirtyBuffers();
    QVector<QAspectJobPtr> dirtyBuffersJobs;
    dirtyBuffersJobs.reserve(dirtyBuffers.size());

    for (const QNodeId bufId : dirtyBuffers) {
        Render::HBuffer bufferHandle = m_nodesManager->lookupHandle<Render::Buffer, Render::BufferManager, Render::HBuffer>(bufId);
        if (!bufferHandle.isNull()) {
            // Create new buffer job
            auto job = Render::LoadBufferJobPtr::create(bufferHandle);
            job->setNodeManager(m_nodesManager);
            dirtyBuffersJobs.push_back(job);
        }
    }

    return dirtyBuffersJobs;
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
