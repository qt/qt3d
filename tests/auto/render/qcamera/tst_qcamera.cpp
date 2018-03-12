/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
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
#include <QObject>
#include <QSignalSpy>

#include <qbackendnodetester.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/qcamera_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/transform_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>

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
        d_func()->setRootAndCreateNodes(qobject_cast<Qt3DCore::QEntity *>(root), nodeTreeChangesForNodes(nodes));

        Render::Entity *rootEntity = nodeManagers()->lookupResource<Render::Entity, Render::EntityManager>(rootEntityId());
        Q_ASSERT(rootEntity);
        m_sceneRoot = rootEntity;
    }

    ~TestAspect();

    void onRegistered() { QRenderAspect::onRegistered(); }
    void onUnregistered() { QRenderAspect::onUnregistered(); }

    Qt3DRender::Render::NodeManagers *nodeManagers() const { return d_func()->m_renderer->nodeManagers(); }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const { return d_func()->m_renderer->frameGraphRoot(); }
    Qt3DRender::Render::RenderSettings *renderSettings() const { return d_func()->m_renderer->settings(); }
    Qt3DRender::Render::Entity *sceneRoot() const { return m_sceneRoot; }

private:
    Render::Entity *m_sceneRoot;
};

TestAspect::~TestAspect()
{
    QRenderAspect::onUnregistered();
}

} // namespace Qt3DRender

QT_END_NAMESPACE

namespace {

void runRequiredJobs(Qt3DRender::TestAspect *test)
{
    Qt3DRender::Render::UpdateWorldTransformJob updateWorldTransform;
    updateWorldTransform.setRoot(test->sceneRoot());
    updateWorldTransform.setManagers(test->nodeManagers());
    updateWorldTransform.run();
}

void fuzzyCompareMatrix(const QMatrix4x4 &a, const QMatrix4x4 &b)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // Fuzzy comparison. qFuzzyCompare is not suitable because zero is compared to small numbers.
            QVERIFY(qAbs(a(i, j) - b(i, j)) < 1.0e-6f);
        }
    }
}

} // anonymous

class tst_QCamera : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QCamera camera;

        // THEN
        QCOMPARE(camera.projectionType(), Qt3DRender::QCameraLens::PerspectiveProjection);
        QCOMPARE(camera.nearPlane(), 0.1f);
        QCOMPARE(camera.farPlane(), 1024.0f);
        QCOMPARE(camera.fieldOfView(), 25.0f);
        QCOMPARE(camera.aspectRatio(), 1.0f);
        QCOMPARE(camera.left(), -0.5f);
        QCOMPARE(camera.right(), 0.5f);
        QCOMPARE(camera.bottom(), -0.5f);
        QCOMPARE(camera.top(), 0.5f);
        QCOMPARE(camera.exposure(), 0.0f);
    }

    void checkTransformWithParent()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QEntity> root(new Qt3DCore::QEntity);

        QScopedPointer<Qt3DCore::QEntity> parent(new Qt3DCore::QEntity(root.data()));
        Qt3DCore::QTransform *parentTransform = new Qt3DCore::QTransform();
        parentTransform->setTranslation(QVector3D(10, 9, 8));
        parentTransform->setRotationX(10.f);
        parentTransform->setRotationY(20.f);
        parentTransform->setRotationZ(30.f);
        parent->addComponent(parentTransform);

        QScopedPointer<Qt3DRender::QCamera> camera(new Qt3DRender::QCamera(parent.data()));

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        runRequiredJobs(test.data());

        Qt3DRender::Render::Entity *cameraEntity = test->nodeManagers()->lookupResource<Qt3DRender::Render::Entity, Qt3DRender::Render::EntityManager>(camera->id());

        // THEN
        QVERIFY(qFuzzyCompare(convertToQMatrix4x4(*cameraEntity->worldTransform()), convertToQMatrix4x4(parentTransform->matrix())));
    }

    void checkTransformWithParentAndLookAt()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QEntity> root(new Qt3DCore::QEntity);

        QScopedPointer<Qt3DCore::QEntity> parent(new Qt3DCore::QEntity(root.data()));
        Qt3DCore::QTransform *parentTransform = new Qt3DCore::QTransform();
        parentTransform->setRotationZ(90.f);
        parent->addComponent(parentTransform);

        QScopedPointer<Qt3DRender::QCamera> camera(new Qt3DRender::QCamera(parent.data()));
        camera->setPosition(QVector3D(1.f, 0.f, 0.f));
        camera->setViewCenter(QVector3D(1.f, 0.f, -1.f)); // look in -z
        camera->setUpVector(QVector3D(0.f, 1.f, 0.f));

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        runRequiredJobs(test.data());

        Qt3DRender::Render::Entity *cameraEntity = test->nodeManagers()->lookupResource<Qt3DRender::Render::Entity, Qt3DRender::Render::EntityManager>(camera->id());

        // THEN
        QMatrix4x4 m;
        m.translate(0.f, 1.f, 0.f); // 90 deg z-rotation + x-translation = y-translation
        m.rotate(90.f, QVector3D(0.f, 0.f, 1.f));

        const QMatrix4x4 worldTransform = convertToQMatrix4x4(*cameraEntity->worldTransform());
        fuzzyCompareMatrix(worldTransform, m);

    }

    void checkTransformOfChild()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QEntity> root(new Qt3DCore::QEntity);

        QScopedPointer<Qt3DCore::QEntity> parent(new Qt3DCore::QEntity(root.data()));
        Qt3DCore::QTransform *parentTransform = new Qt3DCore::QTransform();
        parentTransform->setTranslation(QVector3D(10, 9, 8));
        parent->addComponent(parentTransform);

        QScopedPointer<Qt3DRender::QCamera> camera(new Qt3DRender::QCamera(parent.data()));
        camera->setPosition(QVector3D(2.f, 3.f, 4.f));
        camera->setViewCenter(QVector3D(1.f, 3.f, 4.f)); // looking in -x = 90 deg y-rotation
        camera->setUpVector(QVector3D(0.f, 1.f, 0.f));

        // Child coordinate system:
        // y = camera up vector = global y
        // z = negative camera look direction = global x
        // x = y cross z = global -z
        QScopedPointer<Qt3DCore::QEntity> child(new Qt3DCore::QEntity(camera.data()));
        Qt3DCore::QTransform *childTransform = new Qt3DCore::QTransform();
        childTransform->setTranslation(QVector3D(1.f, 2.f, 3.f));
        child->addComponent(childTransform);

        QScopedPointer<Qt3DRender::TestAspect> test(new Qt3DRender::TestAspect(root.data()));
        runRequiredJobs(test.data());

        Qt3DRender::Render::Entity *childEntity = test->nodeManagers()->lookupResource<Qt3DRender::Render::Entity, Qt3DRender::Render::EntityManager>(child->id());

        // THEN
        QMatrix4x4 m;
        m.translate(10.f, 9.f, 8.f); // parent's world translation
        m.translate(2.f, 3.f, 4.f); // camera's world translation
        m.translate(3.f, 2.f, -1.f); // child's world translation
        m.rotate(90.f, QVector3D(0.f, 1.f, 0.f)); // camera's rotation

        fuzzyCompareMatrix(convertToQMatrix4x4(*childEntity->worldTransform()), m);
    }
};


QTEST_MAIN(tst_QCamera)

#include "tst_qcamera.moc"
