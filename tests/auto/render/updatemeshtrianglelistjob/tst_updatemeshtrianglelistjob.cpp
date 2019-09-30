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
#include <Qt3DRender/private/updatemeshtrianglelistjob_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/loadgeometryjob_p.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include "qmlscenereader.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender { // Needs to be in that namespace to be friend with QRenderAspect

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
        Qt3DRender::QRenderAspect::onRegistered();

        const QVector<Qt3DCore::QNode *> nodes = getNodesForCreation(root);
        d_func()->setRootAndCreateNodes(qobject_cast<Qt3DCore::QEntity *>(root), nodeTreeChangesForNodes(nodes));

        Qt3DRender::Render::Entity *rootEntity = nodeManagers()->lookupResource<Qt3DRender::Render::Entity, Render::EntityManager>(rootEntityId());
        Q_ASSERT(rootEntity);
        m_sceneRoot = rootEntity;
    }

    ~TestAspect()
    {
        QRenderAspect::onUnregistered();
    }

    void onRegistered() { Qt3DRender::QRenderAspect::onRegistered(); }
    void onUnregistered() { Qt3DRender::QRenderAspect::onUnregistered(); }

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

void runRequiredJobs(Qt3DRender::TestAspect *aspect)
{
    Qt3DRender::Render::GeometryRendererManager *geomRendererManager = aspect->nodeManagers()->geometryRendererManager();
    const QVector<Qt3DCore::QNodeId> dirtyGeometryRenderers = geomRendererManager->dirtyGeometryRenderers();
    QVector<Qt3DCore::QAspectJobPtr> dirtyGeometryRendererJobs;
    dirtyGeometryRendererJobs.reserve(dirtyGeometryRenderers.size());

    // Load Geometry
    for (const Qt3DCore::QNodeId geoRendererId : dirtyGeometryRenderers) {
        Qt3DRender::Render::HGeometryRenderer geometryRendererHandle = geomRendererManager->lookupHandle(geoRendererId);
        if (!geometryRendererHandle.isNull()) {
            auto job = Qt3DRender::Render::LoadGeometryJobPtr::create(geometryRendererHandle);
            job->setNodeManagers(aspect->nodeManagers());
            dirtyGeometryRendererJobs.push_back(job);
        }
    }
}

struct NodeCollection
{
    explicit NodeCollection(Qt3DRender::TestAspect *aspect, QObject *frontendRoot)
        : geometryRenderers(frontendRoot->findChildren<Qt3DRender::QGeometryRenderer *>())
        , attributes(frontendRoot->findChildren<Qt3DRender::QAttribute *>())
        , buffers(frontendRoot->findChildren<Qt3DRender::QBuffer *>())
    {
        // THEN
        QCOMPARE(aspect->nodeManagers()->geometryManager()->activeHandles().size(), geometryRenderers.size());
        QCOMPARE(aspect->nodeManagers()->attributeManager()->activeHandles().size(), attributes.size());
        QCOMPARE(aspect->nodeManagers()->bufferManager()->activeHandles().size(), buffers.size());

        for (const Qt3DRender::QGeometryRenderer *g : qAsConst(geometryRenderers)) {
            Qt3DRender::Render::GeometryRenderer *backend = aspect->nodeManagers()->geometryRendererManager()->lookupResource(g->id());
            QVERIFY(backend != nullptr);
            backendGeometryRenderer.push_back(backend);
        }

        for (const Qt3DRender::QAttribute *a : qAsConst(attributes)) {
            Qt3DRender::Render::Attribute *backend = aspect->nodeManagers()->attributeManager()->lookupResource(a->id());
            QVERIFY(backend != nullptr);
            backendAttributes.push_back(backend);
        }

        for (const Qt3DRender::QBuffer *b : qAsConst(buffers)) {
            Qt3DRender::Render::Buffer *backend = aspect->nodeManagers()->bufferManager()->lookupResource(b->id());
            QVERIFY(backend != nullptr);
            backendBuffers.push_back(backend);
        }
    }

    QList<Qt3DRender::QGeometryRenderer *> geometryRenderers;
    QList<Qt3DRender::QAttribute *> attributes;
    QList<Qt3DRender::QBuffer *> buffers;

    QVector<Qt3DRender::Render::GeometryRenderer *> backendGeometryRenderer;
    QVector<Qt3DRender::Render::Attribute *> backendAttributes;
    QVector<Qt3DRender::Render::Buffer *> backendBuffers;
};


} // anonymous

class tst_UpdateMeshTriangleListJob : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::UpdateMeshTriangleListJob backendUpdateMeshTriangleListJob;

        // THEN
        QVERIFY(backendUpdateMeshTriangleListJob.managers() == nullptr);
    }

    void checkInitialize()
    {
        // GIVEN
        Qt3DRender::Render::UpdateMeshTriangleListJob updateMeshTriangleListJob;
        Qt3DRender::Render::NodeManagers managers;

        // WHEN
        updateMeshTriangleListJob.setManagers(&managers);

        // THEN
        QVERIFY(updateMeshTriangleListJob.managers() == &managers);
    }

    void checkRunNoDirtyGeometryRenderNoDirtyAttributesNoDirtyBuffers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/test_scene.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Run required jobs to load geometries and meshes
        runRequiredJobs(test.data());

        // WHEN
        const NodeCollection collection(test.data(), root.data());

        // THEN
        QCOMPARE(collection.geometryRenderers.size(), 1);
        QCOMPARE(collection.attributes.size(), 5);
        QCOMPARE(collection.buffers.size(), 2);

        // WHEN
        for (Qt3DRender::Render::GeometryRenderer *g : collection.backendGeometryRenderer)
            g->unsetDirty();
        for (Qt3DRender::Render::Attribute *a : collection.backendAttributes)
            a->unsetDirty();
        for (Qt3DRender::Render::Buffer *b : collection.backendBuffers)
            b->unsetDirty();

        Qt3DRender::Render::UpdateMeshTriangleListJob backendUpdateMeshTriangleListJob;
        backendUpdateMeshTriangleListJob.setManagers(test->nodeManagers());

        backendUpdateMeshTriangleListJob.run();

        // THEN
        QCOMPARE(test->nodeManagers()->geometryRendererManager()->geometryRenderersRequiringTriangleDataRefresh().size(), 0);
    }

    void checkRunDirtyGeometryRenderNoDirtyAttributesNoDirtyBuffers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/test_scene.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Run required jobs to load geometries and meshes
        runRequiredJobs(test.data());

        // WHEN
        const NodeCollection collection(test.data(), root.data());

        // THEN
        QCOMPARE(collection.geometryRenderers.size(), 1);
        QCOMPARE(collection.attributes.size(), 5);
        QCOMPARE(collection.buffers.size(), 2);

        // WHEN
        for (Qt3DRender::Render::Attribute *a : collection.backendAttributes)
            a->unsetDirty();
        for (Qt3DRender::Render::Buffer *b : collection.backendBuffers)
            b->unsetDirty();

        Qt3DRender::Render::UpdateMeshTriangleListJob backendUpdateMeshTriangleListJob;
        backendUpdateMeshTriangleListJob.setManagers(test->nodeManagers());

        backendUpdateMeshTriangleListJob.run();

        // THEN
        QCOMPARE(test->nodeManagers()->geometryRendererManager()->geometryRenderersRequiringTriangleDataRefresh().size(), 1);
    }

    void checkRunDirtyGeometryRenderDirtyAttributesNoDirtyBuffers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/test_scene.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Run required jobs to load geometries and meshes
        runRequiredJobs(test.data());

        // WHEN
        const NodeCollection collection(test.data(), root.data());

        // THEN
        QCOMPARE(collection.geometryRenderers.size(), 1);
        QCOMPARE(collection.attributes.size(), 5);
        QCOMPARE(collection.buffers.size(), 2);

        // WHEN
        for (Qt3DRender::Render::Buffer *b : collection.backendBuffers)
            b->unsetDirty();

        Qt3DRender::Render::UpdateMeshTriangleListJob backendUpdateMeshTriangleListJob;
        backendUpdateMeshTriangleListJob.setManagers(test->nodeManagers());

        backendUpdateMeshTriangleListJob.run();

        // THEN
        QCOMPARE(test->nodeManagers()->geometryRendererManager()->geometryRenderersRequiringTriangleDataRefresh().size(), 1);
    }

    void checkRunDirtyGeometryRenderDirtyAttributesDirtyBuffers()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/test_scene.qml"));
        QScopedPointer<Qt3DCore::QNode> root(qobject_cast<Qt3DCore::QNode *>(sceneReader.root()));
        QVERIFY(root);
        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));

        // Run required jobs to load geometries and meshes
        runRequiredJobs(test.data());

        // WHEN
        const NodeCollection collection(test.data(), root.data());

        // THEN
        QCOMPARE(collection.geometryRenderers.size(), 1);
        QCOMPARE(collection.attributes.size(), 5);
        QCOMPARE(collection.buffers.size(), 2);

        // WHEN
        for (Qt3DRender::Render::Buffer *b : collection.backendBuffers)
            b->unsetDirty();

        Qt3DRender::Render::UpdateMeshTriangleListJob backendUpdateMeshTriangleListJob;
        backendUpdateMeshTriangleListJob.setManagers(test->nodeManagers());

        backendUpdateMeshTriangleListJob.run();

        // THEN
        QCOMPARE(test->nodeManagers()->geometryRendererManager()->geometryRenderersRequiringTriangleDataRefresh().size(), 1);
    }

};

QTEST_MAIN(tst_UpdateMeshTriangleListJob)

#include "tst_updatemeshtrianglelistjob.moc"
