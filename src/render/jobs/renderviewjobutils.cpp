/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderviewjobutils_p.h"
#include "renderlogging_p.h"

#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/qshaderdata.h>

#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/clearbuffer_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/renderstateset_p.h>
#include <Qt3DRender/private/rendertargetselectornode_p.h>
#include <Qt3DRender/private/renderview_p.h>
#include <Qt3DRender/private/sortmethod_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/shadervariables_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/statesetnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

/*!
    \internal
    Walks up the framegraph tree from \a fgLeaf and builds up as much state
    as possible and populates \a rv. For cases where we can't get the specific state
    (e.g. because it depends upon more than just the framegraph) we store the data from
    the framegraph that will be needed to later when the rest of the data becomes available
*/
void setRenderViewConfigFromFrameGraphLeafNode(RenderView *rv, const FrameGraphNode *fgLeaf)
{
    // The specific RenderPass to be used is also dependent upon the Effect and TechniqueFilter
    // which is referenced by the Material which is referenced by the RenderMesh. So we can
    // only store the filter info in the RenderView structure and use it to do the resolving
    // when we build the RenderCommand list.
    const NodeManagers *manager = rv->nodeManagers();
    const FrameGraphNode *node = fgLeaf;

    while (node) {
        FrameGraphNode::FrameGraphNodeType type = node->nodeType();
        if (node->isEnabled())
            switch (type) {
            case FrameGraphNode::CameraSelector:
                // Can be set only once and we take camera nearest to the leaf node
                if (!rv->renderCamera()) {
                    const CameraSelector *cameraSelector = static_cast<const CameraSelector *>(node);
                    Entity *camNode = manager->renderNodesManager()->lookupResource(cameraSelector->cameraUuid());
                    if (camNode) {
                        CameraLens *lens = camNode->renderComponent<CameraLens>();
                        if (lens && lens->isEnabled()) {
                            rv->setRenderCamera(lens);
                            rv->setViewMatrix(*camNode->worldTransform());
                            rv->setViewProjectionMatrix(lens->projection() * *camNode->worldTransform());

                            //To get the eyePosition of the camera, we need to use the inverse of the
                            //camera's worldTransform matrix.
                            const QMatrix4x4 inverseWorldTransform = camNode->worldTransform()->inverted();
                            const QVector3D eyePosition(inverseWorldTransform.column(3));
                            rv->setEyePosition(eyePosition);
                        }
                    }
                    break;
                }

            case FrameGraphNode::LayerFilter: // Can be set multiple times in the tree
                rv->appendLayerFilter(static_cast<const LayerFilterNode *>(node)->layers());
                break;

            case FrameGraphNode::RenderPassFilter:
                // Can be set once
                // TODO: Amalgamate all render pass filters from leaf to root
                if (!rv->renderPassFilter())
                    rv->setRenderPassFilter(static_cast<const RenderPassFilter *>(node));
                break;

            case FrameGraphNode::RenderTarget: {
                // Can be set once and we take render target nearest to the leaf node
                const RenderTargetSelector *targetSelector = static_cast<const RenderTargetSelector *>(node);
                QNodeId renderTargetUid = targetSelector->renderTargetUuid();
                HTarget renderTargetHandle = manager->renderTargetManager()->lookupHandle(renderTargetUid);
                if (rv->renderTargetHandle().isNull()) {
                    rv->setRenderTargetHandle(renderTargetHandle);

                    RenderTarget *renderTarget = manager->renderTargetManager()->data(renderTargetHandle);
                    if (renderTarget) {
                        // Add renderTarget Handle and build renderCommand AttachmentPack

                        // Copy draw buffers list
                        rv->setDrawBuffers(targetSelector->drawBuffers());

                        // Copy attachments
                        Q_FOREACH (const QNodeId &attachmentId, renderTarget->renderAttachments()) {
                            RenderAttachment *attachment = manager->attachmentManager()->lookupResource(attachmentId);
                            if (attachment)
                                rv->addRenderAttachment(attachment->attachment());
                        }

                    }
                }
                break;
            }

            case FrameGraphNode::ClearBuffer:
                rv->setClearBuffer(static_cast<const ClearBuffer *>(node)->type());
                break;

            case FrameGraphNode::TechniqueFilter:
                // Can be set once
                // TODO Amalgamate all technique filters from leaf to root
                if (rv->techniqueFilter())
                    rv->setTechniqueFilter(static_cast<const TechniqueFilter *>(node));
                break;

            case FrameGraphNode::Viewport: {
                // If the Viewport has already been set in a lower node
                // Make it so that the new viewport is actually
                // a subregion relative to that of the parent viewport
                const ViewportNode *vpNode = static_cast<const ViewportNode *>(node);
                rv->setViewport(computeViewport(rv->viewport(), vpNode));

                // We take the clear color from the viewport node nearest the leaf
                if (!rv->clearColor().isValid())
                    rv->setClearColor(vpNode->clearColor());
                break;
            }

            case FrameGraphNode::SortMethod: {
                const Render::SortMethod *sortMethod = static_cast<const Render::SortMethod *>(node);
                rv->addSortCriteria(sortMethod->criteria());
                break;
            }

            case FrameGraphNode::SubtreeSelector:
                // Has no meaning here. SubtreeSelector was used
                // in a prior step to build the list of RenderViewJobs
                break;

            case FrameGraphNode::StateSet: {
                const Render::StateSetNode *rStateSet = static_cast<const Render::StateSetNode *>(node);
                // Create global RenderStateSet for renderView if no stateSet was set before
                RenderStateSet *stateSet = rv->stateSet();
                if (stateSet == Q_NULLPTR) {
                    stateSet = rv->allocator()->allocate<RenderStateSet>();
                    rv->setStateSet(stateSet);
                }

                // Add renderstates to stateset
                const QList<RenderState *> &states = rStateSet->renderStates();
                Q_FOREACH (RenderState *renderState, states)
                    stateSet->addState(renderState);

                break;
            }

            case FrameGraphNode::NoDraw: {
                rv->setNoDraw(true);
                break;
            }

            case FrameGraphNode::FrustumCulling: {
                rv->setFrustumCulling(true);
                break;
            }

            case FrameGraphNode::Lighting: {
                // TODO
                break;
            }

            default:
                // Should never get here
                qCWarning(Backend) << "Unhandled FrameGraphNode type";
            }

        node = node->parent();
    }
}

/*!
    \internal
    Searches the \a renderer for the best matching Technique from
    \a effect specified by the \a renderView.
*/
Technique *findTechniqueForEffect(Renderer *renderer,
                                  RenderView *renderView,
                                  Effect *effect)
{
    if (!effect)
        return Q_NULLPTR;

    NodeManagers *manager = renderer->nodeManagers();

    // Iterate through the techniques in the effect
    Q_FOREACH (const QNodeId &techniqueId, effect->techniques()) {
        Technique *technique = manager->techniqueManager()->lookupResource(techniqueId);

        if (!technique)
            continue;

        // We need to be sure the renderer is still running <=> still has a GraphicsContext
        if (renderer->isRunning() && *renderer->contextInfo() == *technique->graphicsApiFilter()) {

            // If no techniqueFilter is present, we return the technique as it satisfies OpenGL version
            const TechniqueFilter *techniqueFilter = renderView->techniqueFilter();
            bool foundMatch = (techniqueFilter == Q_NULLPTR || techniqueFilter->filters().isEmpty());
            if (foundMatch)
                return technique;

            // There is a technique filter so we need to check for a technique with suitable criteria.
            // Check for early bail out if the technique doesn't have sufficient number of criteria and
            // can therefore never satisfy the filter
            if (technique->annotations().size() < techniqueFilter->filters().size())
                continue;

            // Iterate through the filter criteria and for each one search for a criteria on the
            // technique that satisfies it
            Q_FOREACH (const QNodeId &filterAnnotationId, techniqueFilter->filters()) {
                foundMatch = false;
                Annotation *filterAnnotation = manager->criterionManager()->lookupResource(filterAnnotationId);

                Q_FOREACH (const QNodeId &techniqueAnnotationId, technique->annotations()) {
                    Annotation *techniqueAnnotation = manager->criterionManager()->lookupResource(techniqueAnnotationId);
                    if ((foundMatch = (*techniqueAnnotation == *filterAnnotation)))
                        break;
                }

                if (!foundMatch) {
                    // No match for TechniqueFilter criterion in any of the technique's criteria.
                    // So no way this can match. Don't bother checking the rest of the criteria.
                    break;
                }
            }

            if (foundMatch)
                return technique; // All criteria matched - we have a winner!
        }
    }

    // We failed to find a suitable technique to use :(
    return Q_NULLPTR;
}


RenderRenderPassList findRenderPassesForTechnique(NodeManagers *manager,
                                                  RenderView *renderView,
                                                  Technique *technique)
{
    Q_ASSERT(manager);
    Q_ASSERT(technique);

    RenderRenderPassList passes;
    Q_FOREACH (const QNodeId &passId, technique->renderPasses()) {
        RenderPass *renderPass = manager->renderPassManager()->lookupResource(passId);

        if (renderPass) {
            const RenderPassFilter *passFilter = renderView->renderPassFilter();
            bool foundMatch = (!passFilter || passFilter->filters().size() == 0);

            // A pass filter is present so we need to check for matching criteria
            if (!foundMatch && renderPass->annotations().size() >= passFilter->filters().size()) {

                // Iterate through the filter criteria and look for render passes with criteria that satisfy them
                Q_FOREACH (const QNodeId &filterAnnotationId, passFilter->filters()) {
                    foundMatch = false;
                    Annotation *filterAnnotation = manager->criterionManager()->lookupResource(filterAnnotationId);

                    Q_FOREACH (const QNodeId &passAnnotationId, renderPass->annotations()) {
                        Annotation *passAnnotation = manager->criterionManager()->lookupResource(passAnnotationId);
                        if ((foundMatch = (*passAnnotation == *filterAnnotation)))
                            break;
                    }

                    if (!foundMatch) {
                        // No match for criterion in any of the render pass' criteria
                        break;
                    }
                }
            }

            if (foundMatch) {
                // Found a renderpass that satisfies our needs. Add it in order
                passes << renderPass;
            }
        }
    }

    return passes;
}


ParameterInfoList::iterator findParamInfo(ParameterInfoList *params, const QString &name)
{
    ParameterInfoList::iterator it = std::lower_bound(params->begin(), params->end(), name);
    if (it != params->end() && it->name != name)
        return params->end();
    return it;
}

void addParametersForIds(ParameterInfoList *params, ParameterManager *manager,
                         const QList<Qt3DCore::QNodeId> &parameterIds)
{
    Q_FOREACH (const QNodeId &paramId, parameterIds) {
        Parameter *param = manager->lookupResource(paramId);
        if (param != Q_NULLPTR) {
            ParameterInfoList::iterator it = std::lower_bound(params->begin(), params->end(), param->name());
            if (it == params->end() || it->name != param->name())
                params->insert(it, ParameterInfo(param->name(), param->value()));
        }
    }
}

void parametersFromMaterialEffectTechnique(ParameterInfoList *infoList,
                                           ParameterManager *manager,
                                           Material *material,
                                           Effect *effect,
                                           Technique *technique)
{
    // The parameters are taken in the following priority order:
    //
    // 1) Material
    // 2) Technique
    // 3) Effect
    //
    // That way a user can override defaults in Effect's and Techniques on a
    // object manner and a Technique can override global defaults from the Effect.
    parametersFromParametersProvider(infoList, manager, material);
    parametersFromParametersProvider(infoList, manager, technique);
    parametersFromParametersProvider(infoList, manager, effect);
}

RenderStateSet *buildRenderStateSet(const QList<RenderState*> &states, QFrameAllocator *allocator)
{
    RenderStateSet *stateSet = allocator->allocate<RenderStateSet>();

    Q_FOREACH (RenderState *renderState, states) {
        stateSet->addState(renderState);
    }

    return stateSet;
}

namespace {

const QString blockArray = QStringLiteral("[%1]");
const int qNodeIdTypeId = qMetaTypeId<QNodeId>();

}

UniformBlockValueBuilder::UniformBlockValueBuilder()
    : shaderDataManager(Q_NULLPTR)
{
}

UniformBlockValueBuilder::~UniformBlockValueBuilder()
{
}

void UniformBlockValueBuilder::buildActiveUniformNameValueMapHelper(const QString &blockName, const QString &qmlPropertyName, const QVariant &value)
{
    // In the end, values are either scalar or a scalar array
    // Composed elements (structs, structs array) are simplified into simple scalars
    if (value.userType() == QMetaType::QVariantList) { // Array
        QVariantList list = value.value<QVariantList>();
        if (list.at(0).userType() == qNodeIdTypeId) { // Array of struct qmlPropertyName[i].structMember
            for (int i = 0; i < list.size(); ++i) {
                if (list.at(i).userType() == qNodeIdTypeId) {
                    ShaderData *subShaderData = shaderDataManager->lookupResource(list.at(i).value<QNodeId>());
                    if (subShaderData)
                        buildActiveUniformNameValueMapStructHelper(subShaderData,
                                                                   blockName + QStringLiteral(".") + qmlPropertyName + blockArray.arg(i),
                                                                   QStringLiteral(""));
                }
            }
        } else { // Array of scalar/vec  qmlPropertyName[0]
            QString varName = blockName + QStringLiteral(".") + qmlPropertyName + QStringLiteral("[0]");
            if (uniforms.contains(varName)) {
                qCDebug(Shaders) << "UBO array member " << varName << " set for update";
                activeUniformNamesToValue.insert(varName, value);
            }
        }
    } else if (value.userType() == qNodeIdTypeId) { // Struct qmlPropertyName.structMember
        ShaderData *rSubShaderData = shaderDataManager->lookupResource(value.value<QNodeId>());
        if (rSubShaderData)
            buildActiveUniformNameValueMapStructHelper(rSubShaderData,
                                                       blockName,
                                                       qmlPropertyName);
    } else { // Scalar / Vec
        QString varName = blockName + QStringLiteral(".") + qmlPropertyName;
        if (uniforms.contains(varName)) {
            qCDebug(Shaders) << "UBO scalar member " << varName << " set for update";
            activeUniformNamesToValue.insert(varName, value);
        }
    }
}

void UniformBlockValueBuilder::buildActiveUniformNameValueMapStructHelper(ShaderData *rShaderData, const QString &blockName, const QString &qmlPropertyName)
{
    const QHash<QString, QVariant> &properties = updatedPropertiesOnly ? rShaderData->updatedProperties() : rShaderData->properties();
    QHash<QString, QVariant>::const_iterator it = properties.begin();
    const QHash<QString, QVariant>::const_iterator end = properties.end();

    while (it != end) {
        QString prefix = qmlPropertyName.isEmpty() ? QStringLiteral("") : QStringLiteral(".");
        buildActiveUniformNameValueMapHelper(blockName + prefix + qmlPropertyName,
                                             it.key(),
                                             it.value());
        ++it;
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
