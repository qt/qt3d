/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

const int RenderViewBuilder::m_optimalParallelJobCount = std::max(QThread::idealThreadCount(), 2);

namespace {

class SyncRenderViewCommandBuilders
{
public:
    explicit SyncRenderViewCommandBuilders(const RenderViewInitializerJobPtr &renderViewJob,
                                           const QVector<RenderViewBuilderJobPtr> &renderViewBuilderJobs,
                                           Renderer *renderer)
        : m_renderViewJob(renderViewJob)
        , m_renderViewBuilderJobs(renderViewBuilderJobs)
        , m_renderer(renderer)
    {}

    void operator()()
    {
        // Append all the commands and sort them
        RenderView *rv = m_renderViewJob->renderView();

        int totalCommandCount = 0;
        for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewBuilderJobs))
            totalCommandCount += renderViewCommandBuilder->commands().size();

        QVector<RenderCommand *> commands;
        commands.reserve(totalCommandCount);

        // Reduction
        for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewBuilderJobs))
            commands += std::move(renderViewCommandBuilder->commands());
        rv->setCommands(commands);

        // Sort the commands
        rv->sort();

        // Enqueue our fully populated RenderView with the RenderThread
        m_renderer->enqueueRenderView(rv, m_renderViewJob->submitOrderIndex());
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
    QVector<RenderViewBuilderJobPtr> m_renderViewBuilderJobs;
    Renderer *m_renderer;
};

class SyncFrustumCulling
{
public:
    explicit SyncFrustumCulling(const RenderViewInitializerJobPtr &renderViewJob,
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
    RenderViewInitializerJobPtr m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
};

class SyncRenderViewInitialization
{
public:
    explicit SyncRenderViewInitialization(const RenderViewInitializerJobPtr &renderViewJob,
                                          const FrustumCullingJobPtr &frustumCullingJob,
                                          const FilterLayerEntityJobPtr &filterEntityByLayerJob,
                                          const FilterProximityDistanceJobPtr &filterProximityJob,
                                          const QVector<MaterialParameterGathererJobPtr> &materialGathererJobs,
                                          const QVector<RenderViewBuilderJobPtr> &renderViewBuilderJobs)
        : m_renderViewJob(renderViewJob)
        , m_frustumCullingJob(frustumCullingJob)
        , m_filterEntityByLayerJob(filterEntityByLayerJob)
        , m_filterProximityJob(filterProximityJob)
        , m_materialGathererJobs(materialGathererJobs)
        , m_renderViewBuilderJobs(renderViewBuilderJobs)
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
        for (const auto &materialGatherer : qAsConst(m_materialGathererJobs)) {
            materialGatherer->setRenderPassFilter(const_cast<RenderPassFilter *>(rv->renderPassFilter()));
            materialGatherer->setTechniqueFilter(const_cast<TechniqueFilter *>(rv->techniqueFilter()));
        }

        // Command builders
        for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewBuilderJobs))
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
    QVector<RenderViewBuilderJobPtr> m_renderViewBuilderJobs;
};

class SyncRenderCommandBuilding
{
public:
    explicit SyncRenderCommandBuilding(const RenderViewInitializerJobPtr &renderViewJob,
                                       const FrustumCullingJobPtr &frustumCullingJob,
                                       const FilterProximityDistanceJobPtr &filterProximityJob,
                                       const LightGathererPtr &lightGathererJob,
                                       const RenderableEntityFilterPtr &renderableEntityFilterJob,
                                       const ComputableEntityFilterPtr &computableEntityFilterJob,
                                       const QVector<MaterialParameterGathererJobPtr> &materialGathererJobs,
                                       const QVector<RenderViewBuilderJobPtr> &renderViewBuilderJobs,
                                       Renderer *renderer,
                                       FrameGraphNode *leafNode)
        : m_renderViewJob(renderViewJob)
        , m_frustumCullingJob(frustumCullingJob)
        , m_filterProximityJob(filterProximityJob)
        , m_lightGathererJob(lightGathererJob)
        , m_renderableEntityFilterJob(renderableEntityFilterJob)
        , m_computableEntityFilterJob(computableEntityFilterJob)
        , m_materialGathererJobs(materialGathererJobs)
        , m_renderViewBuilderJobs(renderViewBuilderJobs)
        , m_renderer(renderer)
        , m_leafNode(leafNode)
    {}

    void operator()()
    {
        // Set the result of previous job computations
        // for final RenderCommand building
        RenderView *rv = m_renderViewJob->renderView();

        if (!rv->noDraw()) {
            rv->setEnvironmentLight(m_lightGathererJob->takeEnvironmentLight());

            // We sort the vector so that the removal can then be performed linearly

            QVector<Entity *> renderableEntities;
            const bool isDraw = !rv->isCompute();
            if (isDraw)
                renderableEntities = std::move(m_renderableEntityFilterJob->filteredEntities());
            else
                renderableEntities = std::move(m_computableEntityFilterJob->filteredEntities());

            // Filter out entities that weren't selected by the layer filters
            std::sort(renderableEntities.begin(), renderableEntities.end());

            QMutexLocker lock(m_renderer->cache()->mutex());
            const QVector<Entity *> filteredEntities = m_renderer->cache()->leafNodeCache.value(m_leafNode).filterEntitiesByLayer;
            lock.unlock();
            // Remove all entities from the compute and renderable vectors that aren't in the filtered layer vector
            renderableEntities = RenderViewBuilder::entitiesInSubset(renderableEntities, filteredEntities);

            // Set the light sources, with layer filters applied.
            QVector<LightSource> lightSources = m_lightGathererJob->lights();
            for (int i = 0; i < lightSources.count(); ++i) {
                if (!filteredEntities.contains(lightSources[i].entity))
                    lightSources.removeAt(i--);
            }
            rv->setLightSources(lightSources);

            if (isDraw) {
                // Filter out frustum culled entity for drawable entities
                if (rv->frustumCulling())
                    renderableEntities = RenderViewBuilder::entitiesInSubset(renderableEntities, m_frustumCullingJob->visibleEntities());
                // Filter out entities which didn't satisfy proximity filtering
                renderableEntities = RenderViewBuilder::entitiesInSubset(renderableEntities, m_filterProximityJob->filteredEntities());
            }

            // Split among the number of command builders
            int i = 0;
            const int m = RenderViewBuilder::optimalJobCount() - 1;
            const int packetSize = renderableEntities.size() / RenderViewBuilder::optimalJobCount();
            while (i < m) {
                const RenderViewBuilderJobPtr renderViewCommandBuilder = m_renderViewBuilderJobs.at(i);
                renderViewCommandBuilder->setRenderables(renderableEntities.mid(i * packetSize, packetSize));
                ++i;
            }
            m_renderViewBuilderJobs.at(i)->setRenderables(renderableEntities.mid(i * packetSize, packetSize + renderableEntities.size() % (m + 1)));
            {
                QMutexLocker rendererCacheLock(m_renderer->cache()->mutex());
                rv->setMaterialParameterTable(m_renderer->cache()->leafNodeCache.value(m_leafNode).materialParameterGatherer);
            }
        }
    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
    FrustumCullingJobPtr m_frustumCullingJob;
    FilterProximityDistanceJobPtr m_filterProximityJob;
    LightGathererPtr m_lightGathererJob;
    RenderableEntityFilterPtr m_renderableEntityFilterJob;
    ComputableEntityFilterPtr m_computableEntityFilterJob;
    QVector<MaterialParameterGathererJobPtr> m_materialGathererJobs;
    QVector<RenderViewBuilderJobPtr> m_renderViewBuilderJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

class SetClearDrawBufferIndex
{
public:
    explicit SetClearDrawBufferIndex(const RenderViewInitializerJobPtr &renderViewJob)
        : m_renderViewJob(renderViewJob)
    {}

    void operator()()
    {
        RenderView *rv = m_renderViewJob->renderView();
        QVector<ClearBufferInfo> &clearBuffersInfo = rv->specificClearColorBufferInfo();
        const AttachmentPack &attachmentPack = rv->attachmentPack();
        for (ClearBufferInfo &clearBufferInfo : clearBuffersInfo)
            clearBufferInfo.drawBufferIndex = attachmentPack.getDrawBufferIndex(clearBufferInfo.attchmentPoint);

    }

private:
    RenderViewInitializerJobPtr m_renderViewJob;
};

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
        // Save the filtered by layer subset into the cache
        const QVector<Entity *> filteredEntities = m_filterEntityByLayerJob->filteredEntities();
        RendererCache::LeafNodeData &dataCacheForLeaf = m_renderer->cache()->leafNodeCache[m_leafNode];
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
    explicit SyncMaterialParameterGatherer(const QVector<MaterialParameterGathererJobPtr> &materialParameterGathererJobs,
                                           Renderer *renderer,
                                           FrameGraphNode *leafNode)
        : m_materialParameterGathererJobs(materialParameterGathererJobs)
        , m_renderer(renderer)
        , m_leafNode(leafNode)
    {
    }

    void operator()()
    {
        QMutexLocker lock(m_renderer->cache()->mutex());
        RendererCache::LeafNodeData &dataCacheForLeaf = m_renderer->cache()->leafNodeCache[m_leafNode];
        dataCacheForLeaf.materialParameterGatherer.clear();

        for (const auto &materialGatherer : qAsConst(m_materialParameterGathererJobs))
            dataCacheForLeaf.materialParameterGatherer.unite(materialGatherer->materialToPassAndParameter());
    }

private:
    QVector<MaterialParameterGathererJobPtr> m_materialParameterGathererJobs;
    Renderer *m_renderer;
    FrameGraphNode *m_leafNode;
};

} // anonymous

RenderViewBuilder::RenderViewBuilder(Render::FrameGraphNode *leafNode, int renderViewIndex, Renderer *renderer)
    : m_leafNode(leafNode)
    , m_renderViewIndex(renderViewIndex)
    , m_renderer(renderer)
    , m_layerCacheNeedsToBeRebuilt(false)
    , m_materialGathererCacheNeedsToBeRebuilt(false)
    , m_renderViewJob(RenderViewInitializerJobPtr::create())
    , m_filterEntityByLayerJob()
    , m_lightGathererJob(Render::LightGathererPtr::create())
    , m_renderableEntityFilterJob(RenderableEntityFilterPtr::create())
    , m_computableEntityFilterJob(ComputableEntityFilterPtr::create())
    , m_frustumCullingJob(new Render::FrustumCullingJob())
    , m_syncFrustumCullingJob(SynchronizerJobPtr::create(SyncFrustumCulling(m_renderViewJob, m_frustumCullingJob), JobTypes::SyncFrustumCulling))
    , m_setClearDrawBufferIndexJob(SynchronizerJobPtr::create(SetClearDrawBufferIndex(m_renderViewJob), JobTypes::ClearBufferDrawIndex))
    , m_syncFilterEntityByLayerJob()
    , m_filterProximityJob(Render::FilterProximityDistanceJobPtr::create())
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

LightGathererPtr RenderViewBuilder::lightGathererJob() const
{
    return m_lightGathererJob;
}

RenderableEntityFilterPtr RenderViewBuilder::renderableEntityFilterJob() const
{
    return m_renderableEntityFilterJob;
}

ComputableEntityFilterPtr RenderViewBuilder::computableEntityFilterJob() const
{
    return m_computableEntityFilterJob;
}

FrustumCullingJobPtr RenderViewBuilder::frustumCullingJob() const
{
    return m_frustumCullingJob;
}

QVector<RenderViewBuilderJobPtr> RenderViewBuilder::renderViewBuilderJobs() const
{
    return m_renderViewBuilderJobs;
}

QVector<MaterialParameterGathererJobPtr> RenderViewBuilder::materialGathererJobs() const
{
    return m_materialGathererJobs;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderViewInitializationJob() const
{
    return m_syncRenderViewInitializationJob;
}

SynchronizerJobPtr RenderViewBuilder::syncFrustumCullingJob() const
{
    return m_syncFrustumCullingJob;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderCommandBuildingJob() const
{
    return m_syncRenderCommandBuildingJob;
}

SynchronizerJobPtr RenderViewBuilder::syncRenderViewCommandBuildersJob() const
{
    return m_syncRenderViewCommandBuildersJob;
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
    EntityManager *entityManager = m_renderer->nodeManagers()->renderNodesManager();
    m_filterProximityJob->setManager(m_renderer->nodeManagers());
    m_renderableEntityFilterJob->setManager(entityManager);
    m_computableEntityFilterJob->setManager(entityManager);
    m_frustumCullingJob->setRoot(m_renderer->sceneRoot());
    m_lightGathererJob->setManager(entityManager);
    m_renderViewJob->setRenderer(m_renderer);
    m_renderViewJob->setFrameGraphLeafNode(m_leafNode);
    m_renderViewJob->setSubmitOrderIndex(m_renderViewIndex);

    // RenderCommand building is the most consuming task -> split it
    // Estimate the number of jobs to create based on the number of entities
    m_renderViewBuilderJobs.reserve(RenderViewBuilder::m_optimalParallelJobCount);
    for (auto i = 0; i < RenderViewBuilder::m_optimalParallelJobCount; ++i) {
        auto renderViewCommandBuilder = Render::RenderViewBuilderJobPtr::create();
        renderViewCommandBuilder->setIndex(m_renderViewIndex);
        renderViewCommandBuilder->setRenderer(m_renderer);
        m_renderViewBuilderJobs.push_back(renderViewCommandBuilder);
    }

    if (m_materialGathererCacheNeedsToBeRebuilt) {
        // Since Material gathering is an heavy task, we split it
        const QVector<HMaterial> materialHandles = m_renderer->nodeManagers()->materialManager()->activeHandles();
        const int elementsPerJob =  materialHandles.size() / RenderViewBuilder::m_optimalParallelJobCount;
        const int lastRemaingElements = materialHandles.size() % RenderViewBuilder::m_optimalParallelJobCount;
        m_materialGathererJobs.reserve(RenderViewBuilder::m_optimalParallelJobCount);
        for (auto i = 0; i < RenderViewBuilder::m_optimalParallelJobCount; ++i) {
            auto materialGatherer = Render::MaterialParameterGathererJobPtr::create();
            materialGatherer->setNodeManagers(m_renderer->nodeManagers());
            if (i == RenderViewBuilder::m_optimalParallelJobCount - 1)
                materialGatherer->setHandles(materialHandles.mid(i * elementsPerJob, elementsPerJob + lastRemaingElements));
            else
                materialGatherer->setHandles(materialHandles.mid(i * elementsPerJob, elementsPerJob));
            m_materialGathererJobs.push_back(materialGatherer);
        }
        m_syncMaterialGathererJob = SynchronizerJobPtr::create(SyncMaterialParameterGatherer(m_materialGathererJobs,
                                                                                             m_renderer,
                                                                                             m_leafNode),
                                                                  JobTypes::SyncMaterialGatherer);
    }

    if (m_layerCacheNeedsToBeRebuilt) {
        m_filterEntityByLayerJob = Render::FilterLayerEntityJobPtr::create();
        m_filterEntityByLayerJob->setManager(m_renderer->nodeManagers());
        m_syncFilterEntityByLayerJob = SynchronizerJobPtr::create(SyncFilterEntityByLayer(m_filterEntityByLayerJob,
                                                                                          m_renderer,
                                                                                          m_leafNode),
                                                                  JobTypes::SyncFilterEntityByLayer);
    }

    m_syncRenderCommandBuildingJob = SynchronizerJobPtr::create(SyncRenderCommandBuilding(m_renderViewJob,
                                                                                          m_frustumCullingJob,
                                                                                          m_filterProximityJob,
                                                                                          m_lightGathererJob,
                                                                                          m_renderableEntityFilterJob,
                                                                                          m_computableEntityFilterJob,
                                                                                          m_materialGathererJobs,
                                                                                          m_renderViewBuilderJobs,
                                                                                          m_renderer,
                                                                                          m_leafNode),
                                                                JobTypes::SyncRenderViewCommandBuilding);

    m_syncRenderViewCommandBuildersJob = SynchronizerJobPtr::create(SyncRenderViewCommandBuilders(m_renderViewJob,
                                                                                                  m_renderViewBuilderJobs,
                                                                                                  m_renderer),
                                                                    JobTypes::SyncRenderViewCommandBuilder);

    m_syncRenderViewInitializationJob = SynchronizerJobPtr::create(SyncRenderViewInitialization(m_renderViewJob,
                                                                                                m_frustumCullingJob,
                                                                                                m_filterEntityByLayerJob,
                                                                                                m_filterProximityJob,
                                                                                                m_materialGathererJobs,
                                                                                                m_renderViewBuilderJobs),
                                                                   JobTypes::SyncRenderViewInitialization);

}

QVector<Qt3DCore::QAspectJobPtr> RenderViewBuilder::buildJobHierachy() const
{
    QVector<Qt3DCore::QAspectJobPtr> jobs;

    jobs.reserve(m_materialGathererJobs.size() + m_renderViewBuilderJobs.size() + 11);

    // Set dependencies

    // Finish the skinning palette job before processing renderviews
    // TODO: Maybe only update skinning palettes for non-culled entities
    m_renderViewJob->addDependency(m_renderer->updateSkinningPaletteJob());

    m_syncFrustumCullingJob->addDependency(m_renderer->updateWorldTransformJob());
    m_syncFrustumCullingJob->addDependency(m_renderer->updateShaderDataTransformJob());
    m_syncFrustumCullingJob->addDependency(m_syncRenderViewInitializationJob);

    m_frustumCullingJob->addDependency(m_renderer->expandBoundingVolumeJob());
    m_frustumCullingJob->addDependency(m_syncFrustumCullingJob);

    m_setClearDrawBufferIndexJob->addDependency(m_syncRenderViewInitializationJob);

    m_syncRenderViewInitializationJob->addDependency(m_renderViewJob);

    m_filterProximityJob->addDependency(m_renderer->expandBoundingVolumeJob());
    m_filterProximityJob->addDependency(m_syncRenderViewInitializationJob);

    m_syncRenderCommandBuildingJob->addDependency(m_syncRenderViewInitializationJob);
    m_syncRenderCommandBuildingJob->addDependency(m_renderableEntityFilterJob);
    m_syncRenderCommandBuildingJob->addDependency(m_computableEntityFilterJob);
    m_syncRenderCommandBuildingJob->addDependency(m_filterProximityJob);
    m_syncRenderCommandBuildingJob->addDependency(m_lightGathererJob);
    m_syncRenderCommandBuildingJob->addDependency(m_frustumCullingJob);

    // Ensure the RenderThread won't be able to process dirtyResources
    // before they have been completely gathered
    m_syncRenderCommandBuildingJob->addDependency(m_renderer->introspectShadersJob());
    m_syncRenderCommandBuildingJob->addDependency(m_renderer->bufferGathererJob());
    m_syncRenderCommandBuildingJob->addDependency(m_renderer->textureGathererJob());

    for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewBuilderJobs)) {
        renderViewCommandBuilder->addDependency(m_syncRenderCommandBuildingJob);
        m_syncRenderViewCommandBuildersJob->addDependency(renderViewCommandBuilder);
    }

    m_renderer->frameCleanupJob()->addDependency(m_syncRenderViewCommandBuildersJob);
    m_renderer->frameCleanupJob()->addDependency(m_setClearDrawBufferIndexJob);

    // Add jobs
    jobs.push_back(m_renderViewJob); // Step 1
    jobs.push_back(m_renderableEntityFilterJob); // Step 1
    jobs.push_back(m_lightGathererJob); // Step 1

    // Note: do it only if OpenGL 4.3+ available
    jobs.push_back(m_computableEntityFilterJob); // Step 1

    jobs.push_back(m_syncRenderViewInitializationJob); // Step 2

    if (m_layerCacheNeedsToBeRebuilt) {
        m_filterEntityByLayerJob->addDependency(m_renderer->updateEntityLayersJob());
        m_filterEntityByLayerJob->addDependency(m_syncRenderViewInitializationJob);
        m_filterEntityByLayerJob->addDependency(m_renderer->updateTreeEnabledJob());

        m_syncFilterEntityByLayerJob->addDependency(m_filterEntityByLayerJob);
        m_syncRenderCommandBuildingJob->addDependency(m_syncFilterEntityByLayerJob);

        jobs.push_back(m_filterEntityByLayerJob); // Step 3
        jobs.push_back(m_syncFilterEntityByLayerJob); // Step 4
    }
    jobs.push_back(m_syncFrustumCullingJob); // Step 3
    jobs.push_back(m_filterProximityJob); // Step 3
    jobs.push_back(m_setClearDrawBufferIndexJob); // Step 3

    if (m_materialGathererCacheNeedsToBeRebuilt) {
        for (const auto &materialGatherer : qAsConst(m_materialGathererJobs))  {
            materialGatherer->addDependency(m_syncRenderViewInitializationJob);
            materialGatherer->addDependency(m_renderer->introspectShadersJob());
            materialGatherer->addDependency(m_renderer->filterCompatibleTechniqueJob());
            jobs.push_back(materialGatherer); // Step3
            m_syncMaterialGathererJob->addDependency(materialGatherer);
        }
        m_syncRenderCommandBuildingJob->addDependency(m_syncMaterialGathererJob);

        jobs.push_back(m_syncMaterialGathererJob); // Step 3
    }

    jobs.push_back(m_frustumCullingJob); // Step 4
    jobs.push_back(m_syncRenderCommandBuildingJob); // Step 5

    for (const auto &renderViewCommandBuilder : qAsConst(m_renderViewBuilderJobs)) // Step 6
        jobs.push_back(renderViewCommandBuilder);

    jobs.push_back(m_syncRenderViewCommandBuildersJob); // Step 7

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

int RenderViewBuilder::optimalJobCount()
{
    return RenderViewBuilder::m_optimalParallelJobCount;
}

QVector<Entity *> RenderViewBuilder::entitiesInSubset(const QVector<Entity *> &entities, const QVector<Entity *> &subset)
{
    QVector<Entity *> intersection;
    intersection.reserve(qMin(entities.size(), subset.size()));
    std::set_intersection(entities.begin(), entities.end(),
                          subset.begin(), subset.end(),
                          std::back_inserter(intersection));

    return intersection;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
