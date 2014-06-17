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

#include <material.h>
#include <mesh.h>
#include <technique.h>
#include <shaderprogram.h>
#include <renderpass.h>
#include <shape.h>
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
#include "renderlogging.h"
#include "parameterbinder.h"

#include <Qt3DCore/cameralens.h>
#include <Qt3DCore/qabstracteffect.h>

#include <effect.h>
#include <QStack>
#include <QSurface>
#include <QElapsedTimer>

// For Debug purposes only
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

Renderer::Renderer()
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
    , m_renderQueues(new RenderQueues())
    , m_frameCount(0)
{
    m_textureProvider = new RenderTextureProvider;

    buildDefaultTechnique();
    buildDefaultMaterial();

    QLoggingCategory::setFilterRules(QString::fromUtf8( // multiline QStringLiteral doesn't compile on Windows...
                                                        "Qt3D.Render.*.debug=false\n"
                                                        //                                     "Qt3D.Render.Rendering.debug=true\n"
                                                        //                                     "Qt3D.Render.RenderNodes.debug=true\n"
                                                        //                                     "Qt3D.Render.Frontend.debug=true\n"
                                                        //                                     "qml.debug=true\n"
                                                        ));
}

void Renderer::buildDefaultTechnique()
{
    m_defaultTechnique = new Technique;
    m_defaultTechnique->setObjectName(QStringLiteral("default-technique"));

    ShaderProgram* defaultShader = new ShaderProgram;
    defaultShader->setVertexSourceFile(QStringLiteral(":/shaders/diffuse.vert"));
    defaultShader->setFragmentSourceFile(QStringLiteral(":/shaders/diffuse.frag"));
    defaultShader->setObjectName(QStringLiteral("DefaultShader"));

    m_defaultRenderShader = new RenderShader();
    m_defaultRenderShader->setPeer(defaultShader);

    RenderPass* basicPass = new RenderPass;
    basicPass->setShaderProgram(defaultShader);

    m_defaultDrawStateSet = new DrawStateSet;
    m_defaultDrawStateSet->addState(DepthTest::getOrCreate(GL_LESS));
    m_defaultDrawStateSet->addState(CullFace::getOrCreate(GL_BACK));
    basicPass->setStateSet(m_defaultDrawStateSet);

    m_defaultTechnique->addPass(basicPass);

    Parameter* vp = new Parameter(m_defaultTechnique, QStringLiteral("position"), Parameter::FloatVec3);
    vp->setMeshAttributeName(QStringLiteral("position"));
    m_defaultTechnique->addParameter(vp);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("position"), QStringLiteral("vertexPosition"), ParameterBinder::Attribute));

    Parameter* np = new Parameter(m_defaultTechnique, QStringLiteral("normal"), Parameter::FloatVec3);
    np->setMeshAttributeName(QStringLiteral("normal"));
    m_defaultTechnique->addParameter(np);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("normal"), QStringLiteral("vertexNormal"), ParameterBinder::Attribute));

    // matrix uniforms from standard
    Parameter* mvMat = new Parameter(m_defaultTechnique, QStringLiteral("modelView"), Parameter::FloatMat4);
    mvMat->setStandardUniform(Parameter::ModelView);
    m_defaultTechnique->addParameter(mvMat);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("modelView"), QStringLiteral("modelViewMatrix"), ParameterBinder::Uniform));

    Parameter* nMat = new Parameter(m_defaultTechnique, QStringLiteral("normalMat"), Parameter::FloatMat4);
    nMat->setStandardUniform(Parameter::ModelViewNormal);
    m_defaultTechnique->addParameter(nMat);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("normalMat"), QStringLiteral("normalMatrix"), ParameterBinder::Uniform));

    Parameter* mvpMat = new Parameter(m_defaultTechnique, QStringLiteral("mvp"), Parameter::FloatMat4);
    mvpMat->setStandardUniform(Parameter::ModelViewProjection);
    m_defaultTechnique->addParameter(mvpMat);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("mvp"), QStringLiteral("mvp"), ParameterBinder::Uniform));

    // diffuse lighting uniforms
    Parameter* lightPos = new Parameter(m_defaultTechnique, QStringLiteral("lightPos"), Parameter::FloatVec4, QVector4D(10.0f, 10.0f, 0.0f, 1.0f));
    m_defaultTechnique->addParameter(lightPos);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("lightPos"), QStringLiteral("lightPosition"), ParameterBinder::Uniform));

    Parameter* lightIntensity = new Parameter(m_defaultTechnique, QStringLiteral("lightIntensity"), Parameter::FloatVec3, QVector3D(0.5f, 0.5f, 0.5f));
    m_defaultTechnique->addParameter(lightIntensity);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("lightIntensity"), QStringLiteral("lightIntensity"), ParameterBinder::Uniform));

    Parameter* kd = new Parameter(m_defaultTechnique, QStringLiteral("diffuse"), Parameter::FloatVec3, QVector3D(1.0f, 0.5f, 0.0f));
    m_defaultTechnique->addParameter(kd);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("diffuse"), QStringLiteral("kd"), ParameterBinder::Uniform));

    Parameter* ka = new Parameter(m_defaultTechnique, QStringLiteral("ambient"), Parameter::FloatVec3, QVector3D(0.2f, 0.2f, 0.2f));
    m_defaultTechnique->addParameter(ka);
    basicPass->addBinding(new ParameterBinder(QStringLiteral("ambient"), QStringLiteral("ka"), ParameterBinder::Uniform));

}

void Renderer::buildDefaultMaterial()
{
    m_defaultMaterial = new Material();
    m_defaultMaterial->setObjectName(QStringLiteral("DefaultMaterial"));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("lightPos"), Parameter::FloatVec4, QVector4D(10.0f, 10.0f, 0.0f, 1.0f)));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("lightIntensity"), Parameter::FloatVec3, QVector3D(0.5f, 0.5f, 0.5f)));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("ambient"), Parameter::FloatVec3, QVector3D(0.2f, 0.2f, 0.2f)));
    m_defaultMaterial->addParameter(new Parameter(m_defaultMaterial, QStringLiteral("diffuse"), Parameter::FloatVec3, QVector3D(1.0f, 0.5f, 0.0f)));

    Effect* defEff = new Effect;
    defEff->addTechnique(m_defaultTechnique);
    m_defaultMaterial->setEffect(defEff);

    // TO DO : We use only the uniform values of the defaultTechnique
    // We should look for values in Effect, Technique and Material and Material values have preference over Technique and Effect
    // And Technique values have preference over Effect
    Q_FOREACH (ParameterBinder *binding, qobject_cast<RenderPass *>(m_defaultTechnique->renderPasses().first())->bindings()) {
        if (binding->bindingType() == ParameterBinder::Uniform) {
            Q_FOREACH (Parameter *param, m_defaultTechnique->parameters()) {
                if (param->name() == binding->parameterName()) {
                    if (!param->isStandardUniform()) {
                        if (param->datatype() >= Parameter::Float && param->datatype() <= Parameter::FloatMat4)
                            m_defaultUniformPack.setUniform(binding->shaderVariableName(), QUniformValue(QUniformValue::Float, param->value()));
                        else if (param->datatype() >= Parameter::Int)
                            m_defaultUniformPack.setUniform(binding->shaderVariableName(), QUniformValue(QUniformValue::Int, param->value()));
                    }
                    else {
                        m_defaultRenderShader->setStandardUniform(param->standardUniform(), binding->shaderVariableName());
                    }
                }
            }
        }
        else {
            m_defaultParameterToGLSLAttributeNames[binding->parameterName()] = binding->shaderVariableName();
        }
    }

}

Renderer::~Renderer()
{
    delete m_graphicsContext;
    delete m_textureProvider;
}

// Called in RenderAspect Thread context
void Renderer::initialize()
{

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
        m_renderQueues->pushFrameQueue();
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
    locker.unlock();

    if (m_graphicsContext == Q_NULLPTR || m_surface == Q_NULLPTR) {
        m_graphicsContext = new QGraphicsContext;
        QOpenGLContext* ctx = new QOpenGLContext;
        m_graphicsContext->setSurface(m_surface);
        QSurfaceFormat sf = m_surface->format();
        ctx->setFormat(sf);
        if (!ctx->create())
            qCWarning(Backend) << Q_FUNC_INFO << "OpenGL context creation failed";
        m_graphicsContext->setOpenGLContext(ctx);
    }

    QElapsedTimer timer;
    quint64 queueElapsed = 0;
    timer.start();
    while (m_renderQueues->queuedFrames() > 0)
    {
        QVector<Render::RenderView *> renderViews = m_renderQueues->popFrameQueue();
        int renderViewsCount = renderViews.size();
        quint64 frameElapsed = queueElapsed;

        m_graphicsContext->beginDrawing();

        for (int i = 0; i < renderViewsCount; i++) {
            // Set the Viewport
            m_graphicsContext->setViewport(renderViews[i]->viewport());
            // Set RenderTarget ...
            // Set the Camera
            RenderCamera *camera = renderViews[i]->camera();
            Q_ASSERT(camera);
            m_graphicsContext->setCamera(camera);
            // Initialize QGraphicsContext for drawing

            executeCommands(renderViews[i]->commands());
            frameElapsed = timer.elapsed() - frameElapsed;
            qCDebug(Rendering) << Q_FUNC_INFO << "Submitted Renderview " << i + 1 << "/" << renderViewsCount  << "in " << frameElapsed << "ms";
            frameElapsed = timer.elapsed();
        }
        m_graphicsContext->endDrawing();
        qDeleteAll(renderViews);
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

    FrameGraphVisitor visitor;
    visitor.traverse(m_frameGraphRoot, this, &renderBinJobs);
    m_renderQueues->setTargetRenderViewCount(renderBinJobs.size());
    return renderBinJobs;
}

// Called during while traversing the FrameGraph for each leaf node context of QAspectThread
QJobPtr Renderer::createRenderViewJob(FrameGraphNode *node, int submitOrderIndex)
{
    RenderViewJobPtr job(new RenderViewJob);
    job->setRenderer(this);
    job->setFrameGraphLeafNode(node);
    job->setSubmitOrderIndex(submitOrderIndex);
    return job;
}

// Called by RenderView->submit() in RenderThread context
void Renderer::executeCommands(const QVector<RenderCommand *> commands)
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

        bool drawIndexed = !meshData->indexAttr().isNull();

        //// Initialize GL
        if (!vao->isCreated()) {
            vao->create();
            vao->bind();

            qCDebug(Rendering) << Q_FUNC_INFO << "Creating new VAO";

            m_graphicsContext->activateShader(shader);

            // TO DO : Do that in a better / nicer way
            Q_FOREACH (QString nm, meshData->attributeNames()) {
                AttributePtr attr(meshData->attributeByName(nm));
                if (command->m_parameterAttributeToShaderNames.contains(nm))
                    m_graphicsContext->specifyAttribute(command->m_parameterAttributeToShaderNames[nm], attr);
                else
                    qCWarning(Render::Rendering) << "Couldn't find a Parameter attribute named " << nm;
            }
            if (drawIndexed)
                m_graphicsContext->specifyIndices(meshData->indexAttr());
            vao->release();
        }

        //// Draw Calls
        // Set state
        if (command->m_stateSet != Q_NULLPTR)
            m_graphicsContext->setCurrentStateSet(command->m_stateSet);
        else
            m_graphicsContext->setCurrentStateSet(m_defaultDrawStateSet);
        m_graphicsContext->activateShader(shader);
        m_graphicsContext->setModelMatrix(command->m_worldMatrix);
        //        m_graphicsContext->setActiveMaterial(mat);

        // All Uniforms for a pass are stored in the QUniformPack of the command
        // Uniforms for Effect, Material and Technique should already have been correctly resolved
        // at that point
        command->m_uniforms.apply(m_graphicsContext);

        vao->bind();
        GLint primType = meshData->primitiveType();
        GLint primCount = meshData->primitiveCount();
        GLint indexType = drawIndexed ? meshData->indexAttr()->type() : 0;

        if (drawIndexed)
            m_graphicsContext->drawElements(primType,
                                            primCount,
                                            indexType,
                                            reinterpret_cast<void*>(meshData->indexAttr()->byteOffset()));
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
