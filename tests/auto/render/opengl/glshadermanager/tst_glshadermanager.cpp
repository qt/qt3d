// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <glresourcemanagers_p.h>
#include <Qt3DCore/qnodeid.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

#include <atomic>
#include <thread>

namespace {

using GLShader = Qt3DRender::Render::OpenGL::GLShader;

bool contains(const std::vector<GLShader *> &shaders, GLShader *shader)
{
    return std::find(shaders.begin(), shaders.end(), shader) != shaders.end();
}

} // anonymous

class tst_GLShaderManager : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void initialState();
    void adopt();
    void lookupResource();
    void abandon();
    void insertAfterRemoval();
    void doesNotShareShaderWithDifferentCode();
    void adoptingTwiceIsNoop();
    void reusedShaderIsNoLongerAbandoned();
    void purgeOnlyReleasesAbandonedShaders();
    void takeActiveResources();
    void concurrentCreateOrAdoptExisting();

private:
    void initShaderNode(Qt3DRender::QShaderProgram &frontend,
                        Qt3DRender::Render::Shader &backend,
                        const QByteArray &vertexCode = {});

    TestRenderer m_renderer;
};

void tst_GLShaderManager::initialState()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;

    // THEN
    QVERIFY(cache.lookupResource(Qt3DCore::QNodeId::createId()) == nullptr);
    QVERIFY(cache.takeActiveResources().empty());
    QVERIFY(cache.takeAbandonned().empty());
}

void tst_GLShaderManager::initShaderNode(Qt3DRender::QShaderProgram &frontend,
                                         Qt3DRender::Render::Shader &backend,
                                         const QByteArray &vertexCode)
{
    frontend.setVertexShaderCode(vertexCode);
    backend.setRenderer(&m_renderer);
    simulateInitializationSync(&frontend, &backend);
}

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

void tst_GLShaderManager::doesNotShareShaderWithDifferentCode()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;
    initShaderNode(frontendShader1, backendShaderNode1, QByteArrayLiteral("vertex1"));
    initShaderNode(frontendShader2, backendShaderNode2, QByteArrayLiteral("vertex2"));

    // WHEN
    GLShader *glShader1 = cache.createOrAdoptExisting(&backendShaderNode1);
    // Renderer sets the code once the shader has been loaded
    glShader1->setShaderCode(backendShaderNode1.shaderCode());
    GLShader *glShader2 = cache.createOrAdoptExisting(&backendShaderNode2);
    glShader2->setShaderCode(backendShaderNode2.shaderCode());

    // THEN
    QVERIFY(glShader1 != nullptr);
    QVERIFY(glShader2 != nullptr);
    QVERIFY(glShader1 != glShader2);
    QCOMPARE(cache.lookupResource(backendShaderNode1.peerId()), glShader1);
    QCOMPARE(cache.lookupResource(backendShaderNode2.peerId()), glShader2);
    QCOMPARE(cache.shaderIdsForProgram(glShader1).size(), 1U);
    QCOMPARE(cache.shaderIdsForProgram(glShader2).size(), 1U);

    qDeleteAll(cache.takeActiveResources());
}

void tst_GLShaderManager::adoptingTwiceIsNoop()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader;
    Qt3DRender::Render::Shader backendShaderNode;
    initShaderNode(frontendShader, backendShaderNode);
    GLShader *glShader1 = cache.createOrAdoptExisting(&backendShaderNode);

    // WHEN
    GLShader *glShader2 = cache.createOrAdoptExisting(&backendShaderNode);

    // THEN
    QCOMPARE(glShader1, glShader2);
    const std::vector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader1);
    QCOMPARE(shaderNodeIds.size(), 1U);
    QCOMPARE(shaderNodeIds.front(), backendShaderNode.peerId());

    qDeleteAll(cache.takeActiveResources());
}

void tst_GLShaderManager::reusedShaderIsNoLongerAbandoned()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;
    initShaderNode(frontendShader1, backendShaderNode1, QByteArrayLiteral("vertex"));
    initShaderNode(frontendShader2, backendShaderNode2, QByteArrayLiteral("vertex"));

    // WHEN
    GLShader *glShader = cache.createOrAdoptExisting(&backendShaderNode1);
    glShader->setShaderCode(backendShaderNode1.shaderCode());
    cache.abandon(glShader, &backendShaderNode1);

    GLShader *reusedGLShader = cache.createOrAdoptExisting(&backendShaderNode2);

    // THEN
    QCOMPARE(reusedGLShader, glShader);
    QCOMPARE(cache.lookupResource(backendShaderNode2.peerId()), glShader);
    const std::vector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader);
    QCOMPARE(shaderNodeIds.size(), 1U);
    QCOMPARE(shaderNodeIds.front(), backendShaderNode2.peerId());

    // Must not be released by a later purge()
    QVERIFY(cache.takeAbandonned().empty());

    qDeleteAll(cache.takeActiveResources());
}

void tst_GLShaderManager::purgeOnlyReleasesAbandonedShaders()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;
    initShaderNode(frontendShader1, backendShaderNode1, QByteArrayLiteral("vertex1"));
    initShaderNode(frontendShader2, backendShaderNode2, QByteArrayLiteral("vertex2"));

    GLShader *glShader1 = cache.createOrAdoptExisting(&backendShaderNode1);
    glShader1->setShaderCode(backendShaderNode1.shaderCode());
    GLShader *glShader2 = cache.createOrAdoptExisting(&backendShaderNode2);
    glShader2->setShaderCode(backendShaderNode2.shaderCode());

    // WHEN
    cache.abandon(glShader1, &backendShaderNode1);
    cache.purge();

    // THEN
    QVERIFY(cache.lookupResource(backendShaderNode1.peerId()) == nullptr);
    QCOMPARE(cache.lookupResource(backendShaderNode2.peerId()), glShader2);
    QVERIFY(cache.takeAbandonned().empty());

    // WHEN
    const std::vector<GLShader *> activeShaders = cache.takeActiveResources();

    // THEN
    QCOMPARE(activeShaders.size(), 1U);
    QCOMPARE(activeShaders.front(), glShader2);

    qDeleteAll(activeShaders);
}

void tst_GLShaderManager::takeActiveResources()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader1;
    Qt3DRender::QShaderProgram frontendShader2;
    Qt3DRender::Render::Shader backendShaderNode1;
    Qt3DRender::Render::Shader backendShaderNode2;
    initShaderNode(frontendShader1, backendShaderNode1, QByteArrayLiteral("vertex1"));
    initShaderNode(frontendShader2, backendShaderNode2, QByteArrayLiteral("vertex2"));

    GLShader *activeShader = cache.createOrAdoptExisting(&backendShaderNode1);
    activeShader->setShaderCode(backendShaderNode1.shaderCode());
    GLShader *abandonedShader = cache.createOrAdoptExisting(&backendShaderNode2);
    abandonedShader->setShaderCode(backendShaderNode2.shaderCode());

    // THEN
    QCOMPARE(cache.lookupResource(backendShaderNode1.peerId()), activeShader);
    QCOMPARE(cache.lookupResource(backendShaderNode2.peerId()), abandonedShader);

    // WHEN
    cache.abandon(abandonedShader, &backendShaderNode2);

    // THEN
    QCOMPARE(cache.lookupResource(backendShaderNode1.peerId()), activeShader);
    QVERIFY(cache.lookupResource(backendShaderNode2.peerId()) == nullptr);

    // WHEN
    const std::vector<GLShader *> shaders = cache.takeActiveResources();

    // THEN -> returns both adopted and abandoned shaders
    QCOMPARE(shaders.size(), 2U);
    QVERIFY(contains(shaders, activeShader));
    QVERIFY(contains(shaders, abandonedShader));

    // THEN -> manager no longer references them
    QVERIFY(cache.lookupResource(backendShaderNode1.peerId()) == nullptr);
    QVERIFY(cache.lookupResource(backendShaderNode2.peerId()) == nullptr);
    QVERIFY(cache.shaderIdsForProgram(activeShader).empty());
    QVERIFY(cache.takeAbandonned().empty());
    QVERIFY(cache.takeActiveResources().empty());

    qDeleteAll(shaders);

    // WHEN -> manager can be reused after its resources were taken
    GLShader *newShader = cache.createOrAdoptExisting(&backendShaderNode1);

    // THEN
    QVERIFY(newShader != nullptr);
    QCOMPARE(cache.lookupResource(backendShaderNode1.peerId()), newShader);

    qDeleteAll(cache.takeActiveResources());
}

void tst_GLShaderManager::concurrentCreateOrAdoptExisting()
{
    constexpr int ThreadCount = 8;
    constexpr int Iterations = 50;

    for (int iteration = 0; iteration < Iterations; ++iteration) {
        // GIVEN
        Qt3DRender::Render::OpenGL::GLShaderManager cache;
        std::array<Qt3DRender::QShaderProgram, ThreadCount> frontendShaders;
        std::array<Qt3DRender::Render::Shader, ThreadCount> backendShaderNodes;
        std::array<GLShader *, ThreadCount> results = {};
        for (int i = 0; i < ThreadCount; ++i)
            initShaderNode(frontendShaders[i], backendShaderNodes[i]);

        // WHEN
        std::atomic<bool> start = false;
        std::vector<std::thread> threads;
        threads.reserve(ThreadCount);
        for (int i = 0; i < ThreadCount; ++i) {
            threads.emplace_back([&, i] {
                while (!start.load())
                    std::this_thread::yield();
                results[i] = cache.createOrAdoptExisting(&backendShaderNodes[i]);
                cache.lookupResource(backendShaderNodes[(i + 1) % ThreadCount].peerId());
            });
        }
        start.store(true);
        for (std::thread &t : threads)
            t.join();

        // THEN -> all nodes share a single GLShader since they have identical code
        QVERIFY(results[0] != nullptr);
        for (int i = 1; i < ThreadCount; ++i)
            QCOMPARE(results[i], results[0]);
        QCOMPARE(cache.shaderIdsForProgram(results[0]).size(), size_t(ThreadCount));
        for (int i = 0; i < ThreadCount; ++i)
            QCOMPARE(cache.lookupResource(backendShaderNodes[i].peerId()), results[0]);

        const std::vector<GLShader *> shaders = cache.takeActiveResources();
        QCOMPARE(shaders.size(), 1U);
        qDeleteAll(shaders);
    }
}

QTEST_APPLESS_MAIN(tst_GLShaderManager)

#include "tst_glshadermanager.moc"
