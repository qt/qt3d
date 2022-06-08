// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/private/qsetfence_p.h>
#include <Qt3DRender/private/setfence_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"
#include "testarbiter.h"

class tst_SetFence : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::SetFence backendSetFence;

        // THEN
        QCOMPARE(backendSetFence.isEnabled(), false);
        QVERIFY(backendSetFence.peerId().isNull());
        QCOMPARE(backendSetFence.nodeType(), Qt3DRender::Render::FrameGraphNode::SetFence);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QSetFence setFence;

        {
            // WHEN
            Qt3DRender::Render::SetFence backendSetFence;
            backendSetFence.setRenderer(&renderer);
            simulateInitializationSync(&setFence, &backendSetFence);

            // THEN
            QCOMPARE(backendSetFence.isEnabled(), true);
            QCOMPARE(backendSetFence.peerId(), setFence.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::SetFence backendSetFence;
            setFence.setEnabled(false);
            backendSetFence.setRenderer(&renderer);
            simulateInitializationSync(&setFence, &backendSetFence);

            // THEN
            QCOMPARE(backendSetFence.peerId(), setFence.id());
            QCOMPARE(backendSetFence.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::SetFence backendSetFence;
        Qt3DRender::QSetFence setFence;
        TestRenderer renderer;
        backendSetFence.setRenderer(&renderer);
        simulateInitializationSync(&setFence, &backendSetFence);

        {
             // WHEN
             const bool newValue = false;
             setFence.setEnabled(newValue);
             backendSetFence.syncFromFrontEnd(&setFence, false);

             // THEN
            QCOMPARE(backendSetFence.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }
};

QTEST_MAIN(tst_SetFence)

#include "tst_setfence.moc"
