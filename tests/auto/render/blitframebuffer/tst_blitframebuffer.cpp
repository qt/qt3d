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
#include <Qt3DRender/qblitframebuffer.h>
#include <Qt3DRender/private/qblitframebuffer_p.h>
#include <Qt3DRender/private/blitframebuffer_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_BlitFramebuffer : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::BlitFramebuffer backendBlitFramebuffer;

        // THEN
        QCOMPARE(backendBlitFramebuffer.nodeType(), Qt3DRender::Render::FrameGraphNode::BlitFramebuffer);
        QCOMPARE(backendBlitFramebuffer.isEnabled(), false);
        QVERIFY(backendBlitFramebuffer.peerId().isNull());
        QVERIFY(backendBlitFramebuffer.sourceRenderTargetId().isNull());
        QVERIFY(backendBlitFramebuffer.destinationRenderTargetId().isNull());
        QCOMPARE(backendBlitFramebuffer.sourceRect(), QRect());
        QCOMPARE(backendBlitFramebuffer.destinationRect(), QRect());
        QCOMPARE(backendBlitFramebuffer.sourceAttachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);
        QCOMPARE(backendBlitFramebuffer.destinationAttachmentPoint(), Qt3DRender::QRenderTargetOutput::Color0);
        QCOMPARE(backendBlitFramebuffer.interpolationMethod(), Qt3DRender::QBlitFramebuffer::Linear);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QRenderTarget *sourceTarget = new Qt3DRender::QRenderTarget;
        Qt3DRender::QRenderTarget *destinationTarget = new Qt3DRender::QRenderTarget;
        Qt3DRender::QBlitFramebuffer blitFramebuffer;
        blitFramebuffer.setSource(sourceTarget);
        blitFramebuffer.setDestination(destinationTarget);
        blitFramebuffer.setSourceRect(QRect(0,0,1,1));
        blitFramebuffer.setDestinationRect(QRect(0,0,1,1));
        blitFramebuffer.setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);
        blitFramebuffer.setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);
        blitFramebuffer.setInterpolationMethod(Qt3DRender::QBlitFramebuffer::Nearest);

        {
            // WHEN
            Qt3DRender::Render::BlitFramebuffer backendBlitFramebuffer;
            backendBlitFramebuffer.setRenderer(&renderer);
            simulateInitializationSync(&blitFramebuffer, &backendBlitFramebuffer);

            // THEN
            QCOMPARE(backendBlitFramebuffer.isEnabled(), true);
            QCOMPARE(backendBlitFramebuffer.peerId(), blitFramebuffer.id());
            QCOMPARE(backendBlitFramebuffer.sourceRenderTargetId(), sourceTarget->id());
            QCOMPARE(backendBlitFramebuffer.destinationRenderTargetId(), destinationTarget->id());
            QCOMPARE(backendBlitFramebuffer.sourceRect(), QRect(0,0,1,1));
            QCOMPARE(backendBlitFramebuffer.destinationRect(), QRect(0,0,1,1));
            QCOMPARE(backendBlitFramebuffer.sourceAttachmentPoint(), Qt3DRender::QRenderTargetOutput::Color1);
            QCOMPARE(backendBlitFramebuffer.destinationAttachmentPoint(),  Qt3DRender::QRenderTargetOutput::Color1);
            QCOMPARE(backendBlitFramebuffer.interpolationMethod(), Qt3DRender::QBlitFramebuffer::Nearest);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::BlitFramebuffer backendBlitFramebuffer;
            backendBlitFramebuffer.setRenderer(&renderer);
            blitFramebuffer.setEnabled(false);
            simulateInitializationSync(&blitFramebuffer, &backendBlitFramebuffer);

            // THEN
            QCOMPARE(backendBlitFramebuffer.peerId(), blitFramebuffer.id());
            QCOMPARE(backendBlitFramebuffer.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::BlitFramebuffer backendBlitFramebuffer;
        Qt3DRender::QBlitFramebuffer blitFramebuffer;
        TestRenderer renderer;
        backendBlitFramebuffer.setRenderer(&renderer);
        simulateInitializationSync(&blitFramebuffer, &backendBlitFramebuffer);

        {
            // WHEN
            const bool newValue = false;
            blitFramebuffer.setEnabled(newValue);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            Qt3DRender::QRenderTarget sourceRenderTarget;
            blitFramebuffer.setSource(&sourceRenderTarget);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.sourceRenderTargetId(), sourceRenderTarget.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            Qt3DRender::QRenderTarget destinationRenderTarget;
            blitFramebuffer.setDestination(&destinationRenderTarget);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.destinationRenderTargetId(), destinationRenderTarget.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const auto newValue = QRect(0,0,1,1);
            blitFramebuffer.setSourceRect(newValue);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.sourceRect(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const auto newValue = QRect(0,0,1,1);
            blitFramebuffer.setDestinationRect(newValue);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.destinationRect(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const auto newValue = Qt3DRender::QRenderTargetOutput::Color1;
            blitFramebuffer.setSourceAttachmentPoint(newValue);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.sourceAttachmentPoint(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
            // WHEN
            const auto newValue = Qt3DRender::QRenderTargetOutput::Color1;
            blitFramebuffer.setDestinationAttachmentPoint(newValue);
            backendBlitFramebuffer.syncFromFrontEnd(&blitFramebuffer, false);

            // THEN
            QCOMPARE(backendBlitFramebuffer.destinationAttachmentPoint(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }

};

QTEST_MAIN(tst_BlitFramebuffer)

#include "tst_blitframebuffer.moc"
