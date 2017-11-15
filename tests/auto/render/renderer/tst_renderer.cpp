/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/renderview_p.h>
#include <Qt3DRender/private/renderviewbuilder_p.h>

class tst_Renderer : public QObject
{
    Q_OBJECT
public :
    tst_Renderer() {}
    ~tst_Renderer() {}

private Q_SLOTS:
    void checkRenderBinJobs()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Qt3DRender::Render::Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        Qt3DRender::Render::RenderSettings settings;
        // owned by FG manager
        Qt3DRender::Render::ViewportNode *fgRoot = new Qt3DRender::Render::ViewportNode();
        const Qt3DCore::QNodeId fgRootId = Qt3DCore::QNodeId::createId();

        nodeManagers.frameGraphManager()->appendNode(fgRootId, fgRoot);
        settings.setActiveFrameGraphId(fgRootId);

        renderer.setNodeManagers(&nodeManagers);
        renderer.setSettings(&settings);
        renderer.initialize();

        const int singleRenderViewJobCount = 11 + 2 * Qt3DRender::Render::RenderViewBuilder::optimalJobCount();
        // RenderViewBuilder renderViewJob,
        //                   renderableEntityFilterJob,
        //                   lightGatherJob,
        //                   computableEntityFilterJob,
        //                   syncRenderViewInitializationJob,
        //                   syncFrustumCullingJob,
        //                   filterEntityByLayerJob,
        //                   filterProximityJob,
        //                   setClearDrawBufferIndexJob,
        //                   frustumCullingJob,
        //                   syncRenderCommandBuldingJob,
        //                   syncRenderViewCommandBuilderJob)
        //                   n * (RenderViewCommandBuildJobs + MaterialGathererJobs

        // WHEN (nothing dirty, no buffers)
        QVector<Qt3DCore::QAspectJobPtr> jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // updateSkinningPaletteJob
                 singleRenderViewJobCount); // Only valid for the first call to renderBinJobs(), since subsequent calls won't have the renderqueue reset


        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::EntityEnabledDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // updateSkinningPaletteJob
                 1); // EntityEnabledDirty

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::TransformDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // WorldTransformJob
                 1 + // UpdateWorldBoundingVolume
                 1 + // UpdateShaderDataTransform
                 1 + // updateSkinningPaletteJob
                 1); // ExpandBoundingVolumeJob

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::GeometryDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // CalculateBoundingVolumeJob
                 1 + // UpdateMeshTriangleListJob
                 1 + // updateSkinningPaletteJob
                 1); // ExpandBoundingVolumeJob

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::BuffersDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // updateSkinningPaletteJob
                 1); // BufferGathererJob

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::ShadersDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // updateSkinningPaletteJob
                 1); // ShaderGathererJob

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::TexturesDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // TexturesGathererJob
                 1 + // updateSkinningPaletteJob
                 1); // SyncTexturesGathererJob

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        renderer.markDirty(Qt3DRender::Render::AbstractRenderer::AllDirty, nullptr);
        jobs = renderer.renderBinJobs();

        // THEN (level
        QCOMPARE(jobs.size(),
                 1 + // updateLevelOfDetailJob
                 1 + // cleanupJob
                 1 + // sendRenderCaptureJob
                 1 + // sendBufferCaptureJob
                 1 + // filterCompatibleTechniquesJob
                 1 + // VAOGatherer
                 1 + // EntityEnabledDirty
                 1 + // WorldTransformJob
                 1 + // UpdateWorldBoundingVolume
                 1 + // UpdateShaderDataTransform
                 1 + // ExpandBoundingVolumeJob
                 1 + // CalculateBoundingVolumeJob
                 1 + // UpdateMeshTriangleListJob
                 1 + // BufferGathererJob
                 1 + // ShaderGathererJob
                 1 + // TexturesGathererJob
                 1 + // updateSkinningPaletteJob
                 1); // SyncTexturesGathererJob

        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);


    }
};

QTEST_MAIN(tst_Renderer)

#include "tst_renderer.moc"
