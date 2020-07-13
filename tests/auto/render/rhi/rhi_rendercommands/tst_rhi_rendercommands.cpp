/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
