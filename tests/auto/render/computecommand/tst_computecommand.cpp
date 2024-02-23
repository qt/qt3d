// Copyright (C) 2016  Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/private/qcomputecommand_p.h>
#include <Qt3DRender/private/computecommand_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"
#include "testarbiter.h"


class tst_ComputeCommand : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::ComputeCommand backendComputeCommand;

        // THEN
        QCOMPARE(backendComputeCommand.isEnabled(), false);
        QVERIFY(backendComputeCommand.peerId().isNull());
        QCOMPARE(backendComputeCommand.x(), 1);
        QCOMPARE(backendComputeCommand.y(), 1);
        QCOMPARE(backendComputeCommand.z(), 1);
        QCOMPARE(backendComputeCommand.hasReachedFrameCount(), false);
        QCOMPARE(backendComputeCommand.runType(), Qt3DRender::QComputeCommand::Continuous);
        QCOMPARE(backendComputeCommand.frameCount(), 0);
    }

    void checkCleanupState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::ComputeCommand backendComputeCommand;
        Qt3DRender::QComputeCommand computeCommand;
        computeCommand.setWorkGroupX(256);
        computeCommand.setWorkGroupY(512);
        computeCommand.setWorkGroupZ(128);
        computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
        computeCommand.trigger(1);

        // WHEN
        backendComputeCommand.setRenderer(&renderer);
        simulateInitializationSync(&computeCommand, &backendComputeCommand);

        backendComputeCommand.setEnabled(true);
        backendComputeCommand.hasReachedFrameCount();

        backendComputeCommand.cleanup();

        // THEN
        QCOMPARE(backendComputeCommand.isEnabled(), false);
        QCOMPARE(backendComputeCommand.hasReachedFrameCount(), false);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QComputeCommand computeCommand;
        computeCommand.setWorkGroupX(256);
        computeCommand.setWorkGroupY(512);
        computeCommand.setWorkGroupZ(128);
        computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
        computeCommand.trigger(6);

        {
            // WHEN
            Qt3DRender::Render::ComputeCommand backendComputeCommand;
            backendComputeCommand.setRenderer(&renderer);
            simulateInitializationSync(&computeCommand, &backendComputeCommand);

            // THEN
            QCOMPARE(backendComputeCommand.isEnabled(), true);
            QCOMPARE(backendComputeCommand.peerId(), computeCommand.id());
            QCOMPARE(backendComputeCommand.x(), computeCommand.workGroupX());
            QCOMPARE(backendComputeCommand.y(), computeCommand.workGroupY());
            QCOMPARE(backendComputeCommand.z(), computeCommand.workGroupZ());
            QCOMPARE(backendComputeCommand.runType(), computeCommand.runType());
            QCOMPARE(backendComputeCommand.frameCount(), 6);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::ComputeCommand backendComputeCommand;
            backendComputeCommand.setRenderer(&renderer);
            computeCommand.setEnabled(false);
            simulateInitializationSync(&computeCommand, &backendComputeCommand);

            // THEN
            QCOMPARE(backendComputeCommand.peerId(), computeCommand.id());
            QCOMPARE(backendComputeCommand.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::ComputeCommand backendComputeCommand;
        Qt3DRender::QComputeCommand computeCommand;
        TestRenderer renderer;
        backendComputeCommand.setRenderer(&renderer);
        simulateInitializationSync(&computeCommand, &backendComputeCommand);

        // THEN
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        {
            // WHEN
            const bool newValue = false;
            computeCommand.setEnabled(newValue);
            backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

            // THEN
            QCOMPARE(backendComputeCommand.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const int newValue = 128;
            computeCommand.setWorkGroupX(newValue);
            backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

            // THEN
            QCOMPARE(backendComputeCommand.x(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const int newValue = 64;
            computeCommand.setWorkGroupY(newValue);
            backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

            // THEN
            QCOMPARE(backendComputeCommand.y(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const int newValue = 32;
            computeCommand.setWorkGroupZ(newValue);
            backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

            // THEN
            QCOMPARE(backendComputeCommand.z(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const Qt3DRender::QComputeCommand::RunType newValue = Qt3DRender::QComputeCommand::Manual;
            computeCommand.setRunType(newValue);
            backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

            // THEN
            QCOMPARE(backendComputeCommand.runType(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const int newValue = 32;
            computeCommand.trigger(newValue);
            backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

            // THEN
            QCOMPARE(backendComputeCommand.frameCount(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::ComputeDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }

    void checkUpdateFrameCount()
    {
        // GIVEN
        TestRenderer renderer;

        Qt3DRender::QComputeCommand computeCommand;
        Qt3DRender::Render::ComputeCommand backendComputeCommand;

        computeCommand.setWorkGroupX(256);
        computeCommand.setWorkGroupY(512);
        computeCommand.setWorkGroupZ(128);
        computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
        computeCommand.trigger(6);


        backendComputeCommand.setRenderer(&renderer);
        simulateInitializationSync(&computeCommand, &backendComputeCommand);

        for (int i = 0; i < 5; ++i) {
            // WHEN
            backendComputeCommand.updateFrameCount();

            // THEN
            QCOMPARE(backendComputeCommand.frameCount(), 6 - (i + 1));
            QCOMPARE(backendComputeCommand.isEnabled(), true);
            QCOMPARE(backendComputeCommand.hasReachedFrameCount(), false);
        }

        // WHEN
        backendComputeCommand.updateFrameCount();

        // THEN
        QCOMPARE(backendComputeCommand.hasReachedFrameCount(), true);
        QCOMPARE(backendComputeCommand.frameCount(), 0);
        // Backend stays with enabled == true, frontend will be updated
        // to be disabled and backend should be disabled on the next sync

        // WHEN
        computeCommand.setEnabled(false);
        backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

        // THEN
        QCOMPARE(backendComputeCommand.isEnabled(), false);
        QCOMPARE(backendComputeCommand.frameCount(), 0);

        // WHEN
        computeCommand.trigger(1);
        backendComputeCommand.syncFromFrontEnd(&computeCommand, false);

        // THEN
        QCOMPARE(computeCommand.isEnabled(), true);
        QCOMPARE(backendComputeCommand.isEnabled(), true);
        QCOMPARE(backendComputeCommand.hasReachedFrameCount(), false);
        QCOMPARE(backendComputeCommand.frameCount(), 1);
    }
};

QTEST_MAIN(tst_ComputeCommand)

#include "tst_computecommand.moc"
