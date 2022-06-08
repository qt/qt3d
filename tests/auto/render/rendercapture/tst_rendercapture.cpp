// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DRender/qrendercapture.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"
#include "testrenderer.h"

class tst_RenderCapture : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QRenderCapture frontend;
        Qt3DRender::Render::RenderCapture backend;

        // WHEN
        backend.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &backend);

        // THEN
        QVERIFY(!backend.peerId().isNull());
        QCOMPARE(backend.wasCaptureRequested(), false);
        QCOMPARE(backend.isEnabled(), true);
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
    }

    void checkEnabledPropertyChange()
    {
        // GIVEN
        Qt3DRender::QRenderCapture frontend;
        Qt3DRender::Render::RenderCapture renderCapture;
        TestRenderer renderer;
        renderCapture.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &renderCapture);

        // WHEN
        frontend.setEnabled(false);
        renderCapture.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(renderCapture.isEnabled(), false);
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
    }

    void checkReceiveRenderCaptureRequest()
    {
        // GIVEN
        Qt3DRender::QRenderCapture frontend;
        Qt3DRender::Render::RenderCapture renderCapture;
        TestRenderer renderer;
        renderCapture.setRenderer(&renderer);
        simulateInitializationSync(&frontend, &renderCapture);

        // WHEN
        frontend.requestCapture();
        renderCapture.syncFromFrontEnd(&frontend, false);

        // THEN
        QCOMPARE(renderCapture.wasCaptureRequested(), true);
    }

    void checkTakeCaptureRequest()
    {
        // GIVEN
        Qt3DRender::Render::RenderCapture renderCapture;
        TestRenderer renderer;
        renderCapture.setRenderer(&renderer);
        renderCapture.setEnabled(true);

        // WHEN
        renderCapture.requestCapture({ 2, QRect(10, 10, 20, 20) });
        renderCapture.requestCapture({ 4, QRect(15, 15, 30, 30) });

        // THEN
        QCOMPARE(renderCapture.wasCaptureRequested(), true);

        // WHEN
        Qt3DRender::QRenderCaptureRequest r1 = renderCapture.takeCaptureRequest();

        // THEN
        QCOMPARE(r1.captureId, 2);
        QCOMPARE(r1.rect, QRect(10, 10, 20, 20));
        QCOMPARE(renderCapture.wasCaptureRequested(), true);

        // WHEN
        Qt3DRender::QRenderCaptureRequest r2 = renderCapture.takeCaptureRequest();

        // THEN
        QCOMPARE(r2.captureId, 4);
        QCOMPARE(r2.rect, QRect(15, 15, 30, 30));
        QCOMPARE(renderCapture.wasCaptureRequested(), false);
    }
};


QTEST_APPLESS_MAIN(tst_RenderCapture)

#include "tst_rendercapture.moc"
