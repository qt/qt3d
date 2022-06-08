// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/private/qattribute_p.h>
#include <Qt3DCore/QBuffer>

#include "testarbiter.h"

class tst_QAttribute: public QObject
{
    Q_OBJECT
public:
    tst_QAttribute()
    {
        qRegisterMetaType<Qt3DCore::QBuffer*>("Qt3DCore::QBuffer*");
    }

private Q_SLOTS:
    void shouldHaveDefaultAttributeNames()
    {
        // GIVEN
        Qt3DCore::QAttribute attribute;

        // THEN
        QCOMPARE(Qt3DCore::QAttribute::defaultPositionAttributeName(), QStringLiteral("vertexPosition"));
        QCOMPARE(Qt3DCore::QAttribute::defaultNormalAttributeName(), QStringLiteral("vertexNormal"));
        QCOMPARE(Qt3DCore::QAttribute::defaultColorAttributeName(), QStringLiteral("vertexColor"));
        QCOMPARE(Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName(), QStringLiteral("vertexTexCoord"));
        QCOMPARE(Qt3DCore::QAttribute::defaultTangentAttributeName(), QStringLiteral("vertexTangent"));
        QCOMPARE(Qt3DCore::QAttribute::defaultJointIndicesAttributeName(), QStringLiteral("vertexJointIndices"));
        QCOMPARE(Qt3DCore::QAttribute::defaultJointWeightsAttributeName(), QStringLiteral("vertexJointWeights"));

        QCOMPARE(attribute.property("defaultPositionAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultPositionAttributeName());
        QCOMPARE(attribute.property("defaultNormalAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultNormalAttributeName());
        QCOMPARE(attribute.property("defaultColorAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultColorAttributeName());
        QCOMPARE(attribute.property("defaultTextureCoordinateAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName());
        QCOMPARE(attribute.property("defaultTangentAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultTangentAttributeName());
        QCOMPARE(attribute.property("defaultJointIndicesAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultJointIndicesAttributeName());
        QCOMPARE(attribute.property("defaultJointWeightsAttributeName").toString(),
                 Qt3DCore::QAttribute::defaultJointWeightsAttributeName());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DCore::QAttribute> attribute(new Qt3DCore::QAttribute());
        arbiter.setArbiterOnNode(attribute.data());

        // WHEN
        attribute->setVertexBaseType(Qt3DCore::QAttribute::Double);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setVertexSize(4);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setName(QStringLiteral("Duntov"));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setCount(883);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setByteStride(1340);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setByteOffset(1584);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setDivisor(1450);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        attribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        Qt3DCore::QBuffer buf;
        attribute->setBuffer(&buf);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        Qt3DCore::QBuffer buf2;
        attribute->setBuffer(&buf2);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();
    }

    void checkBufferBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QAttribute> attribute(new Qt3DCore::QAttribute);
        {
            // WHEN
            Qt3DCore::QBuffer buf;
            attribute->setBuffer(&buf);

            // THEN
            QCOMPARE(buf.parent(), attribute.data());
            QCOMPARE(attribute->buffer(), &buf);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(attribute->buffer() == nullptr);

        {
            // WHEN
            Qt3DCore::QAttribute someOtherAttribute;
            QScopedPointer<Qt3DCore::QBuffer> buf(new Qt3DCore::QBuffer(&someOtherAttribute));
            attribute->setBuffer(buf.data());

            // THEN
            QCOMPARE(buf->parent(), &someOtherAttribute);
            QCOMPARE(attribute->buffer(), buf.data());

            // WHEN
            attribute.reset();
            buf.reset();

            // THEN Should not crash when the buffer is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAttribute)

#include "tst_qattribute.moc"
