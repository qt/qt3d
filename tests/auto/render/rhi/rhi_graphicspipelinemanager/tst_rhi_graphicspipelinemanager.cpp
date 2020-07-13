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
