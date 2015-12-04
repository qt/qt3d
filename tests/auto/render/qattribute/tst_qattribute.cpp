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

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QAttribute: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QAttribute()
    {
        qRegisterMetaType<Qt3DRender::QAbstractBuffer*>("Qt3DCore::QAbstractBuffer*");
    }

    ~tst_QAttribute()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QAttribute *>("attribute");

        Qt3DRender::QAttribute *defaultConstructed = new Qt3DRender::QAttribute();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DRender::QAttribute *customVertex = new Qt3DRender::QAttribute();
        Qt3DRender::QBuffer *buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer);
        customVertex->setBuffer(buffer);
        customVertex->setAttributeType(Qt3DRender::QAbstractAttribute::VertexAttribute);
        customVertex->setCount(454);
        customVertex->setByteStride(427);
        customVertex->setByteOffset(305);
        customVertex->setDivisor(235);
        customVertex->setName("BB");
        customVertex->setDataType(Qt3DRender::QAttribute::Float);
        customVertex->setDataSize(4);
        QTest::newRow("vertex") << customVertex;

        Qt3DRender::QAttribute *customIndex = new Qt3DRender::QAttribute();
        Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer);
        customIndex->setBuffer(indexBuffer);
        customIndex->setAttributeType(Qt3DRender::QAbstractAttribute::IndexAttribute);
        customIndex->setCount(383);
        customIndex->setByteStride(350);
        customIndex->setByteOffset(327);
        customIndex->setDivisor(355);
        customIndex->setName("SB");
        customIndex->setDataType(Qt3DRender::QAttribute::Float);
        customIndex->setDataSize(3);
        QTest::newRow("index") << customIndex;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QAttribute *, attribute);

        // WHEN
        Qt3DRender::QAttribute *clone = static_cast<Qt3DRender::QAttribute *>(QNode::clone(attribute));
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
        QScopedPointer<Qt3DRender::QAttribute> attribute(new Qt3DRender::QAttribute());
        TestArbiter arbiter(attribute.data());

        // WHEN
        attribute->setDataType(Qt3DRender::QAttribute::Double);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "dataType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3DRender::QAttribute::Double));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setDataSize(4);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "dataSize");
        QCOMPARE(change->value().value<uint>(), 4U);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setName(QStringLiteral("Duntov"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "name");
        QCOMPARE(change->value().value<QString>(), QStringLiteral("Duntov"));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setCount(883);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "count");
        QCOMPARE(change->value().value<uint>(), 883U);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setByteStride(1340);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "byteStride");
        QCOMPARE(change->value().value<uint>(), 1340U);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setByteOffset(1584);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "byteOffset");
        QCOMPARE(change->value().value<uint>(), 1584U);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setDivisor(1450);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "divisor");
        QCOMPARE(change->value().value<uint>(), 1450U);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setAttributeType(Qt3DRender::QAbstractAttribute::IndexAttribute);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "attributeType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3DRender::QAbstractAttribute::IndexAttribute));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3DRender::QBuffer buf;
        attribute->setBuffer(&buf);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), buf.id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3DRender::QBuffer buf2;
        attribute->setBuffer(&buf2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), buf2.id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QAttribute)

#include "tst_qattribute.moc"
