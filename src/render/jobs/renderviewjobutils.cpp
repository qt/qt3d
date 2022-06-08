// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderviewjobutils_p.h"
#include <Qt3DRender/private/renderlogging_p.h>

#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/qshaderdata.h>

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/renderstatenode_p.h>
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/renderstateset_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

/*!
    \internal
    Searches the best matching Technique from \a effect specified.
*/
Technique *findTechniqueForEffect(NodeManagers *manager,
                                  const TechniqueFilter *techniqueFilter,
                                  Effect *effect)
{
    if (!effect)
        return nullptr;

    std::vector<Technique*> matchingTechniques;
    const bool hasInvalidTechniqueFilter = (techniqueFilter == nullptr || techniqueFilter->filters().isEmpty());

    // Iterate through the techniques in the effect
    const auto techniqueIds = effect->techniques();
    for (const QNodeId &techniqueId : techniqueIds) {
        Technique *technique = manager->techniqueManager()->lookupResource(techniqueId);

        // Should be valid, if not there likely a problem with node addition/destruction changes
        Q_ASSERT(technique);

        // Check if the technique is compatible with the rendering API
        // If no techniqueFilter is present, we return the technique as it satisfies OpenGL version
        if (technique->isCompatibleWithRenderer() && (hasInvalidTechniqueFilter || technique->isCompatibleWithFilters(techniqueFilter->filters())))
            matchingTechniques.push_back(technique);
    }

    if (matchingTechniques.size() == 0) // We failed to find a suitable technique to use :(
        return nullptr;

    if (matchingTechniques.size() == 1)
        return matchingTechniques.front();

    // Several compatible techniques, return technique with highest major and minor version
    Technique* highest = matchingTechniques.front();
    GraphicsApiFilterData filter = *highest->graphicsApiFilter();
    for (auto it = matchingTechniques.cbegin() + 1; it < matchingTechniques.cend(); ++it) {
        if (filter < *(*it)->graphicsApiFilter()) {
            filter = *(*it)->graphicsApiFilter();
            highest = *it;
        }
    }
    return highest;
}


RenderPassList findRenderPassesForTechnique(NodeManagers *manager,
                                            const RenderPassFilter *passFilter,
                                            Technique *technique)
{
    Q_ASSERT(manager);
    Q_ASSERT(technique);

    RenderPassList passes;
    const auto passIds = technique->renderPasses();
    for (const QNodeId &passId : passIds) {
        RenderPass *renderPass = manager->renderPassManager()->lookupResource(passId);

        if (renderPass && renderPass->isEnabled()) {
            bool foundMatch = (!passFilter || passFilter->filters().size() == 0);

            // A pass filter is present so we need to check for matching criteria
            if (!foundMatch && renderPass->filterKeys().size() >= passFilter->filters().size()) {

                // Iterate through the filter criteria and look for render passes with criteria that satisfy them
                const auto filterKeyIds = passFilter->filters();
                for (const QNodeId &filterKeyId : filterKeyIds) {
                    foundMatch = false;
                    FilterKey *filterFilterKey = manager->filterKeyManager()->lookupResource(filterKeyId);

                    const auto passFilterKeyIds = renderPass->filterKeys();
                    for (const QNodeId &passFilterKeyId : passFilterKeyIds) {
                        FilterKey *passFilterKey = manager->filterKeyManager()->lookupResource(passFilterKeyId);
                        if ((foundMatch = (*passFilterKey == *filterFilterKey)))
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


ParameterInfoList::const_iterator findParamInfo(ParameterInfoList *params, const int nameId)
{
    const ParameterInfoList::const_iterator end = params->cend();
    ParameterInfoList::const_iterator it = std::lower_bound(params->cbegin(), end, nameId);
    if (it != end && it->nameId != nameId)
        return end;
    return it;
}

void addParametersForIds(ParameterInfoList *params, ParameterManager *manager,
                         const Qt3DCore::QNodeIdVector &parameterIds)
{
    for (const QNodeId &paramId : parameterIds) {
        const HParameter parameterHandle = manager->lookupHandle(paramId);
        const Parameter *param = manager->data(parameterHandle);
        ParameterInfoList::iterator it = std::lower_bound(params->begin(), params->end(), param->nameId());
        if (it == params->end() || it->nameId != param->nameId())
            params->insert(it, ParameterInfo(param->nameId(), parameterHandle));
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
    // 2) Effect
    // 3) Technique
    //
    // That way a user can override defaults in Effect's and Techniques on a
    // object manner and a Technique can override global defaults from the Effect.
    parametersFromParametersProvider(infoList, manager, material);
    parametersFromParametersProvider(infoList, manager, effect);
    parametersFromParametersProvider(infoList, manager, technique);
}

// Only add states with types we don't already have
void addStatesToRenderStateSet(RenderStateSet *stateSet,
                               const QList<Qt3DCore::QNodeId> stateIds,
                               RenderStateManager *manager)
{
    for (const Qt3DCore::QNodeId &stateId : stateIds) {
        RenderStateNode *node = manager->lookupResource(stateId);
        if (node && node->isEnabled() && stateSet->canAddStateOfType(node->type())) {
            stateSet->addState(node->impl());
        }
    }
}

ParameterInfo::ParameterInfo(const int nameId, const HParameter &handle)
    : nameId(nameId)
    , handle(handle)
{}

bool ParameterInfo::operator<(const ParameterInfo &other) const noexcept
{
    return nameId < other.nameId;
}

bool ParameterInfo::operator<(const int otherNameId) const noexcept
{
    return nameId < otherNameId;
}

int findIdealNumberOfWorkers(int elementCount, int packetSize, int maxJobCount)
{
    if (elementCount == 0 || packetSize == 0)
        return 0;
    return std::min(std::max(elementCount / packetSize, 1), maxJobCount);
}

std::vector<Entity *> entitiesInSubset(const std::vector<Entity *> &entities, const std::vector<Entity *> &subset)
{
    std::vector<Entity *> intersection;
    intersection.reserve(qMin(entities.size(), subset.size()));
    std::set_intersection(entities.begin(), entities.end(),
                          subset.begin(), subset.end(),
                          std::back_inserter(intersection));

    return intersection;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
