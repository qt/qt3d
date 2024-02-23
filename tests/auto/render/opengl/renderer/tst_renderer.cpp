// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <renderer_p.h>
#include <renderview_p.h>
#include <renderviewbuilder_p.h>
#include <Qt3DRender/private/renderqueue_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/offscreensurfacehelper_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/qmaterial.h>

#include "testaspect.h"

class tst_Renderer : public QObject
{
    Q_OBJECT
public :
    tst_Renderer() {}
    ~tst_Renderer() {}

private Q_SLOTS:

    void initTestCase()
    {
        qputenv("QT3D_RENDERER", "opengl");
    }

    void checkPreRenderBinJobs()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Qt3DRender::Render::OpenGL::Renderer renderer;
        Qt3DRender::Render::OffscreenSurfaceHelper offscreenHelper(&renderer);
        Qt3DRender::Render::RenderSettings settings;
        // owned by FG manager
        Qt3DRender::Render::ViewportNode *fgRoot = new Qt3DRender::Render::ViewportNode();
        const Qt3DCore::QNodeId fgRootId = Qt3DCore::QNodeId::createId();

        nodeManagers.frameGraphManager()->appendNode(fgRootId, fgRoot);
        settings.setActiveFrameGraphId(fgRootId);

        renderer.setNodeManagers(&nodeManagers);
        renderer.setSettings(&settings);
        renderer.setOffscreenSurfaceHelper(&offscreenHelper);
        renderer.initialize();

        // Ensure invoke calls are performed
        QCoreApplication::processEvents();

        // WHEN (nothing dirty, no buffers, no layers to be rebuilt, no materials to be rebuilt)
        std::vector<Qt3DCore::QAspectJobPtr> jobs = renderer.preRenderingJobs();

        // THEN
        QCOMPARE(jobs.size(), 0U);

        // WHEN
        renderer.m_sendBufferCaptureJob->addRequest({Qt3DCore::QNodeId(), {}});
        jobs = renderer.preRenderingJobs();

        // THEN
        QCOMPARE(jobs.size(), 1U); // SendBufferCaptureJob
        // Note: pending render buffer captures are only cleared when the job is run

        // WHEN
        renderer.m_updatedSetFences.push_back({Qt3DCore::QNodeId(), nullptr});
        jobs = renderer.preRenderingJobs();

        // THEN
        QCOMPARE(jobs.size(),
                 1U); // SendBufferCaptureJob´
        // Note: pending set fence handles are only cleared when the job is run

        // Properly shutdown command thread
        renderer.shutdown();
    }

    void checkRenderBinJobs()
    {
        // GIVEN
        Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
        QScopedPointer<Qt3DRender::TestAspect> aspect(new Qt3DRender::TestAspect(rootEntity));
        auto daspect = Qt3DRender::QRenderAspectPrivate::get(aspect.data());

        Qt3DRender::Render::NodeManagers nodeManagers;
        auto &renderer = *(static_cast<Qt3DRender::Render::OpenGL::Renderer *>(daspect->m_renderer));
        Qt3DRender::Render::RenderQueue<Qt3DRender::Render::OpenGL::RenderView> *renderQueue = renderer.renderQueue();
        Qt3DRender::Render::OffscreenSurfaceHelper offscreenHelper(&renderer);
        Qt3DRender::Render::RenderSettings settings;
        // owned by FG manager
        Qt3DRender::Render::ViewportNode *fgRoot = new Qt3DRender::Render::ViewportNode();
        const Qt3DCore::QNodeId fgRootId = Qt3DCore::QNodeId::createId();

        // Create fake material so that we crean materialGathererJobs
        const Qt3DCore::QNodeId materialId = Qt3DCore::QNodeId::createId();
        nodeManagers.materialManager()->getOrCreateResource(materialId);

        nodeManagers.frameGraphManager()->appendNode(fgRootId, fgRoot);
        settings.setActiveFrameGraphId(fgRootId);

        renderer.setNodeManagers(&nodeManagers);
        renderer.setSettings(&settings);
        renderer.setOffscreenSurfaceHelper(&offscreenHelper);
        renderer.initialize();

        // Ensure invoke calls are performed
        QCoreApplication::processEvents();

        // NOTE: FilterCompatibleTechniqueJob and ShaderGathererJob cannot run because the context
        // is not initialized in this test

        const uint renderViewBuilderMaterialCacheJobCount = 1 + 1;
        // syncMaterialGathererJob
        // n * materialGathererJob (where n depends on the numbers of available threads and the number of materials)
        const uint layerCacheJobCount = 2;
        // filterEntityByLayerJob,
        // syncFilterEntityByLayerJob

        const uint singleRenderViewCommandRebuildJobCount  = 1 + Qt3DCore::QAspectJobManager::idealThreadCount();

        const uint singleRenderViewJobCount = 8 + 1 * Qt3DCore::QAspectJobManager::idealThreadCount();
        // RenderViewBuilder renderViewJob,
        //                   syncRenderViewInitializationJob,
        //                   syncFrustumCullingJob,
        //                   filterProximityJob,
        //                   setClearDrawBufferIndexJob,
        //                   frustumCullingJob,
        //                   syncRenderCommandUpdateJob,
        //                   syncRenderViewCommandPostUpdateJob
        //                   n * (RenderViewCommandBuildJobs)

        // WHEN
        std::vector<Qt3DCore::QAspectJobPtr> jobs = renderer.renderBinJobs();

        // THEN -> AllDirty
        // (Renderer is not initialized so FilterCompatibleTechniqueJob
        // and ShaderGathererJob are not added here)
        QCOMPARE(jobs.size(),
                 1 + // UpdateShaderDataTransform
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 1 + // BufferGathererJob
                 1 + // TexturesGathererJob
                 1 + // LightGathererJob
                 1 + // RenderableEntityFilterJob
                 1 + // ComputableEntityFilterJob
                 singleRenderViewJobCount +
                 singleRenderViewCommandRebuildJobCount +
                 layerCacheJobCount +
                 renderViewBuilderMaterialCacheJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN (nothing dirty, no buffers, no layers to be rebuilt, no materials to be rebuilt) (Nothing in cache)
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::FrameGraphDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 singleRenderViewJobCount +
                 singleRenderViewCommandRebuildJobCount +
                 renderViewBuilderMaterialCacheJobCount +
                 layerCacheJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN (nothing dirty, no buffers, no layers to be rebuilt, no materials to be rebuilt) (RV leaf in cache)
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::FrameGraphDirty, nullptr);
        renderer.cache()->leafNodeCache[renderer.m_frameGraphLeaves.front()] = {};
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 singleRenderViewJobCount +
                 singleRenderViewCommandRebuildJobCount +
                 renderViewBuilderMaterialCacheJobCount +
                 layerCacheJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::EntityEnabledDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 singleRenderViewJobCount +
                 layerCacheJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::TransformDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 1 + // UpdateShaderDataTransform
                 singleRenderViewJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::MaterialDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 singleRenderViewJobCount +
                 singleRenderViewCommandRebuildJobCount +
                 renderViewBuilderMaterialCacheJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::GeometryDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 1 + // RenderableEntityFilterPtr
                 singleRenderViewCommandRebuildJobCount +
                 singleRenderViewJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::BuffersDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 1 + // BufferGathererJob
                 singleRenderViewJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::TexturesDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // cleanupJob
                 1 + // VAOGatherer
                 1 + // TexturesGathererJob
                 singleRenderViewJobCount);

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        renderQueue->reset();

        // Properly shutdown command thread
        renderer.shutdown();
    }
};

QTEST_MAIN(tst_Renderer)

#include "tst_renderer.moc"
