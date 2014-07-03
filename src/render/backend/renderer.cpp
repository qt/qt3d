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

#include "renderer.h"

#include <Qt3DCore/nodevisitor.h>
#include <Qt3DCore/entity.h>

#include "framegraphvisitor.h"
#include "cameraselectornode.h"
#include "renderpassfilternode.h"
#include "techniquefilternode.h"
#include "viewportnode.h"

#include "jobs/renderviewjob.h"

#include <qmaterial.h>
#include <qmesh.h>
#include <qtechnique.h>
#include <shaderprogram.h>
#include <qrenderpass.h>
#include <qabstractshapemesh.h>
#include <cameramanager.h>
#include <meshdatamanager.h>
#include <meshmanager.h>
#include <rendermesh.h>
#include <renderqueues.h>
#include <rendermaterial.h>
#include <rendernode.h>
#include <rendertechnique.h>
#include <renderscenebuilder.h>
#include <rendershader.h>
#include <qgraphicscontext.h>
#include <rendercamera.h>
#include <rendertextureprovider.h>
#include <drawstate.h>
#include <states/blendstate.h>
#include <rendernodesmanager.h>
#include <renderview.h>
#include <rendercommand.h>
#include <materialmanager.h>
#include <matrixmanager.h>
#include <vaomanager.h>
#include <shadermanager.h>
#include <techniquemanager.h>
#include <effectmanager.h>
#include <renderpassmanager.h>
#include <texturemanager.h>
#include <texturedatamanager.h>
#include <layermanager.h>
#include <lightmanager.h>
#include "renderlogging.h"
#include "parametermapper.h"

#include <Qt3DCore/cameralens.h>
#include <Qt3DCore/qabstracteffect.h>

#include <qeffect.h>
#include <QStack>
#include <QSurface>
#include <QElapsedTimer>

// For Debug purposes only
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

Renderer::Renderer(int cachedFrames)
    : m_rendererAspect(0)
    , m_frameGraphRoot(0)
    , m_graphicsContext(0)
    , m_surface(0)
    , m_meshDataManager(new MeshDataManager())
    , m_meshManager(new MeshManager())
    , m_cameraManager(new CameraManager())
    , m_renderNodesManager(new RenderNodesManager())
    , m_materialManager(new MaterialManager())
    , m_worldMatrixManager(new MatrixManager())
    , m_localMatrixManager(new MatrixManager())
    , m_vaoManager(new VAOManager())
    , m_shaderManager(new ShaderManager())
    , m_techniqueManager(new TechniqueManager())
    , m_effectManager(new EffectManager())
    , m_renderPassManager(new RenderPassManager())
    , m_textureManager(new TextureManager())
    , m_textureDataManager(new TextureDataManager())
    , m_layerManager(new LayerManager())
    , m_lightManager(new LightManager())
    , m_renderQueues(new RenderQueues(cachedFrames - 1))
    , m_frameCount(0)
    , m_cachedFramesCount(cachedFrames)
{
    m_graphicContextInitialized.fetchAndStoreOrdered(0);
    m_currentPreprocessingFrameIndex = 0;
    m_textureProvider = new RenderTextureProvider;

    buildDefaultTechnique();
    buildDefaultMaterial();

    QLoggingCategory::setFilterRules(QString::fromUtf8( // multiline QStringLiteral doesn't compile on Windows...
                                                        "Qt3D.Render.*.debug=false\n"
                                                        //                                     "Qt3D.Render.Memory.debug=true\n"
                                                        //                                     "Qt3D.Render.Rendering.debug=true\n"
                                                        //                                     "Qt3D.Render.RenderNodes.debug=true\n"
                                                        //                                     "Qt3D.Render.Frontend.debug=true\n"
                                                        //                                     "qml.debug=true\n"
                                                        ));
}

void Renderer::buildDefaultTechnique()
{
    m_defaultTechnique = new QTechnique;
    m_defaultTechnique->setObjectName(QStringLiteral("default-technique"));

    ShaderProgram* defaultShader = new ShaderProgram;
    defaultShader->setVertexSourceFile(QStringLiteral(":/shaders/diffuse.vert"));
    defaultShader->setFragmentSourceFile(QStringLiteral(":/shaders/diffuse.frag"));
    defaultShader->setObjectName(QStringLiteral("DefaultShader"));

    m_defaultRenderShader = new RenderShader();
    m_defaultRenderShader->setPeer(defaultShader);

    QRenderPass* basicPass = new QRenderPass;
    basicPass->setShaderProgram(defaultShader);

    m_defaultDrawStateSet = new DrawStateSet;
    m_defaultDrawStateSet->addState(DepthTest::getOrCreate(GL_LESS));
    m_defaultDrawStateSet->addState(CullFace::getOrCreate(GL_BACK));
    basicPass->setStateSet(m_defaultDrawStateSet);

    m_defaultTechnique->addPass(basicPass);

    basicPass->addBinding(new ParameterMapper(QStringLiteral("position"), QStringLiteral("vertexPosition"), ParameterMapper::Attribute));
    basicPass->addBinding(new ParameterMapper(QStringLiteral("normal"), QStringLiteral("vertexNormal"), ParameterMapper::Attribute));

    // matrix uniforms from standard
    Parameter* mvMat = new Parameter(m_defaultTechnique, QStringLiteral("modelView"), Parameter::ModelView);
    m_defaultTechnique->addParameter(mvMat);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("modelView"), QStringLiteral("modelViewMatrix"), ParameterMapper::StandardUniform));

    Parameter* nMat = new Parameter(m_defaultTechnique, QStringLiteral("normalMat"), Parameter::ModelViewNormal);
    m_defaultTechnique->addParameter(nMat);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("normalMat"), QStringLiteral("normalMatrix"), ParameterMapper::StandardUniform));

    Parameter* mvpMat = new Parameter(m_defaultTechnique, QStringLiteral("mvp"), Parameter::ModelViewProjection);
    m_defaultTechnique->addParameter(mvpMat);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("mvp"), QStringLiteral("mvp"), ParameterMapper::StandardUniform));

    // diffuse lighting uniforms
    Parameter* lightPos = new Parameter(m_defaultTechnique, QStringLiteral("lightPos"), QVector4D(10.0f, 10.0f, 0.0f, 1.0f));
    m_defaultTechnique->addParameter(lightPos);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("lightPos"), QStringLiteral("lightPosition"), ParameterMapper::Uniform));

    Parameter* lightIntensity = new Parameter(m_defaultTechnique, QStringLiteral("lightIntensity"), QVector3D(0.5f, 0.5f, 0.5f));
    m_defaultTechnique->addParameter(lightIntensity);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("lightIntensity"), QStringLiteral("lightIntensity"), ParameterMapper::Uniform));

    Parameter* kd = new Parameter(m_defaultTechnique, QStringLiteral("diffuse"), QVector3D(1.0f, 0.5f, 0.0f));
    m_defaultTechnique->addParameter(kd);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("diffuse"), QStringLiteral("kd"), ParameterMapper::Uniform));

    Parameter* ka = new Parameter(m_defaultTechnique, QStringLiteral("ambient"), QVector3D(0.2f, 0.2f, 0.2f));
    m_defaultTechnique->addParameter(ka);
    basicPass->addBinding(new ParameterMapper(QStringLiteral("ambient"), QStringLiteral("ka"), ParameterMapper::Uniform));

}

void Renderer::buildDefaultMaterial()
{
    m_defaultMaterial = new QMaterial();
    m_defaultMaterial->setObjectName(QStringLiteral("DefaultMaterial"));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("lightPos"), QVector4D(10.0f, 10.0f, 0.0f, 1.0f)));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("lightIntensity"), QVector3D(0.5f, 0.5f, 0.5f)));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("ambient"), QVector3D(0.2f, 0.2f, 0.2f)));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("diffuse"), QVector3D(1.0f, 0.5f, 0.0f)));

    QEffect* defEff = new QEffect;
    defEff->addTechnique(m_defaultTechnique);
    m_defaultMaterial->setEffect(defEff);

}

Renderer::~Renderer()
{
    delete m_graphicsContext;
    delete m_textureProvider;
}

// Called in RenderAspect Thread context
void Renderer::initialize()
{

    m_defaultMaterialHandle = m_materialManager->acquire();
    RenderMaterial *rMaterial = m_materialManager->data(m_defaultMaterialHandle);
    rMaterial->setRendererAspect(m_rendererAspect);
    rMaterial->setPeer(m_defaultMaterial);

    m_defaultEffectHandle = m_effectManager->acquire();
    RenderEffect *rEffect = m_effectManager->data(m_defaultEffectHandle);
    rEffect->setRendererAspect(m_rendererAspect);
    rEffect->setPeer(m_defaultMaterial->effect());

    m_defaultTechniqueHandle = m_techniqueManager->acquire();
    RenderTechnique *rTech = m_techniqueManager->data(m_defaultTechniqueHandle);
    rTech->setRenderer(this);
    rTech->setPeer(m_defaultTechnique);

    m_defaultRenderPassHandle = m_renderPassManager->acquire();
    RenderRenderPass *rPass = m_renderPassManager->data(m_defaultRenderPassHandle);
    rPass->setRenderer(this);
    rPass->setPeer(qobject_cast<QRenderPass*>(m_defaultTechnique->renderPasses().first()));

}

QFrameAllocator *Renderer::currentFrameAllocator(int frameIndex)
{
    // frameIndex between 0 and m_cachedFramesCount
    if (!m_tlsAllocators.hasLocalData()) {
        qCDebug(Render::Backend) << Q_FUNC_INFO << "Initializing local data for Thread " << QThread::currentThread();
        // RenderView has a sizeof 72
        // RenderCommand has a sizeof 128
        // QMatrix4x4 has a sizeof 68
        // May need to fine tune parameters passed to QFrameAllocator for best performances
        // We need to allocate one more buffer than we have frames to handle the case where we're computing frame 5
        // While old frame 5 is being rendered
        QFrameAllocatorQueue *allocatorQueue = new QFrameAllocatorQueue();
        for (int i = 0; i < m_cachedFramesCount; i++)
            allocatorQueue->append(new QFrameAllocator(128, 16, 128));
        m_tlsAllocators.setLocalData(QPair<int, QFrameAllocatorQueue *>(0, allocatorQueue));
    }
    QPair<int, QFrameAllocatorQueue *> &data = m_tlsAllocators.localData();
    // Check if index is the same as the current frame
    // Otherwise we have to clear the QFrameAllocator at frameIndex
    if (data.first != frameIndex) {
        qCDebug(Render::Memory) << Q_FUNC_INFO << "clearing " << frameIndex << " previous" << data.first;
        data.first = frameIndex;
        data.second->at(frameIndex)->clear();
    }
    return data.second->at(frameIndex);
}

void Renderer::setFrameGraphRoot(Render::FrameGraphNode *fgRoot)
{
    qCDebug(Backend) << Q_FUNC_INFO;
    m_frameGraphRoot = fgRoot;
}

Render::FrameGraphNode *Renderer::frameGraphRoot() const
{
    return m_frameGraphRoot;
}

// QAspectThread context
void Renderer::setSceneGraphRoot(Node *sgRoot)
{
    Q_ASSERT(sgRoot);

    //    Scene *scene = Scene::findInTree(sgRoot);
    // Scene needs to be built with scene parsers
    // For each scene parser, check whether scene source format
    // is supported, filter if there is a preference toward which
    // parser should parse the file if two parsers support the same
    // format

    m_sceneGraphRoot = sgRoot;
    RenderSceneBuilder builder(this);
    builder.traverse(m_sceneGraphRoot);
    m_renderSceneRoot = builder.rootNode();
    builder.initializeFrameGraph();
    if (!m_renderSceneRoot)
        qCWarning(Backend) << "Failed to build render scene";
    qCDebug(Backend) << Q_FUNC_INFO << "DUMPING SCENE";
    m_renderSceneRoot->dump();

    // Queue up jobs to do initial full updates of
    //  - Mesh loading + bounding volume calculation
    //  - Local bounding volumes
    //  - World transforms
    //  - World bounding volumes
}

Node *Renderer::sceneGraphRoot() const
{
    return m_sceneGraphRoot;
}

// Called in RenderAspect Thread context
// Cannot do OpenGLContext initialization here
void Renderer::setSurface(QSurface* s)
{
    qCDebug(Backend) << Q_FUNC_INFO << QThread::currentThread();
    m_surface = s;
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

    doRender();
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
        m_currentPreprocessingFrameIndex = (m_currentPreprocessingFrameIndex + 1) % m_cachedFramesCount;
        m_renderQueues->pushFrameQueue();
        qCDebug(Memory) << Q_FUNC_INFO << "Next frame index " << m_currentPreprocessingFrameIndex;
        m_submitRenderViewsCondition.wakeOne();
    }
}

// Happens in RenderThread context when all RenderViewJobs are done
void Renderer::submitRenderViews()
{
    QMutexLocker locker(&m_mutex);
    m_submitRenderViewsCondition.wait(locker.mutex());
    // Allow RenderViewJobs to be processed for the next frame
    // Without having to wait for the current RenderViews to be submitted
    // as long as there is available space in the renderQueues.
    // Otherwise it waits for submission to be done so as to never miss
    // Any important state change that could be in a RenderView
    if (m_graphicsContext == Q_NULLPTR && m_surface != Q_NULLPTR) {
        m_graphicsContext = new QGraphicsContext;
        QOpenGLContext* ctx = new QOpenGLContext;
        m_graphicsContext->setSurface(m_surface);
        m_graphicsContext->setRenderer(this);
        QSurfaceFormat sf = m_surface->format();
        ctx->setFormat(sf);
        if (!ctx->create())
            qCWarning(Backend) << Q_FUNC_INFO << "OpenGL context creation failed";
        m_graphicsContext->setOpenGLContext(ctx);
        m_graphicContextInitialized.fetchAndStoreOrdered(1);
    }
    locker.unlock();

    QElapsedTimer timer;
    quint64 queueElapsed = 0;
    timer.start();
    while (m_renderQueues->queuedFrames() > 0)
    {
        QVector<Render::RenderView *> renderViews = m_renderQueues->nextFrameQueue();
        int renderViewsCount = renderViews.size();
        quint64 frameElapsed = queueElapsed;

        m_graphicsContext->beginDrawing();
        qCDebug(Memory) << Q_FUNC_INFO << "rendering frame " << renderViews.last()->frameIndex() << " Queue " << m_renderQueues->queuedFrames();
        for (int i = 0; i < renderViewsCount; i++) {
            // Set the Viewport
            m_graphicsContext->setViewport(renderViews[i]->viewport());
            // Set RenderTarget ...

            // Initialize QGraphicsContext for drawing
            executeCommands(renderViews.at(i)->commands());


            frameElapsed = timer.elapsed() - frameElapsed;
            qCDebug(Rendering) << Q_FUNC_INFO << "Submitted Renderview " << i + 1 << "/" << renderViewsCount  << "in " << frameElapsed << "ms";
            frameElapsed = timer.elapsed();
        }
        m_graphicsContext->endDrawing();
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
// Called by RendererAspect jobsToExecute context of QAspectThread
QVector<QJobPtr> Renderer::createRenderBinJobs()
{
    // Traverse the current framegraph. For each leaf node create a
    // RenderView and set its configuration then create a job to
    // populate the RenderView with a set of RenderCommands that get
    // their details from the RenderNodes that are visible to the
    // Camera selected by the framegraph configuration

    QVector<QJobPtr> renderBinJobs;

    // We do not create jobs if the graphicContext hasn't been set.
    // That way we will go in submitRenderView which will create the OpenGLContext in the
    // correct thread so that later on Jobs can query for OpenGL Versions, Extensions ...
    if (m_graphicContextInitialized.loadAcquire()) {
        FrameGraphVisitor visitor;
        visitor.traverse(m_frameGraphRoot, this, &renderBinJobs);
        m_renderQueues->setTargetRenderViewCount(renderBinJobs.size());
    }
    else {
        m_submitRenderViewsCondition.wakeOne();
    }
    return renderBinJobs;
}

// Called during while traversing the FrameGraph for each leaf node context of QAspectThread
QJobPtr Renderer::createRenderViewJob(FrameGraphNode *node, int submitOrderIndex)
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

        MeshData *meshData = m_meshDataManager->data(command->m_meshData);
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
            // Either VAO has not been created for MeshData and RenderPass
            // Or there is no RenderPass
            // Tries to use vao for Mesh source and Default Technique
            qCDebug(Backend) << Q_FUNC_INFO << "VAO Handle is null";
            qCDebug(Backend) << Q_FUNC_INFO << "HShader " << command->m_shader;
            // Check if HShader exists. If it doesn't that means there is no RenderPass
            // Otherwise use a default renderpass name
            command->m_vao = m_vaoManager->lookupHandle(QPair<HMeshData, HShader>(command->m_meshData, command->m_shader));
            // Check if VAO pointer for the MeshData / RenderShader exists
            if (command->m_vao.isNull()) {
                qCDebug(Rendering) << Q_FUNC_INFO << "Allocating new VAO";
                command->m_vao = m_vaoManager->getOrAcquireHandle(QPair<HMeshData, HShader>(command->m_meshData, command->m_shader));
                *(m_vaoManager->data(command->m_vao)) = new QOpenGLVertexArrayObject();
            }
        }
        QOpenGLVertexArrayObject *vao = *(m_vaoManager->data(command->m_vao));
        Q_ASSERT(vao);

        // The VAO should be created only once for a MeshData and a ShaderProgram
        // Manager should have a VAO Manager that are indexed by MeshData and Shader
        // RenderCommand should have a handle to the corresponding VAO for the Mesh and Shader

        bool drawIndexed = !meshData->indexAttribute().isNull();

        //// Initialize GL
        if (!vao->isCreated()) {
            vao->create();
            vao->bind();

            qCDebug(Rendering) << Q_FUNC_INFO << "Creating new VAO";

            m_graphicsContext->activateShader(shader);

            // TO DO : Do that in a better / nicer way
            Q_FOREACH (QString nm, meshData->attributeNames()) {
                AttributePtr attr(meshData->attributeByName(nm).staticCast<Attribute>());
                if (command->m_parameterAttributeToShaderNames.contains(nm))
                    m_graphicsContext->specifyAttribute(command->m_parameterAttributeToShaderNames[nm], attr);
                else
                    qCWarning(Render::Rendering) << "Couldn't find a Parameter attribute named " << nm;
            }
            if (drawIndexed)
                m_graphicsContext->specifyIndices(meshData->indexAttribute().staticCast<Attribute>());
            vao->release();
        }

        //// Draw Calls
        // Set state
        if (command->m_stateSet != Q_NULLPTR)
            m_graphicsContext->setCurrentStateSet(command->m_stateSet);
        else
            m_graphicsContext->setCurrentStateSet(m_defaultDrawStateSet);
        m_graphicsContext->activateShader(shader);

        // All Uniforms for a pass are stored in the QUniformPack of the command
        // Uniforms for Effect, Material and Technique should already have been correctly resolved
        // at that point
        m_graphicsContext->setUniforms(command->m_uniforms);

        vao->bind();
        GLint primType = meshData->primitiveType();
        GLint primCount = meshData->primitiveCount();
        GLint indexType = drawIndexed ? meshData->indexAttribute()->type() : 0;

        if (drawIndexed)
            m_graphicsContext->drawElements(primType,
                                            primCount,
                                            indexType,
                                            reinterpret_cast<void*>(meshData->indexAttribute()->byteOffset()));
        else
            m_graphicsContext->drawArrays(primType, 0, primCount);

        int err = glGetError();
        if (err)
            qCWarning(Rendering) << "GL error after drawing mesh:" << QString::number(err, 16);

        vao->release();
    }
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
