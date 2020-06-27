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
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DRender/private/shaderbuilder_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include "testrenderer.h"
#include "testpostmanarbiter.h"

#include <QByteArray>

Q_DECLARE_METATYPE(Qt3DRender::QShaderProgram::ShaderType)

class tst_ShaderBuilder : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private slots:
    void shouldHaveGlobalDefaultPrototypes()
    {
        // GIVEN

        // THEN
        QCOMPARE(Qt3DRender::Render::ShaderBuilder::getPrototypesFile(), QStringLiteral(":/prototypes/default.json"));
        QVERIFY(!Qt3DRender::Render::ShaderBuilder::getPrototypeNames().isEmpty());

        // WHEN
        Qt3DRender::Render::ShaderBuilder::setPrototypesFile(":/prototypes.json");

        // THEN
        QCOMPARE(Qt3DRender::Render::ShaderBuilder::getPrototypesFile(), QStringLiteral(":/prototypes.json"));
        auto prototypeNames = Qt3DRender::Render::ShaderBuilder::getPrototypeNames();
        prototypeNames.sort();
        const auto expectedPrototypeNames = QStringList() << "exposure"
                                                          << "exposureFunction"
                                                          << "fragColor"
                                                          << "lightIntensity"
                                                          << "lightModel"
                                                          << "sampleTexture"
                                                          << "texCoord"
                                                          << "texture"
                                                          << "worldPosition";
        QCOMPARE(prototypeNames, expectedPrototypeNames);
    }

    void shouldHaveInitialState()
    {
        // GIVEN
        Qt3DRender::Render::ShaderBuilder shaderBuilder;

        // THEN
        QVERIFY(!shaderBuilder.isEnabled());
        QVERIFY(shaderBuilder.enabledLayers().isEmpty());
        for (int i = 0; i <= Qt3DRender::QShaderProgram::Compute; i++) {
            const auto type = static_cast<Qt3DRender::QShaderProgram::ShaderType>(i);
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
            frontend->setEnabledLayers({"foo", "bar"});
            QTest::newRow("enabledLayers") << frontend;
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
        TestRenderer renderer;

        // WHEN
        backend.setRenderer(&renderer);
        simulateInitializationSync(frontend, &backend);

        // THEN
        QVERIFY(backend.isEnabled() == frontend->isEnabled());

        if (frontend->shaderProgram())
            QCOMPARE(backend.shaderProgramId(), frontend->shaderProgram()->id());
        else
            QVERIFY(backend.shaderProgramId().isNull());

        QCOMPARE(backend.enabledLayers(), frontend->enabledLayers());

        QCOMPARE(backend.shaderGraph(Qt3DRender::QShaderProgram::Vertex), frontend->vertexShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::QShaderProgram::Vertex), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::QShaderProgram::Vertex), !frontend->vertexShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::QShaderProgram::TessellationControl), frontend->tessellationControlShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::QShaderProgram::TessellationControl), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::QShaderProgram::TessellationControl), !frontend->tessellationControlShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::QShaderProgram::TessellationEvaluation), frontend->tessellationEvaluationShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::QShaderProgram::TessellationEvaluation), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::QShaderProgram::TessellationEvaluation), !frontend->tessellationEvaluationShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::QShaderProgram::Geometry), frontend->geometryShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::QShaderProgram::Geometry), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::QShaderProgram::Geometry), !frontend->geometryShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::QShaderProgram::Fragment), frontend->fragmentShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::QShaderProgram::Fragment), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::QShaderProgram::Fragment), !frontend->fragmentShaderGraph().isEmpty());

        QCOMPARE(backend.shaderGraph(Qt3DRender::QShaderProgram::Compute), frontend->computeShaderGraph());
        QCOMPARE(backend.shaderCode(Qt3DRender::QShaderProgram::Compute), QByteArray());
        QCOMPARE(backend.isShaderCodeDirty(Qt3DRender::QShaderProgram::Compute), !frontend->computeShaderGraph().isEmpty());

        // WHEN
        backend.cleanup();

        // THEN
        QVERIFY(!backend.isEnabled());
        QVERIFY(backend.enabledLayers().isEmpty());
        for (int i = 0; i <= Qt3DRender::QShaderProgram::Compute; i++) {
            const auto type = static_cast<Qt3DRender::QShaderProgram::ShaderType>(i);
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
        Qt3DRender::QShaderProgramBuilder frontend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &backend);

        // WHEN
        frontend.setEnabled(false);
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QVERIFY(!backend.isEnabled());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        frontend.setEnabled(true);
        backend.syncFromFrontEnd(&frontend, false);
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
        Qt3DRender::QShaderProgramBuilder frontend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &backend);

        // WHEN
        Qt3DRender::QShaderProgram prog;
        frontend.setShaderProgram(&prog);
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(backend.shaderProgramId(), prog.id());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        frontend.setShaderProgram(nullptr);
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QVERIFY(backend.shaderProgramId().isNull());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        frontend.setShaderProgram(&prog);
        backend.syncFromFrontEnd(&frontend, false);
        // AND
        backend.cleanup();

        // THEN
        QVERIFY(backend.shaderProgramId().isNull());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();
    }

    void shouldHandleEnabledLayersPropertyChange()
    {
        // GIVEN
        Qt3DRender::Render::ShaderBuilder backend;
        Qt3DRender::QShaderProgramBuilder frontend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &backend);
        const auto layers = QStringList() << "foo" << "bar";

        static const std::pair<
                Qt3DRender::QShaderProgram::ShaderType,
                void (Qt3DRender::QShaderProgramBuilder::*)(const QUrl &)
                >
                shaderTypesToSetters[] = {
            {Qt3DRender::QShaderProgram::Vertex, &Qt3DRender::QShaderProgramBuilder::setVertexShaderGraph},
            {Qt3DRender::QShaderProgram::TessellationControl, &Qt3DRender::QShaderProgramBuilder::setTessellationControlShaderGraph},
            {Qt3DRender::QShaderProgram::TessellationEvaluation, &Qt3DRender::QShaderProgramBuilder::setTessellationEvaluationShaderGraph},
            {Qt3DRender::QShaderProgram::Geometry, &Qt3DRender::QShaderProgramBuilder::setGeometryShaderGraph},
            {Qt3DRender::QShaderProgram::Fragment, &Qt3DRender::QShaderProgramBuilder::setFragmentShaderGraph},
            {Qt3DRender::QShaderProgram::Compute, &Qt3DRender::QShaderProgramBuilder::setComputeShaderGraph},
        };


        for (auto it = std::begin(shaderTypesToSetters), end = std::end(shaderTypesToSetters); it != end; ++it) {
            const auto graphUrl = QUrl::fromEncoded("qrc:/input.json");
            (frontend.*(it->second))(graphUrl);
        }

        // WHEN
        frontend.setEnabledLayers(layers);
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(backend.enabledLayers(), layers);
        for (int i = 0; i <= Qt3DRender::QShaderProgram::Compute; i++) {
            const auto type = static_cast<Qt3DRender::QShaderProgram::ShaderType>(i);
            QVERIFY(backend.isShaderCodeDirty(type));
            backend.generateCode(type); // Resets the dirty flag
        }
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        frontend.setEnabledLayers(layers);
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(backend.enabledLayers(), layers);
        for (int i = 0; i <= Qt3DRender::QShaderProgram::Compute; i++) {
            const auto type = static_cast<Qt3DRender::QShaderProgram::ShaderType>(i);
            QVERIFY(!backend.isShaderCodeDirty(type));
            backend.generateCode(type); // Resets the dirty flag
        }
        QCOMPARE(renderer.dirtyBits(), 0);
        renderer.resetDirty();

        // WHEN
        frontend.setEnabledLayers(QStringList());
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QVERIFY(backend.shaderProgramId().isNull());
        for (int i = 0; i <= Qt3DRender::QShaderProgram::Compute; i++) {
            const auto type = static_cast<Qt3DRender::QShaderProgram::ShaderType>(i);
            QVERIFY(backend.isShaderCodeDirty(type));
            backend.generateCode(type); // Resets the dirty flag
        }
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        frontend.setEnabledLayers(layers);
        backend.syncFromFrontEnd(&frontend, false);
        // AND
        backend.cleanup();

        // THEN
        QVERIFY(backend.enabledLayers().isEmpty());
        for (int i = 0; i <= Qt3DRender::QShaderProgram::Compute; i++) {
            const auto type = static_cast<Qt3DRender::QShaderProgram::ShaderType>(i);
            QVERIFY(!backend.isShaderCodeDirty(type));
            backend.generateCode(type); // Resets the dirty flag
        }
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();
    }

    void shouldHandleShaderGraphPropertiesChanges_data()
    {
        QTest::addColumn<Qt3DRender::QShaderProgram::ShaderType>("type");
        QTest::addColumn<QUrl>("graphUrl");

        QTest::newRow("vertex") << Qt3DRender::QShaderProgram::Vertex
                                << QUrl::fromEncoded("qrc:/vertex.json");

        QTest::newRow("tessControl") << Qt3DRender::QShaderProgram::TessellationControl
                                     << QUrl::fromEncoded("qrc:/tesscontrol.json");

        QTest::newRow("tessEval") << Qt3DRender::QShaderProgram::TessellationEvaluation
                                  << QUrl::fromEncoded("qrc:/tesseval.json");

        QTest::newRow("geometry") << Qt3DRender::QShaderProgram::Geometry
                                  << QUrl::fromEncoded("qrc:/geometry.json");

        QTest::newRow("fragment") << Qt3DRender::QShaderProgram::Fragment
                                  << QUrl::fromEncoded("qrc:/fragment.json");

        QTest::newRow("compute") << Qt3DRender::QShaderProgram::Compute
                                 << QUrl::fromEncoded("qrc:/compute.json");
    }

    void shouldHandleShaderGraphPropertiesChanges()
    {
        // GIVEN
        QFETCH(Qt3DRender::QShaderProgram::ShaderType, type);
        QFETCH(QUrl, graphUrl);

        Qt3DRender::Render::ShaderBuilder backend;
        Qt3DRender::QShaderProgramBuilder frontend;
        TestRenderer renderer;
        backend.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &backend);

        static const QHash<
                Qt3DRender::QShaderProgram::ShaderType,
                void (Qt3DRender::QShaderProgramBuilder::*)(const QUrl &)
                >
                shaderTypesToSetters = {
            {Qt3DRender::QShaderProgram::Vertex, &Qt3DRender::QShaderProgramBuilder::setVertexShaderGraph},
            {Qt3DRender::QShaderProgram::TessellationControl, &Qt3DRender::QShaderProgramBuilder::setTessellationControlShaderGraph},
            {Qt3DRender::QShaderProgram::TessellationEvaluation, &Qt3DRender::QShaderProgramBuilder::setTessellationEvaluationShaderGraph},
            {Qt3DRender::QShaderProgram::Geometry, &Qt3DRender::QShaderProgramBuilder::setGeometryShaderGraph},
            {Qt3DRender::QShaderProgram::Fragment, &Qt3DRender::QShaderProgramBuilder::setFragmentShaderGraph},
            {Qt3DRender::QShaderProgram::Compute, &Qt3DRender::QShaderProgramBuilder::setComputeShaderGraph},
        };

        // WHEN

        (frontend.*(shaderTypesToSetters[type]))(QUrl());
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(backend.shaderGraph(type), QUrl());
        QVERIFY(!backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), 0);
        renderer.resetDirty();

        // WHEN
        (frontend.*(shaderTypesToSetters[type]))(graphUrl);
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        (frontend.*(shaderTypesToSetters[type]))(QUrl());
        backend.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(backend.shaderGraph(type), QUrl());
        QVERIFY(backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();

        // WHEN
        (frontend.*(shaderTypesToSetters[type]))(graphUrl);
        backend.syncFromFrontEnd(&frontend, false);
        // AND
        backend.cleanup();

        // THEN
        QCOMPARE(backend.shaderGraph(type), QUrl());
        QVERIFY(!backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
        renderer.resetDirty();
    }

    void shouldHandleShaderCodeGeneration_data()
    {
        QTest::addColumn<Qt3DRender::QShaderProgram::ShaderType>("type");

        QTest::newRow("vertex") << Qt3DRender::QShaderProgram::Vertex;
        QTest::newRow("tessControl") << Qt3DRender::QShaderProgram::TessellationControl;
        QTest::newRow("tessEval") << Qt3DRender::QShaderProgram::TessellationEvaluation;
        QTest::newRow("geometry") << Qt3DRender::QShaderProgram::Geometry;
        QTest::newRow("fragment") << Qt3DRender::QShaderProgram::Fragment;
        QTest::newRow("compute") << Qt3DRender::QShaderProgram::Compute;
    }

    void shouldHandleShaderCodeGeneration()
    {
        // GIVEN
        Qt3DRender::Render::ShaderBuilder::setPrototypesFile(":/prototypes.json");
        QVERIFY(!Qt3DRender::Render::ShaderBuilder::getPrototypeNames().isEmpty());

        QFETCH(Qt3DRender::QShaderProgram::ShaderType, type);

        const auto gl3Api = []{
            auto api = Qt3DRender::GraphicsApiFilterData();
            api.m_api = Qt3DRender::QGraphicsApiFilter::OpenGL;
            api.m_profile = Qt3DRender::QGraphicsApiFilter::CoreProfile;
            api.m_major = 3;
            api.m_minor = 2;
            return api;
        }();

        const auto es2Api = []{
            auto api = Qt3DRender::GraphicsApiFilterData();
            api.m_api = Qt3DRender::QGraphicsApiFilter::OpenGLES;
            api.m_major = 2;
            api.m_minor = 0;
            return api;
        }();

        const auto readCode = [](const QString &suffix) -> QByteArray {
            const auto filePath = QStringLiteral(":/output.") + suffix;
            QFile file(filePath);
            if (!file.open(QFile::ReadOnly | QFile::Text))
                qFatal("File open failed: %s", qPrintable(filePath));
            return file.readAll();
        };

        const auto gl3Code = readCode("gl3");
        const auto es2Code = readCode("es2");

        Qt3DRender::Render::ShaderBuilder backend;

        // WHEN
        const auto graphUrl = QUrl::fromEncoded("qrc:/input.json");
        backend.setShaderGraph(type, graphUrl);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());

        // WHEN
        backend.setGraphicsApi(gl3Api);
        backend.generateCode(type);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(!backend.isShaderCodeDirty(type));
        QCOMPARE(backend.shaderCode(type), gl3Code);

        // WHEN
        backend.setGraphicsApi(es2Api);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(backend.isShaderCodeDirty(type));
        QCOMPARE(backend.shaderCode(type), gl3Code);

        // WHEN
        backend.generateCode(type);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(!backend.isShaderCodeDirty(type));
        QCOMPARE(backend.shaderCode(type), es2Code);
    }

    void checkCodeUpdatedNotification_data()
    {
        QTest::addColumn<Qt3DRender::QShaderProgram::ShaderType>("type");
        QTest::addColumn<Qt3DRender::QShaderProgram::ShaderType>("notificationType");

        QTest::newRow("vertex") << Qt3DRender::QShaderProgram::Vertex << Qt3DRender::QShaderProgram::Vertex;
        QTest::newRow("tessControl") << Qt3DRender::QShaderProgram::TessellationControl << Qt3DRender::QShaderProgram::TessellationControl;
        QTest::newRow("tessEval") << Qt3DRender::QShaderProgram::TessellationEvaluation << Qt3DRender::QShaderProgram::TessellationEvaluation;
        QTest::newRow("geometry") << Qt3DRender::QShaderProgram::Geometry << Qt3DRender::QShaderProgram::Geometry;
        QTest::newRow("fragment") << Qt3DRender::QShaderProgram::Fragment << Qt3DRender::QShaderProgram::Fragment;
        QTest::newRow("compute") << Qt3DRender::QShaderProgram::Compute << Qt3DRender::QShaderProgram::Compute;
    }


    void checkCodeUpdatedNotification()
    {
        // GIVEN
        QSKIP("Disabled for Qt Base QShaderGenerator Integration");

        Qt3DRender::Render::ShaderBuilder::setPrototypesFile(":/prototypes.json");
        QVERIFY(!Qt3DRender::Render::ShaderBuilder::getPrototypeNames().isEmpty());
        QFETCH(Qt3DRender::QShaderProgram::ShaderType, type);
        QFETCH(Qt3DRender::QShaderProgram::ShaderType, notificationType);
        Q_UNUSED(notificationType)

        const auto gl3Api = []{
            auto api = Qt3DRender::GraphicsApiFilterData();
            api.m_api = Qt3DRender::QGraphicsApiFilter::OpenGL;
            api.m_profile = Qt3DRender::QGraphicsApiFilter::CoreProfile;
            api.m_major = 3;
            api.m_minor = 2;
            return api;
        }();

        const auto readCode = [](const QString &suffix) -> QByteArray {
            const auto filePath = QStringLiteral(":/output.") + suffix;
            QFile file(filePath);
            if (!file.open(QFile::ReadOnly | QFile::Text))
                qFatal("File open failed: %s", qPrintable(filePath));
            return file.readAll();
        };

        const auto gl3Code = readCode("gl3");

        Qt3DRender::Render::ShaderBuilder backend;
        TestArbiter arbiter;
        Qt3DCore::QBackendNodePrivate::get(&backend)->setArbiter(&arbiter);


        // WHEN
        const auto graphUrl = QUrl::fromEncoded("qrc:/input.json");
        backend.setShaderGraph(type, graphUrl);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(backend.isShaderCodeDirty(type));
        QVERIFY(backend.shaderCode(type).isEmpty());

        // WHEN
        backend.setGraphicsApi(gl3Api);
        backend.generateCode(type);

        // THEN
        QCOMPARE(backend.shaderGraph(type), graphUrl);
        QVERIFY(!backend.isShaderCodeDirty(type));
        QCOMPARE(backend.shaderCode(type), gl3Code);
    }
};

QTEST_MAIN(tst_ShaderBuilder)

#include "tst_shaderbuilder.moc"
