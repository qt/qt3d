/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/private/qattribute_p.h>
#include <Qt3DRender/QBuffer>

#include "testpostmanarbiter.h"

class tst_QAttribute: public QObject
{
    Q_OBJECT
public:
    tst_QAttribute()
    {
        qRegisterMetaType<Qt3DRender::QBuffer*>("Qt3DCore::QBuffer*");
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
        customVertex->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
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
        customIndex->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
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
        Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(attribute);
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges = creationChangeGenerator.creationChanges();

        // THEN
        QCOMPARE(creationChanges.size(), 1 + (attribute->buffer() ? 1 : 0));

        const Qt3DCore::QNodeCreatedChangePtr<Qt3DRender::QAttributeData> creationChangeData =
                qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QAttributeData>>(creationChanges.first());
        const Qt3DRender::QAttributeData &cloneData = creationChangeData->data;

        QCOMPARE(attribute->id(), creationChangeData->subjectId());
        QCOMPARE(attribute->isEnabled(), creationChangeData->isNodeEnabled());
        QCOMPARE(attribute->metaObject(), creationChangeData->metaObject());
        QCOMPARE(attribute->name(), cloneData.name);
        QCOMPARE(attribute->count(), cloneData.count);
        QCOMPARE(attribute->byteStride(), cloneData.byteStride);
        QCOMPARE(attribute->byteOffset(), cloneData.byteOffset);
        QCOMPARE(attribute->divisor(), cloneData.divisor);
        QCOMPARE(attribute->vertexBaseType(), cloneData.dataType);
        QCOMPARE(attribute->vertexSize(), cloneData.dataSize);
        QVERIFY(attribute->attributeType() == cloneData.attributeType);
        QCOMPARE(attribute->buffer() ? attribute->buffer()->id() : Qt3DCore::QNodeId(), cloneData.bufferId);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QAttribute> attribute(new Qt3DRender::QAttribute());
        arbiter.setArbiterOnNode(attribute.data());

        // WHEN
        attribute->setDataType(Qt3DRender::QAttribute::Double);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QPropertyUpdatedChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "vertexBaseType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3DRender::QAttribute::Double));
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setDataSize(4);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "vertexSize");
        QCOMPARE(change->value().value<uint>(), 4U);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setName(QStringLiteral("Duntov"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "name");
        QCOMPARE(change->value().value<QString>(), QStringLiteral("Duntov"));
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setCount(883);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "count");
        QCOMPARE(change->value().value<uint>(), 883U);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setByteStride(1340);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "byteStride");
        QCOMPARE(change->value().value<uint>(), 1340U);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setByteOffset(1584);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "byteOffset");
        QCOMPARE(change->value().value<uint>(), 1584U);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setDivisor(1450);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "divisor");
        QCOMPARE(change->value().value<uint>(), 1450U);
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        attribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "attributeType");
        QCOMPARE(change->value().value<int>(), static_cast<int>(Qt3DRender::QAttribute::IndexAttribute));
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3DRender::QBuffer buf;
        attribute->setBuffer(&buf);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), buf.id());
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3DRender::QBuffer buf2;
        attribute->setBuffer(&buf2);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "buffer");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), buf2.id());
        QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

    }
};

QTEST_MAIN(tst_QAttribute)

#include "tst_qattribute.moc"
