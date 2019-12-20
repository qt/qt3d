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

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/private/qattribute_p.h>
#include <Qt3DRender/QBuffer>

#include "testarbiter.h"

class tst_QAttribute: public QObject
{
    Q_OBJECT
public:
    tst_QAttribute()
    {
        qRegisterMetaType<Qt3DRender::QBuffer*>("Qt3DCore::QBuffer*");
    }

private Q_SLOTS:
    void shouldHaveDefaultAttributeNames()
    {
        // GIVEN
        Qt3DRender::QAttribute attribute;

        // THEN
        QCOMPARE(Qt3DRender::QAttribute::defaultPositionAttributeName(), QStringLiteral("vertexPosition"));
        QCOMPARE(Qt3DRender::QAttribute::defaultNormalAttributeName(), QStringLiteral("vertexNormal"));
        QCOMPARE(Qt3DRender::QAttribute::defaultColorAttributeName(), QStringLiteral("vertexColor"));
        QCOMPARE(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName(), QStringLiteral("vertexTexCoord"));
        QCOMPARE(Qt3DRender::QAttribute::defaultTangentAttributeName(), QStringLiteral("vertexTangent"));
        QCOMPARE(Qt3DRender::QAttribute::defaultJointIndicesAttributeName(), QStringLiteral("vertexJointIndices"));
        QCOMPARE(Qt3DRender::QAttribute::defaultJointWeightsAttributeName(), QStringLiteral("vertexJointWeights"));

        QCOMPARE(attribute.property("defaultPositionAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultPositionAttributeName());
        QCOMPARE(attribute.property("defaultNormalAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultNormalAttributeName());
        QCOMPARE(attribute.property("defaultColorAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultColorAttributeName());
        QCOMPARE(attribute.property("defaultTextureCoordinateAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
        QCOMPARE(attribute.property("defaultTangentAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultTangentAttributeName());
        QCOMPARE(attribute.property("defaultJointIndicesAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultJointIndicesAttributeName());
        QCOMPARE(attribute.property("defaultJointWeightsAttributeName").toString(),
                 Qt3DRender::QAttribute::defaultJointWeightsAttributeName());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QAttribute> attribute(new Qt3DRender::QAttribute());
        arbiter.setArbiterOnNode(attribute.data());

        // WHEN
        attribute->setVertexBaseType(Qt3DRender::QAttribute::Double);

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
        attribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        Qt3DRender::QBuffer buf;
        attribute->setBuffer(&buf);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();

        // WHEN
        Qt3DRender::QBuffer buf2;
        attribute->setBuffer(&buf2);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), attribute.data());

        arbiter.clear();
    }

    void checkBufferBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QAttribute> attribute(new Qt3DRender::QAttribute);
        {
            // WHEN
            Qt3DRender::QBuffer buf;
            attribute->setBuffer(&buf);

            // THEN
            QCOMPARE(buf.parent(), attribute.data());
            QCOMPARE(attribute->buffer(), &buf);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(attribute->buffer() == nullptr);

        {
            // WHEN
            Qt3DRender::QAttribute someOtherAttribute;
            QScopedPointer<Qt3DRender::QBuffer> buf(new Qt3DRender::QBuffer(&someOtherAttribute));
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
