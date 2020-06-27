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
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include "testrenderer.h"

class tst_RenderShader : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private slots:

    void hasCoherentInitialState();
    void matchesFrontendPeer();
    void cleanupLeavesACoherentState();
    void dealWithPropertyChanges_data();
    void dealWithPropertyChanges();
    void dealWithFormatChanges();
    void checkSetRendererDirtyOnInitialization();
    void allowToChangeShaderCode_data();
    void allowToChangeShaderCode();
    void checkShaderManager();
};


Qt3DRender::QShaderProgram *createFrontendShader(Qt3DRender::QShaderProgram::Format format = Qt3DRender::QShaderProgram::GLSL)
{
    Qt3DRender::QShaderProgram *shader = new Qt3DRender::QShaderProgram();
    shader->setFormat(format);

    shader->setVertexShaderCode(QByteArrayLiteral(
                                    "#version 150"\
                                    "in vec3 vertexPosition;"\
                                    "in vec3 vertexColor; "\
                                    "out vec3 color;"\
                                    "void main()"\
                                    "{"\
                                    "    color = vertexColor;"\
                                    "    gl_Position = vec4( vertexPosition, 1.0 );"\
                                    "}"));

    shader->setFragmentShaderCode(QByteArrayLiteral(
                                      "#version 150"\
                                      "in vec3 color;"\
                                      "out vec4 fragColor;"\
                                      "void main()"\
                                      "{"\
                                      "   fragColor = vec4( color, 1.0 );"\
                                      "}"));

    return shader;
}

void tst_RenderShader::hasCoherentInitialState()
{
    Qt3DRender::Render::Shader *shader = new Qt3DRender::Render::Shader();

    QCOMPARE(shader->status(), Qt3DRender::QShaderProgram::NotReady);
    QCOMPARE(shader->format(), Qt3DRender::QShaderProgram::GLSL);
    QVERIFY(shader->log().isEmpty());
    QCOMPARE(shader->isDirty(), false);
}

void tst_RenderShader::matchesFrontendPeer()
{
    QScopedPointer<Qt3DRender::QShaderProgram> frontend(createFrontendShader());
    TestRenderer renderer;
    Qt3DRender::Render::Shader backend;

    backend.setRenderer(&renderer);
    simulateInitializationSync(frontend.data(), &backend);
    QCOMPARE(backend.isDirty(), true);

    for (int i = Qt3DRender::QShaderProgram::Vertex; i <= Qt3DRender::QShaderProgram::Compute; ++i)
        QCOMPARE(backend.shaderCode()[i],
                 frontend->shaderCode(static_cast<Qt3DRender::QShaderProgram::ShaderType>(i)));
    QCOMPARE(backend.format(), frontend->format());
}

void tst_RenderShader::cleanupLeavesACoherentState()
{
    QScopedPointer<Qt3DRender::QShaderProgram> frontend(createFrontendShader(Qt3DRender::QShaderProgram::SPIRV));
    TestRenderer renderer;
    Qt3DRender::Render::Shader shader;

    shader.setRenderer(&renderer);
    simulateInitializationSync(frontend.data(), &shader);

    shader.cleanup();

    QCOMPARE(shader.isDirty(), false);
    QCOMPARE(shader.status(), Qt3DRender::QShaderProgram::NotReady);
    QCOMPARE(shader.format(), Qt3DRender::QShaderProgram::GLSL);
}

void tst_RenderShader::dealWithPropertyChanges_data()
{
    QTest::addColumn<Qt3DRender::QShaderProgram::ShaderType>("type");

    QTest::newRow("vertex") << Qt3DRender::QShaderProgram::Vertex;

    QTest::newRow("tessControl") << Qt3DRender::QShaderProgram::TessellationControl;

    QTest::newRow("tessEval") << Qt3DRender::QShaderProgram::TessellationEvaluation;

    QTest::newRow("geometry") << Qt3DRender::QShaderProgram::Geometry;

    QTest::newRow("fragment") << Qt3DRender::QShaderProgram::Fragment;

    QTest::newRow("compute") << Qt3DRender::QShaderProgram::Compute;
}

void tst_RenderShader::dealWithPropertyChanges()
{
    // GIVEN
    QFETCH(Qt3DRender::QShaderProgram::ShaderType, type);

    Qt3DRender::Render::Shader backend;
    Qt3DRender::QShaderProgram shader;

    TestRenderer renderer;
    backend.setRenderer(&renderer);
    simulateInitializationSync(&shader, &backend);

    // WHEN
    shader.setShaderCode(type, QByteArrayLiteral("foo"));
    backend.syncFromFrontEnd(&shader, false);

    // THEN
    QCOMPARE(backend.shaderCode().at(type), QByteArrayLiteral("foo"));
    QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
    QCOMPARE(backend.isDirty(), true);

    renderer.resetDirty();
    QCOMPARE(renderer.dirtyBits(), 0);
    backend.unsetDirty();

    // WHEN
    shader.setShaderCode(type, QByteArrayLiteral("foo"));
    backend.syncFromFrontEnd(&shader, false);

    // THEN
    QCOMPARE(backend.shaderCode().at(type), QByteArrayLiteral("foo"));
    QCOMPARE(renderer.dirtyBits(), 0);
    QCOMPARE(backend.isDirty(), false);

    // WHEN
    shader.setShaderCode(type, QByteArrayLiteral("bar"));
    backend.syncFromFrontEnd(&shader, false);

    // THEN
    QCOMPARE(backend.shaderCode().at(type), QByteArrayLiteral("bar"));
    QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
    renderer.resetDirty();
    QCOMPARE(backend.isDirty(), true);
}

void tst_RenderShader::dealWithFormatChanges()
{
    // GIVEN
    Qt3DRender::Render::Shader backend;
    Qt3DRender::QShaderProgram shader;
    TestRenderer renderer;
    backend.setRenderer(&renderer);
    simulateInitializationSync(&shader, &backend);

    // WHEN
    shader.setFormat(Qt3DRender::QShaderProgram::GLSL);
    backend.syncFromFrontEnd(&shader, false);

    // THEN
    QCOMPARE(backend.format(), Qt3DRender::QShaderProgram::GLSL);
    QCOMPARE(backend.isDirty(), false);
    QCOMPARE(renderer.dirtyBits(), 0);

    // WHEN
    shader.setFormat(Qt3DRender::QShaderProgram::SPIRV);
    backend.syncFromFrontEnd(&shader, false);

    // THEN
    QCOMPARE(backend.format(), Qt3DRender::QShaderProgram::SPIRV);
    QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
    QCOMPARE(backend.isDirty(), true);

    renderer.resetDirty();
    backend.unsetDirty();

    // WHEN
    shader.setFormat(Qt3DRender::QShaderProgram::SPIRV);
    backend.syncFromFrontEnd(&shader, false);

    // THEN
    QCOMPARE(backend.isDirty(), false);
    QCOMPARE(renderer.dirtyBits(), 0);
}

void tst_RenderShader::checkSetRendererDirtyOnInitialization()
{
    // GIVEN
    QScopedPointer<Qt3DRender::QShaderProgram> frontend(createFrontendShader());
    Qt3DRender::Render::Shader shader;
    TestRenderer renderer;

    shader.setRenderer(&renderer);

    // THEN
    QCOMPARE(renderer.dirtyBits(), 0);

    // WHEN
    simulateInitializationSync(frontend.data(), &shader);

    // THEN
    QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
}

void tst_RenderShader::allowToChangeShaderCode_data()
{
    dealWithPropertyChanges_data();
}

void tst_RenderShader::allowToChangeShaderCode()
{
    // GIVEN
    QFETCH(Qt3DRender::QShaderProgram::ShaderType, type);

    Qt3DRender::Render::Shader backend;
    TestRenderer renderer;
    backend.setRenderer(&renderer);

    // WHEN
    backend.setShaderCode(type, QByteArrayLiteral("foo"));

    // THEN
    QCOMPARE(backend.shaderCode().at(type), QByteArrayLiteral("foo"));
    QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
    renderer.resetDirty();

    // WHEN
    backend.setShaderCode(type, QByteArrayLiteral("foo"));

    // THEN
    QCOMPARE(backend.shaderCode().at(type), QByteArrayLiteral("foo"));
    QCOMPARE(renderer.dirtyBits(), 0);
    renderer.resetDirty();

    // WHEN
    backend.setShaderCode(type, QByteArrayLiteral("bar"));

    // THEN
    QCOMPARE(backend.shaderCode().at(type), QByteArrayLiteral("bar"));
    QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::ShadersDirty);
    renderer.resetDirty();
}

void tst_RenderShader::checkShaderManager()
{
    // GIVEN
    Qt3DRender::QShaderProgram shader;
    TestRenderer renderer;
    Qt3DRender::Render::ShaderManager manager;
    Qt3DRender::Render::ShaderFunctor creationFunctor(&renderer, &manager);

    // THEN
    QVERIFY(manager.shaderIdsToCleanup().isEmpty());

    // WHEN
    Qt3DCore::QNodeCreatedChangeBase changeObj(&shader);
    Qt3DCore::QNodeCreatedChangeBasePtr changePtr(&changeObj, [](Qt3DCore::QNodeCreatedChangeBase *) {});
    auto backend = creationFunctor.create(changePtr);

    // THEN
    QVERIFY(backend != nullptr);
    QVERIFY(manager.shaderIdsToCleanup().isEmpty());

    {
        // WHEN
        auto sameBackend = creationFunctor.get(shader.id());
        // THEN
        QCOMPARE(backend, sameBackend);
    }

    // WHEN
    creationFunctor.destroy(shader.id());

    // THEN -> Should be in list of ids to remove and return null on get
    QVERIFY(manager.hasShaderIdToCleanup(shader.id()));
    QVERIFY(creationFunctor.get(shader.id()) == nullptr);

    // WHEN -> Should be removed from list of ids to remove
    creationFunctor.create(changePtr);

    // THEN
    QVERIFY(manager.shaderIdsToCleanup().isEmpty());
    QCOMPARE(creationFunctor.get(shader.id()), backend);
}

QTEST_APPLESS_MAIN(tst_RenderShader)

#include "tst_shader.moc"
