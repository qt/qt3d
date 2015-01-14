/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "renderer_p.h"

#include <Qt3DCore/qentity.h>


#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qparametermapping.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qeffect.h>

#include <Qt3DRenderer/private/renderviewjob_p.h>
#include <Qt3DRenderer/private/blendstate_p.h>
#include <Qt3DRenderer/private/cameraselectornode_p.h>
#include <Qt3DRenderer/private/framegraphvisitor_p.h>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/qgraphicscontext_p.h>
#include <Qt3DRenderer/private/rendercameralens_p.h>
#include <Qt3DRenderer/private/rendercommand_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/private/rendermaterial_p.h>
#include <Qt3DRenderer/private/rendermesh_p.h>
#include <Qt3DRenderer/private/renderpassfilternode_p.h>
#include <Qt3DRenderer/private/renderqueues_p.h>
#include <Qt3DRenderer/private/rendershader_p.h>
#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/rendertechnique_p.h>
#include <Qt3DRenderer/private/rendertextureprovider_p.h>
#include <Qt3DRenderer/private/renderthread_p.h>
#include <Qt3DRenderer/private/renderview_p.h>
#include <Qt3DRenderer/private/techniquefilternode_p.h>
#include <Qt3DRenderer/private/texturedatamanager_p.h>
#include <Qt3DRenderer/private/scenemanager_p.h>
#include <Qt3DRenderer/private/viewportnode_p.h>
#include <Qt3DRenderer/private/abstractsceneparser_p.h>

#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/qabstractaspectjobmanager.h>

#include <QStack>
#include <QSurface>
#include <QElapsedTimer>
#include <QOpenGLDebugLogger>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QDir>
#include <QUrl>
#include <QOffscreenSurface>
#include <QWindow>

// For Debug purposes only
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

static void logOpenGLDebugMessage(const QOpenGLDebugMessage &debugMessage)
{
    qDebug() << "OpenGL debug message:" << debugMessage;
}

const QString SCENE_PARSERS_PATH = QStringLiteral("/sceneparsers");

Renderer::Renderer(int cachedFrames)
    : m_rendererAspect(Q_NULLPTR)
    , m_graphicsContext(Q_NULLPTR)
    , m_surface(Q_NULLPTR)
    , m_textureProvider(new RenderTextureProvider)
    , m_meshDataManager(new MeshDataManager())
    , m_meshManager(new MeshManager())
    , m_cameraManager(new CameraManager())
    , m_renderNodesManager(new EntityManager())
    , m_materialManager(new MaterialManager())
    , m_worldMatrixManager(new MatrixManager())
    , m_vaoManager(new VAOManager())
    , m_shaderManager(new ShaderManager())
    , m_techniqueManager(new TechniqueManager())
    , m_effectManager(new EffectManager())
    , m_renderPassManager(new RenderPassManager())
    , m_textureManager(new TextureManager())
    , m_textureDataManager(new TextureDataManager())
    , m_layerManager(new LayerManager())
    , m_criterionManager(new CriterionManager())
    , m_frameGraphManager(new FrameGraphManager())
    , m_transformManager(new TransformManager())
    , m_renderTargetManager(new RenderTargetManager())
    , m_sceneManager(new SceneManager())
    , m_attachmentManager(new AttachmentManager())
    , m_sortCriterionManager(new SortCriterionManager())
    , m_parameterManager(new ParameterManager())
    , m_shaderDataManager(new ShaderDataManager())
    , m_renderQueues(new RenderQueues(cachedFrames - 1))
    , m_renderThread(new RenderThread(this))
    , m_frameCount(0)
    , m_cachedFramesCount(cachedFrames)
    , m_debugLogger(Q_NULLPTR)
{
    m_currentPreprocessingFrameIndex = 0;

    // Set renderer as running - it will wait in the context of the
    // RenderThread for RenderViews to be submitted
    m_running.fetchAndStoreOrdered(1);
    m_renderThread->waitForStart();

    buildDefaultTechnique();
    buildDefaultMaterial();
    loadSceneParsers();
}

void Renderer::buildDefaultTechnique()
{
    // TODO: Either use public API only or just go direct to the private backend API here
    m_defaultTechnique = new QTechnique;
    m_defaultTechnique->setObjectName(QStringLiteral("default-technique"));

    QShaderProgram* defaultShader = new QShaderProgram;
    defaultShader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/diffuse.vert"))));
    defaultShader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/diffuse.frag"))));
    defaultShader->setObjectName(QStringLiteral("DefaultShader"));

    QRenderPass* basicPass = new QRenderPass;
    basicPass->setShaderProgram(defaultShader);

    m_defaultRenderStateSet = new RenderStateSet;
    m_defaultRenderStateSet->addState(DepthTest::getOrCreate(GL_LESS));
    m_defaultRenderStateSet->addState(CullFace::getOrCreate(GL_BACK));
    //basicPass->setStateSet(m_defaultRenderStateSet);

    m_defaultTechnique->addPass(basicPass);

    // diffuse lighting uniforms
    QParameter* lightPos = new QParameter(QStringLiteral("lightPos"), QVector4D(10.0f, 10.0f, 0.0f, 1.0f));
    m_defaultTechnique->addParameter(lightPos);
    basicPass->addBinding(new QParameterMapping(QStringLiteral("lightPos"), QStringLiteral("lightPosition"), QParameterMapping::Uniform));

    QParameter* lightIntensity = new QParameter(QStringLiteral("lightIntensity"), QVector3D(0.5f, 0.5f, 0.5f));
    m_defaultTechnique->addParameter(lightIntensity);

    QParameter* kd = new QParameter(QStringLiteral("diffuse"), QVector3D(1.0f, 0.5f, 0.0f));
    m_defaultTechnique->addParameter(kd);
    basicPass->addBinding(new QParameterMapping(QStringLiteral("diffuse"), QStringLiteral("kd"), QParameterMapping::Uniform));

    QParameter* ka = new QParameter(QStringLiteral("ambient"), QVector3D(0.2f, 0.2f, 0.2f));
    m_defaultTechnique->addParameter(ka);
    basicPass->addBinding(new QParameterMapping(QStringLiteral("ambient"), QStringLiteral("ka"), QParameterMapping::Uniform));

}

void Renderer::loadSceneParsers()
{
    QString pluginsPath = QLibraryInfo::location(QLibraryInfo::PluginsPath) + SCENE_PARSERS_PATH;
    QDir sceneParsersPluginDir(pluginsPath);

    Q_FOREACH (QString plugin, sceneParsersPluginDir.entryList(QDir::Files)) {
        QPluginLoader loader(sceneParsersPluginDir.absoluteFilePath(plugin));
        AbstractSceneParser *parser = qobject_cast<AbstractSceneParser *>(loader.instance());
        if (parser != Q_NULLPTR)
            m_sceneParsers.append(parser);
        else
            qWarning() << "Failed to load scene parser plugin " << loader.fileName();
    }
}

void Renderer::buildDefaultMaterial()
{
    m_defaultMaterial = new QMaterial();
    m_defaultMaterial->setObjectName(QStringLiteral("DefaultMaterial"));
    m_defaultMaterial->addParameter(new QParameter(QStringLiteral("lightPos"), QVector4D(10.0f, 10.0f, 0.0f, 1.0f)));
    m_defaultMaterial->addParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(0.5f, 0.5f, 0.5f)));
    m_defaultMaterial->addParameter(new QParameter(QStringLiteral("ambient"), QVector3D(0.2f, 0.2f, 0.2f)));
    m_defaultMaterial->addParameter(new QParameter(QStringLiteral("diffuse"), QVector3D(1.0f, 0.5f, 0.0f)));

    QEffect* defEff = new QEffect;
    defEff->addTechnique(m_defaultTechnique);
    m_defaultMaterial->setEffect(defEff);

}

Renderer::~Renderer()
{
    // Bail out of the main render loop. Ensure that even if the render thread
    // is waiting on RenderViews to be populated that we wake up the wait condition.
    // We check for termination immediately after being awoken.
    m_running.fetchAndStoreOrdered(0);
    m_submitRenderViewsCondition.wakeOne();
    m_renderThread->wait();

    // Clean up the TLS allocators
    destroyAllocators();
}

void Renderer::createAllocators()
{
    // Issue a set of jobs to create an allocator in TLS for each worker thread
    Q_ASSERT(m_rendererAspect);
    QAbstractAspectJobManager *jobManager = rendererAspect()->jobManager();
    Q_ASSERT(jobManager);
    jobManager->waitForPerThreadFunction(Renderer::createThreadLocalAllocator, this);
}

void Renderer::destroyAllocators()
{
    // Issue a set of jobs to create an allocator in TLS for each worker thread
    Q_ASSERT(m_rendererAspect);
    QAbstractAspectJobManager *jobManager = rendererAspect()->jobManager();
    Q_ASSERT(jobManager);
    jobManager->waitForPerThreadFunction(Renderer::destroyThreadLocalAllocator, this);
}

QThreadStorage<QPair<int, QFrameAllocatorQueue *> *> *Renderer::tlsAllocators()
{
    return &m_tlsAllocators;
}

/*!
 * For each worker thread we create a QFrameAllocatorQueue which contains m_cachedFrameCount + 1
 * QFrameAllocators. We need an additional QFrameAllocator otherwise we may be clearing the QFrameAllocator
 * of the frame we are currently rendering.
 */
void Renderer::createThreadLocalAllocator(void *renderer)
{
    Q_ASSERT(renderer);
    Renderer *theRenderer = static_cast<Renderer *>(renderer);
    if (!theRenderer->tlsAllocators()->hasLocalData()) {
        // RenderView has a sizeof 72
        // RenderCommand has a sizeof 128
        // QMatrix4x4 has a sizeof 68
        // May need to fine tune parameters passed to QFrameAllocator for best performances
        // We need to allocate one more buffer than we have frames to handle the case where we're computing frame 5
        // While old frame 5 is being rendered
        QFrameAllocatorQueue *allocatorQueue = new QFrameAllocatorQueue();
        for (int i = 0; i <= theRenderer->cachedFramesCount(); i++)
            allocatorQueue->append(new QFrameAllocator(128, 16, 128));
        theRenderer->tlsAllocators()->setLocalData(new QPair<int, QFrameAllocatorQueue *>(0, allocatorQueue));
    }
}

void Renderer::destroyThreadLocalAllocator(void *renderer)
{
    Q_ASSERT(renderer);
    Renderer *theRenderer = static_cast<Renderer *>(renderer);
    if (theRenderer->tlsAllocators()->hasLocalData()) {
        QPair<int, QFrameAllocatorQueue *> *frameAllocatorPair = theRenderer->tlsAllocators()->localData();
        QFrameAllocatorQueue *allocatorQueue = frameAllocatorPair->second;
        qDeleteAll(*allocatorQueue);
        allocatorQueue->clear();
        delete allocatorQueue;
        theRenderer->tlsAllocators()->setLocalData(Q_NULLPTR);
    }
}

// Called in RenderThread context by the run method of RenderThread
// RenderThread has locked the mutex already and unlocks it when this
// method termintates
void Renderer::initialize()
{
    m_waitForWindowToBeSetCondition.wait(mutex());

    QByteArray debugLoggingMode = qgetenv("QT3D_DEBUG_LOGGING");
    bool enableDebugLogging = !debugLoggingMode.isEmpty();

    m_graphicsContext.reset(new QGraphicsContext);
    m_graphicsContext->setRenderer(this);

    QSurfaceFormat sf = m_surface->format();
    if (enableDebugLogging)
        sf.setOption(QSurfaceFormat::DebugContext);

    QOpenGLContext* ctx = new QOpenGLContext;
    ctx->setFormat(sf);
    if (!ctx->create())
        qCWarning(Backend) << Q_FUNC_INFO << "OpenGL context creation failed";
    m_graphicsContext->setOpenGLContext(ctx, m_surface);

    if (enableDebugLogging) {
        bool supported = ctx->hasExtension("GL_KHR_debug");
        if (supported) {
            qCDebug(Backend) << "Qt3D: Enabling OpenGL debug logging";
            m_debugLogger.reset(new QOpenGLDebugLogger);
            if (m_debugLogger->initialize()) {
                QObject::connect(m_debugLogger.data(), &QOpenGLDebugLogger::messageLogged, &logOpenGLDebugMessage);
                QString mode = QString::fromLocal8Bit(debugLoggingMode);
                m_debugLogger->startLogging(mode.toLower().startsWith(QLatin1String("sync"))
                                            ? QOpenGLDebugLogger::SynchronousLogging
                                            : QOpenGLDebugLogger::AsynchronousLogging);

                Q_FOREACH (const QOpenGLDebugMessage &msg, m_debugLogger->loggedMessages())
                    logOpenGLDebugMessage(msg);

            }
        } else {
            qCDebug(Backend) << "Qt3D: OpenGL debug logging requested but GL_KHR_debug not supported";
        }
    }

    // Awake setScenegraphRoot in case it was waiting
    m_waitForInitializationToBeCompleted.wakeOne();
}

/*!
    \internal

    Called in the context of the RenderThread to do any shutdown and cleanup
    that needs to be performed in the thread where the OpenGL context lives
*/
void Renderer::shutdown()
{
    // Stop and destroy the OpenGL logger
    if (m_debugLogger) {
        m_debugLogger->stopLogging();
        m_debugLogger.reset(Q_NULLPTR);
    }

    // Clean up the graphics context
    m_graphicsContext.reset(Q_NULLPTR);
}

/*!
 * Returns the a FrameAllocator for the frame identified by \a frameIndex in the context
 * of the caller thread. This also clears the FrameAllocator before usage.
 */
QFrameAllocator *Renderer::currentFrameAllocator(int frameIndex)
{
    // frameIndex between 0 and m_cachedFramesCount
    // Check if index is the same as the current frame
    // Otherwise we have to clear the QFrameAllocator at frameIndex
    QPair<int, QFrameAllocatorQueue *> *data = m_tlsAllocators.localData();
    if (data->first != frameIndex) {
        qCDebug(Render::Memory) << Q_FUNC_INFO << "clearing " << frameIndex << " previous" << data->first;
        data->first = frameIndex;
        data->second->at(frameIndex)->clear();
    }
    return data->second->at(frameIndex);
}

void Renderer::setFrameGraphRoot(const QNodeId &frameGraphRootUuid)
{
    m_frameGraphRootUuid = frameGraphRootUuid;
    qCDebug(Backend) << Q_FUNC_INFO << m_frameGraphRootUuid;
}

Render::FrameGraphNode *Renderer::frameGraphRoot() const
{
    FrameGraphNode **fgRoot = m_frameGraphManager->lookupResource(m_frameGraphRootUuid);
    if (fgRoot != Q_NULLPTR)
        return *fgRoot;
    return Q_NULLPTR;
}

// QAspectThread context
// Order of execution :
// 1) Initialize -> waiting for Window
// 2) setWindow -> waking Initialize || setSceneGraphRoot waiting
// 3) setWindow -> waking Initialize if setSceneGraphRoot was called before
// 4) Initialize resuming, performing initialization and waking up setSceneGraphRoot
// 5) setSceneGraphRoot called || setSceneGraphRoot resuming if it was waiting
void Renderer::setSceneGraphRoot(RenderEntity *sgRoot)
{
    Q_ASSERT(sgRoot);
    QMutexLocker lock(&m_mutex); // This waits until initialize and setSurface have been called
    if (m_graphicsContext == Q_NULLPTR) // If initialization hasn't been completed we must wait
        m_waitForInitializationToBeCompleted.wait(&m_mutex);

    m_renderSceneRoot = sgRoot;
    if (!m_renderSceneRoot)
        qCWarning(Backend) << "Failed to build render scene";
    m_renderSceneRoot->dump();
    qCDebug(Backend) << Q_FUNC_INFO << "DUMPING SCENE";

    // If that weren't for those lines, the renderer might not event need
    // to know about the renderer aspect
    m_rendererAspect->createBackendNode(m_defaultMaterial);
    m_rendererAspect->createBackendNode(m_defaultMaterial->effect());
    m_rendererAspect->createBackendNode(m_defaultTechnique);
    m_rendererAspect->createBackendNode(m_defaultTechnique->renderPasses().first());
    m_rendererAspect->createBackendNode(m_defaultTechnique->renderPasses().first()->shaderProgram());

    // We create backend resources for all the parameters
    Q_FOREACH (QParameter *p, m_defaultMaterial->parameters())
        m_rendererAspect->createBackendNode(p);
    Q_FOREACH (QParameter *p, m_defaultTechnique->parameters())
        m_rendererAspect->createBackendNode(p);
    Q_FOREACH (QParameter *p, m_defaultMaterial->effect()->parameters())
        m_rendererAspect->createBackendNode(p);


    m_defaultMaterialHandle = m_materialManager->lookupHandle(m_defaultMaterial->id());
    m_defaultEffectHandle = m_effectManager->lookupHandle(m_defaultMaterial->effect()->id());
    m_defaultTechniqueHandle = m_techniqueManager->lookupHandle(m_defaultTechnique->id());
    m_defaultRenderPassHandle = m_renderPassManager->lookupHandle(m_defaultTechnique->renderPasses().first()->id());
    m_defaultRenderShader = m_shaderManager->lookupResource(m_defaultTechnique->renderPasses().first()->shaderProgram()->id());
}

// Called in RenderAspect Thread context
// Cannot do OpenGLContext initialization here
void Renderer::setSurface(QSurface* surface)
{
    qCDebug(Backend) << Q_FUNC_INFO << QThread::currentThread();
    // Locking this mutex will wait until initialize() has been called by
    // RenderThread::run() and the RenderThread is waiting on the
    // m_waitForWindowToBeSetCondition condition.
    //
    // The first time this is called Renderer::setSurface will cause the
    // Renderer::initialize() function to continue execution in the context
    // of the Render Thread. On subsequent calls, just the surface will be
    // updated.

    // TODO: Remove the need for a valid surface from the renderer initialization
    // We can use an offscreen surface to create and assess the OpenGL context.
    // This should allow us to get rid of the "swapBuffers called on a non-exposed
    // window" warning that we sometimes see.
    QMutexLocker locker(&m_mutex);
    m_surface = surface;
    m_waitForWindowToBeSetCondition.wakeOne();
}

QSurface *Renderer::surface() const
{
    return m_surface;
}

void Renderer::render()
{
    // Traversing the framegraph tree from root to lead node
    // Allows us to define the rendering set up
    // Camera, RenderTarget ...

    // Utimately the renderer should be a framework
    // For the processing of the list of renderbins

    // Matrice update, bounding volumes computation ...
    // Should be jobs

    // Qt3D has 2 distincts node trees
    // One scene description
    // One framegraph description

    while (m_running.load() > 0) {
        doRender();
    }
}

void Renderer::doRender()
{
    // Render using current device state and renderer configuration
    submitRenderViews();
}

// Called by threadWeaver RenderViewJobs
void Renderer::enqueueRenderView(Render::RenderView *renderView, int submitOrder)
{
    //    qDebug() << Q_FUNC_INFO << QThread::currentThread();
    m_renderQueues->queueRenderView(renderView, submitOrder);
    if (m_renderQueues->isFrameQueueComplete()) {
        // We can increment the currentProcessingFrameIndex here
        // That index will then be used by RenderViewJobs to know which QFrameAllocator to use
        // Increasing the frameIndex at that point is safe because :
        // - This method is called by the last RenderViewJobs in ThreadWeaver
        // - A new call to generate new RenderViewJobs cannot happen before all RenderViewJobs have finished executing aka locking the AspectThread
        // - The Renderer thread doesn't modify the currentPreprocessingFrameIndex value
        m_currentPreprocessingFrameIndex = (m_currentPreprocessingFrameIndex + 1) % (m_cachedFramesCount + 1);
        m_renderQueues->pushFrameQueue();
        qCDebug(Memory) << Q_FUNC_INFO << "Next frame index " << m_currentPreprocessingFrameIndex;
        m_submitRenderViewsCondition.wakeOne();
    }
}

bool Renderer::canRender() const
{
    // Make sure that we've not been told to terminate whilst waiting on
    // the above wait condition
    if (!m_running.load()) {
        qCDebug(Rendering) << "RenderThread termination requested whilst waiting";
        return false;
    }

    // Make sure that the surface we are rendering too has not been unset
    // (probably due to the window being destroyed or changing QScreens).
    if (!m_surface) {
        qCDebug(Rendering) << "QSurface has been removed";
        return false;
    }

    return true;
}

// Happens in RenderThread context when all RenderViewJobs are done
void Renderer::submitRenderViews()
{
    QMutexLocker locker(&m_mutex);
    m_submitRenderViewsCondition.wait(locker.mutex());
    locker.unlock();

    QElapsedTimer timer;
    quint64 queueElapsed = 0;
    timer.start();

    // We might not want to render on the default FBO
    const GLuint defaultFboId = m_graphicsContext->boundFrameBufferObject();

    while (m_renderQueues->queuedFrames() > 0)
    {
        // Lock the mutex to protect access to m_surface and check if we are still set
        // to the running state and that we have a valid surface on which to draw
        locker.relock();
        if (!canRender()) {
            QVector<Render::RenderView *> renderViews = m_renderQueues->nextFrameQueue();
            qDeleteAll(renderViews);
            m_renderQueues->popFrameQueue();
            return;
        }

        QVector<Render::RenderView *> renderViews = m_renderQueues->nextFrameQueue();
        int renderViewsCount = renderViews.size();
        quint64 frameElapsed = queueElapsed;

        if (renderViewsCount <= 0)
            continue;

        // Bail out if we cannot make the OpenGL context current (e.g. if the window has been destroyed)
        if (!m_graphicsContext->beginDrawing(m_surface, renderViews.first()->clearColor()))
            break;

        qCDebug(Memory) << Q_FUNC_INFO << "rendering frame " << renderViews.last()->frameIndex() << " Queue " << m_renderQueues->queuedFrames();
        for (int i = 0; i < renderViewsCount; i++) {
            // Set RenderTarget ...
            // Activate RenderTarget
            m_graphicsContext->activateRenderTarget(m_renderTargetManager->data(renderViews[i]->renderTargetHandle()),
                                                    renderViews[i]->attachmentPack(), defaultFboId);
            // Clear BackBuffer
            m_graphicsContext->clearBackBuffer(renderViews[i]->clearBuffer());
            // Set the Viewport
            m_graphicsContext->setViewport(renderViews[i]->viewport());

            // Initialize QGraphicsContext for drawing
            executeCommands(renderViews.at(i)->commands());

            frameElapsed = timer.elapsed() - frameElapsed;
            qCDebug(Rendering) << Q_FUNC_INFO << "Submitted Renderview " << i + 1 << "/" << renderViewsCount  << "in " << frameElapsed << "ms";
            frameElapsed = timer.elapsed();
        }

        m_graphicsContext->endDrawing();

        // Let the Aspect Thread get a look in if it needs to change the surface
        locker.unlock();

        qDeleteAll(renderViews);
        m_renderQueues->popFrameQueue();
        queueElapsed = timer.elapsed() - queueElapsed;
        qCDebug(Rendering) << Q_FUNC_INFO << "Submission of Queue " << m_frameCount + 1 << "in " << queueElapsed << "ms <=> " << queueElapsed / renderViewsCount << "ms per RenderView <=> Avg " << 1000.0f / (queueElapsed * 1.0f/ renderViewsCount * 1.0f) << " RenderView/s";
        qCDebug(Rendering) << Q_FUNC_INFO << "Queued frames for rendering remaining " << m_renderQueues->queuedFrames();;
        qCDebug(Rendering) << Q_FUNC_INFO << "Average FPS : " << 1000 / (queueElapsed * 1.0f);
        queueElapsed = timer.elapsed();
        m_frameCount++;
    }
    qCDebug(Rendering) << Q_FUNC_INFO << "Submission Completed " << m_frameCount << " RenderQueues in " << timer.elapsed() << "ms";
    m_frameCount = 0;
}

// Waits to be told to create jobs for the next frame
// Called by QRenderAspect jobsToExecute context of QAspectThread
QVector<QAspectJobPtr> Renderer::createRenderBinJobs()
{
    // Traverse the current framegraph. For each leaf node create a
    // RenderView and set its configuration then create a job to
    // populate the RenderView with a set of RenderCommands that get
    // their details from the RenderNodes that are visible to the
    // Camera selected by the framegraph configuration

    QVector<QAspectJobPtr> renderBinJobs;

    // We do not create jobs if the graphicContext hasn't been set.
    // That way we will go in submitRenderView which will create the OpenGLContext in the
    // correct thread so that later on Jobs can query for OpenGL Versions, Extensions ...
    FrameGraphVisitor visitor;
    visitor.traverse(frameGraphRoot(), this, &renderBinJobs);
    m_renderQueues->setTargetRenderViewCount(renderBinJobs.size());

    return renderBinJobs;
}

// Called during while traversing the FrameGraph for each leaf node context of QAspectThread
QAspectJobPtr Renderer::createRenderViewJob(FrameGraphNode *node, int submitOrderIndex)
{
    RenderViewJobPtr job(new RenderViewJob);
    job->setRenderer(this);
    job->setFrameGraphLeafNode(node);
    job->setSubmitOrderIndex(submitOrderIndex);
    job->setFrameIndex(m_currentPreprocessingFrameIndex);
    return job;
}

// Called by RenderView->submit() in RenderThread context
void Renderer::executeCommands(const QVector<RenderCommand *> &commands)
{
    // Render drawing commands

    // Use the graphicscontext to submit the commands to the underlying
    // graphics API (OpenGL)

    Q_FOREACH (RenderCommand *command, commands) {

        QMeshData *meshData = m_meshDataManager->data(command->m_meshData);
        if (meshData == Q_NULLPTR || meshData->attributeNames().empty()) {
            qCWarning(Rendering) << "RenderCommand should have a mesh";
            continue ;
        }
        RenderShader *shader = m_shaderManager->data(command->m_shader);
        if (shader == Q_NULLPTR) {
            shader = m_defaultRenderShader;
            command->m_parameterAttributeToShaderNames = m_defaultParameterToGLSLAttributeNames;
            command->m_uniforms = m_defaultUniformPack;
        }

        command->m_vao = m_vaoManager->lookupHandle(QPair<HMeshData, HShader>(command->m_meshData, command->m_shader));
        if (command->m_vao.isNull()) {
            // Either VAO has not been created for QMeshData and RenderPass
            // Or there is no RenderPass
            // Tries to use vao for Mesh source and Default Technique
            qCDebug(Backend) << Q_FUNC_INFO << "VAO Handle is null";
            qCDebug(Backend) << Q_FUNC_INFO << "HShader " << command->m_shader;
            // Check if HShader exists. If it doesn't that means there is no RenderPass
            // Otherwise use a default renderpass name
            command->m_vao = m_vaoManager->lookupHandle(QPair<HMeshData, HShader>(command->m_meshData, command->m_shader));
            // Check if VAO pointer for the QMeshData / RenderShader exists
            if (command->m_vao.isNull()) {
                qCDebug(Rendering) << Q_FUNC_INFO << "Allocating new VAO";
                command->m_vao = m_vaoManager->getOrAcquireHandle(QPair<HMeshData, HShader>(command->m_meshData, command->m_shader));
                *(m_vaoManager->data(command->m_vao)) = new QOpenGLVertexArrayObject();
            }
        }
        QOpenGLVertexArrayObject *vao = *(m_vaoManager->data(command->m_vao));
        Q_ASSERT(vao);

        // The VAO should be created only once for a QMeshData and a ShaderProgram
        // Manager should have a VAO Manager that are indexed by QMeshData and Shader
        // RenderCommand should have a handle to the corresponding VAO for the Mesh and Shader

        bool drawIndexed = !meshData->indexAttribute().isNull();

        //// We activate the shader here
        // This will fill the attributes & uniforms info the first time the shader is loaded
        m_graphicsContext->activateShader(shader);

        //// Initialize GL
        // The initialization is performed only once parameters in the command are set
        // Which indicates that the shader has been initialized and that renderview jobs were able to retrieve
        // Uniform and Attributes info from the shader
        // Otherwise we might create a VAO without attribute bindings as the RenderCommand had no way to know about attributes
        // Before the shader was loader
        if (!command->m_parameterAttributeToShaderNames.isEmpty() && !vao->isCreated()) {
            vao->create();
            vao->bind();

            qCDebug(Rendering) << Q_FUNC_INFO << "Creating new VAO";

            // TO DO : Do that in a better / nicer way
            Q_FOREACH (QString nm, meshData->attributeNames()) {
                AttributePtr attr(meshData->attributeByName(nm).staticCast<Attribute>());
                if (command->m_parameterAttributeToShaderNames.contains(nm))
                    m_graphicsContext->specifyAttribute(command->m_parameterAttributeToShaderNames[nm], attr);
                else
                    qCDebug(Render::Rendering) << "Couldn't find a Parameter attribute named " << nm;
            }
            if (drawIndexed)
                m_graphicsContext->specifyIndices(meshData->indexAttribute().staticCast<Attribute>());
            vao->release();
        }

        //// Update program uniforms
        m_graphicsContext->setUniforms(command->m_uniforms);

        //// Draw Calls
        // Set state
        if (command->m_stateSet != Q_NULLPTR)
            m_graphicsContext->setCurrentStateSet(command->m_stateSet);
        else
            m_graphicsContext->setCurrentStateSet(m_defaultRenderStateSet);

        // All Uniforms for a pass are stored in the QUniformPack of the command
        // Uniforms for Effect, Material and Technique should already have been correctly resolved
        // at that point
        if (vao->isCreated()) {
            vao->bind();
            GLint primType = meshData->primitiveType();
            GLint primCount = meshData->primitiveCount();
            GLint indexType = drawIndexed ? meshData->indexAttribute()->type() : 0;

            if (primType == QMeshData::Patches && meshData->verticesPerPatch() != 0)
                m_graphicsContext->setVerticesPerPatch(meshData->verticesPerPatch());

            if (drawIndexed) {
                m_graphicsContext->drawElements(primType,
                                                primCount,
                                                indexType,
                                                reinterpret_cast<void*>(meshData->indexAttribute()->byteOffset()));
            } else {
                m_graphicsContext->drawArrays(primType, 0, primCount);
            }

            int err = m_graphicsContext->openGLContext()->functions()->glGetError();
            if (err)
                qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);

            vao->release();
        }
    }
}

QOpenGLFilter *Renderer::contextInfo() const
{
    return m_graphicsContext->contextInfo();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
