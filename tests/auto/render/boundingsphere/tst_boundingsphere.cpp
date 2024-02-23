// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qmlscenereader.h"
#include "testarbiter.h"

#include <QUrl>

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/calcboundingvolumejob_p.h>
#include <QtQuick/qquickwindow.h>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QPickTriangleEvent>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>
#include <Qt3DRender/private/updateworldboundingvolumejob_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>
#include <Qt3DRender/private/expandboundingvolumejob_p.h>
#include <Qt3DRender/private/calcboundingvolumejob_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/sphere_p.h>

#include <qbackendnodetester.h>

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

class TestAspect : public Qt3DRender::QRenderAspect, public Qt3DCore::QAbstractFrontEndNodeManager
{
public:
    TestAspect(Qt3DCore::QNode *root)
        : Qt3DRender::QRenderAspect()
        , m_sceneRoot(nullptr)
    {
        QRenderAspect::onRegistered();
        m_root = qobject_cast<Qt3DCore::QEntity *>(root);

        const QList<Qt3DCore::QNode *> nodes = getNodesForCreation(root);
        const QList<Qt3DCore::NodeTreeChange> nodeTreeChanges = nodeTreeChangesForNodes(nodes);
        d_func()->setRootAndCreateNodes(m_root, nodeTreeChanges);

        Render::Entity *rootEntity = nodeManagers()->lookupResource<Render::Entity, Render::EntityManager>(rootEntityId());
        Q_ASSERT(rootEntity);
        m_sceneRoot = rootEntity;
    }

    ~TestAspect()
    {
        QRenderAspect::onUnregistered();
    }

    void onRegistered() override { QRenderAspect::onRegistered(); }
    void onUnregistered() override { QRenderAspect::onUnregistered(); }

    Qt3DRender::Render::NodeManagers *nodeManagers() const { return d_func()->m_renderer->nodeManagers(); }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const { return d_func()->m_renderer->frameGraphRoot(); }
    Qt3DRender::Render::RenderSettings *renderSettings() const { return d_func()->m_renderer->settings(); }
    Qt3DCore::QEntity *root() const { return m_root; }
    Qt3DRender::Render::Entity *sceneRoot() const { return m_sceneRoot; }
    Qt3DRender::Render::AbstractRenderer *renderer() const { return d_func()->m_renderer; }

    void registerNode(Qt3DCore::QNode *node) { m_frontEndNodes.insert(node->id(), node); }
    void registerTree(Qt3DCore::QEntity *root) {
        using namespace Qt3DCore;
        QNodeVisitor visitor;
        visitor.traverse(root, [](QNode *) {}, [this](QEntity *entity) {
                registerNode(entity);
                const auto &components = entity->components();
                for (const auto &c : components)
                    registerNode(c);
            });
    }
    Qt3DCore::QNode *lookupNode(Qt3DCore::QNodeId id) const override { return  m_frontEndNodes.value(id, nullptr); }
    QList<Qt3DCore::QNode *> lookupNodes(const QList<Qt3DCore::QNodeId> &ids) const override {
        QList<Qt3DCore::QNode *> res;
        for (const auto &id: ids) {
            auto node = m_frontEndNodes.value(id, nullptr);
            if (node)
                res.push_back(node);
        }
        return  res;
    }

private:
    Qt3DCore::QEntity *m_root;
    Render::Entity *m_sceneRoot;
    QHash<Qt3DCore::QNodeId, Qt3DCore::QNode *> m_frontEndNodes;
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

    Qt3DCore::CalculateBoundingVolumeJob calcCBVolume(nullptr);
    calcCBVolume.setRoot(test->root());

    Qt3DRender::Render::CalculateBoundingVolumeJobPtr calcRBVolume = Qt3DRender::Render::CalculateBoundingVolumeJobPtr::create();
    calcRBVolume->setManagers(test->nodeManagers());
    calcRBVolume->setFrontEndNodeManager(test);
    calcRBVolume->setRoot(test->sceneRoot());

    calcCBVolume.addWatcher(calcRBVolume);

    calcCBVolume.run();
    calcCBVolume.postFrame(nullptr);
    calcRBVolume->run();

    Qt3DRender::Render::UpdateWorldBoundingVolumeJob updateWorldBVolume;
    updateWorldBVolume.setManager(test->nodeManagers()->renderNodesManager());
    updateWorldBVolume.run();

    Qt3DRender::Render::ExpandBoundingVolumeJob expandBVolume;
    expandBVolume.setRoot(test->sceneRoot());
    expandBVolume.setManagers(test->nodeManagers());
    expandBVolume.run();
}

} // anonymous

class tst_BoundingSphere : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private:

private Q_SLOTS:
    void checkIsNull() {
        auto defaultSphere = Qt3DRender::Render::Sphere();
        QVERIFY(defaultSphere.isNull());
    }

    void remainsNotNullAfterTransform() {
        QMatrix4x4 mat;
        mat.translate(-5,-5,-5);
        auto mMat = Matrix4x4(mat);
        auto pointSphere = Qt3DRender::Render::Sphere(Vector3D(5.f,5.f,5.f),0.f);
        pointSphere.transform(mMat);
        QVERIFY(!pointSphere.isNull());
        QVERIFY(pointSphere.center() == Vector3D(0.,0.,0));
        QVERIFY(pointSphere.radius() == 0.f);
    }

    void remainsNullAfterTransform() {
        QMatrix4x4 mat;
        mat.translate(-5,-5,-5);
        auto mMat = Matrix4x4(mat);
        auto defaultSphere = Qt3DRender::Render::Sphere();
        defaultSphere.transform(mMat);
        QVERIFY(defaultSphere.isNull());
    }

    void expandToContainSphere() {
        auto firstValidSphere = Qt3DRender::Render::Sphere(Vector3D(-10.f,-10.f,-10.f),1.f);
        auto secondValidSphere = Qt3DRender::Render::Sphere(Vector3D(10.f,10.f,10.f),1.f);
        firstValidSphere.expandToContain(secondValidSphere);
        QVERIFY(firstValidSphere.center()==Vector3D(0.f,0.f,0.f));
        float dist = static_cast<float>((2 + sqrt(3.*(20)*(20)))/2.);
        QVERIFY(qFuzzyCompare(firstValidSphere.radius(), dist));
    }

    void expandToContainSphereOneInvalid() {
        auto firstValidSphere = Qt3DRender::Render::Sphere(Vector3D(-10.f,-10.f,-10.f),1.f);
        auto defaultSphere = Qt3DRender::Render::Sphere();
        auto copiedSphere = firstValidSphere;
        firstValidSphere.expandToContain(defaultSphere);
        QVERIFY(firstValidSphere.center() == copiedSphere.center());
        QCOMPARE(firstValidSphere.radius(), copiedSphere.radius());
        QVERIFY(!firstValidSphere.isNull());
    }

    void expandToContainOtherSphereInvalid() {
        auto firstValidSphere = Qt3DRender::Render::Sphere(Vector3D(-10.f,-10.f,-10.f),1.f);
        auto defaultSphere = Qt3DRender::Render::Sphere();
        defaultSphere.expandToContain(firstValidSphere);
        QVERIFY(defaultSphere.center() == firstValidSphere.center());
        QCOMPARE(defaultSphere.radius(), firstValidSphere.radius());
        QVERIFY(!defaultSphere.isNull());
    }

    void expandNullSphereWithNullSphere() {
        auto defaultSphere = Qt3DRender::Render::Sphere();
        auto otherDefaultSphere = Qt3DRender::Render::Sphere();
        defaultSphere.expandToContain(otherDefaultSphere);
        QVERIFY(defaultSphere.isNull());
    }

    void expandToContainPoint() {
        auto firstValidSphere = Qt3DRender::Render::Sphere(Vector3D(-10.f,-10.f,-10.f),1.f);
        firstValidSphere.expandToContain(Vector3D(0,0,0));
        QVERIFY(!firstValidSphere.isNull());
        float expectedRadius = static_cast<float>((1 + qSqrt(3.*(10)*(10)))/2.);
        QVERIFY(qFuzzyCompare(firstValidSphere.radius(), expectedRadius));
    }

    void nullSphereExpandToContainPoint() {
        auto defaultSphere = Qt3DRender::Render::Sphere();
        defaultSphere.expandToContain(Vector3D(5,5,5));
        QVERIFY(!defaultSphere.isNull());
        QVERIFY(defaultSphere.center() == Vector3D(5,5,5));
        QVERIFY(qFuzzyIsNull(defaultSphere.radius()));
    }

    void nullSphereExpandToOrigin() {
        auto defaultSphere = Qt3DRender::Render::Sphere();
        defaultSphere.expandToContain(Vector3D(0,0,0));
        QVERIFY(!defaultSphere.isNull());
        QVERIFY(defaultSphere.center() == Vector3D(0,0,0));
        QVERIFY(qFuzzyIsNull(defaultSphere.radius()));
    }

    void ritterSphereCubePoints() {
        QList<Vector3D> cubePts={
            Vector3D(-0.5, -0.5,  0.5),
            Vector3D( 0.5, -0.5, -0.5),
            Vector3D(-0.5,  0.5, -0.5),
            Vector3D( 0.5,  0.5, -0.5),
            Vector3D(-0.5, -0.5, -0.5),
            Vector3D( 0.5, -0.5,  0.5),
            Vector3D(-0.5,  0.5,  0.5),
            Vector3D( 0.5,  0.5,  0.5)
        };
        auto ritterSphere=Qt3DRender::Render::Sphere::fromPoints(cubePts);
        QVERIFY(!ritterSphere.isNull());
        QVERIFY(qFuzzyIsNull(ritterSphere.center().x()));
        QVERIFY(qFuzzyIsNull(ritterSphere.center().y()));
        QVERIFY(qFuzzyIsNull(ritterSphere.center().z()));
        QVERIFY(qFuzzyCompare(ritterSphere.radius(), static_cast<float>(qSqrt(3)/2)));
    }

    void ritterSphereRandomPoints() {
        QList<Vector3D> randomPts={
            Vector3D(-81, 55, 46),
            Vector3D(-91, -73, -42),
            Vector3D(-50, -76, -77),
            Vector3D(-40, 63, 58),
            Vector3D(-28, -2, -57),
            Vector3D(84, 17, 33),
            Vector3D(53, 11, -49),
            Vector3D(-7, -24, -86),
            Vector3D(-89, 6, 76),
            Vector3D(46, -18, -27)
        };

        auto ritterSphere = Qt3DRender::Render::Sphere::fromPoints(randomPts);
        QVERIFY(!ritterSphere.isNull());
        QVERIFY(qFuzzyCompare(ritterSphere.center().x(), 17.f));
        QVERIFY(qFuzzyCompare(ritterSphere.center().y(), -29.5f));
        QVERIFY(qFuzzyCompare(ritterSphere.center().z(), -22.0f));
        QVERIFY(qFuzzyCompare(ritterSphere.radius(), 148.66152831179963f));
    }

    void ritterSphereOnePoint() {
        QList<Vector3D> singlePt={
            Vector3D(-0.5, -0.5, -0.5),
        };
        auto ritterSphere = Qt3DRender::Render::Sphere::fromPoints(singlePt);
        QVERIFY(!ritterSphere.isNull());
        QVERIFY(qFuzzyCompare(ritterSphere.center().x(), -0.5f));
        QVERIFY(qFuzzyCompare(ritterSphere.center().y(), -0.5f));
        QVERIFY(qFuzzyCompare(ritterSphere.center().z(), -0.5f));
        QVERIFY(qFuzzyIsNull(ritterSphere.radius()));
    }

    void checkExtraGeometries_data()
    {
        QTest::addColumn<QString>("qmlFile");
        QTest::addColumn<QVector3D>("sphereCenter");
        QTest::addColumn<float>("sphereRadius");
        QTest::newRow("SphereMesh") << "qrc:/sphere.qml" << QVector3D(0.f, 0.f, 0.f) << 1.f;
        QTest::newRow("CubeMesh") << "qrc:/cube.qml" << QVector3D(0.f, 0.f, 0.f) << static_cast<float>(qSqrt(3.)/2.); // not weird at all
    }

    void checkExtraGeometries()
    {
        // GIVEN
        QFETCH(QString, qmlFile);
        QFETCH(QVector3D, sphereCenter);
        QFETCH(float, sphereRadius);

        QUrl qmlUrl(qmlFile);
        QmlSceneReader sceneReader(qmlUrl);
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Runs Required jobs
        test->registerTree(qobject_cast<Qt3DCore::QEntity *>(root.data()));
        runRequiredJobs(test.data());

        // THEN
        QVERIFY(test->sceneRoot()->worldBoundingVolumeWithChildren());
        const auto boundingSphere = test->sceneRoot()->worldBoundingVolumeWithChildren();
        qDebug() << qmlFile << boundingSphere->radius() << boundingSphere->center();
        QCOMPARE(boundingSphere->radius(), sphereRadius);

        QVERIFY(qFuzzyIsNull(boundingSphere->center().x() - sphereCenter.x()));
        QVERIFY(qFuzzyIsNull(boundingSphere->center().y() - sphereCenter.y()));
        QVERIFY(qFuzzyIsNull(boundingSphere->center().z() - sphereCenter.z()));
    }

    void checkCustomGeometry_data()
    {
        QTest::addColumn<int>("drawVertexCount");
        QTest::addColumn<int>("indexByteOffset");
        QTest::addColumn<QVector3D>("expectedCenter");
        QTest::addColumn<float>("expectedRadius");
        QTest::addColumn<bool>("withPrimitiveRestart");
        QTest::newRow("all") << 0 << 0 << QVector3D(0.0f, 0.0f, -75.0f) << 25.03997f << false;
        QTest::newRow("first only") << 3 << 0 << QVector3D(0, 1, -100) << 1.0f << false;
        QTest::newRow("second only") << 3 << int(3 * sizeof(ushort)) << QVector3D(0, -1, -50) << 1.0f << false;
        QTest::newRow("all with primitive restart") << 0 << 0 << QVector3D(0.0f, 0.0f, -75.0f) << 25.03997f << true;
        QTest::newRow("first only with primitive restart") << 4 << 0 << QVector3D(0, 1, -100) << 1.0f << true;
        QTest::newRow("second only with primitive restart") << 4 << int(3 * sizeof(ushort)) << QVector3D(0, -1, -50) << 1.0f << true;
    }

    void checkCustomGeometry()
    {
        QFETCH(int, drawVertexCount);
        QFETCH(int, indexByteOffset);
        QFETCH(QVector3D, expectedCenter);
        QFETCH(float, expectedRadius);
        QFETCH(bool, withPrimitiveRestart);

        // two triangles with different Z, and an index buffer
        QByteArray vdata;
        vdata.resize(6 * 3 * sizeof(float));
        float *vp = reinterpret_cast<float *>(vdata.data());
        *vp++ = -1.0f;
        *vp++ = 1.0f;
        *vp++ = -100.0f;
        *vp++ = 0.0f;
        *vp++ = 0.0f;
        *vp++ = -100.0f;
        *vp++ = 1.0f;
        *vp++ = 1.0f;
        *vp++ = -100.0f;

        *vp++ = -1.0f;
        *vp++ = -1.0f;
        *vp++ = -50.0f;
        *vp++ = 0.0f;
        *vp++ = 0.0f;
        *vp++ = -50.0f;
        *vp++ = 1.0f;
        *vp++ = -1.0f;
        *vp++ = -50.0f;

        QByteArray idata;
        const int indexCount = withPrimitiveRestart ? 7 : 6;
        idata.resize(indexCount * sizeof(ushort));
        ushort *ip = reinterpret_cast<ushort *>(idata.data());
        *ip++ = 0;
        *ip++ = 1;
        *ip++ = 2;
        if (withPrimitiveRestart)
            *ip++ = 65535;
        *ip++ = 3;
        *ip++ = 4;
        *ip++ = 5;

        QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(entity.data()));
        Qt3DCore::QBuffer *vbuffer = new Qt3DCore::QBuffer;
        Qt3DCore::QBuffer *ibuffer = new Qt3DCore::QBuffer;

        vbuffer->setData(vdata);
        Qt3DRender::Render::Buffer *vbufferBackend = test->nodeManagers()->bufferManager()->getOrCreateResource(vbuffer->id());
        vbufferBackend->setRenderer(test->renderer());
        vbufferBackend->setManager(test->nodeManagers()->bufferManager());
        simulateInitializationSync(vbuffer, vbufferBackend);

        ibuffer->setData(idata);
        Qt3DRender::Render::Buffer *ibufferBackend = test->nodeManagers()->bufferManager()->getOrCreateResource(ibuffer->id());
        ibufferBackend->setRenderer(test->renderer());
        ibufferBackend->setManager(test->nodeManagers()->bufferManager());
        simulateInitializationSync(ibuffer, ibufferBackend);

        Qt3DCore::QGeometry *g = new Qt3DCore::QGeometry;
        for (int i = 0; i < 2; ++i)
            g->addAttribute(new Qt3DCore::QAttribute);

        const QList<Qt3DCore::QAttribute *> attrs = g->attributes();
        Qt3DCore::QAttribute *attr = attrs[0];
        attr->setBuffer(vbuffer);
        attr->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        attr->setVertexBaseType(Qt3DCore::QAttribute::Float);
        attr->setVertexSize(3);
        attr->setCount(6);
        attr->setByteOffset(0);
        attr->setByteStride(3 * sizeof(float));

        attr = attrs[1];
        attr->setBuffer(ibuffer);
        attr->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
        attr->setVertexBaseType(Qt3DCore::QAttribute::UnsignedShort);
        attr->setVertexSize(1);
        attr->setCount(indexCount);
        attr->setByteOffset(indexByteOffset);

        Qt3DRender::QGeometryRenderer *gr = new Qt3DRender::QGeometryRenderer;
        gr->setVertexCount(drawVertexCount); // when 0, indexAttribute->count() is used instead
        gr->setPrimitiveRestartEnabled(withPrimitiveRestart);
        if (withPrimitiveRestart)
            gr->setRestartIndexValue(65535);
        gr->setGeometry(g);
        entity->addComponent(gr);

        Qt3DRender::Render::Attribute *attr0Backend = test->nodeManagers()->attributeManager()->getOrCreateResource(attrs[0]->id());
        attr0Backend->setRenderer(test->renderer());
        simulateInitializationSync(attrs[0], attr0Backend);
        Qt3DRender::Render::Attribute *attr1Backend = test->nodeManagers()->attributeManager()->getOrCreateResource(attrs[1]->id());
        attr1Backend->setRenderer(test->renderer());
        simulateInitializationSync(attrs[1], attr1Backend);

        Qt3DRender::Render::Geometry *gBackend = test->nodeManagers()->geometryManager()->getOrCreateResource(g->id());
        gBackend->setRenderer(test->renderer());
        simulateInitializationSync(g, gBackend);

        Qt3DRender::Render::GeometryRenderer *grBackend = test->nodeManagers()->geometryRendererManager()->getOrCreateResource(gr->id());
        grBackend->setRenderer(test->renderer());
        grBackend->setManager(test->nodeManagers()->geometryRendererManager());
        simulateInitializationSync(gr, grBackend);

        Qt3DRender::Render::Entity *entityBackend = test->nodeManagers()->renderNodesManager()->getOrCreateResource(entity->id());
        entityBackend->setRenderer(test->renderer());
        simulateInitializationSync(entity.data(), entityBackend);

        test->registerTree(entity.data());

        Qt3DCore::CalculateBoundingVolumeJob calcCBVolume(nullptr);
        calcCBVolume.setRoot(test->root());
        calcCBVolume.run();
        calcCBVolume.postFrame(nullptr);

        Qt3DRender::Render::CalculateBoundingVolumeJob calcRBVolume;
        calcRBVolume.setManagers(test->nodeManagers());
        calcRBVolume.setRoot(test->sceneRoot());
        calcRBVolume.setFrontEndNodeManager(test.data());
        calcRBVolume.run();

        Vector3D center = entityBackend->localBoundingVolume()->center();
        float radius = entityBackend->localBoundingVolume()->radius();
        qDebug() << radius << center;

        QCOMPARE(radius, expectedRadius);
        QCOMPARE(center.x(), expectedCenter.x());
        QCOMPARE(center.y(), expectedCenter.y());
        QCOMPARE(center.z(), expectedCenter.z());
    }

    void checkCustomPackedGeometry()
    {
        int drawVertexCount = 6;
        QVector3D expectedCenter(0.0f, 0.0f, -75.0f);
        float expectedRadius = 25.03997f;

        // two triangles with different Z
        QByteArray vdata;
        vdata.resize(6 * 3 * sizeof(float));
        float *vp = reinterpret_cast<float *>(vdata.data());
        *vp++ = -1.0f;
        *vp++ = 1.0f;
        *vp++ = -100.0f;
        *vp++ = 0.0f;
        *vp++ = 0.0f;
        *vp++ = -100.0f;
        *vp++ = 1.0f;
        *vp++ = 1.0f;
        *vp++ = -100.0f;

        *vp++ = -1.0f;
        *vp++ = -1.0f;
        *vp++ = -50.0f;
        *vp++ = 0.0f;
        *vp++ = 0.0f;
        *vp++ = -50.0f;
        *vp++ = 1.0f;
        *vp++ = -1.0f;
        *vp++ = -50.0f;

        QScopedPointer<Qt3DCore::QEntity> entity(new Qt3DCore::QEntity);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(entity.data()));
        Qt3DCore::QBuffer *vbuffer = new Qt3DCore::QBuffer;

        vbuffer->setData(vdata);
        Qt3DRender::Render::Buffer *vbufferBackend = test->nodeManagers()->bufferManager()->getOrCreateResource(vbuffer->id());
        vbufferBackend->setRenderer(test->renderer());
        vbufferBackend->setManager(test->nodeManagers()->bufferManager());
        simulateInitializationSync(vbuffer, vbufferBackend);

        Qt3DCore::QGeometry *g = new Qt3DCore::QGeometry;
        g->addAttribute(new Qt3DCore::QAttribute);

        const QList<Qt3DCore::QAttribute *> attrs = g->attributes();
        Qt3DCore::QAttribute *attr = attrs[0];
        attr->setBuffer(vbuffer);
        attr->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        attr->setVertexBaseType(Qt3DCore::QAttribute::Float);
        attr->setVertexSize(3);
        attr->setCount(6);
        attr->setByteOffset(0);
        attr->setByteStride(0);

        Qt3DRender::QGeometryRenderer *gr = new Qt3DRender::QGeometryRenderer;
        gr->setVertexCount(drawVertexCount);
        gr->setGeometry(g);
        entity->addComponent(gr);

        Qt3DRender::Render::Attribute *attr0Backend = test->nodeManagers()->attributeManager()->getOrCreateResource(attrs[0]->id());
        attr0Backend->setRenderer(test->renderer());
        simulateInitializationSync(attrs[0], attr0Backend);

        Qt3DRender::Render::Geometry *gBackend = test->nodeManagers()->geometryManager()->getOrCreateResource(g->id());
        gBackend->setRenderer(test->renderer());
        simulateInitializationSync(g, gBackend);

        Qt3DRender::Render::GeometryRenderer *grBackend = test->nodeManagers()->geometryRendererManager()->getOrCreateResource(gr->id());
        grBackend->setRenderer(test->renderer());
        grBackend->setManager(test->nodeManagers()->geometryRendererManager());
        simulateInitializationSync(gr, grBackend);

        Qt3DRender::Render::Entity *entityBackend = test->nodeManagers()->renderNodesManager()->getOrCreateResource(entity->id());
        entityBackend->setRenderer(test->renderer());
        simulateInitializationSync(entity.data(), entityBackend);

        test->registerTree(entity.data());

        Qt3DRender::Render::CalculateBoundingVolumeJob calcBVolume;
        calcBVolume.setFrontEndNodeManager(test.data());
        calcBVolume.setManagers(test->nodeManagers());
        calcBVolume.setRoot(test->sceneRoot());
        calcBVolume.run();

        Vector3D center = entityBackend->localBoundingVolume()->center();
        float radius = entityBackend->localBoundingVolume()->radius();
        qDebug() << radius << center;

        QCOMPARE(radius, expectedRadius);
        QCOMPARE(center.x(), expectedCenter.x());
        QCOMPARE(center.y(), expectedCenter.y());
        QCOMPARE(center.z(), expectedCenter.z());
    }
};

QTEST_MAIN(tst_BoundingSphere)

#include "tst_boundingsphere.moc"
