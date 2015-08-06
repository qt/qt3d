/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qpostman_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/QScenePropertyChange>

#include <Qt3DRenderer/qgeometryrenderer.h>
#include <Qt3DRenderer/qgeometryfunctor.h>
#include <Qt3DRenderer/qgeometry.h>
#include <Qt3DRenderer/qattribute.h>
#include <Qt3DRenderer/qbuffer.h>

class TestArbiter;

class TestPostman : public Qt3D::QAbstractPostman
{
public:
    TestPostman(TestArbiter *arbiter)
        : m_arbiter(arbiter)
    {}

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &) Q_DECL_FINAL
    {}

    void setScene(Qt3D::QScene *) Q_DECL_FINAL
    {}

    void notifyBackend(const Qt3D::QSceneChangePtr &e) Q_DECL_FINAL;

private:
    TestArbiter *m_arbiter;
};

class TestArbiter : public Qt3D::QAbstractArbiter
{
public:
    TestArbiter(Qt3D::QNode *node)
        : m_postman(new TestPostman(this))
        , m_node(node)
    {
        assignArbiter(m_node);
    }

    ~TestArbiter()
    {
        Qt3D::QNodePrivate::get(m_node)->setArbiter(Q_NULLPTR);
    }

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e) Q_DECL_FINAL
    {
        events.push_back(e);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangePtr &e) Q_DECL_FINAL
    {
        events.push_back(e);
    }

    void sceneChangeEventWithLock(const Qt3D::QSceneChangeList &e) Q_DECL_FINAL
    {
        events += QVector<Qt3D::QSceneChangePtr>::fromStdVector(e);
    }

    Qt3D::QAbstractPostman *postman() const Q_DECL_FINAL
    {
        return m_postman;
    }

    QVector<Qt3D::QSceneChangePtr> events;

private:
    TestPostman *m_postman;
    Qt3D::QNode *m_node;

    void assignArbiter(Qt3D::QNode *node)
    {
        Qt3D::QNodePrivate::get(node)->setArbiter(this);
        Q_FOREACH (Qt3D::QNode *n, node->childrenNodes())
            assignArbiter(n);
    }
};

void TestPostman::notifyBackend(const Qt3D::QSceneChangePtr &e)
{
    m_arbiter->sceneChangeEventWithLock(e);
}

class TestFunctor : public Qt3D::QGeometryFunctor
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    Qt3D::QGeometry *operator ()() Q_DECL_FINAL
    {
        return Q_NULLPTR;
    }

    bool operator ==(const Qt3D::QGeometryFunctor &other) const
    {
        const TestFunctor *otherFunctor = functor_cast<TestFunctor>(&other);
        if (otherFunctor != Q_NULLPTR)
            return otherFunctor->m_size == m_size;
        return false;
    }

    QT3D_FUNCTOR(TestFunctor)

private:
    int m_size;
};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QGeometryRenderer: public Qt3D::QNode
{
    Q_OBJECT
public:
    ~tst_QGeometryRenderer()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3D::QGeometryRenderer *>("geometryRenderer");

        Qt3D::QGeometryRenderer *defaultConstructed = new Qt3D::QGeometryRenderer();
        QTest::newRow("defaultConstructed") << defaultConstructed ;

        Qt3D::QGeometryRenderer *geometry1 = new Qt3D::QGeometryRenderer();
        geometry1->setGeometry(new Qt3D::QGeometry());
        geometry1->setInstanceCount(1);
        geometry1->setBaseVertex(0);
        geometry1->setBaseInstance(55);
        geometry1->setRestartIndex(-1);
        geometry1->setPrimitiveRestart(false);
        geometry1->setPrimitiveType(Qt3D::QGeometryRenderer::Triangles);
        geometry1->setPrimitiveCount(15);
        geometry1->setGeometryFunctor(Qt3D::QGeometryFunctorPtr(new TestFunctor(383)));
        QTest::newRow("triangle") << geometry1;

        Qt3D::QGeometryRenderer *geometry2 = new Qt3D::QGeometryRenderer();
        geometry2->setGeometry(new Qt3D::QGeometry());
        geometry2->setInstanceCount(200);
        geometry2->setBaseVertex(58);
        geometry2->setBaseInstance(10);
        geometry2->setRestartIndex(65535);
        geometry1->setPrimitiveCount(2056);
        geometry2->setPrimitiveRestart(true);
        geometry2->setPrimitiveType(Qt3D::QGeometryRenderer::Lines);
        geometry2->setGeometryFunctor(Qt3D::QGeometryFunctorPtr(new TestFunctor(305)));
        QTest::newRow("lines with restart") << geometry2;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3D::QGeometryRenderer *, geometryRenderer);

        // WHEN
        Qt3D::QGeometryRenderer *clone = static_cast<Qt3D::QGeometryRenderer *>(QNode::clone(geometryRenderer));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(clone->id(), geometryRenderer->id());
        QCOMPARE(clone->instanceCount(), geometryRenderer->instanceCount());
        QCOMPARE(clone->primitiveCount(), geometryRenderer->primitiveCount());
        QCOMPARE(clone->baseVertex(), geometryRenderer->baseVertex());
        QCOMPARE(clone->baseInstance(), geometryRenderer->baseInstance());
        QCOMPARE(clone->restartIndex(), geometryRenderer->restartIndex());
        QCOMPARE(clone->primitiveRestart(), geometryRenderer->primitiveRestart());
        QCOMPARE(clone->primitiveType(), geometryRenderer->primitiveType());

        if (geometryRenderer->geometry() != Q_NULLPTR) {
            QVERIFY(clone->geometry() != Q_NULLPTR);
            QCOMPARE(clone->geometry()->id(), geometryRenderer->geometry()->id());
        }

        QCOMPARE(clone->geometryFunctor(), geometryRenderer->geometryFunctor());
        if (geometryRenderer->geometryFunctor()) {
            QVERIFY(clone->geometryFunctor());
            QVERIFY(*clone->geometryFunctor() == *geometryRenderer->geometryFunctor());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3D::QGeometryRenderer> geometryRenderer(new Qt3D::QGeometryRenderer());
        TestArbiter arbiter(geometryRenderer.data());

        // WHEN
        geometryRenderer->setInstanceCount(256);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "instanceCount");
        QCOMPARE(change->value().value<int>(), 256);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometryRenderer->setPrimitiveCount(1340);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "primitiveCount");
        QCOMPARE(change->value().value<int>(), 1340);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometryRenderer->setBaseVertex(883);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "baseVertex");
        QCOMPARE(change->value().value<int>(), 883);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometryRenderer->setBaseInstance(1200);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "baseInstance");
        QCOMPARE(change->value().value<int>(), 1200);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometryRenderer->setRestartIndex(65535);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "restartIndex");
        QCOMPARE(change->value().value<int>(), 65535);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometryRenderer->setPrimitiveRestart(true);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "primitiveRestart");
        QCOMPARE(change->value().value<bool>(), true);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        geometryRenderer->setPrimitiveType(Qt3D::QGeometryRenderer::Patches);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "primitiveType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3D::QGeometryRenderer::Patches));
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3D::QGeometryFunctorPtr functor(new TestFunctor(555));
        geometryRenderer->setGeometryFunctor(functor);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "geometryFunctor");
        QCOMPARE(change->value().value<Qt3D::QGeometryFunctorPtr>(), functor);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3D::QGeometry geom;
        geometryRenderer->setGeometry(&geom);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "geometry");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), geom.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // WHEN
        Qt3D::QGeometry geom2;
        geometryRenderer->setGeometry(&geom2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 2);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "geometry");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), geom.id());
        QCOMPARE(change->type(), Qt3D::NodeRemoved);

        change = arbiter.events.last().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "geometry");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), geom2.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();
    }

protected:
    Qt3D::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QGeometryRenderer)

#include "tst_qgeometryrenderer.moc"
