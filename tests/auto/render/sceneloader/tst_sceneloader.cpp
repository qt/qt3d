// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/private/scene_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/private/qentity_p.h>
#include "testarbiter.h"
#include "testrenderer.h"

class tst_SceneLoader : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Scene sceneLoader;
        Qt3DRender::Render::SceneManager sceneManager;

        // THEN
        QVERIFY(sceneLoader.source().isEmpty());
        QVERIFY(sceneLoader.peerId().isNull());
        QVERIFY(sceneManager.takePendingSceneLoaderJobs().empty());


        // GIVEN
        Qt3DRender::QSceneLoader frontendSceneLoader;
        const QUrl newUrl(QStringLiteral("CorvetteMuseum"));
        frontendSceneLoader.setSource(newUrl);

        // WHEN
        sceneLoader.setRenderer(&renderer);
        sceneLoader.setSceneManager(&sceneManager);
        simulateInitializationSync(&frontendSceneLoader, &sceneLoader);

        // THEN
        QVERIFY(!sceneLoader.peerId().isNull());
        QCOMPARE(sceneLoader.source(), newUrl);

        // WHEN
        sceneLoader.cleanup();

        // THEN
        QVERIFY(sceneLoader.source().isEmpty());
    }

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QSceneLoader frontendSceneLoader;
        frontendSceneLoader.setSource(QUrl(QStringLiteral("file:///CorvetteMuseum")));

        Qt3DRender::Render::Scene sceneLoader;
        Qt3DRender::Render::SceneManager sceneManager;

        // WHEN
        sceneLoader.setRenderer(&renderer);
        sceneLoader.setSceneManager(&sceneManager);
        simulateInitializationSync(&frontendSceneLoader, &sceneLoader);

        // THEN
        QCOMPARE(sceneLoader.peerId(), frontendSceneLoader.id());
        QCOMPARE(sceneLoader.source(), frontendSceneLoader.source());
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().empty());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Scene sceneLoader;
        Qt3DRender::Render::SceneManager sceneManager;
        Qt3DRender::QSceneLoader frontendSceneLoader;

        sceneLoader.setRenderer(&renderer);
        sceneLoader.setSceneManager(&sceneManager);
        simulateInitializationSync(&frontendSceneLoader, &sceneLoader);

        // THEN
        QVERIFY(sceneManager.takePendingSceneLoaderJobs().empty());

        // WHEN
        const QUrl newUrl(QStringLiteral("file:///Bownling_Green_KY"));
        frontendSceneLoader.setSource(newUrl);
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN
        QCOMPARE(sceneLoader.source(), newUrl);
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().empty());

        // WHEN
        frontendSceneLoader.setEnabled(false);
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN
        QCOMPARE(sceneLoader.isEnabled(), false);
    }

    void checkProcessEmptyPath()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Scene sceneLoader;
        Qt3DRender::Render::SceneManager sceneManager;
        Qt3DRender::QSceneLoader frontendSceneLoader;

        sceneLoader.setRenderer(&renderer);
        sceneLoader.setSceneManager(&sceneManager);
        simulateInitializationSync(&frontendSceneLoader, &sceneLoader);

        // THEN
        QVERIFY(sceneManager.takePendingSceneLoaderJobs().empty());

        // WHEN
        const QUrl newUrl(QStringLiteral("file:///Bownling_Green_KY"));
        frontendSceneLoader.setSource(newUrl);
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN
        QCOMPARE(sceneLoader.source(), newUrl);
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().empty());

        // WHEN
        frontendSceneLoader.setSource(QUrl());
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN -> we should still have generated a job to reset the scene (immediately)
        QCOMPARE(sceneLoader.source(), QUrl());
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().empty());
    }
};

// Note: setSceneSubtree needs a QCoreApplication
QTEST_MAIN(tst_SceneLoader)

#include "tst_sceneloader.moc"
