/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
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

#include <QtTest/QTest>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qnode_p.h>

#include <renderviewbuilder_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qenvironmentlight.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/filterentitybycomponentjob_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class TestAspect : public QRenderAspect
{
public:
    TestAspect(Qt3DCore::QNode *root)
        : QRenderAspect(Qt3DRender::QRenderAspect::Synchronous)
        , m_jobManager(new Qt3DCore::QAspectJobManager())
    {
        Qt3DCore::QAbstractAspectPrivate::get(this)->m_jobManager = m_jobManager.data();
        QRenderAspect::onRegistered();

        QVector<Qt3DCore::QNode *> nodes;
        Qt3DCore::QNodeVisitor v;
        v.traverse(root, [&nodes](Qt3DCore::QNode *node) {
            Qt3DCore::QNodePrivate *d = Qt3DCore::QNodePrivate::get(node);
            d->m_typeInfo = const_cast<QMetaObject*>(Qt3DCore::QNodePrivate::findStaticMetaObject(node->metaObject()));
            d->m_hasBackendNode = true;
            nodes << node;
        });

        for (const auto node: nodes)
            d_func()->createBackendNode({
                                            node->id(),
                                            Qt3DCore::QNodePrivate::get(node)->m_typeInfo,
                                            Qt3DCore::NodeTreeChange::Added,
                                            node
                                        });
    }

    ~TestAspect()
    {
        QRenderAspect::onUnregistered();
    }

    Qt3DRender::Render::NodeManagers *nodeManagers() const
    {
        return d_func()->m_renderer->nodeManagers();
    }

    Render::OpenGL::Renderer *renderer() const
    {
        return static_cast<Render::OpenGL::Renderer *>(d_func()->m_renderer);
    }

    Render::OpenGL::MaterialParameterGathererJobPtr materialGathererJob() const
    {
        Render::OpenGL::MaterialParameterGathererJobPtr job = Render::OpenGL::MaterialParameterGathererJobPtr::create();
        job->setNodeManagers(nodeManagers());
        return job;
    }

    void onRegistered() { QRenderAspect::onRegistered(); }
    void onUnregistered() { QRenderAspect::onUnregistered(); }

private:
    QScopedPointer<Qt3DCore::QAspectJobManager> m_jobManager;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

namespace {

Qt3DCore::QEntity *buildSimpleScene(Qt3DRender::QFrameGraphNode *fg)
{
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity();

    Qt3DRender::QRenderSettings* renderSettings = new Qt3DRender::QRenderSettings();
    renderSettings->setActiveFrameGraph(fg);
    root->addComponent(renderSettings);

    // Scene
    {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity();
        Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
        Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer();
        e->addComponent(material);
        e->addComponent(geometryRenderer);
        e->setParent(root);
    }
    {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity();
        Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
        Qt3DRender::QComputeCommand *computeCommand = new Qt3DRender::QComputeCommand();
        e->addComponent(material);
        e->addComponent(computeCommand);
        e->setParent(root);
    }

    {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity();
        Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight();
        e->addComponent(light);
        e->setParent(root);
    }

    {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity();
        Qt3DRender::QSpotLight *light = new Qt3DRender::QSpotLight();
        e->addComponent(light);
        e->setParent(root);
    }

    {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity();
        Qt3DRender::QEnvironmentLight *light = new Qt3DRender::QEnvironmentLight();
        e->addComponent(light);
        e->setParent(root);
    }

    return root;
}

Qt3DCore::QEntity *buildEntityFilterTestScene(Qt3DRender::QFrameGraphNode *fg, Qt3DRender::QLayer *layer)
{
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity();

    Qt3DRender::QRenderSettings* renderSettings = new Qt3DRender::QRenderSettings();
    renderSettings->setActiveFrameGraph(fg);
    root->addComponent(renderSettings);

    // Scene
    for (int i = 0; i < 200; ++i) {
        Qt3DCore::QEntity *e = new Qt3DCore::QEntity();
        Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
        Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer();
        e->addComponent(material);
        e->addComponent(geometryRenderer);
        if (i % 2 == 0)
            e->addComponent(layer);
        e->setParent(root);
    }

    return root;
}

} // anonymous


class tst_RenderViewBuilder : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(clearBuffer->id());
        QVERIFY(leafNode != nullptr);

        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());

            // THEN
            QCOMPARE(renderViewBuilder.renderViewIndex(), 0);
            QCOMPARE(renderViewBuilder.renderer(), testAspect.renderer());
            QCOMPARE(renderViewBuilder.layerCacheNeedsToBeRebuilt(), false);
            QCOMPARE(renderViewBuilder.materialGathererCacheNeedsToBeRebuilt(), false);
            QVERIFY(!renderViewBuilder.renderViewJob().isNull());
            QVERIFY(!renderViewBuilder.frustumCullingJob().isNull());
            QVERIFY(!renderViewBuilder.syncPreFrustumCullingJob().isNull());
            QVERIFY(!renderViewBuilder.setClearDrawBufferIndexJob().isNull());

            QVERIFY(renderViewBuilder.filterEntityByLayerJob().isNull());
            QVERIFY(renderViewBuilder.syncFilterEntityByLayerJob().isNull());
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob().isNull());
            QVERIFY(renderViewBuilder.syncRenderViewPostCommandUpdateJob().isNull());
            QVERIFY(renderViewBuilder.syncRenderViewPostInitializationJob().isNull());

            QCOMPARE(renderViewBuilder.renderViewCommandUpdaterJobs().size(), 0);
            QCOMPARE(renderViewBuilder.materialGathererJobs().size(), 0);

            // WHEN
            renderViewBuilder.prepareJobs();

            // THEN
            QVERIFY(!renderViewBuilder.syncRenderViewPreCommandUpdateJob().isNull());
            QVERIFY(!renderViewBuilder.syncRenderViewPostCommandUpdateJob().isNull());
            QVERIFY(!renderViewBuilder.syncRenderViewPostInitializationJob().isNull());
            QVERIFY(renderViewBuilder.filterEntityByLayerJob().isNull());
            QVERIFY(renderViewBuilder.syncFilterEntityByLayerJob().isNull());

            QCOMPARE(renderViewBuilder.renderViewCommandUpdaterJobs().size(), Qt3DRender::Render::OpenGL::RenderViewBuilder::defaultJobCount());
            QCOMPARE(renderViewBuilder.materialGathererJobs().size(), 0);
            QCOMPARE(renderViewBuilder.buildJobHierachy().size(), 8 + 1 * Qt3DRender::Render::OpenGL::RenderViewBuilder::defaultJobCount());
        }

        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
            renderViewBuilder.setOptimalJobCount(2);
            renderViewBuilder.setLayerCacheNeedsToBeRebuilt(true);
            renderViewBuilder.prepareJobs();

            // THEN
            QCOMPARE(renderViewBuilder.layerCacheNeedsToBeRebuilt(), true);
            QVERIFY(!renderViewBuilder.filterEntityByLayerJob().isNull());
            QVERIFY(!renderViewBuilder.syncFilterEntityByLayerJob().isNull());

            // mark jobs dirty and recheck
            QCOMPARE(renderViewBuilder.buildJobHierachy().size(), 10 + renderViewBuilder.optimalJobCount());
        }

        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
            renderViewBuilder.setOptimalJobCount(2);
            renderViewBuilder.setMaterialGathererCacheNeedsToBeRebuilt(true);
            renderViewBuilder.prepareJobs();

            // THEN
            QCOMPARE(renderViewBuilder.materialGathererCacheNeedsToBeRebuilt(), true);
            // We have elementsPerJob = qMax(materialHandles.size() / m_optimalParallelJobCount, 1)
            // Given we have 2 materials -> 1 element per job -> so we need 2 jobs
            QCOMPARE(renderViewBuilder.materialGathererJobs().size(), 2);
            QVERIFY(!renderViewBuilder.syncMaterialGathererJob().isNull());

            // mark jobs dirty and recheck
            QCOMPARE(renderViewBuilder.buildJobHierachy().size(), 13);
        }
    }

    void checkCheckJobDependencies()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));

        auto expandBVJob = Qt3DRender::QRenderAspectPrivate::get(&testAspect)->m_expandBoundingVolumeJob;
        auto wordTransformJob = Qt3DRender::QRenderAspectPrivate::get(&testAspect)->m_worldTransformJob;
        auto updateTreeEnabledJob = Qt3DRender::QRenderAspectPrivate::get(&testAspect)->m_updateTreeEnabledJob;
        auto updateEntityLayerJob = Qt3DRender::QRenderAspectPrivate::get(&testAspect)->m_updateEntityLayersJob;

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(clearBuffer->id());
        QVERIFY(leafNode != nullptr);

        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
            renderViewBuilder.prepareJobs();
            renderViewBuilder.buildJobHierachy();

            // THEN
            // Step 1
            QCOMPARE(renderViewBuilder.renderViewJob()->dependencies().size(), 1); // Depends upon skinning palette update

            // Step 2
            QCOMPARE(renderViewBuilder.syncRenderViewPostInitializationJob()->dependencies().size(), 1);
            QCOMPARE(renderViewBuilder.syncRenderViewPostInitializationJob()->dependencies().constFirst().toStrongRef().data(),
                     renderViewBuilder.renderViewJob().data());

            // Step 3
            QVERIFY(renderViewBuilder.filterEntityByLayerJob().isNull());
            QVERIFY(renderViewBuilder.syncFilterEntityByLayerJob().isNull());

            QCOMPARE(renderViewBuilder.filterProximityJob()->dependencies().size(), 2);
            QVERIFY(renderViewBuilder.filterProximityJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.filterProximityJob()->dependencies().contains(expandBVJob));

            QCOMPARE(renderViewBuilder.setClearDrawBufferIndexJob()->dependencies().size(), 1);
            QCOMPARE(renderViewBuilder.setClearDrawBufferIndexJob()->dependencies().constFirst().toStrongRef().data(),
                     renderViewBuilder.syncRenderViewPostInitializationJob().data());

            QCOMPARE(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().size(), 3);
            QVERIFY(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().contains(wordTransformJob));
            QVERIFY(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().contains(testAspect.renderer()->updateShaderDataTransformJob()));

            // Step 4
            QCOMPARE(renderViewBuilder.frustumCullingJob()->dependencies().size(), 2);
            QVERIFY(renderViewBuilder.frustumCullingJob()->dependencies().contains(renderViewBuilder.syncPreFrustumCullingJob()));
            QVERIFY(renderViewBuilder.frustumCullingJob()->dependencies().contains(expandBVJob));

            QCOMPARE(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().size(), renderViewBuilder.materialGathererJobs().size() + 7);
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.filterProximityJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.frustumCullingJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->introspectShadersJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->bufferGathererJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->textureGathererJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->lightGathererJob()));

            // Step 5
            for (const auto &renderViewBuilderJob : renderViewBuilder.renderViewCommandUpdaterJobs()) {
                QCOMPARE(renderViewBuilderJob->dependencies().size(), 1);
                QCOMPARE(renderViewBuilderJob->dependencies().constFirst().toStrongRef().data(),
                         renderViewBuilder.syncRenderViewPreCommandUpdateJob().data());
            }

            // Step 6
            QCOMPARE(renderViewBuilder.syncRenderViewPostCommandUpdateJob()->dependencies().size(), renderViewBuilder.renderViewCommandUpdaterJobs().size());
            for (const auto &renderViewBuilderJob : renderViewBuilder.renderViewCommandUpdaterJobs()) {
                QVERIFY(renderViewBuilder.syncRenderViewPostCommandUpdateJob()->dependencies().contains(renderViewBuilderJob));
            }
        }
        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
            renderViewBuilder.setLayerCacheNeedsToBeRebuilt(true);
            renderViewBuilder.setMaterialGathererCacheNeedsToBeRebuilt(true);
            renderViewBuilder.prepareJobs();
            renderViewBuilder.buildJobHierachy();

            // THEN
            // Step 1
            QCOMPARE(renderViewBuilder.renderViewJob()->dependencies().size(), 1); // Depends upon skinning palette update

            // Step 2
            QCOMPARE(renderViewBuilder.syncRenderViewPostInitializationJob()->dependencies().size(), 1);
            QCOMPARE(renderViewBuilder.syncRenderViewPostInitializationJob()->dependencies().constFirst().toStrongRef().data(),
                     renderViewBuilder.renderViewJob().data());

            // Step 3
            QCOMPARE(renderViewBuilder.filterEntityByLayerJob()->dependencies().size(), 3);
            QVERIFY(renderViewBuilder.filterEntityByLayerJob()->dependencies().contains(updateEntityLayerJob));
            QVERIFY(renderViewBuilder.filterEntityByLayerJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.filterEntityByLayerJob()->dependencies().contains(updateTreeEnabledJob));

            QCOMPARE(renderViewBuilder.syncFilterEntityByLayerJob()->dependencies().size(), 1);
            QVERIFY(renderViewBuilder.syncFilterEntityByLayerJob()->dependencies().contains(renderViewBuilder.filterEntityByLayerJob()));

            QCOMPARE(renderViewBuilder.filterProximityJob()->dependencies().size(), 2);
            QVERIFY(renderViewBuilder.filterProximityJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.filterProximityJob()->dependencies().contains(expandBVJob));

            QCOMPARE(renderViewBuilder.setClearDrawBufferIndexJob()->dependencies().size(), 1);
            QCOMPARE(renderViewBuilder.setClearDrawBufferIndexJob()->dependencies().constFirst().toStrongRef().data(),
                     renderViewBuilder.syncRenderViewPostInitializationJob().data());

            QCOMPARE(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().size(), 3);
            QVERIFY(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().contains(wordTransformJob));
            QVERIFY(renderViewBuilder.syncPreFrustumCullingJob()->dependencies().contains(testAspect.renderer()->updateShaderDataTransformJob()));

            for (const auto &materialGatherer : renderViewBuilder.materialGathererJobs()) {
                QCOMPARE(materialGatherer->dependencies().size(), 3);
                QVERIFY(materialGatherer->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
                QVERIFY(materialGatherer->dependencies().contains(testAspect.renderer()->introspectShadersJob()));
                QVERIFY(materialGatherer->dependencies().contains(testAspect.renderer()->filterCompatibleTechniqueJob()));
            }

            // Step 4
            QCOMPARE(renderViewBuilder.frustumCullingJob()->dependencies().size(), 2);
            QVERIFY(renderViewBuilder.frustumCullingJob()->dependencies().contains(renderViewBuilder.syncPreFrustumCullingJob()));
            QVERIFY(renderViewBuilder.frustumCullingJob()->dependencies().contains(expandBVJob));

            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.syncRenderViewPostInitializationJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.syncFilterEntityByLayerJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.frustumCullingJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(renderViewBuilder.filterProximityJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->introspectShadersJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->bufferGathererJob()));
            QVERIFY(renderViewBuilder.syncRenderViewPreCommandUpdateJob()->dependencies().contains(testAspect.renderer()->textureGathererJob()));

            // Step 5
            for (const auto &renderViewBuilderJob : renderViewBuilder.renderViewCommandUpdaterJobs()) {
                QCOMPARE(renderViewBuilderJob->dependencies().size(), 1);
                QCOMPARE(renderViewBuilderJob->dependencies().constFirst().toStrongRef().data(),
                         renderViewBuilder.syncRenderViewPreCommandUpdateJob().data());
            }

            // Step 6
            QCOMPARE(renderViewBuilder.syncRenderViewPostCommandUpdateJob()->dependencies().size(), renderViewBuilder.renderViewCommandUpdaterJobs().size());
            for (const auto &renderViewBuilderJob : renderViewBuilder.renderViewCommandUpdaterJobs()) {
                QVERIFY(renderViewBuilder.syncRenderViewPostCommandUpdateJob()->dependencies().contains(renderViewBuilderJob));
            }
        }
    }

    void checkRenderViewJobExecution()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(clearBuffer->id());
        QVERIFY(leafNode != nullptr);

        // WHEN
        Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
        renderViewBuilder.prepareJobs();
        renderViewBuilder.buildJobHierachy();
        renderViewBuilder.renderViewJob()->run();

        // THEN
        QVERIFY(renderViewBuilder.renderViewJob()->renderView() != nullptr);
    }

    void checkLightGatherExecution()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));
        Qt3DRender::Render::OpenGL::Renderer *renderer = testAspect.renderer();

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(clearBuffer->id());
        QVERIFY(leafNode != nullptr);

        // WHEN
        renderer->lightGathererJob()->run();

        // THEN
        QCOMPARE(renderer->lightGathererJob()->lights().size(), 2);
        QVERIFY(renderer->lightGathererJob()->environmentLight() != nullptr);
    }

    void checkRenderableEntitiesFilteringExecution()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));
        Qt3DRender::Render::OpenGL::Renderer *renderer = testAspect.renderer();

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(clearBuffer->id());
        QVERIFY(leafNode != nullptr);

        // WHEN
        renderer->renderableEntityFilterJob()->run();

        // THEN
        QCOMPARE(renderer->renderableEntityFilterJob()->filteredEntities().size(), 1);
    }

    void checkComputableEntitiesFilteringExecution()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));
        Qt3DRender::Render::OpenGL::Renderer *renderer = testAspect.renderer();

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(clearBuffer->id());
        QVERIFY(leafNode != nullptr);

        // WHEN
        renderer->computableEntityFilterJob()->run();

        // THEN
        QCOMPARE(renderer->computableEntityFilterJob()->filteredEntities().size(), 1);
    }

    void checkSyncRenderViewInitializationExecution()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter(clearBuffer);
        Qt3DRender::QFrustumCulling *frustumCulling = new Qt3DRender::QFrustumCulling(layerFilter);
        Qt3DRender::QTechniqueFilter *techniqueFilter = new Qt3DRender::QTechniqueFilter(frustumCulling);
        Qt3DRender::QRenderPassFilter *renderPassFilter = new Qt3DRender::QRenderPassFilter(techniqueFilter);
        Qt3DRender::QLayer *layer = new Qt3DRender::QLayer();

        layerFilter->addLayer(layer);
        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(renderPassFilter->id());
        QVERIFY(leafNode != nullptr);

        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
            renderViewBuilder.prepareJobs();
            renderViewBuilder.buildJobHierachy();

            // THEN
            QCOMPARE(renderViewBuilder.frustumCullingJob()->isActive(), false);
            for (const auto &materialGatherer : renderViewBuilder.materialGathererJobs()) {
                QVERIFY(materialGatherer->techniqueFilter() == nullptr);
                QVERIFY(materialGatherer->renderPassFilter() == nullptr);
            }

            // WHEN
            renderViewBuilder.renderViewJob()->run();
            renderViewBuilder.syncRenderViewPostInitializationJob()->run();

            // THEN
            QCOMPARE(renderViewBuilder.frustumCullingJob()->isActive(), true);
            for (const auto &materialGatherer : renderViewBuilder.materialGathererJobs()) {
                QVERIFY(materialGatherer->techniqueFilter() != nullptr);
                QVERIFY(materialGatherer->renderPassFilter() != nullptr);
            }
        }
        {
            // WHEN
            Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
            renderViewBuilder.setLayerCacheNeedsToBeRebuilt(true);
            renderViewBuilder.prepareJobs();
            renderViewBuilder.buildJobHierachy();

            // THEN
            QCOMPARE(renderViewBuilder.frustumCullingJob()->isActive(), false);
            QCOMPARE(renderViewBuilder.filterEntityByLayerJob()->hasLayerFilter(), false);
            QCOMPARE(renderViewBuilder.filterEntityByLayerJob()->layerFilters().size(), 0);
            for (const auto &materialGatherer : renderViewBuilder.materialGathererJobs()) {
                QVERIFY(materialGatherer->techniqueFilter() == nullptr);
                QVERIFY(materialGatherer->renderPassFilter() == nullptr);
            }

            // WHEN
            renderViewBuilder.renderViewJob()->run();
            renderViewBuilder.syncRenderViewPostInitializationJob()->run();

            // THEN
            QCOMPARE(renderViewBuilder.frustumCullingJob()->isActive(), true);
            QCOMPARE(renderViewBuilder.filterEntityByLayerJob()->hasLayerFilter(), true);
            QCOMPARE(renderViewBuilder.filterEntityByLayerJob()->layerFilters().size(), 1);
            for (const auto &materialGatherer : renderViewBuilder.materialGathererJobs()) {
                QVERIFY(materialGatherer->techniqueFilter() != nullptr);
                QVERIFY(materialGatherer->renderPassFilter() != nullptr);
            }
        }
    }

    void checkSyncFrustumCullingExecution()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::QFrustumCulling *frustumCulling = new Qt3DRender::QFrustumCulling(clearBuffer);
        Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(frustumCulling);
        Qt3DRender::QCamera *camera = new Qt3DRender::QCamera();
        cameraSelector->setCamera(camera);

        Qt3DRender::TestAspect testAspect(buildSimpleScene(viewport));

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(cameraSelector->id());
        QVERIFY(leafNode != nullptr);

        // WHEN
        Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());
        renderViewBuilder.prepareJobs();
        renderViewBuilder.buildJobHierachy();

        // THEN
        QCOMPARE(renderViewBuilder.frustumCullingJob()->viewProjection(), Matrix4x4());

        // WHEN
        renderViewBuilder.renderViewJob()->run();
        renderViewBuilder.syncPreFrustumCullingJob()->run();

        // THEN
        QCOMPARE(convertToQMatrix4x4(renderViewBuilder.frustumCullingJob()->viewProjection()), camera->projectionMatrix() * camera->viewMatrix());
    }

    void checkRemoveEntitiesNotInSubset()
    {
        // GIVEN
        Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
        Qt3DRender::QClearBuffers *clearBuffer = new Qt3DRender::QClearBuffers(viewport);
        Qt3DRender::QLayerFilter *layerFilter = new Qt3DRender::QLayerFilter(clearBuffer);
        Qt3DRender::QLayer *layer = new Qt3DRender::QLayer();
        layerFilter->addLayer(layer);
        Qt3DRender::TestAspect testAspect(buildEntityFilterTestScene(viewport, layer));
        Qt3DRender::Render::OpenGL::Renderer *renderer = testAspect.renderer();

        // THEN
        Qt3DRender::Render::FrameGraphNode *leafNode = testAspect.nodeManagers()->frameGraphManager()->lookupNode(layerFilter->id());
        QVERIFY(leafNode != nullptr);

        // WHEN
        renderer->markDirty(Qt3DRender::Render::AbstractRenderer::AllDirty, nullptr);

        Qt3DRender::Render::OpenGL::RenderViewBuilder renderViewBuilder(leafNode, 0, testAspect.renderer());

        renderViewBuilder.setLayerCacheNeedsToBeRebuilt(true);
        renderViewBuilder.prepareJobs();
        renderViewBuilder.buildJobHierachy();

        renderer->renderableEntityFilterJob()->run();

        renderViewBuilder.renderViewJob()->run();
        renderViewBuilder.syncRenderViewPostInitializationJob()->run();
        renderViewBuilder.filterEntityByLayerJob()->run();

        QVector<Qt3DRender::Render::Entity *> renderableEntity = renderer->renderableEntityFilterJob()->filteredEntities();
        QVector<Qt3DRender::Render::Entity *> filteredEntity = renderViewBuilder.filterEntityByLayerJob()->filteredEntities();

        // THEN
        QCOMPARE(renderableEntity.size(), 200);
        QCOMPARE(filteredEntity.size(), 100);

        std::sort(renderableEntity.begin(), renderableEntity.end());

        // WHEN
        renderableEntity = Qt3DRender::Render::OpenGL::RenderViewBuilder::entitiesInSubset(renderableEntity, filteredEntity);

        // THEN
        QCOMPARE(renderableEntity.size(), 100);
        for (const auto entity : renderableEntity) {
            QVERIFY(filteredEntity.contains(entity));
        }
    }

};

QTEST_MAIN(tst_RenderViewBuilder)

#include "tst_renderviewbuilder.moc"
