/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qmlscenereader.h"
#include "testpostmanarbiter.h"

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <QtQuick/qquickwindow.h>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRayCaster>
#include <Qt3DRender/QScreenRayCaster>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/raycastingjob_p.h>
#include <Qt3DRender/private/pickboundingvolumeutils_p.h>
#include <Qt3DRender/private/updatemeshtrianglelistjob_p.h>
#include <Qt3DRender/private/updateworldboundingvolumejob_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>
#include <Qt3DRender/private/expandboundingvolumejob_p.h>
#include <Qt3DRender/private/calcboundingvolumejob_p.h>
#include <Qt3DRender/private/calcgeometrytrianglevolumes_p.h>
#include <Qt3DRender/private/loadbufferjob_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>

#include <private/qpickevent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QVector<Qt3DCore::QNode *> getNodesForCreation(Qt3DCore::QNode *root)
{
    using namespace Qt3DCore;

    QVector<QNode *> nodes;
    Qt3DCore::QNodeVisitor visitor;
    visitor.traverse(root, [&nodes](QNode *node) {
        nodes.append(node);

        // Store the metaobject of the node in the QNode so that we have it available
        // to us during destruction in the QNode destructor. This allows us to send
        // the QNodeId and the metaobject as typeinfo to the backend aspects so they
        // in turn can find the correct QBackendNodeMapper object to handle the destruction
        // of the corresponding backend nodes.
        QNodePrivate *d = QNodePrivate::get(node);
        d->m_typeInfo = const_cast<QMetaObject*>(QNodePrivate::findStaticMetaObject(node->metaObject()));

        // Mark this node as having been handled for creation so that it is picked up
        d->m_hasBackendNode = true;
    });

    return nodes;
}

QVector<Qt3DCore::NodeTreeChange> nodeTreeChangesForNodes(const QVector<Qt3DCore::QNode *> nodes)
{
    QVector<Qt3DCore::NodeTreeChange> nodeTreeChanges;
    nodeTreeChanges.reserve(nodes.size());

    for (Qt3DCore::QNode *n : nodes) {
        nodeTreeChanges.push_back({
                                      n->id(),
                                      Qt3DCore::QNodePrivate::get(n)->m_typeInfo,
                                      Qt3DCore::NodeTreeChange::Added,
                                      n
                                  });
    }

    return nodeTreeChanges;
}

class TestAspect : public Qt3DRender::QRenderAspect
{
public:
    TestAspect(Qt3DCore::QNode *root)
        : Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Synchronous)
        , m_sceneRoot(nullptr)
    {
        m_engine = new Qt3DCore::QAspectEngine(this);
        m_engine->registerAspect(this);
        Q_ASSERT(d_func()->m_aspectManager);

        // do what QAspectEngine::setRootEntity does since we don't want to enter the simulation loop
        Qt3DCore::QEntityPtr proot(qobject_cast<Qt3DCore::QEntity *>(root), [](Qt3DCore::QEntity *) { });
        Qt3DCore::QAspectEnginePrivate *aed = Qt3DCore::QAspectEnginePrivate::get(m_engine);
        aed->m_root = proot;
        aed->initialize();
        aed->initNodeTree(root);
        const QVector<Qt3DCore::QNode *> nodes = getNodesForCreation(root);
        aed->m_aspectManager->setRootEntity(proot.data(), nodes);

        Render::Entity *rootEntity = nodeManagers()->lookupResource<Render::Entity, Render::EntityManager>(rootEntityId());
        Q_ASSERT(rootEntity);
        m_sceneRoot = rootEntity;
    }

    ~TestAspect()
    {
        using namespace Qt3DCore;
        QNodeVisitor visitor;
        visitor.traverse(m_engine->rootEntity().data(), [](QNode *node) {
            QNodePrivate *d = QNodePrivate::get(node);
            d->m_scene = nullptr;
            d->m_changeArbiter = nullptr;
        });

        m_engine->unregisterAspect(this);
        delete m_engine;
        m_engine = nullptr;
    }

    void onRegistered() { QRenderAspect::onRegistered(); }
    void onUnregistered() { QRenderAspect::onUnregistered(); }

    Qt3DRender::Render::NodeManagers *nodeManagers() const { return d_func()->m_renderer->nodeManagers(); }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const { return d_func()->m_renderer->frameGraphRoot(); }
    Qt3DRender::Render::RenderSettings *renderSettings() const { return d_func()->m_renderer->settings(); }
    Qt3DRender::Render::Entity *sceneRoot() const { return m_sceneRoot; }
    Qt3DCore::QAspectManager *aspectManager() const { return  d_func()->m_aspectManager; }
    Qt3DCore::QChangeArbiter *arbiter() const { return d_func()->m_arbiter; }
private:
    Qt3DCore::QAspectEngine *m_engine;
    Render::Entity *m_sceneRoot;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

namespace {

void runRequiredJobs(Qt3DRender::TestAspect *test)
{
    QCoreApplication::processEvents();
    const auto dn = test->arbiter()->takeDirtyFrontEndNodes();
    Qt3DCore::QAbstractAspectPrivate::get(test)->syncDirtyFrontEndNodes(dn);

    Qt3DRender::Render::UpdateWorldTransformJob updateWorldTransform;
    updateWorldTransform.setRoot(test->sceneRoot());
    updateWorldTransform.setManagers(test->nodeManagers());
    updateWorldTransform.run();

    // For each buffer
    const std::vector<Qt3DRender::Render::HBuffer> &bufferHandles = test->nodeManagers()->bufferManager()->activeHandles();
    for (auto bufferHandle : bufferHandles) {
        Qt3DRender::Render::LoadBufferJob loadBuffer(bufferHandle);
        loadBuffer.setNodeManager(test->nodeManagers());
        loadBuffer.run();
    }

    Qt3DRender::Render::CalculateBoundingVolumeJob calcBVolume;
    calcBVolume.setManagers(test->nodeManagers());
    calcBVolume.setRoot(test->sceneRoot());
    calcBVolume.run();

    Qt3DRender::Render::UpdateWorldBoundingVolumeJob updateWorldBVolume;
    updateWorldBVolume.setManager(test->nodeManagers()->renderNodesManager());
    updateWorldBVolume.run();

    Qt3DRender::Render::ExpandBoundingVolumeJob expandBVolume;
    expandBVolume.setRoot(test->sceneRoot());
    expandBVolume.setManagers(test->nodeManagers());
    expandBVolume.run();

    Qt3DRender::Render::UpdateMeshTriangleListJob updateTriangleList;
    updateTriangleList.setManagers(test->nodeManagers());
    updateTriangleList.run();

    // For each geometry id
    const std::vector<Qt3DRender::Render::HGeometryRenderer> &geometryRenderHandles = test->nodeManagers()->geometryRendererManager()->activeHandles();
    for (auto geometryRenderHandle : geometryRenderHandles) {
        Qt3DCore::QNodeId geometryRendererId = test->nodeManagers()->geometryRendererManager()->data(geometryRenderHandle)->peerId();
        Qt3DRender::Render::CalcGeometryTriangleVolumes calcGeometryTriangles(geometryRendererId, test->nodeManagers());
        calcGeometryTriangles.run();
    }
}

void initializeJob(Qt3DRender::Render::RayCastingJob *job, Qt3DRender::TestAspect *test)
{
    job->setFrameGraphRoot(test->frameGraphRoot());
    job->setRoot(test->sceneRoot());
    job->setManagers(test->nodeManagers());
    job->setRenderSettings(test->renderSettings());
}

} // anonymous

class tst_RayCastingJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void worldSpaceRayCaster_data()
    {
        QTest::addColumn<QUrl>("source");
        QTest::addColumn<QVector3D>("rayOrigin");
        QTest::addColumn<QVector3D>("rayDirection");
        QTest::addColumn<float>("rayLength");
        QTest::addColumn<int>("numIntersections");

        QTest::newRow("left entity") << QUrl("qrc:/testscene_worldraycasting.qml") << QVector3D(-5, 0, 4) << QVector3D(0, 0, -1) << 20.f << 1;
        QTest::newRow("no entity") << QUrl("qrc:/testscene_worldraycasting.qml") << QVector3D(0, 0, 4) << QVector3D(0, 0, -1) << 20.f  << 0;
        QTest::newRow("both entities") << QUrl("qrc:/testscene_worldraycasting.qml") << QVector3D(-8, 0, 0) << QVector3D(1, 0, 0) << 20.f  << 2;
        QTest::newRow("infinite ray") << QUrl("qrc:/testscene_worldraycasting.qml") << QVector3D(-5, 0, 4) << QVector3D(0, 0, -1) << -1.f << 1;
        QTest::newRow("short ray") << QUrl("qrc:/testscene_worldraycasting.qml") << QVector3D(-5, 0, 4) << QVector3D(0, 0, -1) << 2.f << 0;
        QTest::newRow("discard filter - right entity") << QUrl("qrc:/testscene_worldraycastinglayer.qml") << QVector3D(5, 0, 4) << QVector3D(0, 0, -1) << 20.f  << 0;
        QTest::newRow("discard filter - both entities") << QUrl("qrc:/testscene_worldraycastinglayer.qml") << QVector3D(-8, 0, 0) << QVector3D(1, 0, 0) << 20.f  << 1;
        QTest::newRow("multi layer - left entity") << QUrl("qrc:/testscene_worldraycastingalllayers.qml") << QVector3D(-5, 0, 4) << QVector3D(0, 0, -1) << 20.f  << 0;
        QTest::newRow("multi layer - right entity") << QUrl("qrc:/testscene_worldraycastingalllayers.qml") << QVector3D(5, 0, 4) << QVector3D(0, 0, -1) << 20.f  << 1;
        QTest::newRow("parent layer") << QUrl("qrc:/testscene_worldraycastingparentlayer.qml") << QVector3D(-8, 0, 0) << QVector3D(1, 0, 0) << 20.f  << 1;
    }

    void worldSpaceRayCaster()
    {
        QFETCH(QUrl, source);
        QFETCH(QVector3D, rayOrigin);
        QFETCH(QVector3D, rayDirection);
        QFETCH(float, rayLength);
        QFETCH(int, numIntersections);

        // GIVEN
        QmlSceneReader sceneReader(source);
        QScopedPointer<Qt3DCore::QEntity> root(qobject_cast<Qt3DCore::QEntity *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        Qt3DCore::QComponentVector rootComponents = root->components();
        Qt3DRender::QRayCaster *rayCaster = nullptr;
        for (Qt3DCore::QComponent *c: qAsConst(rootComponents)) {
            rayCaster = qobject_cast<Qt3DRender::QRayCaster *>(c);
            if (rayCaster)
                break;
        }
        QVERIFY(rayCaster);

        rayCaster->trigger(rayOrigin, rayDirection, rayLength);

        // Runs Required jobs
        runRequiredJobs(test.data());

        Qt3DRender::Render::RayCaster *backendRayCaster = test->nodeManagers()->rayCasterManager()->lookupResource(rayCaster->id());
        QVERIFY(backendRayCaster);
        Qt3DCore::QBackendNodePrivate::get(backendRayCaster)->setArbiter(test->arbiter());

        // WHEN
        Qt3DRender::Render::RayCastingJob rayCastingJob;
        initializeJob(&rayCastingJob, test.data());

        bool earlyReturn = !rayCastingJob.runHelper();
        rayCastingJob.postFrame(test->aspectManager());
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(!earlyReturn);
        QVERIFY(!backendRayCaster->isEnabled());
        QVERIFY(!rayCaster->isEnabled());
        auto dirtyNodes = test->arbiter()->takeDirtyFrontEndNodes();
        QCOMPARE(dirtyNodes.count(), 1); // hits & disable
        QCOMPARE(rayCaster->hits().size(), numIntersections);

        if (numIntersections)
            QVERIFY(rayCaster->hits().first().entityId());
    }

    void screenSpaceRayCaster_data()
    {
        QTest::addColumn<QUrl>("source");
        QTest::addColumn<QPoint>("rayPosition");
        QTest::addColumn<int>("numIntersections");

        QTest::newRow("left entity") << QUrl("qrc:/testscene_screenraycasting.qml") << QPoint(200, 280) << 1;
        QTest::newRow("no entity") << QUrl("qrc:/testscene_screenraycasting.qml") << QPoint(300, 300) << 0;
    }

    void screenSpaceRayCaster()
    {
        QFETCH(QUrl, source);
        QFETCH(QPoint, rayPosition);
        QFETCH(int, numIntersections);

        // GIVEN
        QmlSceneReader sceneReader(source);
        QScopedPointer<Qt3DCore::QEntity> root(qobject_cast<Qt3DCore::QEntity *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        Qt3DCore::QComponentVector rootComponents = root->components();
        Qt3DRender::QScreenRayCaster *rayCaster = nullptr;
        for (Qt3DCore::QComponent *c: qAsConst(rootComponents)) {
            rayCaster = qobject_cast<Qt3DRender::QScreenRayCaster *>(c);
            if (rayCaster)
                break;
        }
        QVERIFY(rayCaster);

        rayCaster->trigger(rayPosition);

        // Runs Required jobs
        runRequiredJobs(test.data());

        Qt3DRender::Render::RayCaster *backendRayCaster = test->nodeManagers()->rayCasterManager()->lookupResource(rayCaster->id());
        QVERIFY(backendRayCaster);
        Qt3DCore::QBackendNodePrivate::get(backendRayCaster)->setArbiter(test->arbiter());

        // WHEN
        Qt3DRender::Render::RayCastingJob rayCastingJob;
        initializeJob(&rayCastingJob, test.data());

        bool earlyReturn = !rayCastingJob.runHelper();
        rayCastingJob.postFrame(test->aspectManager());
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(!earlyReturn);
        QVERIFY(!backendRayCaster->isEnabled());
        QVERIFY(!rayCaster->isEnabled());
        auto dirtyNodes = test->arbiter()->takeDirtyFrontEndNodes();
        QCOMPARE(dirtyNodes.count(), 1); // hits & disable
        QCOMPARE(rayCaster->hits().size(), numIntersections);

        if (numIntersections)
            QVERIFY(rayCaster->hits().first().entityId());
    }

};

QTEST_MAIN(tst_RayCastingJob)

#include "tst_raycastingjob.moc"
