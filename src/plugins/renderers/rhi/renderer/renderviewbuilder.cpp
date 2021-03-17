/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderviewbuilder_p.h"
#include <Qt3DRender/private/qrenderaspect_p.h>

#include <QThread>

namespace Qt3DRender {

namespace Render {
namespace Rhi {

// In some cases having less jobs is better (especially on fast cpus where
// splitting just adds more overhead). Ideally, we should try to set the value
// depending on the platform/CPU/nbr of cores
const int RenderViewBuilder::m_optimalParallelJobCount = QThread::idealThreadCount();

namespace {

int findIdealNumberOfWorkers(int elementCount, int packetSize = 100)
{
    if (elementCount == 0 || packetSize == 0)
        return 0;
    return std::min(std::max(elementCount / packetSize, 1), RenderViewBuilder::optimalJobCount());
}

class SyncPreCommandBuilding
{
public:
    explicit SyncPreCommandBuilding(
            RenderViewInitializerJobPtr renderViewInitializerJob,
            const QVector<RenderViewCommandBuilderJobPtr> &renderViewCommandBuilderJobs,
            Renderer *renderer, FrameGraphNode *leafNode)
        : m_renderViewInitializer(std::move(renderViewInitializerJob)),
          m_renderViewCommandBuilderJobs(renderViewCommandBuilderJobs),
          m_renderer(renderer),
          m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        // Split commands to build among jobs
        QMutexLocker lock(m_renderer->cache()->mutex());
        // Rebuild RenderCommands for all entities in RV (ignoring filtering)
        RendererCache *cache = m_renderer->cache();
        const RendererCache::LeafNodeData &dataCacheForLeaf = cache->leafNodeCache[m_leafNode];
        RenderView *rv = m_renderViewInitializer->renderView();
        const auto entities = !rv->isCompute() ? cache->renderableEntities : cache->computeEntities;

        rv->setMaterialParameterTable(dataCacheForLeaf.materialParameterGatherer);

        lock.unlock();

        // Split among the ideal number of command builders
        const int idealPacketSize =
                std::min(std::max(100, entities.size() / RenderViewBuilder::optimalJobCount()),
                         entities.size());
        // Try to split work into an ideal number of workers
        const int m = findIdealNumberOfWorkers(entities.size(), idealPacketSize);

        for (int i = 0; i < m; ++i) {
            const RenderViewCommandBuilderJobPtr renderViewCommandBuilder =
                    m_renderViewCommandBuilderJobs.at(i);
            const int count =
                    (i == m - 1) ? entities.size() - (i * idealPacketSize) : idealPacketSize;
            renderViewCommandBuilder->setEntities(entities, i * idealPacketSize, count);
        }
    }

private:
    RenderViewInitializerJobPtr m_renderViewInitializer;
    QVector<RenderViewCommandBuilderJobPtr> m_renderViewCommandBuilderJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

class SyncRenderViewPostCommandUpdate
{
public:
    explicit SyncRenderViewPostCommandUpdate(
            const RenderViewInitializerJobPtr &renderViewJob,
            const QVector<RenderViewCommandUpdaterJobPtr> &renderViewCommandUpdateJobs,
            Renderer *renderer)
        : m_renderViewJob(renderViewJob),
          m_renderViewCommandUpdaterJobs(renderViewCommandUpdateJobs),
          m_renderer(renderer)
    {
    }

    void operator()()
    {
        // Append all the commands and sort them
        RenderView *rv = m_renderViewJob->renderView();

        const EntityRenderCommandDataPtr commandData =
                m_renderViewCommandUpdaterJobs.first()->renderables();

        if (commandData) {
            const QVector<RenderCommand> commands = std::move(commandData->commands);
            rv->setCommands(commands);

            // TO DO: Find way to store commands once or at least only when required
            // Sort the commands
            rv->sort();
        }

        // Enqueue our fully populated RenderView with the RenderThread
        m_renderer->enqueueRenderView(rv, m_renderViewJob->submitOrderIndex());
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
    QVector<RenderViewCommandUpdaterJobPtr> m_renderViewCommandUpdaterJobs;
    Renderer *m_renderer;
};

class SyncPreFrustumCulling
{
public:
    explicit SyncPreFrustumCulling(const RenderViewInitializerJobPtr &renderViewJob,
                                   const FrustumCullingJobPtr &frustumCulling)
        : m_renderViewJob(renderViewJob), m_frustumCullingJob(frustumCulling)
    {
    }

    void operator()()
    {
        RenderView *rv = m_renderViewJob->renderView();

        // Update matrices now that all transforms have been updated
        rv->updateMatrices();

        // Frustum culling
        m_frustumCullingJob->setViewProjection(rv->viewProjectionMatrix());
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
};

class SyncRenderViewPostInitialization
{
public:
    explicit SyncRenderViewPostInitialization(
            const RenderViewInitializerJobPtr &renderViewJob,
            const FrustumCullingJobPtr &frustumCullingJob,
            const FilterLayerEntityJobPtr &filterEntityByLayerJob,
            const FilterProximityDistanceJobPtr &filterProximityJob,
            const QVector<MaterialParameterGathererJobPtr> &materialGathererJobs,
            const QVector<RenderViewCommandUpdaterJobPtr> &renderViewCommandUpdaterJobs,
            const QVector<RenderViewCommandBuilderJobPtr> &renderViewCommandBuilderJobs)
        : m_renderViewJob(renderViewJob),
          m_frustumCullingJob(frustumCullingJob),
          m_filterEntityByLayerJob(filterEntityByLayerJob),
          m_filterProximityJob(filterProximityJob),
          m_materialGathererJobs(materialGathererJobs),
          m_renderViewCommandUpdaterJobs(renderViewCommandUpdaterJobs),
          m_renderViewCommandBuilderJobs(renderViewCommandBuilderJobs)
    {
    }

    void operator()()
    {
        RenderView *rv = m_renderViewJob->renderView();

        // Layer filtering
        if (!m_filterEntityByLayerJob.isNull())
            m_filterEntityByLayerJob->setLayerFilters(rv->layerFilters());

        // Proximity filtering
        m_filterProximityJob->setProximityFilterIds(rv->proximityFilterIds());

        // Material Parameter building
        for (const auto &materialGatherer : qAsConst(m_materialGathererJobs)) {
            materialGatherer->setRenderPassFilter(
                    const_cast<RenderPassFilter *>(rv->renderPassFilter()));
            materialGatherer->setTechniqueFilter(
                    const_cast<TechniqueFilter *>(rv->techniqueFilter()));
        }

        // Command builders and updates
        for (const auto &renderViewCommandUpdater : qAsConst(m_renderViewCommandUpdaterJobs))
            renderViewCommandUpdater->setRenderView(rv);
        for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewCommandBuilderJobs))
            renderViewCommandBuilder->setRenderView(rv);

        // Set whether frustum culling is enabled or not
        m_frustumCullingJob->setActive(rv->frustumCulling());
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
    FilterLayerEntityJobPtr m_filterEntityByLayerJob;
    FilterProximityDistanceJobPtr m_filterProximityJob;
    QVector<MaterialParameterGathererJobPtr> m_materialGathererJobs;
    QVector<RenderViewCommandUpdaterJobPtr> m_renderViewCommandUpdaterJobs;
    QVector<RenderViewCommandBuilderJobPtr> m_renderViewCommandBuilderJobs;
};

class SyncRenderViewPreCommandUpdate
{
public:
    explicit SyncRenderViewPreCommandUpdate(
            const RenderViewInitializerJobPtr &renderViewJob,
            const FrustumCullingJobPtr &frustumCullingJob,
            const FilterProximityDistanceJobPtr &filterProximityJob,
            const QVector<MaterialParameterGathererJobPtr> &materialGathererJobs,
            const QVector<RenderViewCommandUpdaterJobPtr> &renderViewCommandUpdaterJobs,
            const QVector<RenderViewCommandBuilderJobPtr> &renderViewCommandBuilderJobs,
            Renderer *renderer, FrameGraphNode *leafNode, bool fullCommandRebuild)
        : m_renderViewJob(renderViewJob),
          m_frustumCullingJob(frustumCullingJob),
          m_filterProximityJob(filterProximityJob),
          m_materialGathererJobs(materialGathererJobs),
          m_renderViewCommandUpdaterJobs(renderViewCommandUpdaterJobs),
          m_renderViewCommandBuilderJobs(renderViewCommandBuilderJobs),
          m_renderer(renderer),
          m_leafNode(leafNode),
          m_fullRebuild(fullCommandRebuild)
    {
    }

    void operator()()
    {
        // Set the result of previous job computations
        // for final RenderCommand building
        RenderView *rv = m_renderViewJob->renderView();

        if (!rv->noDraw()) {
            ///////// CACHE LOCKED ////////////
            // Retrieve Data from Cache
            RendererCache *cache = m_renderer->cache();
            QMutexLocker lock(cache->mutex());
            Q_ASSERT(cache->leafNodeCache.contains(m_leafNode));

            const bool isDraw = !rv->isCompute();
            const RendererCache::LeafNodeData &dataCacheForLeaf = cache->leafNodeCache[m_leafNode];

            // Rebuild RenderCommands if required
            // This should happen fairly infrequently (FrameGraph Change, Geometry/Material change)
            // and allow to skip that step most of the time
            if (m_fullRebuild) {
                EntityRenderCommandData commandData;
                // Reduction
                {
                    int totalCommandCount = 0;
                    for (const RenderViewCommandBuilderJobPtr &renderViewCommandBuilder :
                         qAsConst(m_renderViewCommandBuilderJobs))
                        totalCommandCount += renderViewCommandBuilder->commandData().size();
                    commandData.reserve(totalCommandCount);
                    // assert(totalCommandCount != 0);
                    for (const RenderViewCommandBuilderJobPtr &renderViewCommandBuilder :
                         qAsConst(m_renderViewCommandBuilderJobs))
                        commandData += std::move(renderViewCommandBuilder->commandData());
                }

                // Store new cache
                RendererCache::LeafNodeData &writableCacheForLeaf =
                        cache->leafNodeCache[m_leafNode];
                writableCacheForLeaf.renderCommandData = std::move(commandData);
            }
            const EntityRenderCommandData commandData = dataCacheForLeaf.renderCommandData;
            const QVector<Entity *> filteredEntities = dataCacheForLeaf.filterEntitiesByLayer;
            QVector<Entity *> renderableEntities =
                    isDraw ? cache->renderableEntities : cache->computeEntities;
            QVector<LightSource> lightSources = cache->gatheredLights;

            rv->setMaterialParameterTable(dataCacheForLeaf.materialParameterGatherer);
            rv->setEnvironmentLight(cache->environmentLight);
            lock.unlock();
            ///////// END OF CACHE LOCKED ////////////

            // Filter out entities that weren't selected by the layer filters
            // Remove all entities from the compute and renderable vectors that aren't in the
            // filtered layer vector
            renderableEntities =
                    RenderViewBuilder::entitiesInSubset(renderableEntities, filteredEntities);

            // Set the light sources, with layer filters applied.
            for (int i = 0; i < lightSources.count(); ++i) {
                if (!filteredEntities.contains(lightSources[i].entity))
                    lightSources.removeAt(i--);
            }
            rv->setLightSources(lightSources);

            if (isDraw) {
                // Filter out frustum culled entity for drawable entities
                if (rv->frustumCulling())
                    renderableEntities = RenderViewBuilder::entitiesInSubset(
                            renderableEntities, m_frustumCullingJob->visibleEntities());
                // Filter out entities which didn't satisfy proximity filtering
                if (!rv->proximityFilterIds().empty())
                    renderableEntities = RenderViewBuilder::entitiesInSubset(
                            renderableEntities, m_filterProximityJob->filteredEntities());
            }

            // Early return in case we have nothing to filter
            if (renderableEntities.size() == 0)
                return;

            // Filter out Render commands for which the Entity wasn't selected because
            // of frustum, proximity or layer filtering
            EntityRenderCommandDataPtr filteredCommandData = EntityRenderCommandDataPtr::create();
            filteredCommandData->reserve(renderableEntities.size());
            // Because dataCacheForLeaf.renderableEntities or computeEntities are sorted
            // What we get out of EntityRenderCommandData is also sorted by Entity
            auto eIt = std::cbegin(renderableEntities);
            const auto eEnd = std::cend(renderableEntities);
            int cIt = 0;
            const int cEnd = commandData.size();

            while (eIt != eEnd) {
                const Entity *targetEntity = *eIt;
                // Advance until we have commands whose Entity has a lower address
                // than the selected filtered entity
                while (cIt != cEnd && commandData.entities.at(cIt) < targetEntity)
                    ++cIt;

                // Push pointers to command data for all commands that match the
                // entity
                while (cIt != cEnd && commandData.entities.at(cIt) == targetEntity) {
                    filteredCommandData->push_back(commandData.entities.at(cIt),
                                                   commandData.commands.at(cIt),
                                                   commandData.passesData.at(cIt));
                    ++cIt;
                }
                ++eIt;
            }

            // Split among the number of command builders
            // The idealPacketSize is at least 100 entities per worker
            const int idealPacketSize = std::min(
                    std::max(100,
                             filteredCommandData->size() / RenderViewBuilder::optimalJobCount()),
                    filteredCommandData->size());
            const int m = findIdealNumberOfWorkers(filteredCommandData->size(), idealPacketSize);

            for (int i = 0; i < m; ++i) {
                const RenderViewCommandUpdaterJobPtr renderViewCommandBuilder =
                        m_renderViewCommandUpdaterJobs.at(i);
                const int count = (i == m - 1) ? filteredCommandData->size() - (i * idealPacketSize)
                                               : idealPacketSize;
                renderViewCommandBuilder->setRenderables(filteredCommandData, i * idealPacketSize,
                                                         count);
            }
        }
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
    FilterProximityDistanceJobPtr m_filterProximityJob;
    QVector<MaterialParameterGathererJobPtr> m_materialGathererJobs;
    QVector<RenderViewCommandUpdaterJobPtr> m_renderViewCommandUpdaterJobs;
    QVector<RenderViewCommandBuilderJobPtr> m_renderViewCommandBuilderJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
    bool m_fullRebuild;
};

class SetClearDrawBufferIndex
{
public:
    explicit SetClearDrawBufferIndex(const RenderViewInitializerJobPtr &renderViewJob)
        : m_renderViewJob(renderViewJob)
    {
    }

    void operator()()
    {
        RenderView *rv = m_renderViewJob->renderView();
        QVector<ClearBufferInfo> &clearBuffersInfo = rv->specificClearColorBufferInfo();
        const AttachmentPack &attachmentPack = rv->attachmentPack();
        for (ClearBufferInfo &clearBufferInfo : clearBuffersInfo)
            clearBufferInfo.drawBufferIndex =
                    attachmentPack.getDrawBufferIndex(clearBufferInfo.attchmentPoint);
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
};

class SyncFilterEntityByLayer
{
public:
    explicit SyncFilterEntityByLayer(const FilterLayerEntityJobPtr &filterEntityByLayerJob,
                                     Renderer *renderer, FrameGraphNode *leafNode)
        : m_filterEntityByLayerJob(filterEntityByLayerJob),
          m_renderer(renderer),
          m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        QMutexLocker lock(m_renderer->cache()->mutex());
        // Save the filtered by layer subset into the cache
        const QVector<Entity *> filteredEntities = m_filterEntityByLayerJob->filteredEntities();
        RendererCache::LeafNodeData &dataCacheForLeaf =
                m_renderer->cache()->leafNodeCache[m_leafNode];
        dataCacheForLeaf.filterEntitiesByLayer = filteredEntities;
    }

private:
    FilterLayerEntityJobPtr m_filterEntityByLayerJob;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

class SyncMaterialParameterGatherer
{
public:
    explicit SyncMaterialParameterGatherer(
            const QVector<MaterialParameterGathererJobPtr> &materialParameterGathererJobs,
            Renderer *renderer, FrameGraphNode *leafNode)
        : m_materialParameterGathererJobs(materialParameterGathererJobs),
          m_renderer(renderer),
          m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        QMutexLocker lock(m_renderer->cache()->mutex());
        RendererCache::LeafNodeData &dataCacheForLeaf =
                m_renderer->cache()->leafNodeCache[m_leafNode];
        dataCacheForLeaf.materialParameterGatherer.clear();

        for (const auto &materialGatherer : qAsConst(m_materialParameterGathererJobs))
            dataCacheForLeaf.materialParameterGatherer.unite(
                    materialGatherer->materialToPassAndParameter());
    }

private:
    QVector<MaterialParameterGathererJobPtr> m_materialParameterGathererJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

} // anonymous

RenderViewBuilder::RenderViewBuilder(Render::FrameGraphNode *leafNode, int renderViewIndex,
                                     Renderer *renderer)
    : m_leafNode(leafNode),
      m_renderViewIndex(renderViewIndex),
      m_renderer(renderer),
      m_layerCacheNeedsToBeRebuilt(false),
      m_materialGathererCacheNeedsToBeRebuilt(false),
      m_renderCommandCacheNeedsToBeRebuilt(false),
      m_renderViewJob(RenderViewInitializerJobPtr::create()),
      m_filterEntityByLayerJob(),
      m_frustumCullingJob(new Render::FrustumCullingJob()),
      m_syncPreFrustumCullingJob(SynchronizerJobPtr::create(
              SyncPreFrustumCulling(m_renderViewJob, m_frustumCullingJob),
              JobTypes::SyncFrustumCulling)),
      m_setClearDrawBufferIndexJob(SynchronizerJobPtr::create(
              SetClearDrawBufferIndex(m_renderViewJob), JobTypes::ClearBufferDrawIndex)),
      m_syncFilterEntityByLayerJob(),
      m_filterProximityJob(Render::FilterProximityDistanceJobPtr::create())
{
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

QVector<RenderViewCommandUpdaterJobPtr> RenderViewBuilder::renderViewCommandUpdaterJobs() const
{
    return m_renderViewCommandUpdaterJobs;
}

QVector<RenderViewCommandBuilderJobPtr> RenderViewBuilder::renderViewCommandBuilderJobs() const
{
    return m_renderViewCommandBuilderJobs;
}

QVector<MaterialParameterGathererJobPtr> RenderViewBuilder::materialGathererJobs() const
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

SynchronizerJobPtr RenderViewBuilder::setClearDrawBufferIndexJob() const
{
    return m_setClearDrawBufferIndexJob;
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

    if (m_renderCommandCacheNeedsToBeRebuilt) {

        m_renderViewCommandBuilderJobs.reserve(RenderViewBuilder::m_optimalParallelJobCount);
        for (auto i = 0; i < RenderViewBuilder::m_optimalParallelJobCount; ++i) {
            auto renderViewCommandBuilder = Render::Rhi::RenderViewCommandBuilderJobPtr::create();
            m_renderViewCommandBuilderJobs.push_back(renderViewCommandBuilder);
        }
        m_syncRenderViewPreCommandBuildingJob = SynchronizerJobPtr::create(
                SyncPreCommandBuilding(m_renderViewJob, m_renderViewCommandBuilderJobs, m_renderer,
                                       m_leafNode),
                JobTypes::SyncRenderViewPreCommandBuilding);
    }

    m_renderViewJob->setRenderer(m_renderer);
    m_renderViewJob->setFrameGraphLeafNode(m_leafNode);
    m_renderViewJob->setSubmitOrderIndex(m_renderViewIndex);

    // RenderCommand building is the most consuming task -> split it
    // Estimate the number of jobs to create based on the number of entities
    m_renderViewCommandUpdaterJobs.reserve(RenderViewBuilder::m_optimalParallelJobCount);
    for (auto i = 0; i < RenderViewBuilder::m_optimalParallelJobCount; ++i) {
        auto renderViewCommandUpdater = Render::Rhi::RenderViewCommandUpdaterJobPtr::create();
        renderViewCommandUpdater->setRenderer(m_renderer);
        m_renderViewCommandUpdaterJobs.push_back(renderViewCommandUpdater);
    }

    if (m_materialGathererCacheNeedsToBeRebuilt) {
        // Since Material gathering is an heavy task, we split it
        const std::vector<HMaterial> &materialHandles =
                m_renderer->nodeManagers()->materialManager()->activeHandles();
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
        m_syncMaterialGathererJob = SynchronizerJobPtr::create(
                SyncMaterialParameterGatherer(m_materialGathererJobs, m_renderer, m_leafNode),
                JobTypes::SyncMaterialGatherer);
    }

    if (m_layerCacheNeedsToBeRebuilt) {
        m_filterEntityByLayerJob = Render::FilterLayerEntityJobPtr::create();
        m_filterEntityByLayerJob->setManager(m_renderer->nodeManagers());
        m_syncFilterEntityByLayerJob = SynchronizerJobPtr::create(
                SyncFilterEntityByLayer(m_filterEntityByLayerJob, m_renderer, m_leafNode),
                JobTypes::SyncFilterEntityByLayer);
    }

    m_syncRenderViewPreCommandUpdateJob = SynchronizerJobPtr::create(
            SyncRenderViewPreCommandUpdate(m_renderViewJob, m_frustumCullingJob,
                                           m_filterProximityJob, m_materialGathererJobs,
                                           m_renderViewCommandUpdaterJobs,
                                           m_renderViewCommandBuilderJobs, m_renderer, m_leafNode,
                                           m_renderCommandCacheNeedsToBeRebuilt),
            JobTypes::SyncRenderViewPreCommandUpdate);

    m_syncRenderViewPostCommandUpdateJob = SynchronizerJobPtr::create(
            SyncRenderViewPostCommandUpdate(m_renderViewJob, m_renderViewCommandUpdaterJobs,
                                            m_renderer),
            JobTypes::SyncRenderViewPostCommandUpdate);

    m_syncRenderViewPostInitializationJob = SynchronizerJobPtr::create(
            SyncRenderViewPostInitialization(m_renderViewJob, m_frustumCullingJob,
                                             m_filterEntityByLayerJob, m_filterProximityJob,
                                             m_materialGathererJobs, m_renderViewCommandUpdaterJobs,
                                             m_renderViewCommandBuilderJobs),
            JobTypes::SyncRenderViewInitialization);
}

QVector<Qt3DCore::QAspectJobPtr> RenderViewBuilder::buildJobHierachy() const
{
    QVector<Qt3DCore::QAspectJobPtr> jobs;
    auto daspect = QRenderAspectPrivate::get(m_renderer->aspect());
    auto expandBVJob = daspect->m_expandBoundingVolumeJob;
    auto worldTransformJob = daspect->m_worldTransformJob;
    auto updateTreeEnabledJob = daspect->m_updateTreeEnabledJob;
    auto updateSkinningPaletteJob = daspect->m_updateSkinningPaletteJob;
    auto updateEntityLayersJob = daspect->m_updateEntityLayersJob;

    jobs.reserve(m_materialGathererJobs.size() + m_renderViewCommandUpdaterJobs.size() + 11);

    // Set dependencies

    // Finish the skinning palette job before processing renderviews
    // TODO: Maybe only update skinning palettes for non-culled entities
    m_renderViewJob->addDependency(updateSkinningPaletteJob);

    m_syncPreFrustumCullingJob->addDependency(worldTransformJob);
    m_syncPreFrustumCullingJob->addDependency(m_renderer->updateShaderDataTransformJob());
    m_syncPreFrustumCullingJob->addDependency(m_syncRenderViewPostInitializationJob);

    m_frustumCullingJob->addDependency(expandBVJob);
    m_frustumCullingJob->addDependency(m_syncPreFrustumCullingJob);

    m_setClearDrawBufferIndexJob->addDependency(m_syncRenderViewPostInitializationJob);

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

    for (const auto &renderViewCommandUpdater : qAsConst(m_renderViewCommandUpdaterJobs)) {
        renderViewCommandUpdater->addDependency(m_syncRenderViewPreCommandUpdateJob);
        m_syncRenderViewPostCommandUpdateJob->addDependency(renderViewCommandUpdater);
    }

    m_renderer->frameCleanupJob()->addDependency(m_syncRenderViewPostCommandUpdateJob);
    m_renderer->frameCleanupJob()->addDependency(m_setClearDrawBufferIndexJob);

    // Add jobs
    jobs.push_back(m_renderViewJob); // Step 1

    jobs.push_back(m_syncRenderViewPostInitializationJob); // Step 2

    if (m_renderCommandCacheNeedsToBeRebuilt) { // Step 3
        m_syncRenderViewPreCommandBuildingJob->addDependency(
                m_renderer->computableEntityFilterJob());
        m_syncRenderViewPreCommandBuildingJob->addDependency(
                m_renderer->renderableEntityFilterJob());
        m_syncRenderViewPreCommandBuildingJob->addDependency(m_syncRenderViewPostInitializationJob);

        if (m_materialGathererCacheNeedsToBeRebuilt)
            m_syncRenderViewPreCommandBuildingJob->addDependency(m_syncMaterialGathererJob);

        jobs.push_back(m_syncRenderViewPreCommandBuildingJob);

        for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewCommandBuilderJobs)) {
            renderViewCommandBuilder->addDependency(m_syncRenderViewPreCommandBuildingJob);
            m_syncRenderViewPreCommandUpdateJob->addDependency(renderViewCommandBuilder);
            jobs.push_back(renderViewCommandBuilder);
        }
    }

    if (m_layerCacheNeedsToBeRebuilt) {
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
    jobs.push_back(m_setClearDrawBufferIndexJob); // Step 3

    if (m_materialGathererCacheNeedsToBeRebuilt) {
        for (const auto &materialGatherer : qAsConst(m_materialGathererJobs)) {
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
    for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewCommandUpdaterJobs)) // Step 6
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
    m_layerCacheNeedsToBeRebuilt = needsToBeRebuilt;
}

bool RenderViewBuilder::layerCacheNeedsToBeRebuilt() const
{
    return m_layerCacheNeedsToBeRebuilt;
}

void RenderViewBuilder::setMaterialGathererCacheNeedsToBeRebuilt(bool needsToBeRebuilt)
{
    m_materialGathererCacheNeedsToBeRebuilt = needsToBeRebuilt;
}

bool RenderViewBuilder::materialGathererCacheNeedsToBeRebuilt() const
{
    return m_materialGathererCacheNeedsToBeRebuilt;
}

void RenderViewBuilder::setRenderCommandCacheNeedsToBeRebuilt(bool needsToBeRebuilt)
{
    m_renderCommandCacheNeedsToBeRebuilt = needsToBeRebuilt;
}

bool RenderViewBuilder::renderCommandCacheNeedsToBeRebuilt() const
{
    return m_renderCommandCacheNeedsToBeRebuilt;
}

int RenderViewBuilder::optimalJobCount()
{
    return RenderViewBuilder::m_optimalParallelJobCount;
}

QVector<Entity *> RenderViewBuilder::entitiesInSubset(const QVector<Entity *> &entities,
                                                      const QVector<Entity *> &subset)
{
    QVector<Entity *> intersection;
    intersection.reserve(qMin(entities.size(), subset.size()));
    std::set_intersection(entities.begin(), entities.end(), subset.begin(), subset.end(),
                          std::back_inserter(intersection));

    return intersection;
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
