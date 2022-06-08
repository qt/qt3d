// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/private/qrendertarget_p.h>
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/managers_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_RenderTarget : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::RenderTarget backendRenderTarget;

        // THEN
        QCOMPARE(backendRenderTarget.isEnabled(), false);
        QVERIFY(backendRenderTarget.peerId().isNull());
        QVERIFY(backendRenderTarget.renderOutputs().empty());
        QCOMPARE(backendRenderTarget.isDirty(), false);
    }

    void checkCleanupState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::RenderTarget backendRenderTarget;
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::QRenderTargetOutput renderTargetOuput;
        renderTarget.addOutput(&renderTargetOuput);

        // WHEN
        backendRenderTarget.setRenderer(&renderer);
        simulateInitializationSync(&renderTarget, &backendRenderTarget);
        backendRenderTarget.cleanup();

        // THEN
        QCOMPARE(backendRenderTarget.isEnabled(), false);
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
        QCOMPARE(backendRenderTarget.isDirty(), false);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::QRenderTargetOutput renderTargetOuput;
        renderTarget.addOutput(&renderTargetOuput);

        {
            // WHEN
            Qt3DRender::Render::RenderTarget backendRenderTarget;
            backendRenderTarget.setRenderer(&renderer);
            simulateInitializationSync(&renderTarget, &backendRenderTarget);
            // THEN
            QCOMPARE(backendRenderTarget.isEnabled(), true);
            QCOMPARE(backendRenderTarget.peerId(), renderTarget.id());
            QCOMPARE(backendRenderTarget.renderOutputs(), (QList<Qt3DCore::QNodeId> { renderTargetOuput.id() }));
            QCOMPARE(backendRenderTarget.isDirty(), true);

            backendRenderTarget.unsetDirty();
        }
        {
            // WHEN
            Qt3DRender::Render::RenderTarget backendRenderTarget;
            renderTarget.setEnabled(false);
            backendRenderTarget.setRenderer(&renderer);
            simulateInitializationSync(&renderTarget, &backendRenderTarget);

            // THEN
            QCOMPARE(backendRenderTarget.peerId(), renderTarget.id());
            QCOMPARE(backendRenderTarget.isEnabled(), false);
            QCOMPARE(backendRenderTarget.isDirty(), true);

            backendRenderTarget.unsetDirty();
        }
    }

    void checkAddNoDuplicateOutput()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::RenderTarget backendRenderTarget;
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::QRenderTargetOutput renderTargetOuput;

        // THEN
        backendRenderTarget.setRenderer(&renderer);
        simulateInitializationSync(&renderTarget, &backendRenderTarget);
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
        backendRenderTarget.unsetDirty();

        // WHEN
        renderTarget.addOutput(&renderTargetOuput);
        backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);
        QCOMPARE(backendRenderTarget.isDirty(), true);
        backendRenderTarget.unsetDirty();

        // WHEN
        renderTarget.addOutput(&renderTargetOuput);
        backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);
        QCOMPARE(backendRenderTarget.isDirty(), false);
    }

    void checkAddRemoveOutput()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::RenderTarget backendRenderTarget;
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::QRenderTargetOutput renderTargetOuput;

        // THEN
        backendRenderTarget.setRenderer(&renderer);
        simulateInitializationSync(&renderTarget, &backendRenderTarget);
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
        backendRenderTarget.unsetDirty();

        // WHEN
        Qt3DRender::QRenderTargetOutput renderTargetOuput1;
        Qt3DRender::QRenderTargetOutput renderTargetOuput2;
        renderTarget.addOutput(&renderTargetOuput1);
        renderTarget.addOutput(&renderTargetOuput2);

        // THEN
        backendRenderTarget.syncFromFrontEnd(&renderTarget, false);
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 2);
        QCOMPARE(backendRenderTarget.isDirty(), true);
        backendRenderTarget.unsetDirty();

        // WHEN
        renderTarget.removeOutput(&renderTargetOuput1);
        backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);
        QCOMPARE(backendRenderTarget.isDirty(), true);
        backendRenderTarget.unsetDirty();

        // WHEN
        renderTarget.removeOutput(&renderTargetOuput1);
        backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);
        QCOMPARE(backendRenderTarget.isDirty(), false);

        // WHEN
        renderTarget.removeOutput(&renderTargetOuput2);
        backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
        QCOMPARE(backendRenderTarget.isDirty(), true);
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::Render::RenderTarget backendRenderTarget;
        TestRenderer renderer;
        backendRenderTarget.setRenderer(&renderer);
        simulateInitializationSync(&renderTarget, &backendRenderTarget);
        backendRenderTarget.unsetDirty();

        {
            // WHEN
            const bool newValue = false;
            renderTarget.setEnabled(newValue);
            backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

            // THEN
            QCOMPARE(backendRenderTarget.isEnabled(), newValue);
        }
        {
            // WHEN
            Qt3DRender::QRenderTargetOutput targetOutput;
            renderTarget.addOutput(&targetOutput);
            backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

            // THEN
            QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);
            QCOMPARE(backendRenderTarget.renderOutputs().first(), targetOutput.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            QCOMPARE(backendRenderTarget.isDirty(), true);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendRenderTarget.unsetDirty();

            // WHEN
            renderTarget.removeOutput(&targetOutput);
            backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

            // THEN
            QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            QCOMPARE(backendRenderTarget.isDirty(), true);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
            backendRenderTarget.unsetDirty();
        }
    }

    void checkRenderTargetManager()
    {
        // GIVEN
        Qt3DRender::QRenderTarget renderTarget;
        TestRenderer renderer;
        Qt3DRender::Render::RenderTargetManager manager;
        Qt3DRender::Render::RenderTargetFunctor creationFunctor(&renderer, &manager);

        // THEN
        QVERIFY(manager.renderTargetIdsToCleanup().isEmpty());

        // WHEN
        auto backend = creationFunctor.create(renderTarget.id());

        // THEN
        QVERIFY(backend != nullptr);
        QVERIFY(manager.renderTargetIdsToCleanup().isEmpty());

        {
            // WHEN
            auto sameBackend = creationFunctor.get(renderTarget.id());
            // THEN
            QCOMPARE(backend, sameBackend);
        }

        // WHEN
        creationFunctor.destroy(renderTarget.id());

        // THEN -> Should be in list of ids to remove and return null on get
        QVERIFY(manager.renderTargetIdsToCleanup().contains(renderTarget.id()));
        QVERIFY(creationFunctor.get(renderTarget.id()) == nullptr);

        // WHEN -> Should be removed from list of ids to remove
        creationFunctor.create(renderTarget.id());

        // THEN
        QVERIFY(manager.renderTargetIdsToCleanup().isEmpty());
        QVERIFY(creationFunctor.get(renderTarget.id()) != nullptr);
    }
};

QTEST_MAIN(tst_RenderTarget)

#include "tst_rendertarget.moc"
