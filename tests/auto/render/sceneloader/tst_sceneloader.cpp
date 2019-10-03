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
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/private/scene_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>
#include "testpostmanarbiter.h"
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
        QVERIFY(sceneManager.takePendingSceneLoaderJobs().isEmpty());


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
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().isEmpty());
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
        QVERIFY(sceneManager.takePendingSceneLoaderJobs().isEmpty());

        // WHEN
        const QUrl newUrl(QStringLiteral("file:///Bownling_Green_KY"));
        frontendSceneLoader.setSource(newUrl);
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN
        QCOMPARE(sceneLoader.source(), newUrl);
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().isEmpty());

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
        QVERIFY(sceneManager.takePendingSceneLoaderJobs().isEmpty());

        // WHEN
        const QUrl newUrl(QStringLiteral("file:///Bownling_Green_KY"));
        frontendSceneLoader.setSource(newUrl);
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN
        QCOMPARE(sceneLoader.source(), newUrl);
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().isEmpty());

        // WHEN
        frontendSceneLoader.setSource(QUrl());
        sceneLoader.syncFromFrontEnd(&frontendSceneLoader, false);

        // THEN -> we should still have generated a job to reset the scene (immediately)
        QCOMPARE(sceneLoader.source(), QUrl());
        QVERIFY(!sceneManager.takePendingSceneLoaderJobs().isEmpty());
    }
};

// Note: setSceneSubtree needs a QCoreApplication
QTEST_MAIN(tst_SceneLoader)

#include "tst_sceneloader.moc"
