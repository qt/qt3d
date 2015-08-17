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

#include <Qt3DRenderer/QAttribute>
#include <Qt3DRenderer/QBuffer>

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
class tst_QAttribute: public Qt3D::QNode
{
    Q_OBJECT
public:
    ~tst_QAttribute()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3D::QAttribute *>("attribute");

        Qt3D::QAttribute *defaultConstructed = new Qt3D::QAttribute();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3D::QAttribute *customVertex = new Qt3D::QAttribute();
        Qt3D::QBuffer *buffer = new Qt3D::QBuffer(Qt3D::QBuffer::VertexBuffer);
        customVertex->setBuffer(buffer);
        customVertex->setAttributeType(Qt3D::QAbstractAttribute::VertexAttribute);
        customVertex->setCount(454);
        customVertex->setByteStride(427);
        customVertex->setByteOffset(305);
        customVertex->setDivisor(235);
        customVertex->setName("BB");
        customVertex->setDataType(Qt3D::QAttribute::Float);
        customVertex->setDataSize(4);
        QTest::newRow("vertex") << customVertex;

        Qt3D::QAttribute *customIndex = new Qt3D::QAttribute();
        Qt3D::QBuffer *indexBuffer = new Qt3D::QBuffer(Qt3D::QBuffer::IndexBuffer);
        customIndex->setBuffer(indexBuffer);
        customIndex->setAttributeType(Qt3D::QAbstractAttribute::IndexAttribute);
        customIndex->setCount(383);
        customIndex->setByteStride(350);
        customIndex->setByteOffset(327);
        customIndex->setDivisor(355);
        customIndex->setName("SB");
        customIndex->setDataType(Qt3D::QAttribute::Float);
        customIndex->setDataSize(3);
        QTest::newRow("index") << customIndex;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3D::QAttribute *, attribute);

        // WHEN
        Qt3D::QAttribute *clone = static_cast<Qt3D::QAttribute *>(QNode::clone(attribute));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(attribute->id(), clone->id());
        QCOMPARE(attribute->name(), clone->name());
        QCOMPARE(attribute->count(), clone->count());
        QCOMPARE(attribute->byteStride(), clone->byteStride());
        QCOMPARE(attribute->byteOffset(), clone->byteOffset());
        QCOMPARE(attribute->divisor(), clone->divisor());
        QCOMPARE(attribute->dataType(), clone->dataType());
        QCOMPARE(attribute->dataSize(), clone->dataSize());
        QVERIFY(attribute->attributeType() == clone->attributeType());

        if (attribute->buffer() != Q_NULLPTR) {
            QVERIFY(clone->buffer() != Q_NULLPTR);
            QVERIFY(attribute->buffer()->id() == clone->buffer()->id());
            QVERIFY(attribute->buffer()->type() == clone->buffer()->type());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3D::QAttribute> attribute(new Qt3D::QAttribute());
        TestArbiter arbiter(attribute.data());

        // WHEN
        attribute->setDataType(Qt3D::QAttribute::Double);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "dataType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3D::QAttribute::Double));
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setDataSize(4);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "dataSize");
        QCOMPARE(change->value().value<uint>(), 4U);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setName(QStringLiteral("Duntov"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "name");
        QCOMPARE(change->value().value<QString>(), QStringLiteral("Duntov"));
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setCount(883);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "count");
        QCOMPARE(change->value().value<uint>(), 883U);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setByteStride(1340);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "byteStride");
        QCOMPARE(change->value().value<uint>(), 1340U);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setByteOffset(1584);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "byteOffset");
        QCOMPARE(change->value().value<uint>(), 1584U);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setDivisor(1450);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "divisor");
        QCOMPARE(change->value().value<uint>(), 1450U);
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setAttributeType(Qt3D::QAbstractAttribute::IndexAttribute);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "attributeType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3D::QAbstractAttribute::IndexAttribute));
        QCOMPARE(change->type(), Qt3D::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3D::QBuffer buf;
        attribute->setBuffer(&buf);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), buf.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

        arbiter.events.clear();

        // WHEN
        Qt3D::QBuffer buf2;
        attribute->setBuffer(&buf2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 2);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), buf.id());
        QCOMPARE(change->type(), Qt3D::NodeRemoved);
        change = arbiter.events.last().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3D::QNodeId>(), buf2.id());
        QCOMPARE(change->type(), Qt3D::NodeAdded);

    }

protected:
    Qt3D::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QAttribute)

#include "tst_qattribute.moc"
