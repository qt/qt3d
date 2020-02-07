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
    simulateInitialization(&frontendShader1, &backendShaderNode1);
    simulateInitialization(&frontendShader2, &backendShaderNode2);

    // THEN
    QVERIFY(cache.lookupResource(backendShaderNode1.peerId()) == nullptr);
    QVERIFY(cache.lookupResource(backendShaderNode2.peerId()) == nullptr);
    QVERIFY(backendShaderNode1.peerId() != backendShaderNode2.peerId());

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *glShader1 = cache.createOrAdoptExisting(&backendShaderNode1);

    // THEN
    QVERIFY(glShader1 != nullptr);
    QVector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader1);
    QCOMPARE(shaderNodeIds.size(), 1);
    QCOMPARE(shaderNodeIds.first(), backendShaderNode1.peerId());

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *glShader2 = cache.createOrAdoptExisting(&backendShaderNode2);

    // THEN
    QCOMPARE(glShader1, glShader2);

    shaderNodeIds = cache.shaderIdsForProgram(glShader2);
    QCOMPARE(shaderNodeIds.size(), 2);
    QCOMPARE(shaderNodeIds.first(), backendShaderNode1.peerId());
    QCOMPARE(shaderNodeIds.last(), backendShaderNode2.peerId());
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
    simulateInitialization(&frontendShader1, &backendShaderNode1);
    simulateInitialization(&frontendShader2, &backendShaderNode2);

    // WHEN
    cache.createOrAdoptExisting(&backendShaderNode1);
    cache.createOrAdoptExisting(&backendShaderNode2);

    // THEN
    Qt3DRender::Render::OpenGL::GLShader *glShader1 = cache.lookupResource(backendShaderNode1.peerId());
    Qt3DRender::Render::OpenGL::GLShader *glShader2 = cache.lookupResource(backendShaderNode2.peerId());
    QVERIFY(glShader1 != nullptr);
    QCOMPARE(glShader1, glShader2);
    const QVector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader1);
    QCOMPARE(shaderNodeIds.size(), 2);
    QVERIFY(shaderNodeIds.contains(frontendShader1.id()));
    QVERIFY(shaderNodeIds.contains(frontendShader2.id()));
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
    simulateInitialization(&frontendShader1, &backendShaderNode1);
    simulateInitialization(&frontendShader2, &backendShaderNode2);
    cache.createOrAdoptExisting(&backendShaderNode1);
    cache.createOrAdoptExisting(&backendShaderNode2);

    // WHEN
    Qt3DRender::Render::OpenGL::GLShader *glShader = cache.lookupResource(backendShaderNode1.peerId());
    cache.abandon(glShader, &backendShaderNode1);

    // THEN
    QVector<Qt3DCore::QNodeId> shaderNodeIds = cache.shaderIdsForProgram(glShader);
    QVERIFY(cache.takeAbandonned().isEmpty());
    QCOMPARE(shaderNodeIds.size(), 1);
    QCOMPARE(shaderNodeIds.first(), backendShaderNode2.peerId());

    // WHEN
    cache.abandon(glShader, &backendShaderNode2);

    // THEN
    shaderNodeIds = cache.shaderIdsForProgram(glShader);
    QCOMPARE(shaderNodeIds.size(), 0);
    const QVector<Qt3DRender::Render::OpenGL::GLShader *> releasedShaders = cache.takeAbandonned();
    QCOMPARE(releasedShaders.size(), 1);
    QCOMPARE(releasedShaders.first(), glShader);
}

void tst_GLShaderManager::insertAfterRemoval()
{
    // GIVEN
    Qt3DRender::Render::OpenGL::GLShaderManager cache;
    Qt3DRender::QShaderProgram frontendShader;
    TestRenderer renderer;
    Qt3DRender::Render::Shader backendShaderNode;


    backendShaderNode.setRenderer(&renderer);
    simulateInitialization(&frontendShader, &backendShaderNode);

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
