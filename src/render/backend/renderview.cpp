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

#include "renderview.h"
#include "qmaterial.h"
#include "renderer.h"
#include "rendercamera.h"
#include "rendercommand.h"
#include "renderentity.h"
#include "meshdatamanager.h"
#include "vaomanager.h"
#include "meshmanager.h"
#include "meshdata.h"
#include "cameramanager.h"
#include "rendernodesmanager.h"
#include "materialmanager.h"
#include "techniquemanager.h"
#include "shadermanager.h"
#include <cameraselectornode.h>
#include <framegraphnode.h>
#include <renderpassfilternode.h>
#include <techniquefilternode.h>
#include <viewportnode.h>
#include <layerfilternode.h>
#include "rendereffect.h"
#include "effectmanager.h"
#include "renderlogging.h"
#include "renderpassmanager.h"
#include "renderrenderpass.h"
#include "parametermapper.h"
#include "parameter.h"
#include "texturemanager.h"
#include "texture.h"
#include "renderlayer.h"
#include "layermanager.h"

#include <Qt3DCore/entity.h>
#include <Qt3DCore/qabstracteffect.h>
#include <Qt3DCore/qabstracttechnique.h>
#include <Qt3DCore/qframeallocator.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderView::standardUniformsPFuncsHash RenderView::m_standardUniformSetters = RenderView::initializeStandardUniformSetters();

RenderView::standardUniformsPFuncsHash RenderView::initializeStandardUniformSetters()
{
    RenderView::standardUniformsPFuncsHash setters;

    setters[Parameter::ModelMatrix] = &RenderView::modelMatrix;
    setters[Parameter::ViewMatrix] = &RenderView::viewMatrix;
    setters[Parameter::ProjectionMatrix] = &RenderView::projectionMatrix;
    setters[Parameter::ModelView] = &RenderView::modelViewMatrix;
    setters[Parameter::ModelViewProjection] = &RenderView::modelViewProjectionMatrix;
    setters[Parameter::ModelInverse] = &RenderView::inverseModelMatrix;
    setters[Parameter::ViewInverse] = &RenderView::inverseViewMatrix;
    setters[Parameter::ProjectionInverse] = &RenderView::inverseProjectionMatrix;
    setters[Parameter::ModelViewInverse] = &RenderView::inverseModelViewMatrix;
    setters[Parameter::ModelViewProjectionInverse] = &RenderView::inverseModelViewProjectionMatrix;
    setters[Parameter::ModelNormal] = &RenderView::modelNormalMatrix;
    setters[Parameter::ModelViewNormal] = &RenderView::modelViewNormalMatrix;

    return setters;
}

QUniformValue *RenderView::modelMatrix(RenderCamera *, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(model);
    return t;
}

QUniformValue *RenderView::viewMatrix(RenderCamera *c, const QMatrix4x4 &) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(c->viewMatrix());
    return t;
}

QUniformValue *RenderView::projectionMatrix(RenderCamera *c, const QMatrix4x4 &) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(c->projection());
    return t;
}

QUniformValue *RenderView::modelViewMatrix(RenderCamera *c, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(c->viewMatrix() * model);
    return t;
}

QUniformValue *RenderView::modelViewProjectionMatrix(RenderCamera *c, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(c->projection() * c->viewMatrix() * model);
    return t;
}

QUniformValue *RenderView::inverseModelMatrix(RenderCamera *, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(model.inverted());
    return t;
}

QUniformValue *RenderView::inverseViewMatrix(RenderCamera *c, const QMatrix4x4 &) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(c->viewMatrix().inverted());
    return t;
}

QUniformValue *RenderView::inverseProjectionMatrix(RenderCamera *c, const QMatrix4x4 &) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue(c->projection().inverted());
    return t;
}

QUniformValue *RenderView::inverseModelViewMatrix(RenderCamera *c, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue((c->viewMatrix() * model).inverted());
    return t;
}

QUniformValue *RenderView::inverseModelViewProjectionMatrix(RenderCamera *c, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix4x4> *t = m_allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
    t->setValue((c->projection() * c->viewMatrix() * model).inverted());
    return t;
}

QUniformValue *RenderView::modelNormalMatrix(RenderCamera *, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix3x3> *t = m_allocator->allocate<SpecifiedUniform<QMatrix3x3> >();
    t->setValue(model.normalMatrix());
    return t;
}

QUniformValue *RenderView::modelViewNormalMatrix(RenderCamera *c, const QMatrix4x4 &model) const
{
    SpecifiedUniform<QMatrix3x3> *t = m_allocator->allocate<SpecifiedUniform<QMatrix3x3> >();
    t->setValue((c->viewMatrix() * model).normalMatrix());
    return t;
}

RenderView::RenderView()
    : m_renderer(Q_NULLPTR)
    , m_allocator(Q_NULLPTR)
    , m_renderCamera(Q_NULLPTR)
    , m_techniqueFilter(0)
    , m_passFilter(0)
    , m_commands()
{
}

void RenderView::setConfigFromFrameGraphLeafNode(FrameGraphNode *fgLeaf)
{
    // The specific RenderPass to be used is also dependent upon the Effect and TechniqueFilter
    // which is referenced by the Material which is referenced by the RenderMesh. So we can
    // only store the filter info in the RenderView structure and use it to do the resolving
    // when we build the RenderCommand list.
    FrameGraphNode *node = fgLeaf;
    while (node != Q_NULLPTR) {
        FrameGraphNode::FrameGraphNodeType type = node->nodeType();
        switch (type) {
        case FrameGraphNode::CameraSelector: {
            CameraSelector *cameraSelector = static_cast<CameraSelector *>(node);
            Entity *cameraEntity = cameraSelector->cameraEntity();
            if (cameraEntity != Q_NULLPTR) {
                m_renderCamera = m_renderer->cameraManager()->lookupResource(cameraEntity->uuid());
                RenderEntity *tmpCamNode = m_renderer->renderNodesManager()->lookupResource(cameraEntity->uuid());
                if (m_renderCamera && tmpCamNode)
                    m_renderCamera->setViewMatrix(*tmpCamNode->worldTransform());
            }
            break;
        }

        case FrameGraphNode::LayerFilter:
            m_layers << static_cast<LayerFilterNode *>(node)->layers();
            break;

        case FrameGraphNode::RenderPassFilter:
            m_passFilter = static_cast<RenderPassFilter *>(node);
            break;

        case FrameGraphNode::RenderTarget:
            qCWarning(Backend) << "RenderTarget not implemented yet";
            break;

        case FrameGraphNode::TechniqueFilter:
            m_techniqueFilter = static_cast<TechniqueFilter *>(node);
            break;

        case FrameGraphNode::Viewport:
            // If the Viewport has already been set in a lower node
            // Make it so that the new viewport is actually
            // a subregion relative to that of the parent viewport
            computeViewport(static_cast<ViewportNode *>(node));
            break;

        default:
            // Should never get here
            qCWarning(Backend) << "Unhandled FrameGraphNode type";
        }

        node = node->parent();
    }
}

void RenderView::sort()
{
    // TODO: Implement me!

    // The goal here is to sort RenderCommand by :
    // 1) Shader
    // 2) DrawStateSet
    // 2) Texture

}

void RenderView::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderView::setAllocator(QFrameAllocator *allocator)
{
    m_allocator = allocator;
}

// Traverse Scene graphTree or culledSceneGraphTree
// ideally m_commands has been sized properly after the
// scene has been culled to the number of nodes in the culled
// scene using reserve().
// Tries to order renderCommand by shader so as to minimize shader changes
void RenderView::buildRenderCommands(RenderEntity *node)
{
    // Build renderCommand for current node

    // 1 RenderCommand per RenderPass pass on an Entity with a Mesh

    Entity *frontEndEntity = Q_NULLPTR;
    if (m_renderCamera != Q_NULLPTR && node->frontEndPeer() != Q_NULLPTR
            && (frontEndEntity = node->frontEndPeer()->asEntity()) != Q_NULLPTR
            && checkContainedWithinLayer(frontEndEntity->uuid())) {
        HMesh meshHandle;
        if (m_renderer->meshManager()->contains(frontEndEntity->uuid()) &&
                (meshHandle = m_renderer->meshManager()->lookupHandle(frontEndEntity->uuid())) != HMesh()) {
            RenderMesh *mesh = m_renderer->meshManager()->data(meshHandle);
            if (mesh == Q_NULLPTR || mesh->peer() == Q_NULLPTR)
                return ;
            if (mesh->meshDirty()) {
                mesh->setMeshData(m_renderer->meshDataManager()->lookupHandle(mesh->peer()->uuid()));
                qCDebug(Backend) << Q_FUNC_INFO << "Updating RenderMesh -> MeshData handle";
            }

            RenderMaterial *material = findMaterialForMeshNode(frontEndEntity->uuid());
            RenderEffect *effect = findEffectForMaterial(material);
            RenderTechnique *technique = findTechniqueForEffect(effect);
            QList<RenderRenderPass *> passes = findRenderPassesForTechnique(technique);

            if (passes.isEmpty()) {
                material = m_renderer->materialManager()->data(m_renderer->defaultMaterialHandle());
                effect = m_renderer->effectManager()->data(m_renderer->defaultEffectHandle());
                technique = m_renderer->techniqueManager()->data(m_renderer->defaultTechniqueHandle());
                passes << m_renderer->renderPassManager()->data(m_renderer->defaultRenderPassHandle());
            }
            QHash<QString, QVariant> parameters = parametersFromMaterialEffectTechnique(material, effect, technique);

            Q_FOREACH (RenderRenderPass *pass, passes) {
                RenderCommand *command = m_allocator->allocate<RenderCommand>();
                command->m_mesh = meshHandle;
                command->m_meshData = mesh->meshData();
                command->m_instancesCount = 0;
                command->m_worldMatrix = *(node->worldTransform());
                command->m_stateSet = Q_NULLPTR;
                setShaderAndUniforms(command, pass, parameters);
                m_commands.append(command);
            }
        }
    }

    // Traverse children
    Q_FOREACH (RenderEntity *child, node->children())
        buildRenderCommands(child);
}

RenderMaterial *RenderView::findMaterialForMeshNode(const QUuid &entityUuid)
{
    // Material is created by the RenderSceneBuilder or RenderNode if it is appended through a change
    // Therefore we only perform lookups in RenderView
    return m_renderer->materialManager()->renderMaterial(entityUuid);
}

// The RenderTechnique && RenderPass instances have to be created in the RenderViewJobs
// As it offers a way to create instances only for techniques and passes that are used.

RenderEffect *RenderView::findEffectForMaterial(RenderMaterial *material)
{
    RenderEffect *effect = Q_NULLPTR;
    if (material != Q_NULLPTR && material->peer() != Q_NULLPTR) {
        QAbstractEffect *fEffect = material->peer()->effect();
        if (fEffect != Q_NULLPTR && (effect = m_renderer->effectManager()->lookupResource(fEffect)) == Q_NULLPTR) {
            if ((effect = m_renderer->effectManager()->getOrCreateResource(fEffect)) != Q_NULLPTR) {
                effect->setRendererAspect(m_renderer->rendererAspect());
                effect->setPeer(fEffect);
            }
        }
    }
    return effect;
}

RenderTechnique *RenderView::findTechniqueForEffect(RenderEffect *effect)
{
    // Check to see if the Effect contains a Technique matching the criteria defined in the Technique Filter
    // Effect (RenderEffect) contains a list of QAbstractTechnique that is updated to reflect the one of its frontend Effect
    // using the QChangeArbiter to avoid race conditions
    // On the other hand, we should maybe rework the filtering below as it accesses the criteria of the Frontend QML texture
    // meaning that those could be changed while we're reading them.
    // That would imply creating a RenderTechnique for each Technique defined in the Effect and having each RenderTechnique contain
    // a list of criteria
    // If TechniqueCriteria and their values were defined as constant, the filtering below would be enough

    // Can we fully perform the technique selection here ?
    // If we need to check for extension / vendor / OpenGL Version we would need access to
    // The QGraphicContext which is only avalaible in the Renderer's thread
    // Current Plan : Have an interface accessible through the Renderer from that thread where we have
    // all the needed information : Vendor, Extensions, GL Versions .... so that we can perform the checks here
    // Also we could define that for defined criterionTypes (Vendor, GL_Version ...) the technique filtering doesn't
    // depend on the TechniqueFilter but entirely on what the system GL interface gives us
    // Furthermode, finding a way to perform this filtering as little as possible could provide some performance improvements
    if (effect != Q_NULLPTR) {
        RenderTechnique *technique = Q_NULLPTR;
        // The filtering process happens only if there is no RenderTechnique found for the current Effect
        // or if the m_techniqueFilter indicates that the filtering isDirty
        if ((technique = m_renderer->techniqueManager()->lookupResource(effect->peer())) == Q_NULLPTR
                && m_techniqueFilter != Q_NULLPTR) {
            TechniqueManager::WriteLocker(m_renderer->techniqueManager());

            // Tries to select first valid technique based on criteria defined in the TechniqueFilter
            QTechnique *filteredTech = Q_NULLPTR;
            Q_FOREACH (QAbstractTechnique *technique, effect->techniques()) {
                QTechnique *tech = Q_NULLPTR;
                if ((tech = qobject_cast<QTechnique*>(technique)) != Q_NULLPTR &&
                        tech->criteria().size() == m_techniqueFilter->filters().count()) {
                    bool findMatch = true;
                    Q_FOREACH (TechniqueCriterion *filterCriterion, m_techniqueFilter->filters()) {
                        Q_FOREACH (TechniqueCriterion *techCriterion, tech->criteria()) {
                            if (*filterCriterion != *techCriterion) {
                                findMatch = false;
                                break;
                            }
                        }
                    }
                    if (findMatch) {
                        filteredTech = tech;
                        break;
                    }
                }
            }
            if (filteredTech == Q_NULLPTR) {
                qCCritical(Render::Backend) << "No Technique was found matching the criteria defined in the TechniqueFilter : Cannot Continue";
            } else {
                technique = m_renderer->techniqueManager()->getOrCreateResource(effect->peer());
                technique->setRenderer(m_renderer);
                technique->setPeer(qobject_cast<QTechnique *>(filteredTech));
            }
        }
        if (technique == Q_NULLPTR)
            qCCritical(Render::Backend) << Q_FUNC_INFO << "No technique found for current Effect";
        return technique;
    }
    return Q_NULLPTR;
}

QList<RenderRenderPass *> RenderView::findRenderPassesForTechnique(RenderTechnique *technique)
{
    QList<RenderRenderPass *> passes;
    if (technique != Q_NULLPTR && technique->peer() != Q_NULLPTR) {
        // TO DO : Improve so we handle the case where the peer or renderPasses in the peer change
        Q_FOREACH (QAbstractRenderPass *pass, technique->peer()->renderPasses()) {
            // TO DO : IF PASS MATCHES CRITERIA
            RenderRenderPass *rPass = m_renderer->renderPassManager()->lookupResource(pass);
            if (rPass == Q_NULLPTR) {
                rPass = m_renderer->renderPassManager()->getOrCreateResource(pass);
                rPass->setRenderer(m_renderer);
                rPass->setPeer(qobject_cast<QRenderPass*>(pass));
            }
            passes << rPass;
        }
    }
    return passes;
}

void RenderView::createRenderTexture(Texture *tex)
{
    if (!m_renderer->textureManager()->contains(tex->uuid())) {
        RenderTexture *rTex = m_renderer->textureManager()->getOrCreateResource(tex->uuid());
        // TO DO : Improve when working to monitor for texture changes ...
        rTex->setPeer(tex);
    }
}

QHash<QString, QVariant> RenderView::parametersFromMaterialEffectTechnique(RenderMaterial *material,
                                                                           RenderEffect *effect,
                                                                           RenderTechnique *technique)
{
    QHash<QString, QVariant> params;

    // Material is preferred over Effect
    // Effect is preferred over Technique
    // By filling the hash in reverse preference order, we're ensured that we preserve preference

    if (effect != Q_NULLPTR)
        Q_FOREACH (const QString &key, effect->parameters().keys())
            params[key] = effect->parameters()[key];

    if (technique != Q_NULLPTR)
        Q_FOREACH (const QString &key, technique->parameters().keys())
            params[key] = technique->parameters()[key];

    if (material != Q_NULLPTR)
        Q_FOREACH (const QString &key, material->parameters().keys())
            params[key] = material->parameters()[key];

    return params;
}

void RenderView::setShaderAndUniforms(RenderCommand *command, RenderRenderPass *rPass, QHash<QString, QVariant> &parameters)
{
    // The VAO Handle is set directly in the renderer thread so as to avoid having to use a mutex here
    // Set shader, technique, and effect by basically doing :
    // ShaderProgramManager[MaterialManager[frontentEntity->uuid()]->Effect->Techniques[TechniqueFilter->name]->RenderPasses[RenderPassFilter->name]];
    // The Renderer knows that if one of those is null, a default material / technique / effect as to be used

    // Find all RenderPasses (in order) matching values set in the RenderPassFilter
    // Get list of parameters for the Material, Effect, and Technique
    // For each ParameterBinder in the RenderPass -> create a QUniformPack
    // Once that works, improve that to try and minimize QUniformPack updates

    if (rPass != Q_NULLPTR && rPass->shaderProgram() != Q_NULLPTR) {
        // Index RenderShader by Shader UUID
        command->m_shader = m_renderer->shaderManager()->lookupHandle(rPass->shaderProgram()->uuid());
        RenderShader *shader = Q_NULLPTR;
        if ((shader = m_renderer->shaderManager()->data(command->m_shader)) == Q_NULLPTR) {
            shader = m_renderer->shaderManager()->getOrCreateResource(rPass->shaderProgram()->uuid());
            shader->setPeer(qobject_cast<ShaderProgram*>(rPass->shaderProgram()));
            command->m_shader = m_renderer->shaderManager()->lookupHandle(rPass->shaderProgram()->uuid());
        }
        // TO DO : To be corrected later on
        //        command->m_stateSet = qobject_cast<RenderPass*>(pass)->stateSet();

        // Builds the QUniformPack, sets shader standard uniforms and store attributes name / glname bindings
        // If a parameter is defined and not found in the bindings it is assumed to be a binding of Uniform type with the glsl name
        // equals to the parameter name

        Q_FOREACH (ParameterMapper *binding, rPass->bindings()) {
            if (!parameters.contains(binding->parameterName())) {
                if (binding->bindingType() == ParameterMapper::Attribute)
                    command->m_parameterAttributeToShaderNames[binding->parameterName()] = binding->shaderVariableName();
                else
                    qCWarning(Render::Backend) << Q_FUNC_INFO << "Trying to bind a Parameter that hasn't been defined " << binding->parameterName();
            }
            else {
                if (binding->bindingType() == ParameterMapper::Uniform) {
                    QVariant value = parameters.take(binding->parameterName());
                    Texture *tex = Q_NULLPTR;
                    if (static_cast<QMetaType::Type>(value.type()) == QMetaType::QObjectStar &&
                            (tex = value.value<Qt3D::Texture*>()) != Q_NULLPTR) {
                        createRenderTexture(tex);
                        command->m_uniforms.setTexture(binding->shaderVariableName(), tex->uuid());
                        TextureUniform *texUniform = m_allocator->allocate<TextureUniform>();
                        texUniform->setTextureId(tex->uuid());
                        command->m_uniforms.setUniform(binding->shaderVariableName(), texUniform);
                    }
                    else {
                        command->m_uniforms.setUniform(binding->shaderVariableName(), QUniformValue::fromVariant(value, m_allocator));
                    }
                }
                else if (binding->bindingType() == ParameterMapper::StandardUniform)
                    command->m_uniforms.setUniform(binding->shaderVariableName(),
                                                   (this->*m_standardUniformSetters[static_cast<Parameter::StandardUniform>(parameters.take(binding->parameterName()).toInt())])(m_renderCamera, command->m_worldMatrix));
            }
        }
        if (!parameters.empty()) {
            qDebug() << "There are params remaining" << parameters.keys();

            // Check for default uniform names
            // Add them if found
            // Otherwise consider additional parameters as user defined uniforms that did not require binding
        }
    }
    else {
        qCWarning(Render::Backend) << Q_FUNC_INFO << "Using default effect as none was provided";
    }
}

void RenderView::computeViewport(ViewportNode *viewportNode)
{
    QRectF tmpViewport(viewportNode->xMin(),
                       viewportNode->yMin(),
                       viewportNode->xMax(),
                       viewportNode->yMax());
    if (m_viewport.isEmpty()) {
        m_viewport = tmpViewport;
    }
    else {
        QRectF oldViewport = m_viewport;
        m_viewport = QRectF(tmpViewport.x() + tmpViewport.width() * oldViewport.x(),
                            tmpViewport.y() + tmpViewport.height() * oldViewport.y(),
                            tmpViewport.width() * oldViewport.width(),
                            tmpViewport.height() * oldViewport.height());
    }
}

bool RenderView::checkContainedWithinLayer(const QUuid &entityUuid)
{
    if (m_layers.isEmpty())
        return true;
    RenderLayer *renderLayer = m_renderer->layerManager()->lookupResource(entityUuid);
    if (renderLayer == Q_NULLPTR || !m_layers.contains(renderLayer->layer()))
        return false;
    return true;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
