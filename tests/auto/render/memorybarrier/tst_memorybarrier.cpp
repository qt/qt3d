// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qmemorybarrier.h>
#include <Qt3DRender/private/qmemorybarrier_p.h>
#include <Qt3DRender/private/memorybarrier_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_MemoryBarrier : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::MemoryBarrier backendMemoryBarrier;

        // THEN
        QCOMPARE(backendMemoryBarrier.nodeType(), Qt3DRender::Render::FrameGraphNode::MemoryBarrier);
        QCOMPARE(backendMemoryBarrier.isEnabled(), false);
        QVERIFY(backendMemoryBarrier.peerId().isNull());
        QCOMPARE(backendMemoryBarrier.waitOperations(), Qt3DRender::QMemoryBarrier::None);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QMemoryBarrier memoryBarrier;
        memoryBarrier.setWaitOperations(Qt3DRender::QMemoryBarrier::VertexAttributeArray);

        {
            // WHEN
            Qt3DRender::Render::MemoryBarrier backendMemoryBarrier;
            backendMemoryBarrier.setRenderer(&renderer);
            simulateInitializationSync(&memoryBarrier, &backendMemoryBarrier);

            // THEN
            QCOMPARE(backendMemoryBarrier.isEnabled(), true);
            QCOMPARE(backendMemoryBarrier.peerId(), memoryBarrier.id());
            QCOMPARE(backendMemoryBarrier.waitOperations(), Qt3DRender::QMemoryBarrier::VertexAttributeArray);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::MemoryBarrier backendMemoryBarrier;
            backendMemoryBarrier.setRenderer(&renderer);
            memoryBarrier.setEnabled(false);
            simulateInitializationSync(&memoryBarrier, &backendMemoryBarrier);

            // THEN
            QCOMPARE(backendMemoryBarrier.peerId(), memoryBarrier.id());
            QCOMPARE(backendMemoryBarrier.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::MemoryBarrier backendMemoryBarrier;
        TestRenderer renderer;
        backendMemoryBarrier.setRenderer(&renderer);
        Qt3DRender::QMemoryBarrier memoryBarrier;

        simulateInitializationSync(&memoryBarrier, &backendMemoryBarrier);

        {
             // WHEN
             const bool newValue = false;
             memoryBarrier.setEnabled(newValue);
             backendMemoryBarrier.syncFromFrontEnd(&memoryBarrier, false);

             // THEN
            QCOMPARE(backendMemoryBarrier.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
             // WHEN
             const Qt3DRender::QMemoryBarrier::Operations newValue(Qt3DRender::QMemoryBarrier::AtomicCounter|Qt3DRender::QMemoryBarrier::ElementArray);
             memoryBarrier.setWaitOperations(newValue);
             backendMemoryBarrier.syncFromFrontEnd(&memoryBarrier, false);

             // THEN
            QCOMPARE(backendMemoryBarrier.waitOperations(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }

};

QTEST_MAIN(tst_MemoryBarrier)

#include "tst_memorybarrier.moc"
