// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderviewbuilder_p.h"
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {
namespace Rhi {

RenderViewBuilder::RenderViewBuilder(Render::FrameGraphNode *leafNode, int renderViewIndex, Renderer *renderer)
    : m_leafNode(leafNode)
    , m_renderViewIndex(renderViewIndex)
    , m_renderer(renderer)
    , m_renderViewJob(RenderViewInitializerJobPtr::create())
    , m_filterEntityByLayerJob()
    , m_frustumCullingJob(new Render::FrustumCullingJob())
    , m_syncPreFrustumCullingJob(CreateSynchronizerJobPtr(SyncPreFrustumCulling(m_renderViewJob, m_frustumCullingJob), JobTypes::SyncFrustumCulling))
    , m_syncFilterEntityByLayerJob()
    , m_filterProximityJob(Render::FilterProximityDistanceJobPtr::create())
{
    // In some cases having less jobs is better (especially on fast cpus where
    // splitting just adds more overhead). Ideally, we should try to set the value
    // depending on the platform/CPU/nbr of cores
    m_optimalParallelJobCount = Qt3DCore::QAspectJobManager::idealThreadCount();
}

RenderViewInitializerJobPtr RenderViewBuilder::renderViewJob() const
{
    return m_renderViewJob;
}

FilterLayerEntityJobPtr RenderViewBuilder::filterEntityByLayerJob() const
{
    return m_filterEntityByLayerJob;
}

FrustumCullingJobPtr RenderViewBuilder::frustumCullingJob() const
{
    return m_frustumCullingJob;
}

const std::vector<RenderViewCommandUpdaterJobPtr> &RenderViewBuilder::renderViewCommandUpdaterJobs() const
{
    return m_renderViewCommandUpdaterJobs;
}

const std::vector<RenderViewCommandBuilderJobPtr> &RenderViewBuilder::renderViewCommandBuilderJobs() const
{
    return m_renderViewCommandBuilderJobs;
}

const std::vector<MaterialParameterGathererJobPtr> &RenderViewBuilder::materialGathererJobs() const
{
    return m_materialGathererJobs;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderViewPostInitializationJob() const
{
    return m_syncRenderViewPostInitializationJob;
}

SynchronizerJobPtr RenderViewBuilder::syncPreFrustumCullingJob() const
{
    return m_syncPreFrustumCullingJob;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderViewPreCommandBuildingJob() const
{
    return m_syncRenderViewPreCommandBuildingJob;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderViewPreCommandUpdateJob() const
{
    return m_syncRenderViewPreCommandUpdateJob;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderViewPostCommandUpdateJob() const
{
    return m_syncRenderViewPostCommandUpdateJob;
}

SynchronizerJobPtr RenderViewBuilder::syncFilterEntityByLayerJob() const
{
    return m_syncFilterEntityByLayerJob;
}

SynchronizerJobPtr RenderViewBuilder::syncMaterialGathererJob() const
{
    return m_syncMaterialGathererJob;
}

FilterProximityDistanceJobPtr RenderViewBuilder::filterProximityJob() const
{
    return m_filterProximityJob;
}

void RenderViewBuilder::prepareJobs()
{
    // Init what we can here
    m_filterProximityJob->setManager(m_renderer->nodeManagers());
    m_frustumCullingJob->setRoot(m_renderer->sceneRoot());

    const bool commandsNeedRebuild = m_rebuildFlags.testFlag(RebuildFlag::FullCommandRebuild);
    if (commandsNeedRebuild) {
        m_renderViewCommandBuilderJobs.reserve(m_optimalParallelJobCount);
        for (auto i = 0; i < m_optimalParallelJobCount; ++i) {
            auto renderViewCommandBuilder = Render::Rhi::RenderViewCommandBuilderJobPtr::create();
            m_renderViewCommandBuilderJobs.push_back(renderViewCommandBuilder);
        }
        m_syncRenderViewPreCommandBuildingJob = CreateSynchronizerJobPtr(SyncPreCommandBuilding(m_renderViewJob,
                                                                                                m_renderViewCommandBuilderJobs,
                                                                                                m_renderer,
                                                                                                m_leafNode),
                                                                         JobTypes::SyncRenderViewPreCommandBuilding);
    }

    m_renderViewJob->setRenderer(m_renderer);
    m_renderViewJob->setFrameGraphLeafNode(m_leafNode);
    m_renderViewJob->setSubmitOrderIndex(m_renderViewIndex);

    // RenderCommand building is the most consuming task -> split it
    // Estimate the number of jobs to create based on the number of entities
    m_renderViewCommandUpdaterJobs.reserve(m_optimalParallelJobCount);
    for (auto i = 0; i < m_optimalParallelJobCount; ++i) {
        auto renderViewCommandUpdater = RenderViewCommandUpdaterJobPtr::create();
        m_renderViewCommandUpdaterJobs.push_back(renderViewCommandUpdater);
    }

    const bool materialCacheNeedsRebuild = m_rebuildFlags.testFlag(RebuildFlag::MaterialCacheRebuild);
    if (materialCacheNeedsRebuild) {
        // Since Material gathering is an heavy task, we split it
        const std::vector<HMaterial> &materialHandles = m_renderer->nodeManagers()->materialManager()->activeHandles();
        const size_t handlesCount = materialHandles.size();
        if (handlesCount) {
            m_materialGathererJobs.reserve(m_optimalParallelJobCount);
            const size_t elementsPerJob =  std::max(handlesCount / m_optimalParallelJobCount, size_t(1));
            size_t elementCount = 0;
            while (elementCount < handlesCount) {
                auto materialGatherer = MaterialParameterGathererJobPtr::create();
                materialGatherer->setNodeManagers(m_renderer->nodeManagers());
                // TO DO: Candidate for std::span if C++20
                materialGatherer->setHandles({materialHandles.begin() + elementCount,
                                              materialHandles.begin() + std::min(elementCount + elementsPerJob, handlesCount)});
                m_materialGathererJobs.push_back(materialGatherer);

                elementCount += elementsPerJob;
            }
        }
        m_syncMaterialGathererJob = CreateSynchronizerJobPtr(SyncMaterialParameterGatherer(m_materialGathererJobs,
                                                                                           m_renderer,
                                                                                           m_leafNode),
                                                             JobTypes::SyncMaterialGatherer);
    }

    const bool layerCacheNeedsRebuild = m_rebuildFlags.testFlag(RebuildFlag::LayerCacheRebuild);
    if (layerCacheNeedsRebuild) {
        m_filterEntityByLayerJob = Render::FilterLayerEntityJobPtr::create();
        m_filterEntityByLayerJob->setManager(m_renderer->nodeManagers());
        m_syncFilterEntityByLayerJob = CreateSynchronizerJobPtr(SyncFilterEntityByLayer(m_filterEntityByLayerJob,
                                                                                          m_renderer,
                                                                                          m_leafNode),
                                                                  JobTypes::SyncFilterEntityByLayer);
    }

    m_syncRenderViewPreCommandUpdateJob = CreateSynchronizerJobPtr(SyncRenderViewPreCommandUpdate(m_renderViewJob,
                                                                                                  m_frustumCullingJob,
                                                                                                  m_filterProximityJob,
                                                                                                  m_materialGathererJobs,
                                                                                                  m_renderViewCommandUpdaterJobs,
                                                                                                  m_renderViewCommandBuilderJobs,
                                                                                                  m_renderer,
                                                                                                  m_leafNode,
                                                                                                  m_rebuildFlags),
                                                                   JobTypes::SyncRenderViewPreCommandUpdate);

    m_syncRenderViewPostCommandUpdateJob = CreateSynchronizerJobPtr(SyncRenderViewPostCommandUpdate(m_renderViewJob,
                                                                                                    m_renderViewCommandUpdaterJobs,
                                                                                                    m_renderer),
                                                                    JobTypes::SyncRenderViewPostCommandUpdate);

    m_syncRenderViewPostInitializationJob = CreateSynchronizerJobPtr(SyncRenderViewPostInitialization(m_renderViewJob,
                                                                                                      m_frustumCullingJob,
                                                                                                      m_filterEntityByLayerJob,
                                                                                                      m_filterProximityJob,
                                                                                                      m_materialGathererJobs,
                                                                                                      m_renderViewCommandUpdaterJobs,
                                                                                                      m_renderViewCommandBuilderJobs),
                                                                     JobTypes::SyncRenderViewInitialization);
}

std::vector<Qt3DCore::QAspectJobPtr> RenderViewBuilder::buildJobHierachy() const
{
    std::vector<Qt3DCore::QAspectJobPtr> jobs;
    auto daspect = QRenderAspectPrivate::get(m_renderer->aspect());
    auto expandBVJob = daspect->m_expandBoundingVolumeJob;
    auto wordTransformJob = daspect->m_worldTransformJob;
    auto updateTreeEnabledJob = daspect->m_updateTreeEnabledJob;
    auto updateSkinningPaletteJob = daspect->m_updateSkinningPaletteJob;
    auto updateEntityLayersJob = daspect->m_updateEntityLayersJob;

    jobs.reserve(m_materialGathererJobs.size() + m_renderViewCommandUpdaterJobs.size() + 11);

    // Set dependencies

    // Finish the skinning palette job before processing renderviews
    // TODO: Maybe only update skinning palettes for non-culled entities
    m_renderViewJob->addDependency(updateSkinningPaletteJob);

    m_syncPreFrustumCullingJob->addDependency(wordTransformJob);
    m_syncPreFrustumCullingJob->addDependency(m_renderer->updateShaderDataTransformJob());
    m_syncPreFrustumCullingJob->addDependency(m_syncRenderViewPostInitializationJob);

    m_frustumCullingJob->addDependency(expandBVJob);
    m_frustumCullingJob->addDependency(m_syncPreFrustumCullingJob);

    m_syncRenderViewPostInitializationJob->addDependency(m_renderViewJob);

    m_filterProximityJob->addDependency(expandBVJob);
    m_filterProximityJob->addDependency(m_syncRenderViewPostInitializationJob);

    m_syncRenderViewPreCommandUpdateJob->addDependency(m_syncRenderViewPostInitializationJob);
    m_syncRenderViewPreCommandUpdateJob->addDependency(m_filterProximityJob);
    m_syncRenderViewPreCommandUpdateJob->addDependency(m_frustumCullingJob);

    // Ensure the RenderThread won't be able to process dirtyResources
    // before they have been completely gathered
    m_syncRenderViewPreCommandUpdateJob->addDependency(m_renderer->introspectShadersJob());
    m_syncRenderViewPreCommandUpdateJob->addDependency(m_renderer->bufferGathererJob());
    m_syncRenderViewPreCommandUpdateJob->addDependency(m_renderer->textureGathererJob());
    m_syncRenderViewPreCommandUpdateJob->addDependency(m_renderer->lightGathererJob());

    for (const auto &renderViewCommandUpdater : m_renderViewCommandUpdaterJobs) {
        renderViewCommandUpdater->addDependency(m_syncRenderViewPreCommandUpdateJob);
        m_syncRenderViewPostCommandUpdateJob->addDependency(renderViewCommandUpdater);
    }

    m_renderer->frameCleanupJob()->addDependency(m_syncRenderViewPostCommandUpdateJob);

    // Add jobs
    jobs.push_back(m_renderViewJob); // Step 1

    jobs.push_back(m_syncRenderViewPostInitializationJob); // Step 2

    const bool commandsNeedRebuild = m_rebuildFlags.testFlag(RebuildFlag::FullCommandRebuild);
    const bool materialCacheNeedsRebuild = m_rebuildFlags.testFlag(RebuildFlag::MaterialCacheRebuild);
    const bool layerCacheNeedsRebuild = m_rebuildFlags.testFlag(RebuildFlag::LayerCacheRebuild);

    if (commandsNeedRebuild) { // Step 3
        m_syncRenderViewPreCommandBuildingJob->addDependency(m_renderer->computableEntityFilterJob());
        m_syncRenderViewPreCommandBuildingJob->addDependency(m_renderer->renderableEntityFilterJob());
        m_syncRenderViewPreCommandBuildingJob->addDependency(m_syncRenderViewPostInitializationJob);

        if (materialCacheNeedsRebuild)
            m_syncRenderViewPreCommandBuildingJob->addDependency(m_syncMaterialGathererJob);

        jobs.push_back(m_syncRenderViewPreCommandBuildingJob);

        for (const auto &renderViewCommandBuilder : m_renderViewCommandBuilderJobs) {
            renderViewCommandBuilder->addDependency(m_syncRenderViewPreCommandBuildingJob);
            m_syncRenderViewPreCommandUpdateJob->addDependency(renderViewCommandBuilder);
            jobs.push_back(renderViewCommandBuilder);
        }
    }

    if (layerCacheNeedsRebuild) {
        m_filterEntityByLayerJob->addDependency(updateEntityLayersJob);
        m_filterEntityByLayerJob->addDependency(m_syncRenderViewPostInitializationJob);
        m_filterEntityByLayerJob->addDependency(updateTreeEnabledJob);

        m_syncFilterEntityByLayerJob->addDependency(m_filterEntityByLayerJob);
        m_syncRenderViewPreCommandUpdateJob->addDependency(m_syncFilterEntityByLayerJob);

        jobs.push_back(m_filterEntityByLayerJob); // Step 3
        jobs.push_back(m_syncFilterEntityByLayerJob); // Step 4
    }
    jobs.push_back(m_syncPreFrustumCullingJob); // Step 3
    jobs.push_back(m_filterProximityJob); // Step 3

    if (materialCacheNeedsRebuild) {
        for (const auto &materialGatherer : m_materialGathererJobs)  {
            materialGatherer->addDependency(m_syncRenderViewPostInitializationJob);
            materialGatherer->addDependency(m_renderer->introspectShadersJob());
            materialGatherer->addDependency(m_renderer->filterCompatibleTechniqueJob());
            jobs.push_back(materialGatherer); // Step3
            m_syncMaterialGathererJob->addDependency(materialGatherer);
        }
        m_syncRenderViewPreCommandUpdateJob->addDependency(m_syncMaterialGathererJob);

        jobs.push_back(m_syncMaterialGathererJob); // Step 3
    }

    jobs.push_back(m_frustumCullingJob); // Step 4
    jobs.push_back(m_syncRenderViewPreCommandUpdateJob); // Step 5

    // Build RenderCommands or Update RenderCommand Uniforms
    for (const auto &renderViewCommandBuilder : m_renderViewCommandUpdaterJobs) // Step 6
        jobs.push_back(renderViewCommandBuilder);

    jobs.push_back(m_syncRenderViewPostCommandUpdateJob); // Step 7

    return jobs;
}

Renderer *RenderViewBuilder::renderer() const
{
    return m_renderer;
}

int RenderViewBuilder::renderViewIndex() const
{
    return m_renderViewIndex;
}

void RenderViewBuilder::setLayerCacheNeedsToBeRebuilt(bool needsToBeRebuilt)
{
    m_rebuildFlags.setFlag(RebuildFlag::LayerCacheRebuild, needsToBeRebuilt);
}

bool RenderViewBuilder::layerCacheNeedsToBeRebuilt() const
{
    return m_rebuildFlags.testFlag(RebuildFlag::LayerCacheRebuild);
}

void RenderViewBuilder::setMaterialGathererCacheNeedsToBeRebuilt(bool needsToBeRebuilt)
{
    m_rebuildFlags.setFlag(RebuildFlag::MaterialCacheRebuild, needsToBeRebuilt);
}

bool RenderViewBuilder::materialGathererCacheNeedsToBeRebuilt() const
{
    return m_rebuildFlags.testFlag(RebuildFlag::MaterialCacheRebuild);
}

void RenderViewBuilder::setRenderCommandCacheNeedsToBeRebuilt(bool needsToBeRebuilt)
{
    m_rebuildFlags.setFlag(RebuildFlag::FullCommandRebuild, needsToBeRebuilt);
}

bool RenderViewBuilder::renderCommandCacheNeedsToBeRebuilt() const
{
    return m_rebuildFlags.testFlag(RebuildFlag::FullCommandRebuild);
}

void RenderViewBuilder::setLightCacheNeedsToBeRebuilt(bool needsToBeRebuilt)
{
    m_rebuildFlags.setFlag(RebuildFlag::LightCacheRebuild, needsToBeRebuilt);
}

bool RenderViewBuilder::lightCacheNeedsToBeRebuilt() const
{
    return m_rebuildFlags.testFlag(RebuildFlag::LightCacheRebuild);
}

int RenderViewBuilder::optimalJobCount() const
{
    return m_optimalParallelJobCount;
}

void RenderViewBuilder::setOptimalJobCount(int v)
{
    m_optimalParallelJobCount = v;
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
