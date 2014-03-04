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

#include "framegraph/framegraphnode.h"
#include "framegraph/framegraphvisitor.h"

#include <material.h>
#include <mesh.h>
#include <effect.h>
#include <technique.h>
#include <shaderprogram.h>
#include <renderpass.h>
#include <camera.h>
#include <shape.h>

#include <meshmanager.h>
#include <rendermesh.h>
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

#include <QStack>
#include <QDebug>
#include <QSurface>

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


        foreach (Mesh* mesh, ent->componentsOfType<Mesh>()) {
            Material* mat = m_materialStack.empty() ? NULL : m_materialStack.top();
            m_renderer->buildMeshes(mesh, mat, ent->sceneMatrix());
        }

        foreach (Shape* shape, ent->componentsOfType<Shape>()) {
            Material* mat = m_materialStack.empty() ? NULL : m_materialStack.top();
            m_renderer->buildShape(shape, mat, ent->sceneMatrix());
        }

        foreach (Camera* cam, ent->componentsOfType<Camera>()) {
            m_renderer->foundCamera(cam, ent->sceneMatrix());
        }

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
    , m_camera(0)
    , m_renderCamera(0)
    , m_meshManager(new MeshManager(this))
{
    m_temporaryAllBin = NULL;
    m_graphicsContext = new QGraphicsContext;
    // Done in set surface
//    m_graphicsContext->setOpenGLContext(new QOpenGLContext);

    m_frameTimer = new QTimer(this);
    connect(m_frameTimer, SIGNAL(timeout()), this, SLOT(onFrame()));
    m_frameTimer->setInterval(16);

    m_textureProvider = new RenderTextureProvider;

    buildDefaultTechnique();
    buildDefaultMaterial();
}

void Renderer::buildDefaultTechnique()
{
    m_defaultTechnique = new Technique;
    m_defaultTechnique->setObjectName("default-technique");

    ShaderProgram* defaultShader = new ShaderProgram;
    defaultShader->setVertexSourceFile(":/shaders/diffuse.vert");
    defaultShader->setFragmentSourceFile(":/shaders/diffuse.frag");
    defaultShader->setObjectName("DefaultShader");

    RenderPass* basicPass = new RenderPass;
    basicPass->setShaderProgram(defaultShader);

    DrawStateSet* ss = new DrawStateSet;
    ss->addState(DepthTest::getOrCreate(GL_LESS));
    ss->addState(CullFace::getOrCreate(GL_BACK));
    basicPass->setStateSet(ss);

    m_defaultTechnique->addPass(basicPass);

    Parameter* vp = new Parameter(m_defaultTechnique, "position", GL_FLOAT_VEC3);
    vp->setMeshAttributeName("position");
    m_defaultTechnique->addParameter(vp);
    basicPass->addAttributeBinding(vp, "vertexPosition");

    Parameter* np = new Parameter(m_defaultTechnique, "normal", GL_FLOAT_VEC3);
    np->setMeshAttributeName("normal");
    m_defaultTechnique->addParameter(np);
    basicPass->addAttributeBinding(np, "vertexNormal");

    // matrix uniforms from standard
    Parameter* mvMat = new Parameter(m_defaultTechnique, "modelView", GL_FLOAT_MAT4);
    mvMat->setStandardUniform(Parameter::ModelView);
    m_defaultTechnique->addParameter(mvMat);
    basicPass->addUniformBinding(mvMat, "modelViewMatrix");

    Parameter* nMat = new Parameter(m_defaultTechnique, "normalMat", GL_FLOAT_MAT3);
    nMat->setStandardUniform(Parameter::ModelViewNormal);
    m_defaultTechnique->addParameter(nMat);
    basicPass->addUniformBinding(nMat, "normalMatrix");

    Parameter* mvpMat = new Parameter(m_defaultTechnique, "mvp", GL_FLOAT_MAT4);
    mvpMat->setStandardUniform(Parameter::ModelViewProjection);
    m_defaultTechnique->addParameter(mvpMat);
    basicPass->addUniformBinding(mvpMat, "mvp");

    // diffuse lighting uniforms
    Parameter* lightPos = new Parameter(m_defaultTechnique, "lightPos", GL_FLOAT_VEC4);
    m_defaultTechnique->addParameter(lightPos);
    basicPass->addUniformBinding(lightPos, "lightPosition");

    Parameter* lightIntensity = new Parameter(m_defaultTechnique, "lightIntensity", GL_FLOAT_VEC3);
    m_defaultTechnique->addParameter(lightIntensity);
    basicPass->addUniformBinding(lightIntensity, "lightIntensity");

    Parameter* kd = new Parameter(m_defaultTechnique, "kd", GL_FLOAT_VEC3);
    m_defaultTechnique->addParameter(kd);
    basicPass->addUniformBinding(kd, "kd");

    Parameter* ka = new Parameter(m_defaultTechnique, "ka", GL_FLOAT_VEC3);
    m_defaultTechnique->addParameter(ka);
    basicPass->addUniformBinding(ka, "ka");
}

void Renderer::buildDefaultMaterial()
{
    m_defaultMaterial = new Material;
    m_defaultMaterial->setObjectName("DefaultMaterial");
    m_defaultMaterial->setParameter("lightPos", QVector4D(10.0f, 10.0f, 0.0f, 1.0f));
    m_defaultMaterial->setParameter("lightIntensity", QVector3D(0.5f, 0.5f, 0.5f));
    m_defaultMaterial->setParameter("ka", QVector3D(0.2f, 0.2f, 0.2f));
    m_defaultMaterial->setParameter("kd", QVector3D(1.0f, 0.5f, 0.0f));

    Effect* defEff = new Effect;
    defEff->addTechnique(m_defaultTechnique);
    m_defaultMaterial->setEffect(defEff);
}

Renderer::~Renderer()
{
    delete m_graphicsContext;
    delete m_textureProvider;
}

void Renderer::initialize()
{

}

void Renderer::setFrameGraphRoot(FrameGraphNode *fgRoot)
{
    qDebug() << Q_FUNC_INFO;
    m_frameGraphRoot = fgRoot;
}

FrameGraphNode *Renderer::frameGraphRoot() const
{
    return m_frameGraphRoot;
}

void Renderer::setSceneObject(Qt3D::Node *obj)
{
    Scene* sc = Scene::findInTree(obj);
    if (sc) {
        setSceneGraphRoot(sc);
    } else
        qWarning() << Q_FUNC_INFO << "couldn't find Scene object";
}

void Renderer::onFrame()
{
    render();
}

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

        qDebug() << "building temporary backend";
        m_sceneGraphRoot = scene;
        TemporaryBackendBuilder tbb(this);
        tbb.traverse(m_sceneGraphRoot);
        qDebug() << "done building backend";

        QMetaObject::invokeMethod(m_frameTimer, "start");
    } else {
        // Test new scene builder
        m_sceneGraphRoot = sgRoot;

        RenderSceneBuilder builder(this);
        builder.traverse(m_sceneGraphRoot);
        RenderNode *root = builder.rootNode();
        if (!root)
            qWarning() << "Failed to build render scene";
        m_renderSceneRoot = root;

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

void Renderer::setCamera(Camera *cam)
{
    m_camera = cam;
    m_renderCamera = new RenderCamera(m_rendererAspect);
    m_renderCamera->setPeer(cam);
    m_renderCamera->setProjection(cam->projectionMatrix());

    m_graphicsContext->setCamera(m_renderCamera);
}

void Renderer::setSurface(QSurface* s)
{
    qDebug() << Q_FUNC_INFO;
    m_graphicsContext->setSurface(s);

    QSurfaceFormat sf = s->format();

    QOpenGLContext* ctx = new QOpenGLContext;
    ctx->setFormat(sf);
    if (!ctx->create()) {
        qWarning() << Q_FUNC_INFO << "OpenGL context creation failed";
    }

    m_graphicsContext->setOpenGLContext(ctx);
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
    // Traverse the framegraph
    //   FrameGraphVisitor visitor;
    //   visitor.traverse(m_frameGraphRoot, this);



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

    // begin hack renderer
    if (!m_renderCamera)
        return;

    m_renderCamera->sync();

    m_graphicsContext->beginDrawing();

    foreach (Drawable* dr, m_initList)
        dr->initializeGL(m_graphicsContext);
    m_initList.clear();

    int err = glGetError();
    if (err)
        qWarning() << "GL error before submitting bins:" << err;

    m_temporaryAllBin->sendDrawingCommands(m_graphicsContext);

    m_graphicsContext->endDrawing();

    // end hack renderer
}

void Renderer::doRender()
{
    // Render using current device state and renderer configuration
    qDebug() << Q_FUNC_INFO;
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
        RenderShader* rs = new RenderShader(sp);
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
    RenderTechnique* rt = new RenderTechnique(tech);
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
        qDebug() << "creating render material for mat:" << mat->objectName();

        RenderMaterial* rmat = new RenderMaterial(m_rendererAspect);
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

        RenderMesh* rmesh = new RenderMesh(mesh);
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

        RenderMesh* rmesh = new RenderMesh(NULL);
        rmesh->setData(shapeMeshData);
        rmesh->setTechniqueAndPass(t, p);
        rmesh->setModelMatrix(mm);
        rmesh->setMaterial(rmat);

        m_initList.push_back(rmesh);
        bin->addDrawable(rmesh);

    } // of technique pass iteration
}


void Renderer::foundCamera(Camera *cam, const QMatrix4x4 &mm)
{
    Q_UNUSED(mm);
    cam->setViewMatrix(mm.inverted());
    setCamera(cam);
}

} // namespace Render
} // namespace Qt3D
