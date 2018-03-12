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
#include <qbackendnodetester.h>
#include <private/qframeallocator_p.h>
#include <private/qframeallocator_p_p.h>
#include <private/memorybarrier_p.h>
#include <testpostmanarbiter.h>
#include <testrenderer.h>
#include <private/shader_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <renderview_p.h>
#include <renderviewjobutils_p.h>
#include <rendercommand_p.h>
#include <renderer_p.h>
#include <glresourcemanagers_p.h>
#include <private/shader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

namespace {

void compareShaderParameterPacks(const ShaderParameterPack &t1,
                                 const ShaderParameterPack &t2)
{
    const PackUniformHash hash1 = t1.uniforms();
    const PackUniformHash hash2 = t2.uniforms();

    QCOMPARE(hash1.keys.size(), hash2.keys.size());

    for (int i = 0, m = hash1.keys.size(); i < m; ++i) {
        const int key = hash1.keys.at(i);
        QCOMPARE(hash1.value(key), hash2.value(key));
    }
}

} // anonymous

class tst_RenderViews : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkRenderViewSizeFitsWithAllocator()
    {
        QSKIP("Allocated Disabled");
        QVERIFY(sizeof(RenderView) <= 192);
        QVERIFY(sizeof(RenderView::InnerData) <= 192);
    }

    void checkRenderViewInitialState()
    {
        // GIVEN
        RenderView renderView;

        // THEN
        QCOMPARE(renderView.memoryBarrier(), QMemoryBarrier::None);
    }

    void checkMemoryBarrierInitialization()
    {
        // GIVEN
        RenderView renderView;

        // THEN
        QCOMPARE(renderView.memoryBarrier(), QMemoryBarrier::None);

        // WHEN
        const QMemoryBarrier::Operations barriers(QMemoryBarrier::BufferUpdate|QMemoryBarrier::ShaderImageAccess);
        renderView.setMemoryBarrier(barriers);

        // THEN
        QCOMPARE(renderView.memoryBarrier(), barriers);
    }

    void checkSetRenderViewConfig()
    {
        TestRenderer renderer;
        {
            // GIVEN
            const QMemoryBarrier::Operations barriers(QMemoryBarrier::AtomicCounter|QMemoryBarrier::ShaderStorage);
            Qt3DRender::QMemoryBarrier frontendBarrier;
            FrameGraphManager frameGraphManager;
            MemoryBarrier backendBarrier;
            RenderView renderView;
            // setRenderViewConfigFromFrameGraphLeafNode assumes node has a manager
            backendBarrier.setFrameGraphManager(&frameGraphManager);
            backendBarrier.setRenderer(&renderer);

            // WHEN
            frontendBarrier.setWaitOperations(barriers);
            simulateInitializationSync(&frontendBarrier, &backendBarrier);

            // THEN
            QCOMPARE(renderView.memoryBarrier(), QMemoryBarrier::None);
            QCOMPARE(backendBarrier.waitOperations(), barriers);

            // WHEN
            Qt3DRender::Render::OpenGL::setRenderViewConfigFromFrameGraphLeafNode(&renderView, &backendBarrier);

            // THEN
            QCOMPARE(backendBarrier.waitOperations(), renderView.memoryBarrier());
        }
        // TO DO: Complete tests for other framegraph node types
    }

    void checkRenderCommandBackToFrontSorting()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        RenderView renderView;
        QVector<RenderCommand> rawCommands;
        QVector<QSortPolicy::SortType> sortTypes;

        renderer.setNodeManagers(&nodeManagers);
        renderView.setRenderer(&renderer);

        sortTypes.push_back(QSortPolicy::BackToFront);

        for (int i = 0; i < 200; ++i) {
            RenderCommand c;
            c.m_depth = float(i);
            rawCommands.push_back(c);
        }

        // WHEN
        renderView.addSortType(sortTypes);
        renderView.setCommands(rawCommands);
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands.size(), sortedCommands.size());
        for (int j = 1; j < sortedCommands.size(); ++j)
            QVERIFY(sortedCommands.at(j - 1).m_depth > sortedCommands.at(j).m_depth);

        // RenderCommands are deleted by RenderView dtor
        renderer.shutdown();
    }

    void checkRenderCommandMaterialSorting()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        RenderView renderView;
        QVector<RenderCommand> rawCommands;
        QVector<QSortPolicy::SortType> sortTypes;

        renderer.setNodeManagers(&nodeManagers);
        renderView.setRenderer(&renderer);

        sortTypes.push_back(QSortPolicy::Material);

        GLShader *dnas[5] = {
            reinterpret_cast<GLShader *>(0x250),
            reinterpret_cast<GLShader *>(0x500),
            reinterpret_cast<GLShader *>(0x1000),
            reinterpret_cast<GLShader *>(0x1500),
            reinterpret_cast<GLShader *>(0x2000)
        };

        for (int i = 0; i < 20; ++i) {
            RenderCommand c;
            c.m_glShader = dnas[i % 5];
            rawCommands.push_back(c);
        }

        // WHEN
        renderView.addSortType(sortTypes);
        renderView.setCommands(rawCommands);
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands.size(), sortedCommands.size());
        GLShader *targetShader;

        for (int j = 0; j < sortedCommands.size(); ++j) {

            if (j % 4 == 0) {
                targetShader = sortedCommands.at(j).m_glShader;
                if (j > 0)
                    QVERIFY(targetShader != sortedCommands.at(j - 1).m_glShader);
            }
            QCOMPARE(targetShader, sortedCommands.at(j).m_glShader);
        }

        // RenderCommands are deleted by RenderView dtor
        renderer.shutdown();
    }

    void checkRenderViewUniformMinification_data()
    {
        QTest::addColumn<QVector<QShaderProgram*>>("shaders");
        QTest::addColumn<QVector<ShaderParameterPack>>("rawParameters");
        QTest::addColumn<QVector<ShaderParameterPack>>("expectedMinimizedParameters");

        Qt3DCore::QNodeId fakeTextureNodeId = Qt3DCore::QNodeId::createId();

        ShaderParameterPack pack1;
        pack1.setUniform(1, UniformValue(883));
        pack1.setUniform(2, UniformValue(1584.0f));
        pack1.setTexture(3, 0, fakeTextureNodeId);

        QShaderProgram *shader1 = new QShaderProgram();
        QShaderProgram *shader2 = new QShaderProgram();

        shader1->setShaderCode(QShaderProgram::Vertex, QByteArrayLiteral("1"));
        shader2->setShaderCode(QShaderProgram::Vertex, QByteArrayLiteral("2"));

        ShaderParameterPack minifiedPack1;

        QTest::newRow("NoMinification")
                << (QVector<QShaderProgram*>() << shader1 << shader2)
                << (QVector<ShaderParameterPack>() << pack1 << pack1)
                << (QVector<ShaderParameterPack>() << pack1 << pack1);

        QTest::newRow("SingleShaderMinified")
                << (QVector<QShaderProgram*>() << shader1 << shader1 << shader1)
                << (QVector<ShaderParameterPack>() << pack1 << pack1 << pack1)
                << (QVector<ShaderParameterPack>() << pack1 << minifiedPack1 << minifiedPack1);

        QTest::newRow("MultipleShadersMinified")
                << (QVector<QShaderProgram*>() << shader1 << shader1 << shader1 << shader2 << shader2 << shader2)
                << (QVector<ShaderParameterPack>() << pack1 << pack1 << pack1 << pack1 << pack1 << pack1)
                << (QVector<ShaderParameterPack>() << pack1 << minifiedPack1 << minifiedPack1 << pack1 << minifiedPack1 << minifiedPack1);
    }

    void checkRenderViewUniformMinification()
    {
        QFETCH(QVector<QShaderProgram*>, shaders);
        QFETCH(QVector<ShaderParameterPack>, rawParameters);
        QFETCH(QVector<ShaderParameterPack>, expectedMinimizedParameters);

        Qt3DRender::Render::NodeManagers nodeManagers;
        Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        renderer.setNodeManagers(&nodeManagers);

        GLShaderManager *shaderManager = renderer.glResourceManagers()->glShaderManager();
        for (int i = 0, m = shaders.size(); i < m; ++i) {
            Shader* backend = new Shader();
            backend->setRenderer(&renderer);
            simulateInitializationSync(shaders.at(i), backend);
            shaderManager->createOrAdoptExisting(backend);
        }

        RenderView renderView;
        QVector<RenderCommand> rawCommands;
        renderView.setRenderer(&renderer);

        for (int i = 0, m = shaders.size(); i < m; ++i) {
            RenderCommand c;
            c.m_shaderId = shaders.at(i)->id();
            c.m_glShader = shaderManager->lookupResource(c.m_shaderId);
            c.m_parameterPack = rawParameters.at(i);
            rawCommands.push_back(c);
        }

        // WHEN
        renderView.setCommands(rawCommands);
        renderView.addSortType((QVector<QSortPolicy::SortType>() << QSortPolicy::Uniform));
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands, sortedCommands);

        for (int i = 0, m = shaders.size(); i < m; ++i) {
            const RenderCommand c = sortedCommands.at(i);
            QCOMPARE(c.m_shaderId, shaders.at(i)->id());
            compareShaderParameterPacks(c.m_parameterPack, expectedMinimizedParameters.at(i));
        }

        renderer.shutdown();
    }


    void checkRenderCommandFrontToBackSorting()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        RenderView renderView;
        QVector<RenderCommand> rawCommands;
        QVector<QSortPolicy::SortType> sortTypes;

        renderer.setNodeManagers(&nodeManagers);
        renderView.setRenderer(&renderer);

        sortTypes.push_back(QSortPolicy::FrontToBack);

        for (int i = 0; i < 200; ++i) {
            RenderCommand c;
            c.m_depth = float(i);
            rawCommands.push_back(c);
        }

        // WHEN
        renderView.addSortType(sortTypes);
        renderView.setCommands(rawCommands);
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands.size(), sortedCommands.size());
        for (int j = 1; j < sortedCommands.size(); ++j)
            QVERIFY(sortedCommands.at(j - 1).m_depth < sortedCommands.at(j).m_depth);

        // RenderCommands are deleted by RenderView dtor
        renderer.shutdown();
    }

    void checkRenderCommandStateCostSorting()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        RenderView renderView;
        QVector<RenderCommand> rawCommands;
        QVector<QSortPolicy::SortType> sortTypes;

        renderer.setNodeManagers(&nodeManagers);
        renderView.setRenderer(&renderer);

        sortTypes.push_back(QSortPolicy::StateChangeCost);

        for (int i = 0; i < 200; ++i) {
            RenderCommand c;
            c.m_changeCost = i;
            rawCommands.push_back(c);
        }

        // WHEN
        renderView.addSortType(sortTypes);
        renderView.setCommands(rawCommands);
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands.size(), sortedCommands.size());
        for (int j = 1; j < sortedCommands.size(); ++j)
            QVERIFY(sortedCommands.at(j - 1).m_changeCost > sortedCommands.at(j).m_changeCost);

        // RenderCommands are deleted by RenderView dtor
        renderer.shutdown();
    }

    void checkRenderCommandCombinedStateMaterialDepthSorting()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManagers;
        Renderer renderer(Qt3DRender::QRenderAspect::Synchronous);
        RenderView renderView;
        QVector<RenderCommand> rawCommands;
        QVector<QSortPolicy::SortType> sortTypes;

        renderer.setNodeManagers(&nodeManagers);
        renderView.setRenderer(&renderer);

        sortTypes.push_back(QSortPolicy::StateChangeCost);
        sortTypes.push_back(QSortPolicy::Material);
        sortTypes.push_back(QSortPolicy::BackToFront);

        GLShader *dna[5] = {
            reinterpret_cast<GLShader *>(0x250),
            reinterpret_cast<GLShader *>(0x500),
            reinterpret_cast<GLShader *>(0x1000),
            reinterpret_cast<GLShader *>(0x1500),
            reinterpret_cast<GLShader *>(0x2000)
        };

        float depth[3] = {
            10.0f,
            25.0f,
            30.0f
        };

        int stateChangeCost[2] = {
            100,
            200
        };

        auto buildRC = [] (GLShader *dna, float depth, int changeCost) {
            RenderCommand c;
            c.m_glShader = dna;
            c.m_depth = depth;
            c.m_changeCost = changeCost;
            return c;
        };

        RenderCommand c5 = buildRC(dna[3], depth[1], stateChangeCost[1]);
        RenderCommand c3 = buildRC(dna[3], depth[0], stateChangeCost[1]);
        RenderCommand c4 = buildRC(dna[2], depth[1], stateChangeCost[1]);
        RenderCommand c8 = buildRC(dna[1], depth[1], stateChangeCost[1]);
        RenderCommand c0 = buildRC(dna[0], depth[2], stateChangeCost[1]);

        RenderCommand c2 = buildRC(dna[2], depth[2], stateChangeCost[0]);
        RenderCommand c9 = buildRC(dna[2], depth[0], stateChangeCost[0]);
        RenderCommand c1 = buildRC(dna[1], depth[0], stateChangeCost[0]);
        RenderCommand c7 = buildRC(dna[0], depth[2], stateChangeCost[0]);
        RenderCommand c6 = buildRC(dna[0], depth[1], stateChangeCost[0]);

        rawCommands << c0 << c1 << c2 << c3 << c4 << c5 << c6 << c7 << c8 << c9;

        // WHEN
        renderView.addSortType(sortTypes);
        renderView.setCommands(rawCommands);
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands.size(), sortedCommands.size());

        // Ordered by higher state, higher shaderDNA and higher depth
        QCOMPARE(c0, sortedCommands.at(4));
        QCOMPARE(c3, sortedCommands.at(1));
        QCOMPARE(c4, sortedCommands.at(2));
        QCOMPARE(c5, sortedCommands.at(0));
        QCOMPARE(c8, sortedCommands.at(3));

        QCOMPARE(c1, sortedCommands.at(7));
        QCOMPARE(c2, sortedCommands.at(5));
        QCOMPARE(c6, sortedCommands.at(9));
        QCOMPARE(c7, sortedCommands.at(8));
        QCOMPARE(c9, sortedCommands.at(6));

        // RenderCommands are deleted by RenderView dtor
        renderer.shutdown();
    }

    void checkRenderCommandTextureSorting()
    {
        // GIVEN
        RenderView renderView;
        QVector<QSortPolicy::SortType> sortTypes;

        sortTypes.push_back(QSortPolicy::Texture);


        Qt3DCore::QNodeId tex1 = Qt3DCore::QNodeId::createId();
        Qt3DCore::QNodeId tex2 = Qt3DCore::QNodeId::createId();
        Qt3DCore::QNodeId tex3 = Qt3DCore::QNodeId::createId();
        Qt3DCore::QNodeId tex4 = Qt3DCore::QNodeId::createId();

        RenderCommand a;
        {
            ShaderParameterPack pack;
            pack.setTexture(0, 0, tex1);
            pack.setTexture(1, 0, tex3);
            pack.setTexture(2, 0, tex4);
            pack.setTexture(3, 0, tex2);
            a.m_parameterPack = pack;
        }
        RenderCommand b;
        RenderCommand c;
        {
            ShaderParameterPack pack;
            pack.setTexture(0, 0, tex1);
            pack.setTexture(3, 0, tex2);
            c.m_parameterPack = pack;
        }
        RenderCommand d;
        {
            ShaderParameterPack pack;
            pack.setTexture(1, 0, tex3);
            pack.setTexture(2, 0, tex4);
            d.m_parameterPack = pack;
        }
        RenderCommand e;
        {
            ShaderParameterPack pack;
            pack.setTexture(3, 0, tex2);
            e.m_parameterPack = pack;
        }
        RenderCommand f;
        {
            ShaderParameterPack pack;
            pack.setTexture(3, 0, tex2);
            f.m_parameterPack = pack;
        }
        RenderCommand g;
        {
            ShaderParameterPack pack;
            pack.setTexture(0, 0, tex1);
            pack.setTexture(1, 0, tex3);
            pack.setTexture(2, 0, tex4);
            pack.setTexture(3, 0, tex2);
            g.m_parameterPack = pack;
        }

        // WHEN
        QVector<RenderCommand> rawCommands = {a, b, c, d, e, f, g};
        renderView.addSortType(sortTypes);
        renderView.setCommands(rawCommands);
        renderView.sort();

        // THEN
        const QVector<RenderCommand> sortedCommands = renderView.commands();
        QCOMPARE(rawCommands.size(), sortedCommands.size());
        QCOMPARE(sortedCommands.at(0), a);
        QCOMPARE(sortedCommands.at(1), g);
        QCOMPARE(sortedCommands.at(2), d);
        QCOMPARE(sortedCommands.at(3), c);
        QCOMPARE(sortedCommands.at(4), e);
        QCOMPARE(sortedCommands.at(5), f);
        QCOMPARE(sortedCommands.at(6), b);
        // RenderCommands are deleted by RenderView dtor
    }
private:
};

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE

//APPLESS_
QTEST_MAIN(Qt3DRender::Render::OpenGL::tst_RenderViews)

#include "tst_renderviews.moc"
