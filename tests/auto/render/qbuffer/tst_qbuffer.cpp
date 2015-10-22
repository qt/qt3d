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

#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferfunctor.h>

#include "testpostmanarbiter.h"

class TestFunctor : public Qt3DRender::QBufferFunctor
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    QByteArray operator ()() Q_DECL_FINAL
    {
        return QByteArray();
    }

    bool operator ==(const Qt3DRender::QBufferFunctor &other) const
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
class tst_QBuffer: public Qt3DCore::QNode
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
        QTest::addColumn<Qt3DRender::QBuffer *>("buffer");

        Qt3DRender::QBuffer *defaultConstructed = new Qt3DRender::QBuffer();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer);
        buffer->setUsage(Qt3DRender::QBuffer::DynamicRead);
        buffer->setData(QByteArrayLiteral("There's no replacement"));
        buffer->setBufferFunctor(Qt3DRender::QBufferFunctorPtr(new TestFunctor(883)));
        QTest::newRow("vertex") << buffer;

        Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer);
        indexBuffer->setUsage(Qt3DRender::QBuffer::StaticCopy);
        indexBuffer->setData(QByteArrayLiteral("For displacement"));
        indexBuffer->setBufferFunctor(Qt3DRender::QBufferFunctorPtr(new TestFunctor(1340)));
        indexBuffer->setSync(true);
        QTest::newRow("index") << indexBuffer;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QBuffer *, buffer);

        // WHEN
        Qt3DRender::QBuffer *clone = static_cast<Qt3DRender::QBuffer *>(QNode::clone(buffer));

        // THEN
        QVERIFY(clone != Q_NULLPTR);

        QCOMPARE(buffer->id(), clone->id());
        QCOMPARE(buffer->data(), clone->data());
        QCOMPARE(buffer->usage(), clone->usage());
        QCOMPARE(buffer->type(), clone->type());
        QCOMPARE(buffer->bufferFunctor(), clone->bufferFunctor());
        QCOMPARE(buffer->isSync(), clone->isSync());
        if (buffer->bufferFunctor()) {
            QVERIFY(clone->bufferFunctor());
            QVERIFY(*clone->bufferFunctor() == *buffer->bufferFunctor());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QBuffer> buffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer));
        TestArbiter arbiter(buffer.data());

        // WHEN
        buffer->setType(Qt3DRender::QBuffer::IndexBuffer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "type");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3DRender::QBuffer::IndexBuffer));

        arbiter.events.clear();

        // WHEN
        buffer->setUsage(Qt3DRender::QBuffer::DynamicCopy);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "usage");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3DRender::QBuffer::DynamicCopy));

        arbiter.events.clear();

        // WHEN
        buffer->setData(QByteArrayLiteral("Z28"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "data");
        QCOMPARE(change->value().value<QByteArray>(), QByteArrayLiteral("Z28"));

        arbiter.events.clear();

        // WHEN
        Qt3DRender::QBufferFunctorPtr functor(new TestFunctor(355));
        buffer->setBufferFunctor(functor);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "bufferFunctor");
        QCOMPARE(change->value().value<Qt3DRender::QBufferFunctorPtr>(), functor);

        arbiter.events.clear();

        // WHEN
        buffer->setSync(true);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "sync");
        QCOMPARE(change->value().toBool(), true);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QBuffer)

#include "tst_qbuffer.moc"
