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

#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qbufferfunctor.h>

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

class TestFunctor : public Qt3D::QBufferFunctor
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    QByteArray operator ()() Q_DECL_FINAL
    {
        return QByteArray();
    }

    bool operator ==(const Qt3D::QBufferFunctor &other) const
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
class tst_QBuffer: public Qt3D::QNode
{
    Q_OBJECT
public:
    ~tst_QBuffer()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3D::QBuffer *>("buffer");

        Qt3D::QBuffer *defaultConstructed = new Qt3D::QBuffer();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3D::QBuffer *buffer = new Qt3D::QBuffer(Qt3D::QBuffer::VertexBuffer);
        buffer->setUsage(Qt3D::QBuffer::DynamicRead);
        buffer->setData(QByteArrayLiteral("There's no replacement"));
        buffer->setBufferFunctor(Qt3D::QBufferFunctorPtr(new TestFunctor(883)));
        QTest::newRow("vertex") << buffer;

        Qt3D::QBuffer *indexBuffer = new Qt3D::QBuffer(Qt3D::QBuffer::IndexBuffer);
        indexBuffer->setUsage(Qt3D::QBuffer::StaticCopy);
        indexBuffer->setData(QByteArrayLiteral("For displacement"));
        indexBuffer->setBufferFunctor(Qt3D::QBufferFunctorPtr(new TestFunctor(1340)));
        QTest::newRow("index") << indexBuffer;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3D::QBuffer *, buffer);

        // WHEN
        Qt3D::QBuffer *clone = static_cast<Qt3D::QBuffer *>(QNode::clone(buffer));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(buffer->id(), clone->id());
        QCOMPARE(buffer->data(), clone->data());
        QCOMPARE(buffer->usage(), clone->usage());
        QCOMPARE(buffer->type(), clone->type());
        QCOMPARE(buffer->bufferFunctor(), clone->bufferFunctor());
        if (buffer->bufferFunctor()) {
            QVERIFY(clone->bufferFunctor());
            QVERIFY(*clone->bufferFunctor() == *buffer->bufferFunctor());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3D::QBuffer> buffer(new Qt3D::QBuffer(Qt3D::QBuffer::VertexBuffer));
        TestArbiter arbiter(buffer.data());

        // WHEN
        buffer->setType(Qt3D::QBuffer::IndexBuffer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3D::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "type");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3D::QBuffer::IndexBuffer));

        arbiter.events.clear();

        // WHEN
        buffer->setUsage(Qt3D::QBuffer::DynamicCopy);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "usage");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3D::QBuffer::DynamicCopy));

        arbiter.events.clear();

        // WHEN
        buffer->setData(QByteArrayLiteral("Z28"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "data");
        QCOMPARE(change->value().value<QByteArray>(), QByteArrayLiteral("Z28"));

        arbiter.events.clear();

        // WHEN
        Qt3D::QBufferFunctorPtr functor(new TestFunctor(355));
        buffer->setBufferFunctor(functor);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3D::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "bufferFunctor");
        QCOMPARE(change->value().value<Qt3D::QBufferFunctorPtr>(), functor);

        arbiter.events.clear();
    }

protected:
    Qt3D::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QBuffer)

#include "tst_qbuffer.moc"
