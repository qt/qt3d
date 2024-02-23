// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/private/updateshaderdatatransformjob_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include "qmlscenereader.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QList<Qt3DCore::QNode *> getNodesForCreation(Qt3DCore::QNode *root)
{
    using namespace Qt3DCore;

    QList<QNode *> nodes;
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

QList<Qt3DCore::NodeTreeChange> nodeTreeChangesForNodes(const QList<Qt3DCore::QNode *> nodes)
{
    QList<Qt3DCore::NodeTreeChange> nodeTreeChanges;
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
        : Qt3DRender::QRenderAspect()
        , m_sceneRoot(nullptr)
    {
        Qt3DRender::QRenderAspect::onRegistered();

        const QList<Qt3DCore::QNode *> nodes = getNodesForCreation(root);
        d_func()->setRootAndCreateNodes(qobject_cast<Qt3DCore::QEntity *>(root), nodeTreeChangesForNodes(nodes));

        Qt3DRender::Render::Entity *rootEntity = nodeManagers()->lookupResource<Qt3DRender::Render::Entity, Render::EntityManager>(rootEntityId());
        Q_ASSERT(rootEntity);
        m_sceneRoot = rootEntity;
    }

    ~TestAspect()
    {
        QRenderAspect::onUnregistered();
    }

    void onRegistered() override { Qt3DRender::QRenderAspect::onRegistered(); }
    void onUnregistered() override { Qt3DRender::QRenderAspect::onUnregistered(); }

    Qt3DRender::Render::NodeManagers *nodeManagers() const { return d_func()->m_renderer->nodeManagers(); }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const { return d_func()->m_renderer->frameGraphRoot(); }
    Qt3DRender::Render::RenderSettings *renderSettings() const { return d_func()->m_renderer->settings(); }
    Qt3DRender::Render::Entity *sceneRoot() const { return m_sceneRoot; }

private:
    Qt3DRender::Render::Entity *m_sceneRoot;
};

} // Qt3DRender

QT_END_NAMESPACE

namespace {

void runRequiredJobs(Qt3DRender::TestAspect *test)
{
    Qt3DRender::Render::UpdateWorldTransformJob updateWorldTransform;
    updateWorldTransform.setRoot(test->sceneRoot());
    updateWorldTransform.setManagers(test->nodeManagers());
    updateWorldTransform.run();
}

struct NodeCollection
{
    explicit NodeCollection(Qt3DRender::TestAspect *aspect, QObject *frontendRoot)
        : shaderData(frontendRoot->findChildren<Qt3DRender::QShaderData *>())
    {
        // THEN
        QCOMPARE(aspect->nodeManagers()->shaderDataManager()->activeHandles().size(), size_t(shaderData.size()));

        for (const Qt3DRender::QShaderData *s : std::as_const(shaderData)) {
            Qt3DRender::Render::ShaderData *backend = aspect->nodeManagers()->shaderDataManager()->lookupResource(s->id());
            QVERIFY(backend != nullptr);
            backendShaderData.push_back(backend);
        }
    }

    QList<Qt3DRender::QShaderData *> shaderData;
    QList<Qt3DRender::Render::ShaderData *> backendShaderData;
};

} // anonymous

class tst_UpdateShaderDataTransformJob : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::UpdateShaderDataTransformJob backendUpdateShaderDataTransformJob;

        // THEN
        QVERIFY(backendUpdateShaderDataTransformJob.managers() == nullptr);
    }

    void checkInitializeState()
    {
        // GIVEN
        Qt3DRender::Render::UpdateShaderDataTransformJob backendUpdateShaderDataTransformJob;
        Qt3DRender::Render::NodeManagers managers;

        // WHEN
        backendUpdateShaderDataTransformJob.setManagers(&managers);

        // THEN
        QVERIFY(backendUpdateShaderDataTransformJob.managers() == &managers);
    }

    void checkRunModelToEye()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/test_scene_model_to_eye.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Properly compute the world transforms
        runRequiredJobs(test.data());

        // WHEN
        Qt3DRender::QCamera *camera = root->findChild<Qt3DRender::QCamera *>();
        const NodeCollection collection(test.data(), root.data());

        // THEN
        QCOMPARE(collection.shaderData.size(), 1);
        QVERIFY(camera != nullptr);

        // WHEN
        Qt3DRender::Render::ShaderData *backendShaderData = collection.backendShaderData.first();
        const QHash<QString, Qt3DRender::Render::ShaderData::PropertyValue> &props = backendShaderData->properties();

        // THEN
        QCOMPARE(props.size(), 2);
        QVERIFY(props.contains(QStringLiteral("eyePosition")));
        QVERIFY(props.contains(QStringLiteral("eyePositionTransformed")));

        QCOMPARE(props[QStringLiteral("eyePosition")].value.value<QVector3D>(), QVector3D(1.0f, 1.0f, 1.0f));
        QCOMPARE(props[QStringLiteral("eyePositionTransformed")].value.toInt(), int(Qt3DRender::Render::ShaderData::ModelToEye));

        // WHEN
        Qt3DRender::Render::UpdateShaderDataTransformJob backendUpdateShaderDataTransformJob;
        backendUpdateShaderDataTransformJob.setManagers(test->nodeManagers());
        backendUpdateShaderDataTransformJob.run();

        // THEN
        // See scene file to find translation
        const Qt3DRender::Render::ShaderData::PropertyValue propertyValue = props[QStringLiteral("eyePosition")];
        QCOMPARE(backendShaderData->getTransformedProperty(&propertyValue, Matrix4x4(camera->viewMatrix())).value<Vector3D>(),
                 Matrix4x4(camera->viewMatrix()).map(Vector3D(1.0f, 1.0f, 1.0f) + Vector3D(0.0f, 5.0f, 0.0f)));
    }

    void checkRunModelToWorld()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/test_scene_model_to_world.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Properly compute the world transforms
        runRequiredJobs(test.data());

        // WHEN
        Qt3DRender::QCamera *camera = root->findChild<Qt3DRender::QCamera *>();
        const NodeCollection collection(test.data(), root.data());

        // THEN
        QCOMPARE(collection.shaderData.size(), 1);
        QVERIFY(camera != nullptr);

        // WHEN
        Qt3DRender::Render::ShaderData *backendShaderData = collection.backendShaderData.first();
        const QHash<QString, Qt3DRender::Render::ShaderData::PropertyValue> &props = backendShaderData->properties();

        // THEN
        QCOMPARE(props.size(), 2);
        QVERIFY(props.contains(QStringLiteral("position")));
        QVERIFY(props.contains(QStringLiteral("positionTransformed")));

        QCOMPARE(props[QStringLiteral("position")].value.value<QVector3D>(), QVector3D(1.0f, 1.0f, 1.0f));
        QCOMPARE(props[QStringLiteral("positionTransformed")].value.toInt(), int(Qt3DRender::Render::ShaderData::ModelToWorld));

        // WHEN
        Qt3DRender::Render::UpdateShaderDataTransformJob backendUpdateShaderDataTransformJob;
        backendUpdateShaderDataTransformJob.setManagers(test->nodeManagers());
        backendUpdateShaderDataTransformJob.run();

        // THEN
        // See scene file to find translation
        const Qt3DRender::Render::ShaderData::PropertyValue propertyValue = props[QStringLiteral("position")];
        QCOMPARE(backendShaderData->getTransformedProperty(&propertyValue, Matrix4x4(camera->viewMatrix())).value<Vector3D>(),
                 Vector3D(1.0f, 1.0f, 1.0f) + Vector3D(5.0f, 5.0f, 5.0f));
    }
};

QTEST_MAIN(tst_UpdateShaderDataTransformJob)

#include "tst_updateshaderdatatransformjob.moc"
