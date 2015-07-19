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

#include <Qt3DRenderer/qopenglfilter.h>
#include <Qt3DRenderer/sphere.h>
#include <Qt3DRenderer/qshaderdata.h>

#include <Qt3DRenderer/private/cameraselectornode_p.h>
#include <Qt3DRenderer/private/clearbuffer_p.h>
#include <Qt3DRenderer/private/layerfilternode_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/rendereffect_p.h>
#include <Qt3DRenderer/private/renderpassfilternode_p.h>
#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/rendertargetselectornode_p.h>
#include <Qt3DRenderer/private/renderview_p.h>
#include <Qt3DRenderer/private/sortmethod_p.h>
#include <Qt3DRenderer/private/techniquefilternode_p.h>
#include <Qt3DRenderer/private/viewportnode_p.h>
#include <Qt3DRenderer/private/shadervariables_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/rendershaderdata_p.h>
#include <Qt3DRenderer/private/statesetnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

/*!
    \internal
    Walks up the framegraph tree from \p fgLeaf and builds up as much state
    as possible and populates \p rv. For cases where we can't get the specific state
    (e.g. because it depends upon more than just the framegraph) we store the data from
    the framegraph that will be needed to later when the rest of the data becomes available
*/
void setRenderViewConfigFromFrameGraphLeafNode(RenderView *rv, const FrameGraphNode *fgLeaf)
{
    // The specific RenderPass to be used is also dependent upon the Effect and TechniqueFilter
    // which is referenced by the Material which is referenced by the RenderMesh. So we can
    // only store the filter info in the RenderView structure and use it to do the resolving
    // when we build the RenderCommand list.
    const Renderer *renderer = rv->renderer();
    const FrameGraphNode *node = fgLeaf;

    while (node) {
        FrameGraphNode::FrameGraphNodeType type = node->nodeType();
        if (node->isEnabled())
            switch (type) {
            case FrameGraphNode::CameraSelector:
                // Can be set only once and we take camera nearest to the leaf node
                if (!rv->renderCamera()) {
                    const CameraSelector *cameraSelector = static_cast<const CameraSelector *>(node);
                    RenderEntity *camNode = renderer->renderNodesManager()->lookupResource(cameraSelector->cameraUuid());
                    if (camNode) {
                        RenderCameraLens *lens = camNode->renderComponent<RenderCameraLens>();
                        if (lens && lens->isEnabled()) {
                            rv->setRenderCamera(lens);
                            rv->setViewMatrix(*camNode->worldTransform());
                            rv->setEyePosition(camNode->worldBoundingVolume()->center());
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
                HTarget renderTargetHandle = renderer->renderTargetManager()->lookupHandle(renderTargetUid);
                if (rv->renderTargetHandle().isNull()) {
                    rv->setRenderTargetHandle(renderTargetHandle);

                    RenderTarget *renderTarget = renderer->renderTargetManager()->data(renderTargetHandle);
                    if (renderTarget) {
                        // Add renderTarget Handle and build renderCommand AttachmentPack

                        // Copy draw buffers list
                        rv->setDrawBuffers(targetSelector->drawBuffers());

                        // Copy attachments
                        Q_FOREACH (const QNodeId &attachmentId, renderTarget->renderAttachments()) {
                            RenderAttachment *attachment = renderer->attachmentManager()->lookupResource(attachmentId);
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

            default:
                // Should never get here
                qCWarning(Backend) << "Unhandled FrameGraphNode type";
            }

        node = node->parent();
    }
}

/*!
    \internal
    Searches the \a renderer for the best matching RenderTechnique from
    \a effect specified by the \a renderView.
*/
RenderTechnique *findTechniqueForEffect(Renderer *renderer,
                                        RenderView *renderView,
                                        RenderEffect *effect)
{
    if (!effect)
        return Q_NULLPTR;

    // Iterate through the techniques in the effect
    Q_FOREACH (const QNodeId &techniqueId, effect->techniques()) {
        RenderTechnique *technique = renderer->techniqueManager()->lookupResource(techniqueId);

        if (!technique)
            continue;

        // We need to be sure the renderer is still running <=> still has a QGraphicsContext
        if (renderer->isRunning() && *renderer->contextInfo() == *technique->openGLFilter()) {

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
                RenderAnnotation *filterAnnotation = renderer->criterionManager()->lookupResource(filterAnnotationId);

                Q_FOREACH (const QNodeId &techniqueAnnotationId, technique->annotations()) {
                    RenderAnnotation *techniqueAnnotation = renderer->criterionManager()->lookupResource(techniqueAnnotationId);
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


RenderRenderPassList findRenderPassesForTechnique(Renderer *renderer,
                                                  RenderView *renderView,
                                                  RenderTechnique *technique)
{
    Q_ASSERT(renderer);
    Q_ASSERT(technique);

    RenderRenderPassList passes;
    Q_FOREACH (const QNodeId &passId, technique->renderPasses()) {
        RenderRenderPass *renderPass = renderer->renderPassManager()->lookupResource(passId);

        if (renderPass) {
            const RenderPassFilter *passFilter = renderView->renderPassFilter();
            bool foundMatch = (!passFilter || passFilter->filters().size() == 0);

            // A pass filter is present so we need to check for matching criteria
            if (!foundMatch && renderPass->annotations().size() >= passFilter->filters().size()) {

                // Iterate through the filter criteria and look for render passes with criteria that satisfy them
                Q_FOREACH (const QNodeId &filterAnnotationId, passFilter->filters()) {
                    foundMatch = false;
                    RenderAnnotation *filterAnnotation = renderer->criterionManager()->lookupResource(filterAnnotationId);

                    Q_FOREACH (const QNodeId &passAnnotationId, renderPass->annotations()) {
                        RenderAnnotation *passAnnotation = renderer->criterionManager()->lookupResource(passAnnotationId);
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
                                const QList<QNodeId> &parameterIds)
{
    Q_FOREACH (const QNodeId &paramId, parameterIds) {
        RenderParameter *param = manager->lookupResource(paramId);
        if (param != Q_NULLPTR) {
            ParameterInfoList::iterator it = std::lower_bound(params->begin(), params->end(), param->name());
            if (it == params->end() || it->name != param->name())
                params->insert(it, ParameterInfo(param->name(), param->value()));
        }
    }
}

void parametersFromMaterialEffectTechnique(ParameterInfoList *infoList,
                                           ParameterManager *manager,
                                           RenderMaterial *material,
                                           RenderEffect *effect,
                                           RenderTechnique *technique)
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
                    RenderShaderData *subShaderData = shaderDataManager->lookupResource(list.at(i).value<QNodeId>());
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
        RenderShaderData *rSubShaderData = shaderDataManager->lookupResource(value.value<QNodeId>());
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

void UniformBlockValueBuilder::buildActiveUniformNameValueMapStructHelper(RenderShaderData *rShaderData, const QString &blockName, const QString &qmlPropertyName)
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
} // namespace Qt3D

QT_END_NAMESPACE
