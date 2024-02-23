// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <glresourcemanagers_p.h>
#include <Qt3DCore/qnodeid.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_GLShaderManager : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void adopt();
    void lookupResource();
    void abandon();
    void insertAfterRemoval();
};

void tst_GLShaderManager::adopt()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    TestRenderer renderer;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;

    backendShaderNode1.setRenderer(&renderer);
    backendShaderNode2.setRenderer(&renderer);
    simulateInitializationSync(&frontendShader1, &backendShaderNode1);
    simulateInitializationSync(&frontendShader2, &backendShaderNode2);

    // THEN
    QVERIFY(cache.lookupResource(backendShaderNode1.peerId()) == nullptr);
    QVERIFY(cache.lookupResource(backendShaderNode2.peerId()) == nullptr);
    QVERIFY(backendShaderNode1.peerId() != backendShaderNode2.peerId());

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *glShader1 = cache.createOrAdoptExisting(&backendShaderNode1);

    // THEN
    QVERIFY(glShader1 != nullptr);
    std::vector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader1);
    QCOMPARE(shaderNodeIds.size(), 1U);
    QCOMPARE(shaderNodeIds.front(), backendShaderNode1.peerId());

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *glShader2 = cache.createOrAdoptExisting(&backendShaderNode2);

    // THEN
    QCOMPARE(glShader1, glShader2);

    shaderNodeIds = cache.shaderIdsForProgram(glShader2);
    QCOMPARE(shaderNodeIds.size(), 2U);
    QCOMPARE(shaderNodeIds.front(), backendShaderNode1.peerId());
    QCOMPARE(shaderNodeIds.back(), backendShaderNode2.peerId());
}

void tst_GLShaderManager::lookupResource()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    TestRenderer renderer;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;

    backendShaderNode1.setRenderer(&renderer);
    backendShaderNode2.setRenderer(&renderer);
    simulateInitializationSync(&frontendShader1, &backendShaderNode1);
    simulateInitializationSync(&frontendShader2, &backendShaderNode2);

    // WHEN
    cache.createOrAdoptExisting(&backendShaderNode1);
    cache.createOrAdoptExisting(&backendShaderNode2);

    // THEN
    Qt3DRender::Render::OpenGL::GLShader *glShader1 = cache.lookupResource(backendShaderNode1.peerId());
    Qt3DRender::Render::OpenGL::GLShader *glShader2 = cache.lookupResource(backendShaderNode2.peerId());
    QVERIFY(glShader1 != nullptr);
    QCOMPARE(glShader1, glShader2);
    const std::vector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader1);
    QCOMPARE(shaderNodeIds.size(), 2U);
    QVERIFY(std::find(shaderNodeIds.begin(),
                      shaderNodeIds.end(),
                      frontendShader1.id()) != shaderNodeIds.end());
    QVERIFY(std::find(shaderNodeIds.begin(),
                      shaderNodeIds.end(),
                      frontendShader2.id()) != shaderNodeIds.end());
}

void tst_GLShaderManager::abandon()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    TestRenderer renderer;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;

    backendShaderNode1.setRenderer(&renderer);
    backendShaderNode2.setRenderer(&renderer);
    simulateInitializationSync(&frontendShader1, &backendShaderNode1);
    simulateInitializationSync(&frontendShader2, &backendShaderNode2);
    cache.createOrAdoptExisting(&backendShaderNode1);
    cache.createOrAdoptExisting(&backendShaderNode2);

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *glShader = cache.lookupResource(backendShaderNode1.peerId());
    cache.abandon(glShader, &backendShaderNode1);

    // THEN
    std::vector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader);
    QVERIFY(cache.takeAbandonned().empty());
    QCOMPARE(shaderNodeIds.size(), 1U);
    QCOMPARE(shaderNodeIds.front(), backendShaderNode2.peerId());

    // WHEN
    cache.abandon(glShader, &backendShaderNode2);

    // THEN
    shaderNodeIds = cache.shaderIdsForProgram(glShader);
    QCOMPARE(shaderNodeIds.size(), 0U);
    const std::vector<Qt3DRender::Render::OpenGL::GLShader *> releasedShaders = cache.takeAbandonned();
    QCOMPARE(releasedShaders.size(), 1U);
    QCOMPARE(releasedShaders.front(), glShader);
}

void tst_GLShaderManager::insertAfterRemoval()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader;
    TestRenderer renderer;
    Qt3DRender::Render::Shader backendShaderNode;


    backendShaderNode.setRenderer(&renderer);
    simulateInitializationSync(&frontendShader, &backendShaderNode);

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *apiShader1 = cache.createOrAdoptExisting(&backendShaderNode);
    const Qt3DRender::Render::OpenGL::GLShader *originalApiShader = apiShader1;

    // THEN
    auto apiShader2 = cache.lookupResource(frontendShader.id());
    QVERIFY(apiShader1 != nullptr);
    QVERIFY(apiShader2 != nullptr);
    QVERIFY(apiShader1 == originalApiShader);
    QVERIFY(apiShader1 == apiShader2);

    // WHEN
    cache.abandon(apiShader1, &backendShaderNode);

    // THEN
    Qt3DRender::Render::OpenGL::GLShader *apiShaderEmpty = cache.lookupResource(frontendShader.id());
    QVERIFY(apiShaderEmpty == nullptr);

    // WHEN
    apiShader1 = cache.createOrAdoptExisting(&backendShaderNode);
    cache.purge();
    apiShader2 = cache.lookupResource(frontendShader.id());

    // THEN
    QVERIFY(apiShader1 != nullptr);
    QVERIFY(apiShader2 != nullptr);
    QVERIFY(apiShader1 == apiShader2);
    QVERIFY(apiShader2 == originalApiShader);
}

QTEST_APPLESS_MAIN(tst_GLShaderManager)

#include "tst_glshadermanager.moc"
