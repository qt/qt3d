/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DRender::Render::RenderTarget backendRenderTarget;

        // WHEN
        backendRenderTarget.setEnabled(true);
        backendRenderTarget.appendRenderOutput(Qt3DCore::QNodeId::createId());
        backendRenderTarget.appendRenderOutput(Qt3DCore::QNodeId::createId());

        backendRenderTarget.cleanup();

        // THEN
        QCOMPARE(backendRenderTarget.isEnabled(), false);
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
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
            QCOMPARE(backendRenderTarget.renderOutputs(), QVector<Qt3DCore::QNodeId>() << renderTargetOuput.id());
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
        }
    }

    void checkAddNoDuplicateOutput()
    {
        // GIVEN
        Qt3DRender::Render::RenderTarget backendRenderTarget;

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);

        // WHEN
        Qt3DCore::QNodeId renderTargetOutputId = Qt3DCore::QNodeId::createId();
        backendRenderTarget.appendRenderOutput(renderTargetOutputId);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);

        // WHEN
        backendRenderTarget.appendRenderOutput(renderTargetOutputId);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);
    }

    void checkAddRemoveOutput()
    {
        // GIVEN
        Qt3DRender::Render::RenderTarget backendRenderTarget;

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);

        // WHEN
        Qt3DCore::QNodeId renderTargetOutputId1 = Qt3DCore::QNodeId::createId();
        Qt3DCore::QNodeId renderTargetOutputId2 = Qt3DCore::QNodeId::createId();
        backendRenderTarget.appendRenderOutput(renderTargetOutputId1);
        backendRenderTarget.appendRenderOutput(renderTargetOutputId2);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 2);

        // WHEN
        backendRenderTarget.removeRenderOutput(Qt3DCore::QNodeId());

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 2);

        // WHEN
        backendRenderTarget.removeRenderOutput(renderTargetOutputId1);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);

        // WHEN
        backendRenderTarget.removeRenderOutput(renderTargetOutputId1);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 1);

        // WHEN
        backendRenderTarget.removeRenderOutput(renderTargetOutputId2);

        // THEN
        QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QRenderTarget renderTarget;
        Qt3DRender::Render::RenderTarget backendRenderTarget;
        TestRenderer renderer;
        backendRenderTarget.setRenderer(&renderer);
        simulateInitializationSync(&renderTarget, &backendRenderTarget);

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
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

            // WHEN
            renderTarget.removeOutput(&targetOutput);
            backendRenderTarget.syncFromFrontEnd(&renderTarget, false);

            // THEN
            QCOMPARE(backendRenderTarget.renderOutputs().size(), 0);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::AllDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
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
        Qt3DCore::QNodeCreatedChangeBase changeObj(&renderTarget);
        Qt3DCore::QNodeCreatedChangeBasePtr changePtr(&changeObj, [](Qt3DCore::QNodeCreatedChangeBase *) {});
        auto backend = creationFunctor.create(changePtr);

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
        creationFunctor.create(changePtr);

        // THEN
        QVERIFY(manager.renderTargetIdsToCleanup().isEmpty());
        QVERIFY(creationFunctor.get(renderTarget.id()) != nullptr);
    }
};

QTEST_MAIN(tst_RenderTarget)

#include "tst_rendertarget.moc"
