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

#include <QUrl>

#include <QtTest/QTest>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
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
#include <Qt3DRender/private/updatemeshtrianglelistjob_p.h>
#include <Qt3DRender/private/updateworldboundingvolumejob_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>
#include <Qt3DRender/private/expandboundingvolumejob_p.h>
#include <Qt3DRender/private/calcboundingvolumejob_p.h>
#include <Qt3DRender/private/calcgeometrytrianglevolumes_p.h>
#include <Qt3DRender/private/loadbufferjob_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/sphere_p.h>

#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DExtras/qcylindermesh.h>
#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DExtras/qplanemesh.h>

#include <qbackendnodetester.h>

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
        QRenderAspect::onRegistered();

        const QVector<Qt3DCore::QNode *> nodes = getNodesForCreation(root);
        const QVector<Qt3DCore::NodeTreeChange> nodeTreeChanges = nodeTreeChangesForNodes(nodes);
        d_func()->setRootAndCreateNodes(qobject_cast<Qt3DCore::QEntity *>(root), nodeTreeChanges);

        Render::Entity *rootEntity = nodeManagers()->lookupResource<Render::Entity, Render::EntityManager>(rootEntityId());
        Q_ASSERT(rootEntity);
        m_sceneRoot = rootEntity;
    }

    ~TestAspect()
    {
        QRenderAspect::onUnregistered();
    }

    void onRegistered() { QRenderAspect::onRegistered(); }
    void onUnregistered() { QRenderAspect::onUnregistered(); }

    Qt3DRender::Render::NodeManagers *nodeManagers() const { return d_func()->m_renderer->nodeManagers(); }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const { return d_func()->m_renderer->frameGraphRoot(); }
    Qt3DRender::Render::RenderSettings *renderSettings() const { return d_func()->m_renderer->settings(); }
    Qt3DRender::Render::Entity *sceneRoot() const { return m_sceneRoot; }
    Qt3DRender::Render::AbstractRenderer *renderer() const { return d_func()->m_renderer; }

private:
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
        QVERIFY(firstValidSphere.radius() == copiedSphere.radius());
        QVERIFY(!firstValidSphere.isNull());
    }

    void expandToContainOtherSphereInvalid() {
        auto firstValidSphere = Qt3DRender::Render::Sphere(Vector3D(-10.f,-10.f,-10.f),1.f);
        auto defaultSphere = Qt3DRender::Render::Sphere();
        defaultSphere.expandToContain(firstValidSphere);
        QVERIFY(defaultSphere.center() == firstValidSphere.center());
        QVERIFY(defaultSphere.radius() == firstValidSphere.radius());
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
        QVector<Vector3D> cubePts={
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
        QVector<Vector3D> randomPts={
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
        QVector<Vector3D> singlePt={
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
        Qt3DRender::QBuffer *vbuffer = new Qt3DRender::QBuffer;
        Qt3DRender::QBuffer *ibuffer = new Qt3DRender::QBuffer;

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

        Qt3DRender::QGeometry *g = new Qt3DRender::QGeometry;
        for (int i = 0; i < 2; ++i)
            g->addAttribute(new Qt3DRender::QAttribute);

        const QVector<Qt3DRender::QAttribute *> attrs = g->attributes();
        Qt3DRender::QAttribute *attr = attrs[0];
        attr->setBuffer(vbuffer);
        attr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        attr->setVertexBaseType(Qt3DRender::QAttribute::Float);
        attr->setVertexSize(3);
        attr->setCount(6);
        attr->setByteOffset(0);
        attr->setByteStride(3 * sizeof(float));

        attr = attrs[1];
        attr->setBuffer(ibuffer);
        attr->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        attr->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
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

        Qt3DRender::Render::CalculateBoundingVolumeJob calcBVolume;
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
        Qt3DRender::QBuffer *vbuffer = new Qt3DRender::QBuffer;

        vbuffer->setData(vdata);
        Qt3DRender::Render::Buffer *vbufferBackend = test->nodeManagers()->bufferManager()->getOrCreateResource(vbuffer->id());
        vbufferBackend->setRenderer(test->renderer());
        vbufferBackend->setManager(test->nodeManagers()->bufferManager());
        simulateInitializationSync(vbuffer, vbufferBackend);

        Qt3DRender::QGeometry *g = new Qt3DRender::QGeometry;
        g->addAttribute(new Qt3DRender::QAttribute);

        const QVector<Qt3DRender::QAttribute *> attrs = g->attributes();
        Qt3DRender::QAttribute *attr = attrs[0];
        attr->setBuffer(vbuffer);
        attr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        attr->setVertexBaseType(Qt3DRender::QAttribute::Float);
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

        Qt3DRender::Render::CalculateBoundingVolumeJob calcBVolume;
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
