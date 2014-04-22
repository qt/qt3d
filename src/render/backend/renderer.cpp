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

#include <nodevisitor.h>
#include <entity.h>
#include <scene.h>

#include "framegraphvisitor.h"
#include "cameraselectornode.h"
#include "renderpassfilternode.h"
#include "techniquefilternode.h"
#include "viewportnode.h"

#include "jobs/renderviewjob.h"

#include <material.h>
#include <mesh.h>
#include <effect.h>
#include <technique.h>
#include <shaderprogram.h>
#include <renderpass.h>
#include <cameralens.h>
#include <shape.h>

#include <cameramanager.h>
#include <meshmanager.h>
#include <rendermesh.h>
#include <renderqueues.h>
#include <renderbin.h>
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

#include "renderlogging.h"
#include <QStack>
#include <QSurface>
#include <QElapsedTimer>

// For Debug purposes only
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace {

class TemporaryBackendBuilder : public NodeVisitor
{
public:
    TemporaryBackendBuilder(Render::Renderer* r) :
        m_renderer(r)
    {

    }

protected:
    virtual void visitEntity(Entity* ent)
    {
        ent->update();

        bool haveMaterial = false;
        QList<Material*> mats = ent->componentsOfType<Material>();
        if (!mats.empty()) {
            haveMaterial = true;
            m_materialStack.push(mats.front());
        }

        // TMP REPLACE WITH TRANSFORM SCENE MATRIX
        QMatrix4x4 sceneMatrix;

        foreach (Mesh* mesh, ent->componentsOfType<Mesh>()) {
            Material* mat = m_materialStack.empty() ? NULL : m_materialStack.top();
            m_renderer->buildMeshes(mesh, mat, sceneMatrix);
        }

        foreach (Shape* shape, ent->componentsOfType<Shape>()) {
            Material* mat = m_materialStack.empty() ? NULL : m_materialStack.top();
            m_renderer->buildShape(shape, mat, sceneMatrix);
        }

        //        foreach (Camera* cam, ent->componentsOfType<Camera>()) {
        //            m_renderer->foundCamera(cam, sceneMatrix);
        //        }

        NodeVisitor::visitEntity(ent);

        if (haveMaterial)
            m_materialStack.pop();
    }

private:
    QStack<Material*> m_materialStack;
    Render::Renderer* m_renderer;
};

} // anonymous namespace

namespace Render {

Renderer::Renderer()
    : m_rendererAspect(0)
    , m_frameGraphRoot(0)
    , m_graphicsContext(0)
    , m_surface(0)
    , m_meshManager(new MeshManager())
    , m_cameraManager(new CameraManager())
    , m_renderNodesManager(new RenderNodesManager())
    , m_renderQueues(new RenderQueues())
    , m_frameCount(0)
{
    m_temporaryAllBin = NULL;

    //        m_frameTimer = new QTimer(this);
    //        connect(m_frameTimer, SIGNAL(timeout()), this, SLOT(onFrame()));
    //        m_frameTimer->setInterval(1000);

    m_textureProvider = new RenderTextureProvider;

    buildDefaultTechnique();
    buildDefaultMaterial();

}

void Renderer::buildDefaultTechnique()
{
    m_defaultTechnique = new Technique;
    m_defaultTechnique->setObjectName(QStringLiteral("default-technique"));

    ShaderProgram* defaultShader = new ShaderProgram;
    defaultShader->setVertexSourceFile(QStringLiteral(":/shaders/diffuse.vert"));
    defaultShader->setFragmentSourceFile(QStringLiteral(":/shaders/diffuse.frag"));
    defaultShader->setObjectName(QStringLiteral("DefaultShader"));

    RenderPass* basicPass = new RenderPass;
    basicPass->setShaderProgram(defaultShader);

    DrawStateSet* ss = new DrawStateSet;
    ss->addState(DepthTest::getOrCreate(GL_LESS));
    ss->addState(CullFace::getOrCreate(GL_BACK));
    basicPass->setStateSet(ss);

    m_defaultTechnique->addPass(basicPass);

    Parameter* vp = new Parameter(m_defaultTechnique, QStringLiteral("position"), GL_FLOAT_VEC3);
    vp->setMeshAttributeName(QStringLiteral("position"));
    m_defaultTechnique->addParameter(vp);
    basicPass->addAttributeBinding(vp, QStringLiteral("vertexPosition"));

    Parameter* np = new Parameter(m_defaultTechnique, QStringLiteral("normal"), GL_FLOAT_VEC3);
    np->setMeshAttributeName(QStringLiteral("normal"));
    m_defaultTechnique->addParameter(np);
    basicPass->addAttributeBinding(np, QStringLiteral("vertexNormal"));

    // matrix uniforms from standard
    Parameter* mvMat = new Parameter(m_defaultTechnique, QStringLiteral("modelView"), GL_FLOAT_MAT4);
    mvMat->setStandardUniform(Parameter::ModelView);
    m_defaultTechnique->addParameter(mvMat);
    basicPass->addUniformBinding(mvMat, QStringLiteral("modelViewMatrix"));

    Parameter* nMat = new Parameter(m_defaultTechnique, QStringLiteral("normalMat"), GL_FLOAT_MAT3);
    nMat->setStandardUniform(Parameter::ModelViewNormal);
    m_defaultTechnique->addParameter(nMat);
    basicPass->addUniformBinding(nMat, QStringLiteral("normalMatrix"));

    Parameter* mvpMat = new Parameter(m_defaultTechnique, QStringLiteral("mvp"), GL_FLOAT_MAT4);
    mvpMat->setStandardUniform(Parameter::ModelViewProjection);
    m_defaultTechnique->addParameter(mvpMat);
    basicPass->addUniformBinding(mvpMat, QStringLiteral("mvp"));

    // diffuse lighting uniforms
    Parameter* lightPos = new Parameter(m_defaultTechnique, QStringLiteral("lightPos"), GL_FLOAT_VEC4);
    m_defaultTechnique->addParameter(lightPos);
    basicPass->addUniformBinding(lightPos, QStringLiteral("lightPosition"));

    Parameter* lightIntensity = new Parameter(m_defaultTechnique, QStringLiteral("lightIntensity"), GL_FLOAT_VEC3);
    m_defaultTechnique->addParameter(lightIntensity);
    basicPass->addUniformBinding(lightIntensity, QStringLiteral("lightIntensity"));

    Parameter* kd = new Parameter(m_defaultTechnique, QStringLiteral("kd"), GL_FLOAT_VEC3);
    m_defaultTechnique->addParameter(kd);
    basicPass->addUniformBinding(kd, QStringLiteral("kd"));

    Parameter* ka = new Parameter(m_defaultTechnique, QStringLiteral("ka"), GL_FLOAT_VEC3);
    m_defaultTechnique->addParameter(ka);
    basicPass->addUniformBinding(ka, QStringLiteral("ka"));
}

void Renderer::buildDefaultMaterial()
{
    m_defaultMaterial = new Material;
    m_defaultMaterial->setObjectName(QStringLiteral("DefaultMaterial"));
    m_defaultMaterial->setParameter(QStringLiteral("lightPos"), QVector4D(10.0f, 10.0f, 0.0f, 1.0f));
    m_defaultMaterial->setParameter(QStringLiteral("lightIntensity"), QVector3D(0.5f, 0.5f, 0.5f));
    m_defaultMaterial->setParameter(QStringLiteral("ka"), QVector3D(0.2f, 0.2f, 0.2f));
    m_defaultMaterial->setParameter(QStringLiteral("kd"), QVector3D(1.0f, 0.5f, 0.0f));

    Effect* defEff = new Effect;
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

void Renderer::setSceneObject(Qt3D::Node *obj)
{
    Scene* sc = Scene::findInTree(obj);
    if (sc) {
        setSceneGraphRoot(sc);
    } else
        qCWarning(Backend) << Q_FUNC_INFO << "couldn't find Scene object";
}

// QAspectThread context
void Renderer::setSceneGraphRoot(Node *sgRoot)
{
    Q_ASSERT(sgRoot);

    // TODO: Unify this to one root node and scene builder
    Scene *scene = Scene::findInTree(sgRoot);
    if (scene)
    {
        // Scene needs to be built with scene parsers
        // For each scene parser, check whether scene source format
        // is supported, filter if there is a preference toward which
        // parser should parse the file if two parsers support the same
        // format

        qCDebug(Backend) << "building temporary backend";
        m_sceneGraphRoot = scene;
        TemporaryBackendBuilder tbb(this);
        tbb.traverse(m_sceneGraphRoot);
        qCDebug(Backend) << "done building backend";

        //        QMetaObject::invokeMethod(m_frameTimer, "start");
    } else {
        // Test new scene builder
        m_sceneGraphRoot = sgRoot;
        RenderSceneBuilder builder(this);
        builder.traverse(m_sceneGraphRoot);
        RenderNode *root = builder.rootNode();
        if (!root)
            qCWarning(Backend) << "Failed to build render scene";
        m_renderSceneRoot = root;

        qCDebug(Backend) << Q_FUNC_INFO << "DUMPING SCENE";
        root->dump();

        // Queue up jobs to do initial full updates of
        //  - Mesh loading + bounding volume calculation
        //  - Local bounding volumes
        //  - World transforms
        //  - World bounding volumes
    }
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

void Renderer::setDefaultTechnique(Technique *t)
{
    Effect* defEff = m_defaultMaterial->effect();
    bool materialWasUsing = (defEff->techniques().front() ==
                             m_defaultTechnique);

    m_defaultTechnique =  t;
    if (materialWasUsing) {
        defEff->clearTechniques();
        defEff->addTechnique(t);
    }
}

void Renderer::setDefaultMaterial(Material *mat)
{
    m_defaultMaterial = mat;
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
    // begin hack renderer
    //    qDebug() << Q_FUNC_INFO << QThread::currentThread();


    //    if (!m_renderCamera)
    //        return;

    // If fist time scenegraph is set
    //    m_waitForRenderViewsJobsCondition.wakeAll();


    //    m_renderCamera->sync();

    //    m_graphicsContext->beginDrawing();

    //    foreach (Drawable* dr, m_initList)
    //        dr->initializeGL(m_graphicsContext);
    //    m_initList.clear();

    //    int err = glGetError();
    //    if (err)
    //        qWarning() << "GL error before submitting bins:" << err;

    //    m_temporaryAllBin->sendDrawingCommands(m_graphicsContext);

    //    m_graphicsContext->endDrawing();

    // end hack renderer
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
    qCDebug(Backend) << Q_FUNC_INFO << 1 << QThread::currentThread();
    QMutexLocker locker(&m_mutex);
    m_submitRenderViewsCondition.wait(locker.mutex());
    // Allow RenderViewJobs to be processed for the next frame
    // Without having to wait for the current RenderViews to be submitted
    // as long as there is available space in the renderQueues.
    // Otherwise it waits for submission to be done so as to never miss
    // Any important state change that could be in a RenderView
    locker.unlock();

    qCDebug(Backend) << Q_FUNC_INFO << 2 << QThread::currentThread();
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

    while (m_renderQueues->queuedFrames() > 0)
    {
        QVector<Render::RenderView *> renderViews = m_renderQueues->popFrameQueue();
        QElapsedTimer timer;
        timer.start();
        for (int i = 0; i < renderViews.size(); i++)
            executeCommands(renderViews[i]);
        qCDebug(Backend) << Q_FUNC_INFO << "Submission took " << timer.elapsed() << "ms";
        qDeleteAll(renderViews);
        m_frameCount++;
    }
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
    //    qDebug() << Q_FUNC_INFO << QThread::currentThread();
    RenderViewJobPtr job(new RenderViewJob);
    job->setRenderer(this);
    job->setFrameGraphLeafNode(node);
    job->setSubmitOrderIndex(submitOrderIndex);
    return job;
}

// Called by RenderView->submit() in RenderThread context
void Renderer::executeCommands(const RenderView *renderView)
{
    qCDebug(Backend) << Q_FUNC_INFO;
    // Render drawing commands

    // Use the graphicscontext to submit the commands to the underlying
    // graphics API (OpenGL)
    QVector<RenderCommand *> commands = renderView->commands();

    // Set the Viewport
    m_graphicsContext->setViewport(renderView->viewport());
    // Set the Camera

    RenderCamera *camera = renderView->camera();
    Q_ASSERT(camera);
    m_graphicsContext->setCamera(camera);
    // Initialize QGraphicsContext for drawing
    m_graphicsContext->beginDrawing();
    // Set RenderTarget ...

    RenderMaterial *mat = getOrCreateMaterial(m_defaultMaterial);
    RenderTechnique *technique = mat->technique();

    Q_FOREACH (RenderCommand *command, commands) {

        //// Initialize GL
        if (command->m_meshData.attributeNames().empty())
            continue ;
        command->m_vao.create(); // At the moment VAO isn't really needed
        command->m_vao.bind();

        bool drawIndexed = !command->m_meshData.indexAttr().isNull();
        m_graphicsContext->activateShader(technique->shaderForPass(0));

        foreach (QString nm, command->m_meshData.attributeNames()) {
            AttributePtr attr(command->m_meshData.attributeByName(nm));
            QString glsl = technique->glslNameForMeshAttribute(0, nm);
            if (glsl.isEmpty())
                continue; // not used in this pass
            m_graphicsContext->specifyAttribute(glsl, attr);
        }
        if (drawIndexed)
            m_graphicsContext->specifyIndices(command->m_meshData.indexAttr());
        command->m_vao.release();

        //// Draw Calls
        // Set state
        m_graphicsContext->setCurrentStateSet(technique->stateSetForPass(0));
        //        m_graphicsContext->activateShader(technique->shaderForPass(0));
        m_graphicsContext->setModelMatrix(command->m_worldMatrix);
        m_graphicsContext->setActiveMaterial(mat);
        mat->setUniformsForPass(0, m_graphicsContext);

        command->m_vao.bind();
        GLint primType = command->m_meshData.primitiveType();
        GLint primCount = command->m_meshData.primitiveCount();
        GLint indexType = drawIndexed ? command->m_meshData.indexAttr()->type() : 0;

        if (drawIndexed)
            m_graphicsContext->drawElements(primType,
                                            primCount,
                                            indexType,
                                            reinterpret_cast<void*>(command->m_meshData.indexAttr()->byteOffset()));
        else
            m_graphicsContext->drawArrays(primType, 0, primCount);

        int err = glGetError();
        if (err)
            qCWarning(Backend) << "GL error after drawing mesh:" << QString::number(err, 16);

        command->m_vao.release();

        command->m_vao.destroy();
    }
    m_graphicsContext->endDrawing();
}

RenderTechnique* Renderer::techniqueForMaterial(Material* mat)
{
    Effect* eff = mat->effect();
    Technique *tech;
    if (eff) {
        Q_ASSERT(!eff->techniques().empty());

        // FIXME - technique selection happens in here, now we know
        // the renderer characteristics
        tech = eff->techniques().front();
    } else {
        tech = m_defaultTechnique;
    }

    if (!m_techniqueHash.contains(tech)) {
        RenderTechnique* rtech = createTechnique(tech);
        m_techniqueHash[tech] = rtech;
    }

    return m_techniqueHash.value(tech);
}

RenderShader* Renderer::getOrCreateShader(ShaderProgram* sp)
{
    if (!m_shaderHash.contains(sp)) {
        RenderShader* rs = new RenderShader();
        rs->setPeer(sp);
        m_shaderHash[sp] = rs;
    }

    return m_shaderHash.value(sp);
}

RenderBin* Renderer::getOrCreateBinForPass(Technique* t, RenderPass* p)
{
    Q_UNUSED(t);
    Q_UNUSED(p);

    // TODO - select bins by priority

    if (!m_temporaryAllBin)
        m_temporaryAllBin = new OpaqueBin;

    return m_temporaryAllBin;
}

RenderTechnique* Renderer::createTechnique(Technique* tech)
{
    RenderTechnique* rt = new RenderTechnique();
    rt->setPeer(tech);
    for (unsigned int p=0; p<rt->passCount(); ++p) {
        RenderPass* frontendPass = tech->renderPasses().at(p);
        RenderShader* rshader = getOrCreateShader(frontendPass->shaderProgram());
        rt->setShaderForPass(p, rshader);

        RenderBin* bin = getOrCreateBinForPass(tech, frontendPass);
        rt->setBinForPass(p, bin);
    }

    return rt;
}

RenderMaterial* Renderer::getOrCreateMaterial(Material* mat)
{
    if (!m_materialHash.contains(mat)) {
        qCDebug(Backend) << "creating render material for mat:" << mat->objectName();

        RenderMaterial* rmat = new RenderMaterial();
        rmat->setRendererAspect(m_rendererAspect);
        rmat->setPeer(mat);
        rmat->setTextureProvider(m_textureProvider);
        m_materialHash[mat] = rmat;

        // specifying technique will make the material sync parameter
        // state from its peeer material.
        rmat->setTechnique(techniqueForMaterial(mat));
    }

    return m_materialHash.value(mat);
}

void Renderer::buildMeshes(Mesh* mesh, Material* mat, const QMatrix4x4& mm)
{
    if (mat == NULL) {
        mat = m_defaultMaterial;
    }

    RenderMaterial* rmat = getOrCreateMaterial(mat);
    RenderTechnique* t = rmat->technique();

    for (unsigned int p=0; p<t->passCount(); ++p) {
        RenderBin* bin = t->binForPass(p);

        RenderMesh* rmesh = new RenderMesh();
        rmesh->setPeer(mesh);
        rmesh->setData(mesh->data());
        rmesh->setTechniqueAndPass(t, p);
        rmesh->setModelMatrix(mm);
        rmesh->setMaterial(rmat);

        m_initList.push_back(rmesh);
        bin->addDrawable(rmesh);

    } // of technique pass iteration
}

void Renderer::buildShape(Shape* shape, Material* mat, const QMatrix4x4& mm)
{
    if (mat == NULL) {
        mat = m_defaultMaterial;
    }

    MeshDataPtr shapeMeshData = shape->data();
    RenderMaterial* rmat = getOrCreateMaterial(mat);
    RenderTechnique* t = rmat->technique();

    for (unsigned int p=0; p<t->passCount(); ++p) {
        RenderBin* bin = t->binForPass(p);

        RenderMesh* rmesh = new RenderMesh();
        rmesh->setData(shapeMeshData);
        rmesh->setTechniqueAndPass(t, p);
        rmesh->setModelMatrix(mm);
        rmesh->setMaterial(rmat);

        m_initList.push_back(rmesh);
        bin->addDrawable(rmesh);

    } // of technique pass iteration
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
