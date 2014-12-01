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

#include "renderview_p.h"
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DRenderer/sphere.h>

#include <Qt3DRenderer/private/cameraselectornode_p.h>
#include <Qt3DRenderer/private/framegraphnode_p.h>
#include <Qt3DRenderer/private/layerfilternode_p.h>
#include <Qt3DRenderer/private/qmeshdata_p.h>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/qparameter_p.h>
#include <Qt3DRenderer/private/rendercameralens_p.h>
#include <Qt3DRenderer/private/rendercommand_p.h>
#include <Qt3DRenderer/private/rendereffect_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/renderlayer_p.h>
#include <Qt3DRenderer/private/renderlight_p.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/private/renderpassfilternode_p.h>
#include <Qt3DRenderer/private/renderrenderpass_p.h>
#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/techniquefilternode_p.h>
#include <Qt3DRenderer/private/viewportnode_p.h>

#include <Qt3DRenderer/qparametermapping.h>

#include <Qt3DCore/qentity.h>

#include <QtGui/qsurface.h>

#include <algorithm>

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

namespace  {

// TODO: Should we treat lack of layer data as implicitly meaning that an
// entity is in all layers?
bool isEntityInLayers(const RenderEntity *entity, const QStringList &layers)
{
    if (layers.isEmpty())
        return true;

    // TODO: find all RenderLayer components associated to node, not just the first one
    RenderLayer *renderLayer = entity->renderComponent<RenderLayer>();
    if (renderLayer) {
        Q_FOREACH (const QString &layerName, renderLayer->layers())
            if (layers.contains(layerName))
                return true;
    }

    return false;
}

} // anonymouse namespace

RenderView::StandardUniformsPFuncsHash RenderView::ms_standardUniformSetters = RenderView::initializeStandardUniformSetters();
QStringList RenderView::ms_standardAttributesNames = RenderView::initializeStandardAttributeNames();

RenderView::StandardUniformsPFuncsHash RenderView::initializeStandardUniformSetters()
{
    RenderView::StandardUniformsPFuncsHash setters;

    setters.insert(QStringLiteral("modelMatrix"), &RenderView::modelMatrix);
    setters.insert(QStringLiteral("viewMatrix"), &RenderView::viewMatrix);
    setters.insert(QStringLiteral("projectionMatrix"), &RenderView::projectionMatrix);
    setters.insert(QStringLiteral("modelView"), &RenderView::modelViewMatrix);
    setters.insert(QStringLiteral("modelViewProjection"), &RenderView::modelViewProjectionMatrix);
    setters.insert(QStringLiteral("mvp"), &RenderView::modelViewProjectionMatrix);
    setters.insert(QStringLiteral("inverseModelMatrix"), &RenderView::inverseModelMatrix);
    setters.insert(QStringLiteral("inverViewMatrix"), &RenderView::inverseViewMatrix);
    setters.insert(QStringLiteral("inverseProjectionMatrix"), &RenderView::inverseProjectionMatrix);
    setters.insert(QStringLiteral("inverseModelView"), &RenderView::inverseModelViewMatrix);
    setters.insert(QStringLiteral("inverseModelViewProjection"), &RenderView::inverseModelViewProjectionMatrix);
    setters.insert(QStringLiteral("modelNormalMatrix"), &RenderView::modelNormalMatrix);
    setters.insert(QStringLiteral("modelViewNormal"), &RenderView::modelViewNormalMatrix);
    setters.insert(QStringLiteral("viewportMatrix"), &RenderView::viewportMatrix);
    setters.insert(QStringLiteral("inverseViewportMatrix"), &RenderView::inverseViewportMatrix);

    return setters;
}

QStringList RenderView::initializeStandardAttributeNames()
{
    QStringList attributesNames;

    attributesNames << QMeshData::defaultPositionAttributeName();
    attributesNames << QMeshData::defaultTextureCoordinateAttributeName();
    attributesNames << QMeshData::defaultNormalAttributeName();
    attributesNames << QMeshData::defaultColorAttributeName();
    attributesNames << QMeshData::defaultTangentAttributeName();

    return attributesNames;
}

QUniformValue *RenderView::modelMatrix(const QMatrix4x4 &model) const
{
    return QUniformValue::fromVariant(model, m_allocator);
}

QUniformValue *RenderView::viewMatrix(const QMatrix4x4 &) const
{
    return QUniformValue::fromVariant(*m_data->m_viewMatrix, m_allocator);
}

QUniformValue *RenderView::projectionMatrix(const QMatrix4x4 &) const
{
    return QUniformValue::fromVariant(m_data->m_renderCamera->projection(), m_allocator);
}

QUniformValue *RenderView::modelViewMatrix(const QMatrix4x4 &model) const
{
    return QUniformValue::fromVariant(*m_data->m_viewMatrix * model, m_allocator);
}

QUniformValue *RenderView::modelViewProjectionMatrix(const QMatrix4x4 &model) const
{
    QMatrix4x4 projection;
    if (m_data->m_renderCamera)
        projection = m_data->m_renderCamera->projection();
    return QUniformValue::fromVariant(projection * *m_data->m_viewMatrix * model, m_allocator);
}

QUniformValue *RenderView::inverseModelMatrix(const QMatrix4x4 &model) const
{
    return QUniformValue::fromVariant(model.inverted(), m_allocator);
}

QUniformValue *RenderView::inverseViewMatrix(const QMatrix4x4 &) const
{
    return QUniformValue::fromVariant(m_data->m_viewMatrix->inverted(), m_allocator);
}

QUniformValue *RenderView::inverseProjectionMatrix(const QMatrix4x4 &) const
{
    QMatrix4x4 projection;
    if (m_data->m_renderCamera)
        projection = m_data->m_renderCamera->projection();
    return QUniformValue::fromVariant(projection.inverted(), m_allocator);
}

QUniformValue *RenderView::inverseModelViewMatrix(const QMatrix4x4 &model) const
{
    return QUniformValue::fromVariant((*m_data->m_viewMatrix * model).inverted(), m_allocator);
}

QUniformValue *RenderView::inverseModelViewProjectionMatrix(const QMatrix4x4 &model) const
{
    QMatrix4x4 projection;
    if (m_data->m_renderCamera)
        projection = m_data->m_renderCamera->projection();
    return QUniformValue::fromVariant((projection * *m_data->m_viewMatrix * model).inverted(0), m_allocator);
}

QUniformValue *RenderView::modelNormalMatrix(const QMatrix4x4 &model) const
{
    return QUniformValue::fromVariant(QVariant::fromValue(model.normalMatrix()), m_allocator);
}

QUniformValue *RenderView::modelViewNormalMatrix(const QMatrix4x4 &model) const
{
    return QUniformValue::fromVariant(QVariant::fromValue((*m_data->m_viewMatrix * model).normalMatrix()), m_allocator);
}

// TODO: Move this somewhere global where QGraphicsContext::setViewport() can use it too
static QRectF resolveViewport(const QRectF &fractionalViewport, const QSize &surfaceSize)
{
    return QRectF(fractionalViewport.x() * surfaceSize.width(),
                  (1.0 - fractionalViewport.y() - fractionalViewport.height()) * surfaceSize.height(),
                  fractionalViewport.width() * surfaceSize.width(),
                  fractionalViewport.height() * surfaceSize.height());
}

QUniformValue *RenderView::viewportMatrix(const QMatrix4x4 &model) const
{
    // TODO: Can we avoid having to pass the model matrix in to these functions?
    Q_UNUSED(model);
    QMatrix4x4 viewportMatrix;
    QSize surfaceSize = m_renderer->surface()->size();
    viewportMatrix.viewport(resolveViewport(*m_viewport, surfaceSize));
    return QUniformValue::fromVariant(QVariant::fromValue(viewportMatrix), m_allocator);

}

QUniformValue *RenderView::inverseViewportMatrix(const QMatrix4x4 &model) const
{
    Q_UNUSED(model);
    QMatrix4x4 viewportMatrix;
    QSize surfaceSize = m_renderer->surface()->size();
    viewportMatrix.viewport(resolveViewport(*m_viewport, surfaceSize));
    QMatrix4x4 inverseViewportMatrix = viewportMatrix.inverted();
    return QUniformValue::fromVariant(QVariant::fromValue(inverseViewportMatrix), m_allocator);

}

RenderView::RenderView()
    : m_renderer(Q_NULLPTR)
    , m_allocator(Q_NULLPTR)
    , m_data(Q_NULLPTR)
    , m_clearColor(Q_NULLPTR)
    , m_viewport(Q_NULLPTR)
    , m_clearBuffer(QClearBuffer::None)
    , m_commands()
{
}

RenderView::~RenderView()
{
    if (m_allocator == Q_NULLPTR) // Mainly needed for unit tests
        return;

    Q_FOREACH (RenderCommand *command, m_commands) {
        // Deallocate all uniform values of the QUniformPack of each RenderCommand
        Q_FOREACH (const QUniformValue *v, command->m_uniforms.uniforms().values())
            m_allocator->deallocate<QUniformValue>(const_cast<QUniformValue *>(v));

        if (command->m_stateSet != Q_NULLPTR) // We do not delete the RenderState as that is stored statically
            m_allocator->deallocate<RenderStateSet>(command->m_stateSet);

        // Deallocate RenderCommand
        m_allocator->deallocate<RenderCommand>(command);
    }

    // Deallocate viewMatrix
    m_allocator->deallocate<QMatrix4x4>(m_data->m_viewMatrix);
    // Deallocate viewport rect
    m_allocator->deallocate<QRectF>(m_viewport);
    // Deallocate clearColor
    m_allocator->deallocate<QColor>(m_clearColor);
    // Deallocate m_data
    m_allocator->deallocate<InnerData>(m_data);
}

// We need to overload the delete operator so that when the Renderer deletes the list of RenderViews, each RenderView
// can clear itself with the frame allocator and deletes its RenderViews
void RenderView::operator delete(void *ptr)
{
    RenderView *rView = static_cast<RenderView *>(ptr);
    if (rView != Q_NULLPTR && rView->m_allocator != Q_NULLPTR)
        rView->m_allocator->deallocateRawMemory(rView, sizeof(RenderView));
}

// Since placement new is used we need a matching operator delete, at least MSVC complains otherwise
void RenderView::operator delete(void *ptr, void *)
{
    RenderView *rView = static_cast<RenderView *>(ptr);
    if (rView != Q_NULLPTR && rView->m_allocator != Q_NULLPTR)
        rView->m_allocator->deallocateRawMemory(rView, sizeof(RenderView));
}

void RenderView::sort()
{
    // Compares the bitsetKey of the RenderCommands
    // Key[Depth | StateCost | Shader]

    std::sort(m_commands.begin(), m_commands.end());
}

// Tries to order renderCommand by shader so as to minimize shader changes
void RenderView::buildRenderCommands(RenderEntity *node)
{
    // Build renderCommand for current node
    if (isEntityInLayers(node, m_data->m_layers)) {
        RenderMesh *mesh = Q_NULLPTR;
        if (node->componentHandle<RenderMesh, 16>() != HMesh()
                && (mesh = node->renderComponent<RenderMesh>()) != Q_NULLPTR) {
            if (!mesh->meshData().isNull())
            {
                // Perform culling here
                // As shaders may be deforming, transforming the mesh
                // We might want to make that optional or dependent on an explicit bounding box item

                // Find the material, effect, technique and set of render passes to use
                RenderMaterial *material = Q_NULLPTR;
                RenderEffect *effect = Q_NULLPTR;
                if ((material = node->renderComponent<RenderMaterial>()) != Q_NULLPTR)
                    effect = m_renderer->effectManager()->lookupResource(material->effect());
                RenderTechnique *technique = findTechniqueForEffect(m_renderer, this, effect);

                RenderRenderPassList passes;
                if (technique) {
                    passes = findRenderPassesForTechnique(m_renderer, this, technique);
                } else {
                    material = m_renderer->materialManager()->data(m_renderer->defaultMaterialHandle());
                    effect = m_renderer->effectManager()->data(m_renderer->defaultEffectHandle());
                    technique = m_renderer->techniqueManager()->data(m_renderer->defaultTechniqueHandle());
                    passes << m_renderer->renderPassManager()->data(m_renderer->defaultRenderPassHandle());
                }

                // Get the parameters for our selected rendering setup
                ParameterInfoList parameters;
                parametersFromMaterialEffectTechnique(&parameters, m_renderer->parameterManager(), material, effect, technique);

                // 1 RenderCommand per RenderPass pass on an Entity with a Mesh
                Q_FOREACH (RenderRenderPass *pass, passes) {
                    RenderCommand *command = m_allocator->allocate<RenderCommand>();
                    command->m_depth = m_data->m_eyePos.distanceToPoint(node->worldBoundingVolume()->center());
                    command->m_meshData = mesh->meshData();
                    command->m_instancesCount = 0;

                    // TODO: Build the state set for a render pass only once per-pass. Not once per rendercommand and pass.
                    command->m_stateSet = buildRenderStateSet(pass, m_allocator);
                    if (command->m_stateSet != Q_NULLPTR)
                        command->m_changeCost = m_renderer->defaultRenderState()->changeCost(command->m_stateSet);
                    setShaderAndUniforms(command, pass, parameters, *(node->worldTransform()));
                    buildSortingKey(command);
                    m_commands.append(command);
                }
            }
        }
    }

    // Traverse children
    Q_FOREACH (RenderEntity *child, node->children())
        buildRenderCommands(child);
}

const AttachmentPack &RenderView::attachmentPack() const
{
    return m_attachmentPack;
}

void RenderView::setUniformValue(QUniformPack &uniformPack, const QString &name, const QVariant &value)
{
    QTexture *tex = Q_NULLPTR;
    if ((tex = value.value<Qt3D::QTexture *>()) != Q_NULLPTR) {
        uniformPack.setTexture(name, tex->uuid());
        TextureUniform *texUniform = m_allocator->allocate<TextureUniform>();
        texUniform->setTextureId(tex->uuid());
        uniformPack.setUniform(name, texUniform);
    }
    else {
        uniformPack.setUniform(name, QUniformValue::fromVariant(value, m_allocator));
    }
}

void RenderView::buildSortingKey(RenderCommand *command)
{
    // Build a bitset key depending on the SortingCriterion
    int sortCount = m_data->m_sortingCriteria.count();
    command->m_sortingType.global = 0;

    // Default sorting
    if (sortCount == 0)
        command->m_sortingType.sorts[0] = QSortCriterion::StateChangeCost;

    // Handle at most 4 filters at once
    for (int i = 0; i < sortCount && i < 4; i++) {
        SortCriterion *sC = m_renderer->sortCriterionManager()->lookupResource(m_data->m_sortingCriteria[i]);
        command->m_sortingType.sorts[i] = sC->sortType();
    }
}

void RenderView::setShaderAndUniforms(RenderCommand *command, RenderRenderPass *rPass, ParameterInfoList &parameters, const QMatrix4x4 &worldTransform)
{
    // The VAO Handle is set directly in the renderer thread so as to avoid having to use a mutex here
    // Set shader, technique, and effect by basically doing :
    // ShaderProgramManager[MaterialManager[frontentEntity->uuid()]->Effect->Techniques[TechniqueFilter->name]->RenderPasses[RenderPassFilter->name]];
    // The Renderer knows that if one of those is null, a default material / technique / effect as to be used

    // Find all RenderPasses (in order) matching values set in the RenderPassFilter
    // Get list of parameters for the Material, Effect, and Technique
    // For each ParameterBinder in the RenderPass -> create a QUniformPack
    // Once that works, improve that to try and minimize QUniformPack updates

    if (rPass != Q_NULLPTR)
    {
        // Index RenderShader by Shader UUID
        command->m_shader = m_renderer->shaderManager()->lookupHandle(rPass->shaderProgram());
        RenderShader *shader = Q_NULLPTR;
        if ((shader = m_renderer->shaderManager()->data(command->m_shader)) != Q_NULLPTR) {

            // Builds the QUniformPack, sets shader standard uniforms and store attributes name / glname bindings
            // If a parameter is defined and not found in the bindings it is assumed to be a binding of Uniform type with the glsl name
            // equals to the parameter name
            const QStringList &uniformNames = shader->uniformsNames();
            const QStringList &attributeNames = shader->attributesNames();

            // Set fragData Name and index
            // Later on we might want to relink the shader if attachments have changed
            // But for now we set them once and for all
            QHash<QString, int> fragOutputs;
            if (!m_renderTarget.isNull() && !shader->isLoaded()) {
                Q_FOREACH (const Attachment &att, m_attachmentPack.attachments()) {
                    if (att.m_type <= QRenderAttachment::ColorAttachment15)
                        fragOutputs.insert(att.m_name, att.m_type);
                }
            }

            if (!uniformNames.isEmpty() || !attributeNames.isEmpty()) {

                // Set default standard uniforms without bindings
                Q_FOREACH (const QString &uniformName, uniformNames) {
                    if (ms_standardUniformSetters.contains(uniformName))
                        command->m_uniforms.setUniform(uniformName,
                                                       (this->*ms_standardUniformSetters[uniformName])(worldTransform));
                }

                // Set default attributes
                Q_FOREACH (const QString &attributeName, attributeNames) {
                    if (ms_standardAttributesNames.contains(attributeName))
                        command->m_parameterAttributeToShaderNames[attributeName] = attributeName;
                }

                // Set uniforms and attributes explicitly binded
                Q_FOREACH (QParameterMapping *binding, rPass->bindings()) {
                    ParameterInfoList::iterator it = findParamInfo(&parameters, binding->parameterName());
                    if (it == parameters.end()) {
                        if (binding->bindingType() == QParameterMapping::Attribute
                            && attributeNames.contains(binding->shaderVariableName())) {
                            command->m_parameterAttributeToShaderNames.insert(binding->parameterName(), binding->shaderVariableName());
                        } else if (binding->bindingType() == QParameterMapping::StandardUniform
                                   && uniformNames.contains(binding->shaderVariableName())
                                   && ms_standardUniformSetters.contains(binding->parameterName())) {
                            command->m_uniforms.setUniform(binding->shaderVariableName(),
                                                           (this->*ms_standardUniformSetters[binding->parameterName()])(worldTransform));
                        } else if (binding->bindingType() == QParameterMapping::FragmentOutput
                                   && fragOutputs.contains(binding->parameterName())) {
                            fragOutputs.insert(binding->shaderVariableName(), fragOutputs.take(binding->parameterName()));
                        } else {
                            qCWarning(Render::Backend) << Q_FUNC_INFO << "Trying to bind a Parameter that hasn't been defined " << binding->parameterName();
                        }
                    } else {
                        setUniformValue(command->m_uniforms, binding->shaderVariableName(), it->value);
                        parameters.erase(it);
                    }
                }

                // If there are remaining parameters, those are set as uniforms
                if (!uniformNames.isEmpty() && !parameters.isEmpty()) {
                    ParameterInfoList::iterator it = parameters.begin();
                    while (it != parameters.end()) {
                        if (uniformNames.contains(it->name)) {
                            setUniformValue(command->m_uniforms, it->name, it->value);
                            it = parameters.erase(it);
                        } else {
                            // Else param unused by current shader
                            ++it;
                        }
                    }
                }

                // Sets lights in shader
                // Maybe we should provide a flag in the shader to specify if it needs lights
                // that might save a bit of performances
                QHash<QString, int> m_countOfLightTypes;
                Q_FOREACH (const LightPair &lightPair, m_data->m_lights) {
                    RenderLight *light = m_renderer->lightManager()->data(lightPair.first);
                    if (light != Q_NULLPTR) {
                        int lightIdx = m_countOfLightTypes[light->lightBlockName()]++;

                        QHash<QString, QVariant> lightProperties = light->lightProperties();
                        lightProperties[QStringLiteral("position")] = lightPair.second.map(QVector3D());
                        Q_FOREACH (const QString &propertyName, lightProperties.keys()) {
                            QString lightShaderUniform(light->lightUniformName() + QStringLiteral("[") + QString::number(lightIdx) + QStringLiteral("].") + propertyName);
                            if (uniformNames.contains(lightShaderUniform)) {
                                command->m_uniforms.setUniform(lightShaderUniform, QUniformValue::fromVariant(lightProperties[propertyName], m_allocator));
                            }
                        }
                    }
                }
            }
            // Set frag outputs in the shaders if hash not empty
            if (!fragOutputs.isEmpty())
                shader->setFragOutputs(fragOutputs);
        }
    }
    else {
        qCWarning(Render::Backend) << Q_FUNC_INFO << "Using default effect as none was provided";
    }
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
