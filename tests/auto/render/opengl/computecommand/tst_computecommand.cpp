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
#include <renderer_p.h>
#include "testrenderer.h"
#include "testarbiter.h"
#include "qbackendnodetester.h"

// tst_Renderer is a friend class of Renderer
class tst_Renderer : public Qt3DRender::Render::OpenGL::Renderer
{
public:
    tst_Renderer()
        : Qt3DRender::Render::OpenGL::Renderer()
    {}

    ~tst_Renderer() {
        shutdown();
    }
};


class tst_ComputeCommand : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkDisablesFrontend()
    {
        // GIVEN
        Qt3DRender::Render::NodeManagers nodeManager;
        tst_Renderer renderer;

        Qt3DCore::QAspectManager manager;
        Qt3DCore::QScene scene;

        Qt3DCore::QEntity rootEntity;
        Qt3DCore::QNodePrivate::get(&rootEntity)->setScene(&scene);

        Qt3DRender::QComputeCommand computeCommand;
        Qt3DRender::Render::ComputeCommand *backendComputeCommand = nullptr;

        renderer.setNodeManagers(&nodeManager);

        // WHEN
        computeCommand.setParent(&rootEntity);
        // RootEntity is the entry point to retrieve the scene instance for lookups
        manager.setRootEntity(&rootEntity, {});

        // THEN
        QVERIFY(scene.lookupNode(computeCommand.id()) != nullptr);

        // WHEN
        auto handle = nodeManager.computeJobManager()->getOrAcquireHandle(computeCommand.id());
        backendComputeCommand = nodeManager.computeJobManager()->data(handle);

        // WHEN
        computeCommand.setWorkGroupX(256);
        computeCommand.setWorkGroupY(512);
        computeCommand.setWorkGroupZ(128);
        computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
        computeCommand.trigger(1);

        backendComputeCommand->setRenderer(&renderer);
        simulateInitializationSync(&computeCommand, backendComputeCommand);

        // THEN
        QCOMPARE(backendComputeCommand->frameCount(),1);
        QCOMPARE(backendComputeCommand->isEnabled(), true);
        QCOMPARE(computeCommand.isEnabled(), true);
        QCOMPARE(backendComputeCommand->hasReachedFrameCount(), false);

        // WHEN
        backendComputeCommand->updateFrameCount();

        // THEN
        QCOMPARE(backendComputeCommand->frameCount(), 0);
        QCOMPARE(backendComputeCommand->hasReachedFrameCount(), true);


        // Still enabled as we have yet to notify the fronted
        QCOMPARE(backendComputeCommand->isEnabled(), true);
        QCOMPARE(computeCommand.isEnabled(), true);

        // WHEN
        renderer.jobsDone(&manager);    // so Renderer::sendDisablesToFrontend gets called

        // THEN
        QCOMPARE(computeCommand.isEnabled(), false);
        QCOMPARE(backendComputeCommand->hasReachedFrameCount(), false);

        // WHEN
        backendComputeCommand->syncFromFrontEnd(&computeCommand, false);

        // THEN
        QCOMPARE(backendComputeCommand->frameCount(), 0);
        QCOMPARE(backendComputeCommand->isEnabled(), false);
    }
};

QTEST_MAIN(tst_ComputeCommand)

#include "tst_computecommand.moc"
