/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <QtQuick/qquickwindow.h>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QNoPicking>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QPickTriangleEvent>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>
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
#include <Qt3DRender/private/qobjectpicker_p.h>
#include <Qt3DRender/private/nopicking_p.h>

#include <QSignalSpy>

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

void initializePickBoundingVolumeJob(Qt3DRender::Render::PickBoundingVolumeJob *job, Qt3DRender::TestAspect *test)
{
    job->setFrameGraphRoot(test->frameGraphRoot());
    job->setRoot(test->sceneRoot());
    job->setManagers(test->nodeManagers());
    job->setRenderSettings(test->renderSettings());
}

} // anonymous

class tst_PickBoundingVolumeJob : public QObject
{
    Q_OBJECT
private:
    void generateAllPickingSettingsCombinations()
    {
        QTest::addColumn<Qt3DRender::QPickingSettings::PickMethod>("pickMethod");
        QTest::addColumn<Qt3DRender::QPickingSettings::PickResultMode>("pickResultMode");
        QTest::addColumn<Qt3DRender::QPickingSettings::FaceOrientationPickingMode>("faceOrientationPickingMode");

        QTest::newRow("volume, nearest, front") << Qt3DRender::QPickingSettings::BoundingVolumePicking
                                                << Qt3DRender::QPickingSettings::NearestPick
                                                << Qt3DRender::QPickingSettings::FrontFace;

        QTest::newRow("volume, nearest, back") << Qt3DRender::QPickingSettings::BoundingVolumePicking
                                               << Qt3DRender::QPickingSettings::NearestPick
                                               << Qt3DRender::QPickingSettings::BackFace;

        QTest::newRow("volume, nearest, front+back") << Qt3DRender::QPickingSettings::BoundingVolumePicking
                                                     << Qt3DRender::QPickingSettings::NearestPick
                                                     << Qt3DRender::QPickingSettings::FrontAndBackFace;

        QTest::newRow("volume, all, front") << Qt3DRender::QPickingSettings::BoundingVolumePicking
                                            << Qt3DRender::QPickingSettings::AllPicks
                                            << Qt3DRender::QPickingSettings::FrontFace;

        QTest::newRow("volume, all, back") << Qt3DRender::QPickingSettings::BoundingVolumePicking
                                           << Qt3DRender::QPickingSettings::AllPicks
                                           << Qt3DRender::QPickingSettings::BackFace;

        QTest::newRow("volume, all, front+back") << Qt3DRender::QPickingSettings::BoundingVolumePicking
                                                 << Qt3DRender::QPickingSettings::AllPicks
                                                 << Qt3DRender::QPickingSettings::FrontAndBackFace;

        QTest::newRow("triangle, nearest, front") << Qt3DRender::QPickingSettings::TrianglePicking
                                                  << Qt3DRender::QPickingSettings::NearestPick
                                                  << Qt3DRender::QPickingSettings::FrontFace;

        QTest::newRow("triangle, nearest, back") << Qt3DRender::QPickingSettings::TrianglePicking
                                                 << Qt3DRender::QPickingSettings::NearestPick
                                                 << Qt3DRender::QPickingSettings::BackFace;

        QTest::newRow("triangle, nearest, front+back") << Qt3DRender::QPickingSettings::TrianglePicking
                                                       << Qt3DRender::QPickingSettings::NearestPick
                                                       << Qt3DRender::QPickingSettings::FrontAndBackFace;

        QTest::newRow("triangle, all, front") << Qt3DRender::QPickingSettings::TrianglePicking
                                              << Qt3DRender::QPickingSettings::AllPicks
                                              << Qt3DRender::QPickingSettings::FrontFace;

        QTest::newRow("triangle, all, back") << Qt3DRender::QPickingSettings::TrianglePicking
                                             << Qt3DRender::QPickingSettings::AllPicks
                                             << Qt3DRender::QPickingSettings::BackFace;

        QTest::newRow("triangle, all, front+back") << Qt3DRender::QPickingSettings::TrianglePicking
                                                   << Qt3DRender::QPickingSettings::AllPicks
                                                   << Qt3DRender::QPickingSettings::FrontAndBackFace;
    }

private Q_SLOTS:

    void viewportCameraAreaGather()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // THEN
        QVERIFY(test->frameGraphRoot() != nullptr);
        Qt3DRender::QCamera *camera = root->findChild<Qt3DRender::QCamera *>();
        QVERIFY(camera != nullptr);
        QQuickWindow *window = root->findChild<QQuickWindow *>();
        QVERIFY(camera != nullptr);
        QCOMPARE(window->size(), QSize(600, 600));

        // WHEN
        Qt3DRender::Render::PickingUtils::ViewportCameraAreaGatherer gatherer;
        QVector<Qt3DRender::Render::PickingUtils::ViewportCameraAreaDetails> results = gatherer.gather(test->frameGraphRoot());

        // THEN
        QCOMPARE(results.size(), 1);
        auto vca = results.first();
        QCOMPARE(vca.area, QSize(600, 600));
        QCOMPARE(vca.cameraId, camera->id());
        QCOMPARE(vca.viewport, QRectF(0., 0., 1., 1.));
    }

    void checkCurrentPickerChange_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkCurrentPickerChange()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker1 = nullptr;
        Qt3DRender::QObjectPicker *picker2 = nullptr;

        if (pickers.first()->objectName() == QLatin1String("Picker1")) {
            picker1 = pickers.first();
            picker2 = pickers.last();
        } else {
            picker1 = pickers.last();
            picker2 = pickers.first();
        }

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        // THEN
        QVERIFY(pickBVJob.currentPicker().isNull());

        // WHEN
        QList<QPair<QObject *,QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207., 303.), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!earlyReturn);
        QVERIFY(!pickBVJob.currentPicker().isNull());
        Qt3DRender::Render::ObjectPicker *backendPicker = test->nodeManagers()->data<Qt3DRender::Render::ObjectPicker, Qt3DRender::Render::ObjectPickerManager>(pickBVJob.currentPicker());
        QVERIFY(backendPicker != nullptr);
        QCOMPARE(backendPicker->peerId(), picker1->id());

        // WHEN
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(207., 303.), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!earlyReturn);
        QVERIFY(pickBVJob.currentPicker().isNull());

        // WHEN
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(390., 300.), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!earlyReturn);
        QVERIFY(!pickBVJob.currentPicker().isNull());
        backendPicker = test->nodeManagers()->data<Qt3DRender::Render::ObjectPicker, Qt3DRender::Render::ObjectPickerManager>(pickBVJob.currentPicker());
        QVERIFY(backendPicker != nullptr);
        QCOMPARE(backendPicker->peerId(), picker2->id());

        // WHEN
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(390., 300.), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!earlyReturn);
        QVERIFY(pickBVJob.currentPicker().isNull());
    }

    void checkEarlyReturnWhenNoMouseEvents_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkEarlyReturnWhenNoMouseEvents()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        // THEN
        QVERIFY(pickBVJob.currentPicker().isNull());

        // WHEN
        bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(earlyReturn);

        // WHEN
        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(400., 440.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!earlyReturn);
    }

    void checkEarlyReturnWhenMoveEventsAndNoCurrentPickers_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkEarlyReturnWhenMoveEventsAndNoCurrentPickers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseMove, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);

        // THEN
        QVERIFY(pickBVJob.currentPicker().isNull());

        // WHEN
        const bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(earlyReturn);
    }

    void checkEarlyReturnWhenAllPickersDisabled_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkEarlyReturnWhenAllPickersDisabled()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_pickersdisabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings =
                root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers =
                root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(earlyReturn);
    }

    void checkEarlyReturnWhenMoveEventsAndDragDisabledPickers_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkEarlyReturnWhenMoveEventsAndDragDisabledPickers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragdisabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!pickBVJob.currentPicker().isNull());
        QVERIFY(!earlyReturn);

        // WHEN
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseMove, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(earlyReturn);
    }

    void checkNoEarlyReturnWhenMoveEventsAndDragEnabledPickers_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkNoEarlyReturnWhenMoveEventsAndDragEnabledPickers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!pickBVJob.currentPicker().isNull());
        QVERIFY(!earlyReturn);

        // WHEN
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseMove, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(!earlyReturn);
    }

    void checkEarlyReturnWhenNoProperFrameGraph_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkEarlyReturnWhenNoProperFrameGraph()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_improperframegraph.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        // WHEN
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        const bool earlyReturn = !pickBVJob.runHelper();

        // THEN
        QVERIFY(pickBVJob.currentPicker().isNull());
        QVERIFY(earlyReturn);
    }

    void checkDispatchMouseEvent_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkDispatchMouseEvent()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);
        test->renderSettings()->syncFromFrontEnd(renderSettings.first(), false);

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker1 = nullptr;
        if (pickers.first()->objectName() == QLatin1String("Picker1"))
            picker1 = pickers.first();
        else
            picker1 = pickers.last();

        Qt3DRender::Render::ObjectPicker *backendPicker1 = test->nodeManagers()->objectPickerManager()->lookupResource(picker1->id());
        QVERIFY(backendPicker1);
        Qt3DCore::QBackendNodePrivate::get(backendPicker1)->setArbiter(test->arbiter());

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        const bool backAndFrontPicking =
                (pickMethod == Qt3DRender::QPickingSettings::TrianglePicking) &&
                (pickResultMode == Qt3DRender::QPickingSettings::AllPicks) &&
                (faceOrientationPickingMode == Qt3DRender::QPickingSettings::FrontAndBackFace);

        // WHEN -> Pressed on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QSignalSpy mouseButtonPressedSpy(picker1, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy(picker1, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy(picker1, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy(picker1, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy.isValid());
        QVERIFY(mouseMovedSpy.isValid());
        QVERIFY(mouseButtonReleasedSpy.isValid());
        QVERIFY(mouseClickedSpy.isValid());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker1->isPressed());
        QVERIFY(picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);

        // WHEN -> Move on same object
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseMove, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Moved
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker1->isPressed());
        QVERIFY(picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);

        // WHEN -> Release on object
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Released + Clicked
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseButtonReleasedSpy.count(), /*backAndFrontPicking ? 2 :*/ 1);
        QCOMPARE(mouseClickedSpy.count(), 1);

        mouseButtonPressedSpy.clear();
        mouseMovedSpy.clear();
        mouseButtonReleasedSpy.clear();
        mouseClickedSpy.clear();

        // WHEN -> Release outside of object
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(0.0, 0.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Released
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 1);
    }

    void checkDispatchReleaseEventOnLastPickerWhenMovingOutOfViewport()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        settings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
        settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
        settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontAndBackFace);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker1 = nullptr;
        if (pickers.first()->objectName() == QLatin1String("Picker1"))
            picker1 = pickers.first();
        else
            picker1 = pickers.last();

        Qt3DRender::Render::ObjectPicker *backendPicker1 = test->nodeManagers()->objectPickerManager()->lookupResource(picker1->id());
        QVERIFY(backendPicker1);
        Qt3DCore::QBackendNodePrivate::get(backendPicker1)->setArbiter(&arbiter);

        QSignalSpy mouseButtonPressedSpy(picker1, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy(picker1, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy(picker1, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy(picker1, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy.isValid());
        QVERIFY(mouseMovedSpy.isValid());
        QVERIFY(mouseButtonReleasedSpy.isValid());
        QVERIFY(mouseClickedSpy.isValid());


        // WHEN -> Pressed on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker1->isPressed());
        QVERIFY(picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);

        // WHEN -> Releasing out of the viewport
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(10000.0, 10000.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Should have received released event
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 1);


        // WHEN -> Releasing out of the viewport
        mouseButtonPressedSpy.clear();
        mouseMovedSpy.clear();
        mouseButtonReleasedSpy.clear();
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(10000.0, 10000.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Should have received nothing
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 0);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
    }

    void checkDispatchHoverEvent_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkDispatchHoverEvent()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabledhoverenabled.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker1 = nullptr;
        if (pickers.first()->objectName() == QLatin1String("Picker1"))
            picker1 = pickers.first();
        else
            picker1 = pickers.last();

        Qt3DRender::Render::ObjectPicker *backendPicker1 = test->nodeManagers()->objectPickerManager()->lookupResource(picker1->id());
        QVERIFY(backendPicker1);
        Qt3DCore::QBackendNodePrivate::get(backendPicker1)->setArbiter(&arbiter);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        QSignalSpy mouseEntered(picker1, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited(picker1, &Qt3DRender::QObjectPicker::exited);

        QVERIFY(mouseEntered.isValid());
        QVERIFY(mouseExited.isValid());

        // WHEN -> Hover on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::HoverMove, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Entered
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseEntered.count(), 1);
        QCOMPARE(mouseExited.count(), 0);

        // WHEN -> HoverMove Out
        events.clear();
        events.push_back({nullptr, QMouseEvent(QEvent::HoverMove, QPointF(20.0, 40.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN - Exited
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseEntered.count(), 1);
        QCOMPARE(mouseExited.count(), 1);

        mouseEntered.clear();
        mouseExited.clear();

        // WHEN -> HoverMove In + Pressed other
        events.clear();
        events.push_back({nullptr, QMouseEvent(QEvent::HoverMove, QPointF(207.0, 303.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        events.push_back({nullptr, QMouseEvent(QEvent::MouseButtonPress, QPointF(0.0, 0.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN - Entered, Exited
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker1->isPressed());
        QVERIFY(!picker1->isPressed());
        QCOMPARE(mouseEntered.count(), 1);
        QCOMPARE(mouseExited.count(), 1);
    }

    void shouldDispatchMouseEventFromChildren_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void shouldDispatchMouseEventFromChildren()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_childentity.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 1);

        Qt3DRender::QObjectPicker *picker = pickers.first();
        QCOMPARE(pickers.first()->objectName(), QLatin1String("Picker"));

        Qt3DRender::Render::ObjectPicker *backendPicker = test->nodeManagers()->objectPickerManager()->lookupResource(picker->id());
        QVERIFY(backendPicker);
        Qt3DCore::QBackendNodePrivate::get(backendPicker)->setArbiter(&arbiter);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        const bool backAndFrontPicking =
                (pickMethod == Qt3DRender::QPickingSettings::TrianglePicking) &&
                (pickResultMode == Qt3DRender::QPickingSettings::AllPicks) &&
                (faceOrientationPickingMode == Qt3DRender::QPickingSettings::FrontAndBackFace);

        QSignalSpy mouseButtonPressedSpy(picker, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy(picker, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy(picker, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy(picker, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy.isValid());
        QVERIFY(mouseMovedSpy.isValid());
        QVERIFY(mouseButtonReleasedSpy.isValid());
        QVERIFY(mouseClickedSpy.isValid());

        // WHEN -> Pressed on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(400.0, 300.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker->isPressed());
        QVERIFY(picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);

        // WHEN -> Move on same object
        mouseButtonPressedSpy.clear();
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseMove, QPointF(400.0, 300.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Moved
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker->isPressed());
        QVERIFY(picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 0);
        QCOMPARE(mouseMovedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);

        // WHEN -> Release on object
        mouseMovedSpy.clear();
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(400.0, 300.0),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Released + Clicked
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker->isPressed());
        QVERIFY(!picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 0);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 1);
        QCOMPARE(mouseClickedSpy.count(), 1);

        // WHEN -> Release outside of object
        events.clear();
        mouseButtonPressedSpy.clear();
        mouseMovedSpy.clear();
        mouseButtonReleasedSpy.clear();

        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(400., 300.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(0., 0.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Released
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker->isPressed());
        QVERIFY(!picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 1);
        QCOMPARE(mouseClickedSpy.count(), 1);
    }

    void checkPickerGrabbing_data()
    {
        generateAllPickingSettingsCombinations();
    }

    void checkPickerGrabbing()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_dragenabledoverlapping.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        QFETCH(Qt3DRender::QPickingSettings::PickMethod, pickMethod);
        QFETCH(Qt3DRender::QPickingSettings::PickResultMode, pickResultMode);
        QFETCH(Qt3DRender::QPickingSettings::FaceOrientationPickingMode, faceOrientationPickingMode);
        settings->setPickMethod(pickMethod);
        settings->setPickResultMode(pickResultMode);
        settings->setFaceOrientationPickingMode(faceOrientationPickingMode);

        const bool backAndFrontPicking =
                (pickMethod == Qt3DRender::QPickingSettings::TrianglePicking) &&
                (pickResultMode == Qt3DRender::QPickingSettings::AllPicks) &&
                (faceOrientationPickingMode == Qt3DRender::QPickingSettings::FrontAndBackFace);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter1;
        TestArbiter arbiter2;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker1 = nullptr;
        Qt3DRender::QObjectPicker *picker2 = nullptr;
        if (pickers.first()->objectName() == QLatin1String("Picker1")) {
            picker1 = pickers.first();
            picker2 = pickers.last();
        } else {
            picker1 = pickers.last();
            picker2 = pickers.first();
        }

        Qt3DRender::Render::ObjectPicker *backendPicker1 = test->nodeManagers()->objectPickerManager()->lookupResource(picker1->id());
        QVERIFY(backendPicker1);
        Qt3DCore::QBackendNodePrivate::get(backendPicker1)->setArbiter(&arbiter1);

        Qt3DRender::Render::ObjectPicker *backendPicker2 = test->nodeManagers()->objectPickerManager()->lookupResource(picker2->id());
        QVERIFY(backendPicker2);
        Qt3DCore::QBackendNodePrivate::get(backendPicker2)->setArbiter(&arbiter2);

        QCOMPARE(test->renderSettings()->pickMethod(), pickMethod);
        QCOMPARE(test->renderSettings()->pickResultMode(), pickResultMode);
        QCOMPARE(test->renderSettings()->faceOrientationPickingMode(), faceOrientationPickingMode);

        QSignalSpy mouseEntered1(picker1, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited1(picker1, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy1(picker1, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy1(picker1, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy1(picker1, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy1(picker1, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy1.isValid());
        QVERIFY(mouseMovedSpy1.isValid());
        QVERIFY(mouseButtonReleasedSpy1.isValid());
        QVERIFY(mouseClickedSpy1.isValid());
        QVERIFY(mouseEntered1.isValid());
        QVERIFY(mouseExited1.isValid());

        QSignalSpy mouseEntered2(picker2, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited2(picker2, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy2(picker2, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy2(picker2, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy2(picker2, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy2(picker2, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy2.isValid());
        QVERIFY(mouseMovedSpy2.isValid());
        QVERIFY(mouseButtonReleasedSpy2.isValid());
        QVERIFY(mouseClickedSpy2.isValid());
        QVERIFY(mouseEntered2.isValid());
        QVERIFY(mouseExited2.isValid());

        // WHEN -> Pressed on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(320., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker1->isPressed());
        QVERIFY(picker1->isPressed());

        QCOMPARE(mouseButtonPressedSpy1.count(), backAndFrontPicking ? 2 : 1);
        QCOMPARE(mouseMovedSpy1.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy1.count(), 0);
        QCOMPARE(mouseClickedSpy1.count(), 0);
        QCOMPARE(mouseEntered1.count(), 0);
        QCOMPARE(mouseExited1.count(), 0);

        // WHEN -> Move on next object, show stay on previous picker unless all picks are requested
        events.clear();
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseMove, QPointF(280., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Moved over the second picker, is the first one still pressed
        QVERIFY(!earlyReturn);
        if (pickResultMode != Qt3DRender::QPickingSettings::AllPicks) {
            QVERIFY(backendPicker1->isPressed());
            QVERIFY(picker1->isPressed());
            QCOMPARE(mouseButtonPressedSpy1.count(), 1);
            QCOMPARE(mouseMovedSpy1.count(), 1);
            QCOMPARE(mouseButtonReleasedSpy1.count(), 0);
            QCOMPARE(mouseClickedSpy1.count(), 0);
            QCOMPARE(mouseEntered1.count(), 0);
            QCOMPARE(mouseExited1.count(), 0);
        } else {
            QVERIFY(!picker2->isPressed());
            QCOMPARE(mouseButtonPressedSpy2.count(), 0);
            QCOMPARE(mouseMovedSpy2.count(), backAndFrontPicking ? 2 : 1);
            QCOMPARE(mouseButtonReleasedSpy2.count(), 0);
            QCOMPARE(mouseClickedSpy2.count(), 0);
            QCOMPARE(mouseEntered2.count(), 1);
            QCOMPARE(mouseExited2.count(), 0);
        }
    }

    void checkParentNoPickerChildPicker()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_parententity.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        settings->setPickMethod(Qt3DRender::QPickingSettings::BoundingVolumePicking);
        settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
        settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        // large no pickable object encapsing child and camera should not interfere with smaller picking object
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 1);

        Qt3DRender::QObjectPicker *picker = pickers.first();
        QCOMPARE(pickers.first()->objectName(), QLatin1String("Picker"));

        Qt3DRender::Render::ObjectPicker *backendPicker = test->nodeManagers()->objectPickerManager()->lookupResource(picker->id());
        QVERIFY(backendPicker);
        Qt3DCore::QBackendNodePrivate::get(backendPicker)->setArbiter(&arbiter);

        QSignalSpy mouseEntered(picker, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited(picker, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy(picker, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy(picker, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy(picker, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy(picker, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy.isValid());
        QVERIFY(mouseMovedSpy.isValid());
        QVERIFY(mouseButtonReleasedSpy.isValid());
        QVERIFY(mouseClickedSpy.isValid());
        QVERIFY(mouseEntered.isValid());
        QVERIFY(mouseExited.isValid());

        // WHEN -> Pressed on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(400., 300.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker->isPressed());
        QVERIFY(picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);
        QCOMPARE(mouseEntered.count(), 0);
        QCOMPARE(mouseExited.count(), 0);
    }

    void checkPickerAndViewports()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_viewports.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        settings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
        settings->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
        settings->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        // object partially obscured by another viewport, make sure only visible portion is pickable
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker = pickers.last();
        QCOMPARE(picker->objectName(), QLatin1String("Picker2"));

        Qt3DRender::Render::ObjectPicker *backendPicker = test->nodeManagers()->objectPickerManager()->lookupResource(picker->id());
        QVERIFY(backendPicker);
        Qt3DCore::QBackendNodePrivate::get(backendPicker)->setArbiter(&arbiter);

        QSignalSpy mouseEntered(picker, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited(picker, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy(picker, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy(picker, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy(picker, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy(picker, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy.isValid());
        QVERIFY(mouseMovedSpy.isValid());
        QVERIFY(mouseButtonReleasedSpy.isValid());
        QVERIFY(mouseClickedSpy.isValid());
        QVERIFY(mouseEntered.isValid());
        QVERIFY(mouseExited.isValid());

        // WHEN -> Pressed on object in vp1
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(280., 300.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker->isPressed());
        QVERIFY(picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);
        QCOMPARE(mouseEntered.count(), 0);
        QCOMPARE(mouseExited.count(), 0);

        // WHEN reset -> Presset on object in vp2
        backendPicker->cleanup();
        backendPicker->setEnabled(true);
        events.clear();

        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(320., 300.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Nothing happened
        QVERIFY(!earlyReturn);
        QVERIFY(!backendPicker->isPressed());
        QVERIFY(picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);
        QCOMPARE(mouseEntered.count(), 0);
        QCOMPARE(mouseExited.count(), 0);
    }

    void checkMultipleRayDirections_data()
    {
        QTest::addColumn<QVector3D>("cameraOrigin");
        QTest::addColumn<QVector3D>("cameraUpVector");

        int k = 0;
        const int n = 10;
        for (int j=0; j<n; j++) {
            QMatrix4x4 m;
            m.rotate(360.f / (float)n * (float)j, 0.f, 0.f, 1.f);
            for (int i=0; i<n; i++) {
                const double angle = M_PI * 2. / (double)n * i;
                const double x = std::sin(angle) * 10.;
                const double z = std::cos(angle) * 10.;
                QVector3D pos(x, 0.f, z);
                QVector3D up(0.f, 1.f, 0.f);
                QTest::newRow(QString::number(k++).toLatin1().data()) << m * pos << m * up;
            }
        }
    }

    void checkMultipleRayDirections()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_cameraposition.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QList<Qt3DRender::QRenderSettings *> renderSettings = root->findChildren<Qt3DRender::QRenderSettings *>();
        QCOMPARE(renderSettings.size(), 1);
        Qt3DRender::QPickingSettings *settings = renderSettings.first()->pickingSettings();

        settings->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter;

        QList<Qt3DRender::QCamera *> cameras = root->findChildren<Qt3DRender::QCamera *>();
        QCOMPARE(cameras.size(), 1);
        Qt3DRender::QCamera *camera = cameras.first();

        QFETCH(QVector3D, cameraUpVector);
        camera->setUpVector(cameraUpVector);

        QFETCH(QVector3D, cameraOrigin);
        camera->setPosition(cameraOrigin);

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 1);

        Qt3DRender::QObjectPicker *picker = pickers.front();

        Qt3DRender::Render::ObjectPicker *backendPicker = test->nodeManagers()->objectPickerManager()->lookupResource(picker->id());
        QVERIFY(backendPicker);
        Qt3DCore::QBackendNodePrivate::get(backendPicker)->setArbiter(&arbiter);

        QSignalSpy mouseEntered(picker, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited(picker, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy(picker, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy(picker, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy(picker, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy(picker, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy.isValid());
        QVERIFY(mouseMovedSpy.isValid());
        QVERIFY(mouseButtonReleasedSpy.isValid());
        QVERIFY(mouseClickedSpy.isValid());
        QVERIFY(mouseEntered.isValid());
        QVERIFY(mouseExited.isValid());

        // WHEN -> Pressed on object
        Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
        initializePickBoundingVolumeJob(&pickBVJob, test.data());

        QList<QPair<QObject *, QMouseEvent>> events;
        events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(303., 303.),
                                               Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
        pickBVJob.setMouseEvents(events);
        bool earlyReturn = !pickBVJob.runHelper();
        Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

        // THEN -> Pressed
        QVERIFY(!earlyReturn);
        QVERIFY(backendPicker->isPressed());
        QVERIFY(picker->isPressed());
        QCOMPARE(mouseButtonPressedSpy.count(), 1);
        QCOMPARE(mouseMovedSpy.count(), 0);
        QCOMPARE(mouseButtonReleasedSpy.count(), 0);
        QCOMPARE(mouseClickedSpy.count(), 0);
        QCOMPARE(mouseEntered.count(), 0);
        QCOMPARE(mouseExited.count(), 0);
    }

    void checkPriorityPicking()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_priorityoverlapping.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        TestArbiter arbiter1;
        TestArbiter arbiter2;

        // Runs Required jobs
        runRequiredJobs(test.data());

        // THEN
        QList<Qt3DRender::QObjectPicker *> pickers = root->findChildren<Qt3DRender::QObjectPicker *>();
        QCOMPARE(pickers.size(), 2);

        Qt3DRender::QObjectPicker *picker1 = nullptr;
        Qt3DRender::QObjectPicker *picker2 = nullptr;
        if (pickers.first()->objectName() == QLatin1String("Picker1")) {
            picker1 = pickers.first();
            picker2 = pickers.last();
        } else {
            picker1 = pickers.last();
            picker2 = pickers.first();
        }

        Qt3DRender::Render::ObjectPicker *backendPicker1 = test->nodeManagers()->objectPickerManager()->lookupResource(picker1->id());
        QVERIFY(backendPicker1);
        Qt3DCore::QBackendNodePrivate::get(backendPicker1)->setArbiter(&arbiter1);

        Qt3DRender::Render::ObjectPicker *backendPicker2 = test->nodeManagers()->objectPickerManager()->lookupResource(picker2->id());
        QVERIFY(backendPicker2);
        Qt3DCore::QBackendNodePrivate::get(backendPicker2)->setArbiter(&arbiter2);

        QSignalSpy mouseEntered1(picker1, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited1(picker1, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy1(picker1, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy1(picker1, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy1(picker1, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy1(picker1, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy1.isValid());
        QVERIFY(mouseMovedSpy1.isValid());
        QVERIFY(mouseButtonReleasedSpy1.isValid());
        QVERIFY(mouseClickedSpy1.isValid());
        QVERIFY(mouseEntered1.isValid());
        QVERIFY(mouseExited1.isValid());

        QSignalSpy mouseEntered2(picker2, &Qt3DRender::QObjectPicker::entered);
        QSignalSpy mouseExited2(picker2, &Qt3DRender::QObjectPicker::exited);
        QSignalSpy mouseButtonPressedSpy2(picker2, &Qt3DRender::QObjectPicker::pressed);
        QSignalSpy mouseMovedSpy2(picker2, &Qt3DRender::QObjectPicker::moved);
        QSignalSpy mouseButtonReleasedSpy2(picker2, &Qt3DRender::QObjectPicker::released);
        QSignalSpy mouseClickedSpy2(picker2, &Qt3DRender::QObjectPicker::clicked);

        QVERIFY(mouseButtonPressedSpy2.isValid());
        QVERIFY(mouseMovedSpy2.isValid());
        QVERIFY(mouseButtonReleasedSpy2.isValid());
        QVERIFY(mouseClickedSpy2.isValid());
        QVERIFY(mouseEntered2.isValid());
        QVERIFY(mouseExited2.isValid());

        // WHEN both have priority == 0, select closest
        {
            Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
            initializePickBoundingVolumeJob(&pickBVJob, test.data());

            // WHEN -> Pressed on object
            QList<QPair<QObject *, QMouseEvent>> events;
            events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(300., 300.),
                                                   Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
            pickBVJob.setMouseEvents(events);
            bool earlyReturn = !pickBVJob.runHelper();
            Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

            // THEN -> Select picker with highest priority
            QVERIFY(!earlyReturn);
            QVERIFY(backendPicker1->isPressed());
            QVERIFY(picker1->isPressed());
            QCOMPARE(mouseButtonPressedSpy1.count(), 1);
            QCOMPARE(mouseMovedSpy1.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy1.count(), 0);
            QCOMPARE(mouseClickedSpy1.count(), 0);
            QCOMPARE(mouseEntered1.count(), 0);
            QCOMPARE(mouseExited1.count(), 0);

            QVERIFY(!backendPicker2->isPressed());
            QVERIFY(!picker2->isPressed());
            QCOMPARE(mouseButtonPressedSpy2.count(), 0);
            QCOMPARE(mouseMovedSpy2.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy2.count(), 0);
            QCOMPARE(mouseClickedSpy2.count(), 0);
            QCOMPARE(mouseEntered2.count(), 0);
            QCOMPARE(mouseExited2.count(), 0);

            events.clear();
            events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(300., 300.),
                                                   Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
            pickBVJob.setMouseEvents(events);
            pickBVJob.runHelper();
            Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

            QVERIFY(!backendPicker1->isPressed());
            QVERIFY(!picker1->isPressed());
            QCOMPARE(mouseButtonPressedSpy1.count(), 1);
            QCOMPARE(mouseMovedSpy1.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy1.count(), 1);
            QCOMPARE(mouseClickedSpy1.count(), 1);
            QCOMPARE(mouseEntered1.count(), 0);
            QCOMPARE(mouseExited1.count(), 0);

            QVERIFY(!backendPicker2->isPressed());
            QVERIFY(!picker2->isPressed());
            QCOMPARE(mouseButtonPressedSpy2.count(), 0);
            QCOMPARE(mouseMovedSpy2.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy2.count(), 0);
            QCOMPARE(mouseClickedSpy2.count(), 0);
            QCOMPARE(mouseEntered2.count(), 0);
            QCOMPARE(mouseExited2.count(), 0);
        }

        mouseButtonPressedSpy1.clear();
        mouseButtonReleasedSpy1.clear();
        mouseClickedSpy1.clear();

        // WHEN furthest one has higher priority, select furthest one
        {
            backendPicker2->setPriority(1000);
            QCOMPARE(backendPicker2->priority(), 1000);

            Qt3DRender::Render::PickBoundingVolumeJob pickBVJob;
            initializePickBoundingVolumeJob(&pickBVJob, test.data());

            // WHEN -> Pressed on object
            QList<QPair<QObject *, QMouseEvent>> events;
            events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonPress, QPointF(300., 300.),
                                                   Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
            pickBVJob.setMouseEvents(events);
            bool earlyReturn = !pickBVJob.runHelper();
            Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

            // THEN -> Select picker with highest priority
            QVERIFY(!earlyReturn);
            QVERIFY(!backendPicker1->isPressed());
            QVERIFY(!picker1->isPressed());
            QCOMPARE(mouseButtonPressedSpy1.count(), 0);
            QCOMPARE(mouseMovedSpy1.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy1.count(), 0);
            QCOMPARE(mouseClickedSpy1.count(), 0);
            QCOMPARE(mouseEntered1.count(), 0);
            QCOMPARE(mouseExited1.count(), 0);

            QVERIFY(backendPicker2->isPressed());
            QVERIFY(picker2->isPressed());
            QCOMPARE(mouseButtonPressedSpy2.count(), 1);
            QCOMPARE(mouseMovedSpy2.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy2.count(), 0);
            QCOMPARE(mouseClickedSpy2.count(), 0);
            QCOMPARE(mouseEntered2.count(), 0);
            QCOMPARE(mouseExited2.count(), 0);

            events.clear();
            events.push_back({nullptr, QMouseEvent(QMouseEvent::MouseButtonRelease, QPointF(300., 300.),
                                                   Qt::LeftButton, Qt::LeftButton, Qt::NoModifier)});
            pickBVJob.setMouseEvents(events);
            pickBVJob.runHelper();
            Qt3DCore::QAspectJobPrivate::get(&pickBVJob)->postFrame(test->aspectManager());

            QVERIFY(!backendPicker1->isPressed());
            QVERIFY(!picker1->isPressed());
            QCOMPARE(mouseButtonPressedSpy1.count(), 0);
            QCOMPARE(mouseMovedSpy1.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy1.count(), 0);
            QCOMPARE(mouseClickedSpy1.count(), 0);
            QCOMPARE(mouseEntered1.count(), 0);
            QCOMPARE(mouseExited1.count(), 0);

            QVERIFY(!backendPicker2->isPressed());
            QVERIFY(!picker2->isPressed());
            QCOMPARE(mouseButtonPressedSpy2.count(), 1);
            QCOMPARE(mouseMovedSpy2.count(), 0);
            QCOMPARE(mouseButtonReleasedSpy2.count(), 1);
            QCOMPARE(mouseClickedSpy2.count(), 1);
            QCOMPARE(mouseEntered2.count(), 0);
            QCOMPARE(mouseExited2.count(), 0);
        }
    }

    void checkNoPickingFGPicking()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/testscene_nopicking.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // THEN
        QVERIFY(test->frameGraphRoot() != nullptr);
        Qt3DRender::QNoPicking *noPicking = root->findChild<Qt3DRender::QNoPicking *>();
        QVERIFY(noPicking != nullptr);
        Qt3DRender::QCamera *camera = root->findChild<Qt3DRender::QCamera *>();
        QVERIFY(camera != nullptr);
        QQuickWindow *window = root->findChild<QQuickWindow *>();
        QVERIFY(camera != nullptr);
        QCOMPARE(window->size(), QSize(600, 600));

        // WHEN
        Qt3DRender::Render::PickingUtils::ViewportCameraAreaGatherer gatherer;
        QVector<Qt3DRender::Render::PickingUtils::ViewportCameraAreaDetails> results = gatherer.gather(test->frameGraphRoot());

        // THEN
        QCOMPARE(results.size(), 0);

        // WHEN
        Qt3DRender::Render::FrameGraphNode *backendFGNode = test->nodeManagers()->frameGraphManager()->lookupNode(noPicking->id());
        QVERIFY(backendFGNode);
        QCOMPARE(backendFGNode->nodeType(), Qt3DRender::Render::FrameGraphNode::NoPicking);
        Qt3DRender::Render::NoPicking * backendNoPicking = static_cast<Qt3DRender::Render::NoPicking *>(backendFGNode);
        backendNoPicking->setEnabled(false);

        // THEN
        QVERIFY(!backendNoPicking->isEnabled());

        // WHEN
        results = gatherer.gather(test->frameGraphRoot());

        // THEN
        QCOMPARE(results.size(), 1);
        auto vca = results.first();
        QCOMPARE(vca.area, QSize(600, 600));
        QCOMPARE(vca.cameraId, camera->id());
        QCOMPARE(vca.viewport, QRectF(0., 0., 1., 1.));
    }

};

QTEST_MAIN(tst_PickBoundingVolumeJob)

#include "tst_pickboundingvolumejob.moc"
