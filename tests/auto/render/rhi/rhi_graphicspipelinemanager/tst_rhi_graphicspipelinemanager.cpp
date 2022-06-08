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
#include <rhiresourcemanagers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class tst_Rhi_GraphicsPipelineManager : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkAttributeInfoComparison()
    {
        // GIVEN
        RHIGraphicsPipelineManager pipelineManagers;

        // WHEN
        std::vector<AttributeInfo> infoList1 = {
            { 1, QRhiVertexInputBinding::PerVertex, 32, 0, 0 },
            { 2, QRhiVertexInputBinding::PerVertex, 32, 8, 0 },
        };
        const int infoList1ID = pipelineManagers.getIdForAttributeVec(infoList1);

        // THEN
        QCOMPARE(infoList1ID, 0);

        // WHEN
        std::vector<AttributeInfo> infoList2 = {
            { 1, QRhiVertexInputBinding::PerVertex, 32, 0, 0 },
            { 2, QRhiVertexInputBinding::PerVertex, 32, 8, 0 },
            { 3, QRhiVertexInputBinding::PerVertex, 32, 16, 0 },
        };
        const int infoList2ID = pipelineManagers.getIdForAttributeVec(infoList2);

        // THEN
        QCOMPARE(infoList2ID, 1);

        // WHEN
        std::vector<AttributeInfo> infoList3 = {
            { 1, QRhiVertexInputBinding::PerVertex, 32, 0, 0 },
            { 2, QRhiVertexInputBinding::PerVertex, 32, 8, 0 },
        };
        const int infoList3ID = pipelineManagers.getIdForAttributeVec(infoList3);

        // THEN
        QCOMPARE(infoList3ID, infoList1ID);
    }
};

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

QTEST_MAIN(Qt3DRender::Render::Rhi::tst_Rhi_GraphicsPipelineManager)

#include "tst_rhi_graphicspipelinemanager.moc"
