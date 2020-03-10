/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/offscreensurfacehelper_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/entity_p.h>

#include "testaspect.h"
#include "testrenderer.h"

class TestRendererAspect : public Qt3DRender::TestAspect {
public:
    TestRendererAspect(Qt3DCore::QEntity *root) : Qt3DRender::TestAspect(root) {
        auto d = Qt3DCore::QAbstractAspectPrivate::get(this);
        d->m_rootId = root->id();
    }

    void replaceWithTestRenderer() {
        auto daspect = Qt3DRender::QRenderAspectPrivate::get(this);

        auto sceneRoot = daspect->m_renderer->sceneRoot();
        daspect->m_renderer->shutdown();
        delete daspect->m_renderer;
        daspect->m_renderer = new TestRenderer();
        daspect->m_renderer->setNodeManagers(daspect->m_nodeManagers);
        daspect->m_renderer->setOffscreenSurfaceHelper(daspect->m_offscreenHelper);
        sceneRoot->setRenderer(daspect->m_renderer);
        daspect->m_renderer->setSceneRoot(sceneRoot);

        // Give it a setting instance
        Qt3DRender::Render::RenderSettings settings;
        // owned by FG manager
        Qt3DRender::Render::ViewportNode *fgRoot = new Qt3DRender::Render::ViewportNode();
        const Qt3DCore::QNodeId fgRootId = Qt3DCore::QNodeId::createId();

        daspect->m_nodeManagers->frameGraphManager()->appendNode(fgRootId, fgRoot);
        settings.setActiveFrameGraphId(fgRootId);

        daspect->m_renderer->setSettings(&settings);
    }
};

class tst_Aspect : public QObject
{
    Q_OBJECT
public :
    tst_Aspect() = default;
    ~tst_Aspect() = default;

private Q_SLOTS:

    void checkJobs()
    {
        // GIVEN
        Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
        QScopedPointer<TestRendererAspect> aspect(new TestRendererAspect(rootEntity));
        auto daspect = Qt3DRender::QRenderAspectPrivate::get(aspect.data());
        daspect->m_renderAfterJobs = true;
        aspect->onEngineStartup();

        // replace with empty renderer
        aspect->replaceWithTestRenderer();

        // WHEN
        daspect->m_renderer->markDirty(Qt3DRender::Render::AbstractRenderer::AllDirty, nullptr);

        auto jobs = daspect->jobsToExecute(1.);
//        qDebug() << jobs.size();
//        for (auto job: jobs)
//            qDebug() << Qt3DCore::QAspectJobPrivate::get(job.data())->m_jobName;

        // THEN -> AllDirty
        QCOMPARE(jobs.size(),
                 1 + // UpdateTreeEnabled
                 1 + // UpdateTransform
                 1 + // UpdateWorldBoundingVolume
                 1 + // CalcBoundingVolume
                 1 + // ExpandBoundingVolume
                 1 + // SyncLoadingJobs
                 1 + // UpdateSkinningPalette
                 1 + // UpdateLevelOfDetail
                 1 + // UpdateLayerEntity
                 1 + // PickBoundingVolume
                 1 + // RayCasting
                 0   // No skeleton, no scene loading, no geometry, no buffers
                );

        // WHEN
        daspect->m_renderer->clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        jobs = daspect->jobsToExecute(1.);

        // THEN -> Nothing dirty
        QCOMPARE(jobs.size(),
                 1 + // SyncLoadingJobs
                 1 + // UpdateSkinningPalette
                 1 + // UpdateLevelOfDetail
                 1 + // PickBoundingVolume
                 1 + // RayCasting
                 0   // No skeleton, no scene loading, no geometry, no buffers
                );

        // WHEN
        daspect->m_renderer->clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        daspect->m_renderer->markDirty(Qt3DRender::Render::AbstractRenderer::EntityEnabledDirty, nullptr);
        jobs = daspect->jobsToExecute(1.);

        // THEN -> enabled dirty
        QCOMPARE(jobs.size(),
                 1 + // UpdateTreeEnabled
                 1 + // SyncLoadingJobs
                 1 + // UpdateSkinningPalette
                 1 + // UpdateLevelOfDetail
                 1 + // PickBoundingVolume
                 1 + // RayCasting
                 0   // No skeleton, no scene loading, no geometry, no buffers
                );

        // WHEN
        daspect->m_renderer->clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        daspect->m_renderer->markDirty(Qt3DRender::Render::AbstractRenderer::TransformDirty, nullptr);
        jobs = daspect->jobsToExecute(1.);

        // THEN -> transform dirty
        QCOMPARE(jobs.size(),
                 1 + // UpdateTransform
                 1 + // UpdateWorldBoundingVolume
                 1 + // ExpandBoundingVolume
                 1 + // SyncLoadingJobs
                 1 + // UpdateSkinningPalette
                 1 + // UpdateLevelOfDetail
                 1 + // PickBoundingVolume
                 1 + // RayCasting
                 0   // No skeleton, no scene loading, no geometry, no buffers
                );

        // WHEN
        daspect->m_renderer->clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        daspect->m_renderer->markDirty(Qt3DRender::Render::AbstractRenderer::GeometryDirty, nullptr);
        jobs = daspect->jobsToExecute(1.);

        // THEN -> geometry dirty
        QCOMPARE(jobs.size(),
                 1 + // CalcBoundingVolume
                 1 + // ExpandBoundingVolume
                 1 + // SyncLoadingJobs
                 1 + // UpdateSkinningPalette
                 1 + // UpdateLevelOfDetail
                 1 + // PickBoundingVolume
                 1 + // RayCasting
                 0   // No skeleton, no scene loading, no geometry, no buffers
                );
    }
};

QTEST_MAIN(tst_Aspect)

#include "tst_aspect.moc"
