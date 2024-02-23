// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DRender/QBlitFramebuffer>
#include <Qt3DRender/private/qblitframebuffer_p.h>

#include "testarbiter.h"

class tst_QBlitFrameBuffer: public QObject
{
    Q_OBJECT

public:
    tst_QBlitFrameBuffer() : QObject()
    {
        qRegisterMetaType<Qt3DRender::QRenderTarget*>();
    }

private Q_SLOTS:

    void checkCreationData_data()
    {
        QTest::addColumn<Qt3DRender::QBlitFramebuffer *>("blitFramebuffer");

        Qt3DRender::QBlitFramebuffer *defaultConstructed = new Qt3DRender::QBlitFramebuffer();
        Qt3DRender::QRenderTarget *sourceRenderTarget = new Qt3DRender::QRenderTarget();
        Qt3DRender::QRenderTarget *destinationRenderTarget = new Qt3DRender::QRenderTarget();
        defaultConstructed->setSource(sourceRenderTarget);
        defaultConstructed->setDestination(destinationRenderTarget);
        QTest::newRow("defaultConstructed") << defaultConstructed;
    }

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::QBlitFramebuffer blitFramebuffer;

        // THEN
        QCOMPARE(blitFramebuffer.source(), nullptr);
        QCOMPARE(blitFramebuffer.destination(), nullptr);
        QCOMPARE(blitFramebuffer.sourceRect(), QRect());
        QCOMPARE(blitFramebuffer.destinationRect(), QRect());
        QCOMPARE(blitFramebuffer.sourceAttachmentPoint(), Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0);
        QCOMPARE(blitFramebuffer.destinationAttachmentPoint(), Qt3DRender::QRenderTargetOutput::AttachmentPoint::Color0);
        QCOMPARE(blitFramebuffer.interpolationMethod(), Qt3DRender::QBlitFramebuffer::Linear);
    }

    void checkPropertyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QBlitFramebuffer> blitFramebuffer(new Qt3DRender::QBlitFramebuffer());
        arbiter.setArbiterOnNode(blitFramebuffer.data());

        Qt3DRender::QRenderTarget *sourceRenderTarget = new Qt3DRender::QRenderTarget;
        Qt3DRender::QRenderTarget *destinationRenderTarget = new Qt3DRender::QRenderTarget;

        // sourceRenderTarget
        // WHEN
        blitFramebuffer->setSource(sourceRenderTarget);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // WHEN
        blitFramebuffer->setSource(sourceRenderTarget);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        blitFramebuffer->setSource(nullptr);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // destinationRenderTarget
        // WHEN
        blitFramebuffer->setDestination(destinationRenderTarget);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // WHEN
        blitFramebuffer->setDestination(destinationRenderTarget);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        blitFramebuffer->setDestination(nullptr);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // sourceRect
        // WHEN
        blitFramebuffer->setSourceRect(QRect(0,0,1,1));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // WHEN
        blitFramebuffer->setSourceRect(QRect(0,0,1,1));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        blitFramebuffer->setSourceRect(QRect());

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // destinationRect
        blitFramebuffer->setDestinationRect(QRect(0,0,1,1));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // WHEN
        blitFramebuffer->setDestinationRect(QRect(0,0,1,1));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        blitFramebuffer->setDestinationRect(QRect());

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // sourceAttachmentPoint
        // WHEN
        blitFramebuffer->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // WHEN
        blitFramebuffer->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        blitFramebuffer->setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // destinationAttachmentPoint
        // WHEN
        blitFramebuffer->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();

        // WHEN
        blitFramebuffer->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color1);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        // WHEN
        blitFramebuffer->setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), blitFramebuffer.data());

        arbiter.clear();
    }

    void checkSourceDestReset()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QBlitFramebuffer> blitFramebuffer(new Qt3DRender::QBlitFramebuffer());
        arbiter.setArbiterOnNode(blitFramebuffer.data());

        Qt3DRender::QRenderTarget *sourceRenderTarget = new Qt3DRender::QRenderTarget; // no parent
        Qt3DRender::QRenderTarget *destinationRenderTarget = new Qt3DRender::QRenderTarget(sourceRenderTarget); // have a parent

        // WHEN
        blitFramebuffer->setSource(sourceRenderTarget);
        blitFramebuffer->setDestination(destinationRenderTarget);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(sourceRenderTarget->parent(), blitFramebuffer.data());
        QCOMPARE(destinationRenderTarget->parent(), sourceRenderTarget); // already set parent must not change

        // WHEN
        delete sourceRenderTarget;

        // THEN
        QVERIFY(!blitFramebuffer->source());
        QVERIFY(!blitFramebuffer->destination()); // gone too since destinationRenderTarget was parented to sourceRenderTarget
    }
};

QTEST_MAIN(tst_QBlitFrameBuffer)

#include "tst_qblitframebuffer.moc"
