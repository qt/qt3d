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

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QGeometry: public Qt3D::QNode
{
    Q_OBJECT
public:
    ~tst_QGeometry()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3D::QGeometry *>("geometry");
        QTest::addColumn<int>("attributeCount");

        Qt3D::QGeometry *defaultConstructed = new Qt3D::QGeometry();
        QTest::newRow("defaultConstructed") << defaultConstructed << 0;

        Qt3D::QGeometry *geometry1 = new Qt3D::QGeometry();
        geometry1->setVerticesPerPatch(2);
        geometry1->addAttribute(new Qt3D::QAttribute(Q_NULLPTR, QStringLiteral("Attr1"), Qt3D::QAttribute::Float, 3, 454));
        geometry1->addAttribute(new Qt3D::QAttribute(Q_NULLPTR, QStringLiteral("Attr2"), Qt3D::QAttribute::Float, 4, 555));
        QTest::newRow("2 attributes") << geometry1 << 2;


        Qt3D::QGeometry *geometry2 = new Qt3D::QGeometry();
        Qt3D::QAttribute *attribute = new Qt3D::QAttribute(Q_NULLPTR, QStringLiteral("Attr2"), Qt3D::QAttribute::Float, 4, 383);
        geometry2->addAttribute(new Qt3D::QAttribute(Q_NULLPTR, QStringLiteral("Attr1"), Qt3D::QAttribute::Float, 3, 427));
        geometry2->addAttribute(attribute);
        geometry2->addAttribute(new Qt3D::QAttribute(Q_NULLPTR, QStringLiteral("Attr3"), Qt3D::QAttribute::Float, 2, 327));
        geometry2->removeAttribute(attribute);
        geometry2->setVerticesPerPatch(3);
        QTest::newRow("3 - 1 attributes") << geometry2 << 2;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3D::QGeometry *, geometry);
        QFETCH(int, attributeCount);

        // WHEN
        Qt3D::QGeometry *clone = static_cast<Qt3D::QGeometry *>(QNode::clone(geometry));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(geometry->id(), clone->id());
        QCOMPARE(attributeCount, geometry->attributes().count());
        QCOMPARE(attributeCount, clone->attributes().count());
        QCOMPARE(geometry->verticesPerPatch(), clone->verticesPerPatch());

        for (int i = 0; i < attributeCount; ++i) {
            Qt3D::QAttribute *originalAttribute = static_cast<Qt3D::QAttribute *>(geometry->attributes()[i]);
            Qt3D::QAttribute *cloneAttribute = static_cast<Qt3D::QAttribute *>(clone->attributes()[i]);

            QCOMPARE(originalAttribute->id(), cloneAttribute->id());
            QCOMPARE(originalAttribute->name(), cloneAttribute->name());
            QCOMPARE(originalAttribute->buffer(), cloneAttribute->buffer());
            QCOMPARE(originalAttribute->count(), cloneAttribute->count());
            QCOMPARE(originalAttribute->byteStride(), cloneAttribute->byteStride());
            QCOMPARE(originalAttribute->byteOffset(), cloneAttribute->byteOffset());
            QCOMPARE(originalAttribute->divisor(), cloneAttribute->divisor());
            QCOMPARE(originalAttribute->attributeType(), cloneAttribute->attributeType());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3D::QGeometry> geometry(new Qt3D::QGeometry());
        TestArbiter arbiter(geometry.data());

        // WHEN
        Qt3D::QAttribute attr;
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "attribute");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), attr.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // WHEN
        geometry->addAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        geometry->removeAttribute(&attr);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "attribute");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), attr.id());
        QCOMPARE(change->type(), Qt3D::NodeRemoved);

        arbiter.events.clear();

        // WHEN
        geometry->setVerticesPerPatch(2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "verticesPerPatch");
        QCOMPARE(change->value().toInt(), 2);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3D::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QGeometry)

#include "tst_qgeometry.moc"
