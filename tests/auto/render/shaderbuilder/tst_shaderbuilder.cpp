/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DRender/private/shaderbuilder_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include "testrenderer.h"

Q_DECLARE_METATYPE(Qt3DRender::Render::ShaderBuilder::ShaderType)

class tst_ShaderBuilder : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private slots:
    void shouldHaveInitialState()
    {
        // GIVEN
        Qt3DRender::Render::ShaderBuilder shaderBuilder;

        // THEN
        QVERIFY(!shaderBuilder.isEnabled());
        for (int i = 0; i <= Qt3DRender::Render::ShaderBuilder::Compute; i++) {
            const auto type = static_cast<Qt3DRender::Render::ShaderBuilder::ShaderType>(i);
            QCOMPARE(shaderBuilder.shaderGraph(type), QUrl());
            QCOMPARE(shaderBuilder.shaderCode(type), QByteArray());
            QVERIFY(!shaderBuilder.isShaderCodeDirty(type));
        }
    }

    void shouldHavePropertiesMirroringFromItsPeer_data()
    {
        QTest::addColumn<Qt3DRender::QShaderProgramBuilder*>("frontend");

        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            QTest::newRow("empty") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            auto program = new Qt3DRender::QShaderProgram(frontend);
            frontend->setShaderProgram(program);
            QTest::newRow("shaderProgram") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            frontend->setVertexShaderGraph(QUrl::fromEncoded("qrc:/vertex.json"));
            QTest::newRow("vertex") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            frontend->setTessellationControlShaderGraph(QUrl::fromEncoded("qrc:/tesscontrol.json"));
            QTest::newRow("tessellationControl") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            frontend->setTessellationEvaluationShaderGraph(QUrl::fromEncoded("qrc:/tesseval.json"));
            QTest::newRow("tessellationEvaluation") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            frontend->setGeometryShaderGraph(QUrl::fromEncoded("qrc:/geometry.json"));
            QTest::newRow("geometry") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            frontend->setFragmentShaderGraph(QUrl::fromEncoded("qrc:/fragment.json"));
            QTest::newRow("fragment") << frontend;
        }
        {
            auto frontend = new Qt3DRender::QShaderProgramBuilder;
            frontend->setComputeShaderGraph(QUrl::fromEncoded("qrc:/compute.json"));
            QTest::newRow("compute") << frontend;
        }
    }

    void shouldHavePropertiesMirroringFromItsPeer()
    {
        // GIVEN
        QFETCH(Qt3DRender::QShaderProgramBuilder*, frontend);
        Qt3DRender::Render::ShaderBuilder backend;

        // WHEN
        simulateInitialization(frontend, &backend);

        // THEN
        QVERIFY(backend.isEnabled() == frontend->isEnabled());

        if (frontend->shaderProgram())
            QCOMPARE(backend.shaderProgramId(), frontend->shaderProgram()->id());
        else
            QVERIFY(backend.shaderProgramId().isNull());

        QCOMPARE(backend.shaderGraph(Qt3DRender::Render::ShaderBuilder::Vertex), frontend->vertexShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::Render::ShaderBuilder::Vertex), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::Render::ShaderBuilder::Vertex), !frontend->vertexShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::Render::ShaderBuilder::TessellationControl), frontend->tessellationControlShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::Render::ShaderBuilder::TessellationControl), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::Render::ShaderBuilder::TessellationControl), !frontend->tessellationControlShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::Render::ShaderBuilder::TessellationEvaluation), frontend->tessellationEvaluationShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::Render::ShaderBuilder::TessellationEvaluation), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::Render::ShaderBuilder::TessellationEvaluation), !frontend->tessellationEvaluationShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::Render::ShaderBuilder::Geometry), frontend->geometryShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::Render::ShaderBuilder::Geometry), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::Render::ShaderBuilder::Geometry), !frontend->geometryShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::Render::ShaderBuilder::Fragment), frontend->fragmentShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::Render::ShaderBuilder::Fragment), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::Render::ShaderBuilder::Fragment), !frontend->fragmentShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::Render::ShaderBuilder::Compute), frontend->computeShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::Render::ShaderBuilder::Compute), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::Render::ShaderBuilder::Compute), !frontend->computeShaderGraph().isEmpty());

        // WHEN
        backend.cleanup();

        // THEN
        QVERIFY(!backend.isEnabled());
        for (int i = 0; i <= Qt3DRender::Render::ShaderBuilder::Compute; i++) {
            const auto type = static_cast<Qt3DRender::Render::ShaderBuilder::ShaderType>(i);
            QCOMPARE(backend.shaderGraph(type), QUrl());
            QCOMPARE(backend.shaderCode(type), QByteArray());
            QVERIFY(!backend.isShaderCodeDirty(type));
        }

        delete frontend;
    }

    void shouldHandleEnablePropertyChange()
    {
        // GIVEN
        Qt3DRender::Render::ShaderBuilder backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);

        // WHEN
        auto updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(true);
        updateChange->setPropertyName("enabled");
        backend.sceneChangeEvent(updateChange);

        // THEN
        QVERIFY(backend.isEnabled());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(false);
        updateChange->setPropertyName("enabled");
        backend.sceneChangeEvent(updateChange);

        // THEN
        QVERIFY(!backend.isEnabled());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(true);
        updateChange->setPropertyName("enabled");
        backend.sceneChangeEvent(updateChange);
        // AND
        backend.cleanup();

        // THEN
        QVERIFY(!backend.isEnabled());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();
    }

    void shouldHandleShaderProgramPropertyChange()
    {
        // GIVEN
        Qt3DRender::Render::ShaderBuilder backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);
        const auto programId = Qt3DCore::QNodeId::createId();

        // WHEN
        auto updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(QVariant::fromValue(programId));
        updateChange->setPropertyName("shaderProgram");
        backend.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backend.shaderProgramId(), programId);
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(QVariant::fromValue(Qt3DCore::QNodeId()));
        updateChange->setPropertyName("shaderProgram");
        backend.sceneChangeEvent(updateChange);

        // THEN
        QVERIFY(backend.shaderProgramId().isNull());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(QVariant::fromValue(programId));
        updateChange->setPropertyName("shaderProgram");
        backend.sceneChangeEvent(updateChange);
        // AND
        backend.cleanup();

        // THEN
        QVERIFY(backend.shaderProgramId().isNull());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();
    }

    void shouldHandleShaderGraphPropertiesChanges_data()
    {
        QTest::addColumn<QByteArray>("property");
        QTest::addColumn<Qt3DRender::Render::ShaderBuilder::ShaderType>("type");
        QTest::addColumn<QUrl>("graphUrl");

        QTest::newRow("vertex") << QByteArrayLiteral("vertexShaderGraph")
                                << Qt3DRender::Render::ShaderBuilder::Vertex
                                << QUrl::fromEncoded("qrc:/vertex.json");

        QTest::newRow("tessControl") << QByteArrayLiteral("tessellationControlShaderGraph")
                                     << Qt3DRender::Render::ShaderBuilder::TessellationControl
                                     << QUrl::fromEncoded("qrc:/tesscontrol.json");

        QTest::newRow("tessEval") << QByteArrayLiteral("tessellationEvaluationShaderGraph")
                                  << Qt3DRender::Render::ShaderBuilder::TessellationEvaluation
                                  << QUrl::fromEncoded("qrc:/tesseval.json");

        QTest::newRow("geometry") << QByteArrayLiteral("geometryShaderGraph")
                                  << Qt3DRender::Render::ShaderBuilder::Geometry
                                  << QUrl::fromEncoded("qrc:/geometry.json");

        QTest::newRow("fragment") << QByteArrayLiteral("fragmentShaderGraph")
                                  << Qt3DRender::Render::ShaderBuilder::Fragment
                                  << QUrl::fromEncoded("qrc:/fragment.json");

        QTest::newRow("compute") << QByteArrayLiteral("computeShaderGraph")
                                 << Qt3DRender::Render::ShaderBuilder::Compute
                                 << QUrl::fromEncoded("qrc:/compute.json");
    }

    void shouldHandleShaderGraphPropertiesChanges()
    {
        // GIVEN
        QFETCH(QByteArray, property);
        QFETCH(Qt3DRender::Render::ShaderBuilder::ShaderType, type);
        QFETCH(QUrl, graphUrl);

        Qt3DRender::Render::ShaderBuilder backend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);

        // WHEN
        auto updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(QUrl());
        updateChange->setPropertyName(property);
        backend.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backend.shaderGraph(type), QUrl());
        QVERIFY(!backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(graphUrl);
        updateChange->setPropertyName(property);
        backend.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(QUrl());
        updateChange->setPropertyName(property);
        backend.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backend.shaderGraph(type), QUrl());
        QVERIFY(backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setValue(graphUrl);
        updateChange->setPropertyName(property);
        backend.sceneChangeEvent(updateChange);
        // AND
        backend.cleanup();

        // THEN
        QCOMPARE(backend.shaderGraph(type), QUrl());
        QVERIFY(!backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();
    }
};

QTEST_APPLESS_MAIN(tst_ShaderBuilder)

#include "tst_shaderbuilder.moc"
