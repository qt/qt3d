// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <testarbiter.h>
#include <testrenderer.h>
#include <private/shader_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <rendercommand_p.h>
#include <renderer_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class tst_Rhi_RenderCommands : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkRenderCommandInitialState()
    {
        // GIVEN
        RenderCommand renderCommand;

        // THEN
        QVERIFY(!renderCommand.isValid());
    }

    void checkAttributeInfoInitialState()
    {
        // GIVEN
        AttributeInfo info;

        // THEN
        QCOMPARE(info.nameId, -1);
        QCOMPARE(info.classification,  QRhiVertexInputBinding::PerVertex);
        QCOMPARE(info.stride, size_t(0));
        QCOMPARE(info.offset, size_t(0));
        QCOMPARE(info.divisor, size_t(0));
    }

    void checkAttributeInfoComparison()
    {
        // GIVEN
        AttributeInfo info1 { 883, QRhiVertexInputBinding::PerVertex, 32, 0, 0 };
        AttributeInfo info2 { 883, QRhiVertexInputBinding::PerVertex, 32, 0, 0 };

        // THEN
        QVERIFY(info1 == info2);
        QVERIFY(!(info1 != info2));

        // WHEN
        info2.nameId = 0;

        // THEN
        QVERIFY(!(info1 == info2));
        QVERIFY(info1 != info2);

        // WHEN
        info2.nameId = 883;
        info2.classification = QRhiVertexInputBinding::PerInstance;

        // THEN
        QVERIFY(!(info1 == info2));
        QVERIFY(info1 != info2);

        // WHEN
        info2.classification = QRhiVertexInputBinding::PerVertex;
        info2.stride = 0;

        // THEN
        QVERIFY(!(info1 == info2));
        QVERIFY(info1 != info2);

        // WHEN
        info2.stride = 32;
        info2.offset = 8;

        // THEN
        QVERIFY(!(info1 == info2));
        QVERIFY(info1 != info2);

        // THEN
        info2.offset = 0;
        info2.divisor = 1;

        // THEN
        QVERIFY(!(info1 == info2));
        QVERIFY(info1 != info2);

        // THEN
        info2.divisor = 0;

        // THEN
        QVERIFY(info1 == info2);
        QVERIFY(!(info1 != info2));
    }
};

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

QTEST_MAIN(Qt3DRender::Render::Rhi::tst_Rhi_RenderCommands)

#include "tst_rhi_rendercommands.moc"
