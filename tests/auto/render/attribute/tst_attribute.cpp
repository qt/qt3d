// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DCore/qbuffer.h>
#include "testrenderer.h"

class tst_Attribute : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Attribute renderAttribute;
        renderAttribute.setRenderer(&renderer);

        Qt3DCore::QAttribute attribute;
        attribute.setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
        attribute.setByteOffset(1200);
        attribute.setByteStride(883);
        attribute.setCount(427);
        attribute.setDivisor(305);
        attribute.setName(QStringLiteral("C3"));
        attribute.setVertexBaseType(Qt3DCore::QAttribute::UnsignedShort);
        attribute.setVertexSize(3);

        Qt3DCore::QBuffer buffer;
        buffer.setUsage(Qt3DCore::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("Corvette"));
        attribute.setBuffer(&buffer);

        // WHEN
        simulateInitializationSync(&attribute, &renderAttribute);

        // THEN
        QCOMPARE(renderAttribute.peerId(), attribute.id());
        QCOMPARE(renderAttribute.isDirty(), true);
        QCOMPARE(renderAttribute.vertexBaseType(), attribute.vertexBaseType());
        QCOMPARE(renderAttribute.vertexSize(), attribute.vertexSize());
        QCOMPARE(renderAttribute.attributeType(), attribute.attributeType());
        QCOMPARE(renderAttribute.byteOffset(), attribute.byteOffset());
        QCOMPARE(renderAttribute.byteStride(), attribute.byteStride());
        QCOMPARE(renderAttribute.count(), attribute.count());
        QCOMPARE(renderAttribute.divisor(), attribute.divisor());
        QCOMPARE(renderAttribute.name(), attribute.name());
        QCOMPARE(renderAttribute.bufferId(), buffer.id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Attribute renderAttribute;
        renderAttribute.setRenderer(&renderer);

        // THEN
        QVERIFY(renderAttribute.peerId().isNull());
        QVERIFY(renderAttribute.bufferId().isNull());
        QVERIFY(renderAttribute.name().isEmpty());
        QCOMPARE(renderAttribute.isDirty(), false);
        QCOMPARE(renderAttribute.vertexBaseType(), Qt3DCore::QAttribute::Float);
        QCOMPARE(renderAttribute.vertexSize(), 1U);
        QCOMPARE(renderAttribute.attributeType(), Qt3DCore::QAttribute::VertexAttribute);
        QCOMPARE(renderAttribute.byteOffset(), 0U);
        QCOMPARE(renderAttribute.byteStride(), 0U);
        QCOMPARE(renderAttribute.count(), 0U);
        QCOMPARE(renderAttribute.divisor(), 0U);

        // GIVEN
        Qt3DCore::QAttribute attribute;
        attribute.setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
        attribute.setByteOffset(1200);
        attribute.setByteStride(883);
        attribute.setCount(427);
        attribute.setDivisor(305);
        attribute.setName(QStringLiteral("C3"));
        attribute.setVertexBaseType(Qt3DCore::QAttribute::Double);
        attribute.setVertexSize(4);
        Qt3DCore::QBuffer buffer;
        buffer.setUsage(Qt3DCore::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("C7"));
        attribute.setBuffer(&buffer);

        // WHEN
        simulateInitializationSync(&attribute, &renderAttribute);
        renderAttribute.cleanup();

        // THEN
        QVERIFY(renderAttribute.bufferId().isNull());
        QVERIFY(renderAttribute.name().isEmpty());
        QCOMPARE(renderAttribute.isDirty(), false);
        QCOMPARE(renderAttribute.vertexBaseType(), Qt3DCore::QAttribute::Float);
        QCOMPARE(renderAttribute.vertexSize(), 1U);
        QCOMPARE(renderAttribute.attributeType(), Qt3DCore::QAttribute::VertexAttribute);
        QCOMPARE(renderAttribute.byteOffset(), 0U);
        QCOMPARE(renderAttribute.byteStride(), 0U);
        QCOMPARE(renderAttribute.count(), 0U);
        QCOMPARE(renderAttribute.divisor(), 0U);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DCore::QAttribute attribute;
        TestRenderer renderer;
        Qt3DRender::Render::Attribute renderAttribute;
        renderAttribute.setRenderer(&renderer);
        simulateInitializationSync(&attribute, &renderAttribute);
        renderAttribute.cleanup();
        renderer.resetDirty();

        QVERIFY(!renderAttribute.isDirty());
        QVERIFY(!renderer.dirtyBits());

        // WHEN
        attribute.setVertexBaseType(Qt3DCore::QAttribute::Int);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.vertexBaseType(), Qt3DCore::QAttribute::Int);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());
        QVERIFY(!renderer.dirtyBits());

        // WHEN
        attribute.setVertexSize(3);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.vertexSize(), 3U);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.attributeType(), Qt3DCore::QAttribute::IndexAttribute);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setAttributeType(Qt3DCore::QAttribute::DrawIndirectAttribute);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.attributeType(), Qt3DCore::QAttribute::DrawIndirectAttribute);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setCount(1340);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.count(), 1340U);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setName(QStringLiteral("L88"));
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.name(), QStringLiteral("L88"));
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setByteOffset(555U);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.byteOffset(), 555U);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setByteStride(454);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.byteStride(), 454U);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        attribute.setDivisor(1450);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.divisor(), 1450U);
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());

        // WHEN
        auto buffer = new Qt3DCore::QBuffer();
        attribute.setBuffer(buffer);
        renderAttribute.syncFromFrontEnd(&attribute, false);

        // THEN
        QCOMPARE(renderAttribute.bufferId(), buffer->id());
        QVERIFY(renderAttribute.isDirty());
        QVERIFY(renderer.dirtyBits() != 0);

        renderAttribute.unsetDirty();
        renderer.resetDirty();
        QVERIFY(!renderAttribute.isDirty());
    }
};


QTEST_APPLESS_MAIN(tst_Attribute)

#include "tst_attribute.moc"
