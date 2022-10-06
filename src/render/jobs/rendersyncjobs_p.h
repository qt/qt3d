// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERSYNCJOBS_H
#define QT3DRENDER_RENDER_RENDERSYNCJOBS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/renderviewinitializerjob_p.h>
#include <Qt3DRender/private/frustumcullingjob_p.h>
#include <Qt3DRender/private/filterlayerentityjob_p.h>
#include <Qt3DRender/private/filterproximitydistancejob_p.h>
#include <Qt3DRender/private/materialparametergathererjob_p.h>
#include <Qt3DRender/private/renderviewcommandbuilderjob_p.h>
#include <Qt3DRender/private/renderviewcommandupdaterjob_p.h>
#include <Qt3DRender/private/renderercache_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

enum RebuildFlag {
    FullCommandRebuild = 1 << 0,
    LayerCacheRebuild = 1 << 1,
    MaterialCacheRebuild = 1 << 2,
    LightCacheRebuild = 1 << 3
};
Q_DECLARE_FLAGS(RebuildFlagSet, RebuildFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(RebuildFlagSet)

#define RenderViewInitializerJobPtrAlias RenderViewInitializerJobPtr<RenderView, Renderer>
#define RenderViewCommandBuilderJobPtrAlias RenderViewCommandBuilderJobPtr<RenderView, RenderCommand>
#define RenderViewCommandUpdaterJobPtrAlias RenderViewCommandUpdaterJobPtr<RenderView, RenderCommand>

template<class RenderView, class Renderer, class RenderCommand>
class SyncPreCommandBuilding
{
public:
    explicit SyncPreCommandBuilding(RenderViewInitializerJobPtrAlias renderViewInitializerJob,
                                    const std::vector<RenderViewCommandBuilderJobPtrAlias> &renderViewCommandBuilderJobs,
                                    Renderer *renderer,
                                    FrameGraphNode *leafNode)
        : m_renderViewInitializer(renderViewInitializerJob)
        , m_renderViewCommandBuilderJobs(renderViewCommandBuilderJobs)
        , m_renderer(renderer)
        , m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        // Split commands to build among jobs

        // Rebuild RenderCommands for all entities in RV (ignoring filtering)
        auto *cache = m_renderer->cache();
        QMutexLocker lock(cache->mutex());

        Q_ASSERT(cache->leafNodeCache.contains(m_leafNode));
        // The cache leaf should already have been created so we don't need to protect the access
        const auto &dataCacheForLeaf = cache->leafNodeCache[m_leafNode];
        RenderView *rv = m_renderViewInitializer->renderView();
        const auto &entities = !rv->isCompute() ? cache->renderableEntities : cache->computeEntities;

        rv->setMaterialParameterTable(dataCacheForLeaf.materialParameterGatherer);

        // Split among the ideal number of command builders
        const int jobCount = int(m_renderViewCommandBuilderJobs.size());
        const int entityCount = int(entities.size());
        const int idealPacketSize = std::min(std::max(10, entityCount / jobCount), entityCount);
        // Try to split work into an ideal number of workers
        const int m = findIdealNumberOfWorkers(entityCount, idealPacketSize, jobCount);

        const Entity **entitiesPtr = const_cast<const Entity **>(entities.data());
        for (int i = 0; i < m; ++i) {
            const auto &renderViewCommandBuilder = m_renderViewCommandBuilderJobs[i];
            const int count = (i == m - 1) ? entityCount - (i * idealPacketSize) : idealPacketSize;
            renderViewCommandBuilder->setEntities(entitiesPtr, i * idealPacketSize, count);
        }
    }

private:
    RenderViewInitializerJobPtrAlias m_renderViewInitializer;
    std::vector<RenderViewCommandBuilderJobPtrAlias> m_renderViewCommandBuilderJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

template<class RenderView, class Renderer, class RenderCommand>
class SyncRenderViewPostCommandUpdate
{
public:
    explicit SyncRenderViewPostCommandUpdate(const RenderViewInitializerJobPtrAlias &renderViewJob,
                                             const std::vector<RenderViewCommandUpdaterJobPtrAlias> &renderViewCommandUpdateJobs,
                                             Renderer *renderer)
        : m_renderViewJob(renderViewJob)
        , m_renderViewCommandUpdaterJobs(renderViewCommandUpdateJobs)
        , m_renderer(renderer)
    {
    }

    void operator()()
    {
        // Append all the commands and sort them
        RenderView *rv = m_renderViewJob->renderView();

        if (!rv->noDraw()) {
            // Sort command on RenderView
            rv->sort();
        }
        // Enqueue our fully populated RenderView with the RenderThread
        m_renderer->enqueueRenderView(rv, m_renderViewJob->submitOrderIndex());
    }

private:
    RenderViewInitializerJobPtrAlias m_renderViewJob;
    std::vector<RenderViewCommandUpdaterJobPtrAlias> m_renderViewCommandUpdaterJobs;
    Renderer *m_renderer;
};

template<class RenderView, class Renderer>
class SyncPreFrustumCulling
{
public:
    explicit SyncPreFrustumCulling(const RenderViewInitializerJobPtrAlias &renderViewJob,
                                   const FrustumCullingJobPtr &frustumCulling)
        : m_renderViewJob(renderViewJob)
        , m_frustumCullingJob(frustumCulling)
    {}

    void operator()()
    {
        RenderView *rv = m_renderViewJob->renderView();

        // Update matrices now that all transforms have been updated
        rv->updateMatrices();

        // Frustum culling
        m_frustumCullingJob->setViewProjection(rv->viewProjectionMatrix());
    }

private:
    RenderViewInitializerJobPtrAlias m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
};

template<class RenderView, class Renderer, class RenderCommand>
class SyncRenderViewPostInitialization
{
public:
    explicit SyncRenderViewPostInitialization(const RenderViewInitializerJobPtrAlias &renderViewJob,
                                              const FrustumCullingJobPtr &frustumCullingJob,
                                              const FilterLayerEntityJobPtr &filterEntityByLayerJob,
                                              const FilterProximityDistanceJobPtr &filterProximityJob,
                                              const std::vector<MaterialParameterGathererJobPtr> &materialGathererJobs,
                                              const std::vector<RenderViewCommandUpdaterJobPtrAlias> &renderViewCommandUpdaterJobs,
                                              const std::vector<RenderViewCommandBuilderJobPtrAlias> &renderViewCommandBuilderJobs)
        : m_renderViewJob(renderViewJob)
        , m_frustumCullingJob(frustumCullingJob)
        , m_filterEntityByLayerJob(filterEntityByLayerJob)
        , m_filterProximityJob(filterProximityJob)
        , m_materialGathererJobs(materialGathererJobs)
        , m_renderViewCommandUpdaterJobs(renderViewCommandUpdaterJobs)
        , m_renderViewCommandBuilderJobs(renderViewCommandBuilderJobs)
    {}

    void operator()()
    {
        RenderView *rv = m_renderViewJob->renderView();

        // Layer filtering
        if (!m_filterEntityByLayerJob.isNull())
            m_filterEntityByLayerJob->setLayerFilters(rv->layerFilters());

        // Proximity filtering
        m_filterProximityJob->setProximityFilterIds(rv->proximityFilterIds());

        // Material Parameter building
        for (const auto &materialGatherer : m_materialGathererJobs) {
            materialGatherer->setRenderPassFilter(const_cast<RenderPassFilter *>(rv->renderPassFilter()));
            materialGatherer->setTechniqueFilter(const_cast<TechniqueFilter *>(rv->techniqueFilter()));
        }

        // Command builders and updates
        for (const auto &renderViewCommandUpdater : m_renderViewCommandUpdaterJobs)
            renderViewCommandUpdater->setRenderView(rv);
        for (const auto &renderViewCommandBuilder : m_renderViewCommandBuilderJobs)
            renderViewCommandBuilder->setRenderView(rv);

        // Set whether frustum culling is enabled or not
        m_frustumCullingJob->setActive(rv->frustumCulling());
    }

private:
    RenderViewInitializerJobPtrAlias m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
    FilterLayerEntityJobPtr m_filterEntityByLayerJob;
    FilterProximityDistanceJobPtr m_filterProximityJob;
    std::vector<MaterialParameterGathererJobPtr> m_materialGathererJobs;
    std::vector<RenderViewCommandUpdaterJobPtrAlias> m_renderViewCommandUpdaterJobs;
    std::vector<RenderViewCommandBuilderJobPtrAlias> m_renderViewCommandBuilderJobs;
};

template<class RenderView, class Renderer, class RenderCommand>
class SyncRenderViewPreCommandUpdate
{
public:
    explicit SyncRenderViewPreCommandUpdate(const RenderViewInitializerJobPtrAlias &renderViewJob,
                                            const FrustumCullingJobPtr &frustumCullingJob,
                                            const FilterProximityDistanceJobPtr &filterProximityJob,
                                            const std::vector<MaterialParameterGathererJobPtr> &materialGathererJobs,
                                            const std::vector<RenderViewCommandUpdaterJobPtrAlias> &renderViewCommandUpdaterJobs,
                                            const std::vector<RenderViewCommandBuilderJobPtrAlias> &renderViewCommandBuilderJobs,
                                            Renderer *renderer,
                                            FrameGraphNode *leafNode,
                                            RebuildFlagSet rebuildFlags)
        : m_renderViewJob(renderViewJob)
        , m_frustumCullingJob(frustumCullingJob)
        , m_filterProximityJob(filterProximityJob)
        , m_materialGathererJobs(materialGathererJobs)
        , m_renderViewCommandUpdaterJobs(renderViewCommandUpdaterJobs)
        , m_renderViewCommandBuilderJobs(renderViewCommandBuilderJobs)
        , m_renderer(renderer)
        , m_leafNode(leafNode)
        , m_rebuildFlags(rebuildFlags)
    {}

    void operator()()
    {
        // Set the result of previous job computations
        // for final RenderCommand building
        RenderView *rv = m_renderViewJob->renderView();

        if (!rv->noDraw()) {
            ///////// CACHE LOCKED ////////////
            // Retrieve Data from Cache
            auto *cache = m_renderer->cache();
            QMutexLocker lock(cache->mutex());
            Q_ASSERT(cache->leafNodeCache.contains(m_leafNode));

            // We don't need to protect the cache access as
            // 1) The cache leaf is created
            // 2) We are only reading conccurently the cache values that are shared across all RV
            // 3) Each instance of this job is reading and writing in its own cache leaf so there's
            // no conflict

            const bool isDraw = !rv->isCompute();
            auto &cacheForLeaf = cache->leafNodeCache[m_leafNode];

            const bool fullRebuild = m_rebuildFlags.testFlag(RebuildFlag::FullCommandRebuild);
            const bool layerFilteringRebuild = m_rebuildFlags.testFlag(RebuildFlag::LayerCacheRebuild);
            const bool lightsCacheRebuild = m_rebuildFlags.testFlag(RebuildFlag::LightCacheRebuild);
            const bool cameraDirty = cacheForLeaf.viewProjectionMatrix != rv->viewProjectionMatrix();
            const bool hasProximityFilter = !rv->proximityFilterIds().empty();
            bool commandFilteringRequired =
                    fullRebuild ||
                    layerFilteringRebuild ||
                    lightsCacheRebuild ||
                    cameraDirty ||
                    hasProximityFilter;

            // If we have no filteredRenderCommandDataViews then we should have fullRebuild set to true
            // otherwise something is wrong
            Q_ASSERT(fullRebuild || cacheForLeaf.filteredRenderCommandDataViews);

            // Rebuild RenderCommands if required
            // This should happen fairly infrequently (FrameGraph Change, Geometry/Material change)
            // and allow to skip that step most of the time
            if (fullRebuild) {
                EntityRenderCommandData<RenderCommand> commandData;
                // Reduction
                {
                    int totalCommandCount = 0;
                    for (const RenderViewCommandBuilderJobPtrAlias &renderViewCommandBuilder : std::as_const(m_renderViewCommandBuilderJobs))
                        totalCommandCount += int(renderViewCommandBuilder->commandData().size());
                    commandData.reserve(totalCommandCount);
                    for (const RenderViewCommandBuilderJobPtrAlias &renderViewCommandBuilder : std::as_const(m_renderViewCommandBuilderJobs))
                        commandData += std::move(renderViewCommandBuilder->commandData());
                }

                // Store new cache
                auto dataView = EntityRenderCommandDataViewPtr<RenderCommand>::create();
                dataView->data = std::move(commandData);
                // Store the update dataView
                cacheForLeaf.filteredRenderCommandDataViews = dataView;
            }


            // Should be fairly infrequent
            if (layerFilteringRebuild || fullRebuild) {
                // Filter out renderable entities that weren't selected by the layer filters and store that in cache
                cacheForLeaf.layeredFilteredRenderables = entitiesInSubset(
                            isDraw ? cache->renderableEntities : cache->computeEntities,
                            cacheForLeaf.filterEntitiesByLayer);
                // Set default value for filteredAndCulledRenderables
                if (isDraw)
                    cacheForLeaf.filteredAndCulledRenderables = cacheForLeaf.layeredFilteredRenderables;
            }

            // Should be fairly infrequent
            if (lightsCacheRebuild) {
                // Filter out light sources that weren't selected by the
                // layer filters and store that in cache
                const std::vector<Entity *> &layeredFilteredEntities = cacheForLeaf.filterEntitiesByLayer;
                std::vector<LightSource> filteredLightSources = cache->gatheredLights;

                auto it = filteredLightSources.begin();

                while (it != filteredLightSources.end()) {
                    if (!std::binary_search(layeredFilteredEntities.begin(),
                                            layeredFilteredEntities.end(),
                                            it->entity))
                        it = filteredLightSources.erase(it);
                    else
                        ++it;
                }
                cacheForLeaf.layeredFilteredLightSources = std::move(filteredLightSources);
            }

            // This is likely very frequent
            if (cameraDirty) {
                // Record the updated viewProjectionMatrix in the cache to allow check to be performed
                // next frame
                cacheForLeaf.viewProjectionMatrix = rv->viewProjectionMatrix();
            }

            // Filter out frustum culled entity for drawable entities and store in cache
            // We need to check this regardless of whether the camera has moved since
            // entities in the scene themselves could have moved
            if (isDraw && rv->frustumCulling()) {
                const std::vector<Entity *> &subset = entitiesInSubset(
                            cacheForLeaf.layeredFilteredRenderables,
                            m_frustumCullingJob->visibleEntities());
                // Force command filtering if what we contain in cache and what we filtered differ
                commandFilteringRequired |= (subset != cacheForLeaf.filteredAndCulledRenderables);
                cacheForLeaf.filteredAndCulledRenderables = subset;
            }

            rv->setMaterialParameterTable(cacheForLeaf.materialParameterGatherer);
            rv->setEnvironmentLight(cache->environmentLight);

            // Set the light sources, with layer filters applied.
            rv->setLightSources(cacheForLeaf.layeredFilteredLightSources);

            std::vector<Entity *> renderableEntities = isDraw ? cacheForLeaf.filteredAndCulledRenderables : cacheForLeaf.layeredFilteredRenderables;

            // TO DO: Find a way to do that only if proximity entities has changed
            if (isDraw) {
                // Filter out entities which didn't satisfy proximity filtering
                if (hasProximityFilter)
                    renderableEntities = entitiesInSubset(renderableEntities,
                                                          m_filterProximityJob->filteredEntities());
            }

            EntityRenderCommandDataViewPtr<RenderCommand> filteredCommandData = cacheForLeaf.filteredRenderCommandDataViews;

            // Set RenderCommandDataView on RV (will be used later on to sort commands ...)
            rv->setRenderCommandDataView(filteredCommandData);

            // Filter out Render commands for which the Entity wasn't selected because
            // of frustum, proximity or layer filtering
            if (commandFilteringRequired) {
                const std::vector<const Entity *> &entities = filteredCommandData->data.entities;
                // Because cacheForLeaf.renderableEntities or computeEntities are sorted
                // What we get out of EntityRenderCommandData is also sorted by Entity
                auto eIt = renderableEntities.cbegin();
                const auto eEnd = renderableEntities.cend();
                size_t cIt = 0;
                const size_t cEnd = entities.size();

                std::vector<size_t> filteredCommandIndices;
                filteredCommandIndices.reserve(renderableEntities.size());

                while (eIt != eEnd) {
                    const Entity *targetEntity = *eIt;
                    // Advance until we have commands whose Entity has a lower address
                    // than the selected filtered entity
                    while (cIt != cEnd && entities[cIt] < targetEntity)
                        ++cIt;

                    // Push pointers to command data for all commands that match the
                    // entity
                    while (cIt != cEnd && entities[cIt] == targetEntity) {
                        filteredCommandIndices.push_back(cIt);
                        ++cIt;
                    }
                    ++eIt;
                }

                // Store result in cache
                cacheForLeaf.filteredRenderCommandDataViews->indices = std::move(filteredCommandIndices);
            }

            // Split among the number of command updaters
            const int jobCount = int(m_renderViewCommandUpdaterJobs.size());
            const int commandCount = int(filteredCommandData->size());
            const int idealPacketSize = std::min(std::max(10, commandCount), commandCount);
            const int m = findIdealNumberOfWorkers(commandCount, idealPacketSize, jobCount);

            for (int i = 0; i < m; ++i) {
                // TO DO: Based on whether we had to update the commands
                // we should be able to know what needs to be recomputed
                // -> lights/standard uniforms ... might no have to be set over and over again
                // if they are identical
                const RenderViewCommandUpdaterJobPtrAlias &renderViewCommandUpdater = m_renderViewCommandUpdaterJobs.at(i);
                const size_t count = (i == m - 1) ? commandCount - (i * idealPacketSize) : idealPacketSize;
                renderViewCommandUpdater->setRenderablesSubView({filteredCommandData, size_t(i * idealPacketSize), count});
            }
        }
    }

private:
    RenderViewInitializerJobPtrAlias m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
    FilterProximityDistanceJobPtr m_filterProximityJob;
    std::vector<MaterialParameterGathererJobPtr> m_materialGathererJobs;
    std::vector<RenderViewCommandUpdaterJobPtrAlias> m_renderViewCommandUpdaterJobs;
    std::vector<RenderViewCommandBuilderJobPtrAlias> m_renderViewCommandBuilderJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
    RebuildFlagSet m_rebuildFlags;
};

template<class Renderer>
class SyncFilterEntityByLayer
{
public:
    explicit SyncFilterEntityByLayer(const FilterLayerEntityJobPtr &filterEntityByLayerJob,
                                     Renderer *renderer,
                                     FrameGraphNode *leafNode)
        : m_filterEntityByLayerJob(filterEntityByLayerJob)
        , m_renderer(renderer)
        , m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        QMutexLocker lock(m_renderer->cache()->mutex());
        Q_ASSERT(m_renderer->cache()->leafNodeCache.contains(m_leafNode));
        // The cache leaf should already have been created so we don't need to protect the access
        auto &dataCacheForLeaf = m_renderer->cache()->leafNodeCache[m_leafNode];
        // Save the filtered by layer subset into the cache
        dataCacheForLeaf.filterEntitiesByLayer = std::move(m_filterEntityByLayerJob->filteredEntities());
    }

private:
    FilterLayerEntityJobPtr m_filterEntityByLayerJob;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

template<class Renderer>
class SyncMaterialParameterGatherer
{
public:
    explicit SyncMaterialParameterGatherer(const std::vector<MaterialParameterGathererJobPtr> &materialParameterGathererJobs,
                                           Renderer *renderer,
                                           FrameGraphNode *leafNode)
        : m_materialParameterGathererJobs(materialParameterGathererJobs)
        , m_renderer(renderer)
        , m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        // The cache leaf was created by SyncRenderViewPostInitialization on which we depend
        // so we don't need to protect the access
        QMutexLocker lock(m_renderer->cache()->mutex());
        auto &dataCacheForLeaf = m_renderer->cache()->leafNodeCache[m_leafNode];
        dataCacheForLeaf.materialParameterGatherer.clear();

        for (const auto &materialGatherer : m_materialParameterGathererJobs) {
            const MaterialParameterGathererData &source = materialGatherer->materialToPassAndParameter();
            for (auto it = std::begin(source); it != std::end(source); ++it) {
                Q_ASSERT(!dataCacheForLeaf.materialParameterGatherer.contains(it.key()));
                dataCacheForLeaf.materialParameterGatherer.insert(it.key(), it.value());
            }
        }
    }

private:
    std::vector<MaterialParameterGathererJobPtr> m_materialParameterGathererJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // RENDERSYNCJOBS_H
