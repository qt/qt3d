// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DCore/private/qbuffer_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"
#include "testrenderer.h"

class tst_RenderBuffer : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;
        Qt3DCore::QBuffer buffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        buffer.setUsage(Qt3DCore::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("Corvette"));

        // WHEN
        renderBuffer.setRenderer(&renderer);
        renderBuffer.setManager(&bufferManager);
        simulateInitializationSync(&buffer, &renderBuffer);

        // THEN
        QCOMPARE(renderBuffer.peerId(), buffer.id());
        QCOMPARE(renderBuffer.isDirty(), true);
        QCOMPARE(renderBuffer.usage(), buffer.usage());
        QCOMPARE(renderBuffer.data(), buffer.data());
        QCOMPARE(renderBuffer.pendingBufferUpdates().size(), 1U);
        QCOMPARE(renderBuffer.pendingBufferUpdates().front().offset, -1);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::Buffer backendBuffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        // THEN
        QCOMPARE(backendBuffer.isDirty(), false);
        QCOMPARE(backendBuffer.usage(), Qt3DCore::QBuffer::StaticDraw);
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.peerId().isNull());
        QVERIFY(backendBuffer.pendingBufferUpdates().empty());

        // GIVEN
        Qt3DCore::QBuffer frontendBuffer;

        // WHEN
        backendBuffer.setManager(&bufferManager);
        backendBuffer.setRenderer(&renderer);
        simulateInitializationSync(&frontendBuffer, &backendBuffer);

        // THEN
        QCOMPARE(backendBuffer.isDirty(), true);
        QCOMPARE(backendBuffer.usage(), Qt3DCore::QBuffer::StaticDraw);
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.pendingBufferUpdates().empty());

        // WHEN
        frontendBuffer.setUsage(Qt3DCore::QBuffer::DynamicCopy);
        frontendBuffer.setData(QByteArrayLiteral("C7KR4"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.usage(), Qt3DCore::QBuffer::DynamicCopy);
        QCOMPARE(backendBuffer.isDirty(), true);
        QCOMPARE(backendBuffer.data(), QByteArrayLiteral("C7KR4"));
        QVERIFY(!backendBuffer.pendingBufferUpdates().empty());

        // WHEN
        frontendBuffer.updateData(2, QByteArrayLiteral("LS5"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.isDirty(), true);
        QCOMPARE(backendBuffer.data(), QByteArrayLiteral("C7LS5"));
        // WHEN
        backendBuffer.cleanup();

        // THEN
        QCOMPARE(backendBuffer.isDirty(), false);
        QCOMPARE(backendBuffer.usage(), Qt3DCore::QBuffer::StaticDraw);
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.pendingBufferUpdates().empty());
    }


    void checkForceFullUploadOnFirstTime()
    {
        // GIVEN
        Qt3DRender::Render::Buffer backendBuffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;
        Qt3DCore::QBuffer frontendBuffer;

        QByteArray data("111456789\0");

        frontendBuffer.setData(data);
        frontendBuffer.updateData(1, QByteArray("23\0"));

        // THEN
        QCOMPARE(frontendBuffer.data(), QByteArray("123456789\0"));

        // WHEN
        backendBuffer.setManager(&bufferManager);
        backendBuffer.setRenderer(&renderer);
        simulateInitializationSync(&frontendBuffer, &backendBuffer);

        // THEN
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1U);
        Qt3DCore::QBufferUpdate fullUpdate = backendBuffer.pendingBufferUpdates().front();
        QCOMPARE(fullUpdate.offset, -1);
        QVERIFY(fullUpdate.data.isEmpty());
        QCOMPARE(frontendBuffer.data(), backendBuffer.data());

        backendBuffer.pendingBufferUpdates().clear();

        // WHEN
        frontendBuffer.updateData(1, QByteArray("00\0"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(frontendBuffer.data(), QByteArray("100456789\0"));
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1U);
        fullUpdate = backendBuffer.pendingBufferUpdates().front();
        QCOMPARE(fullUpdate.offset, 1);
        QCOMPARE(fullUpdate.data, QByteArray("00\0"));
        QCOMPARE(frontendBuffer.data(), backendBuffer.data());

        // WHEN
        frontendBuffer.updateData(1, QByteArray("22\0"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, true);

        // THEN
        QCOMPARE(frontendBuffer.data(), QByteArray("122456789\0"));
        fullUpdate = backendBuffer.pendingBufferUpdates().front();
        QCOMPARE(fullUpdate.offset, -1);
        QVERIFY(fullUpdate.data.isEmpty());
        QCOMPARE(frontendBuffer.data(), backendBuffer.data());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DCore::QBuffer frontendBuffer;
        Qt3DRender::Render::Buffer backendBuffer;
        backendBuffer.setRenderer(&renderer);
        simulateInitializationSync(&frontendBuffer, &backendBuffer);

        // THEN
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.usage() != Qt3DCore::QBuffer::DynamicRead);
        QVERIFY(!backendBuffer.isDirty());
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        frontendBuffer.setUsage(Qt3DCore::QBuffer::DynamicRead);
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.usage(), Qt3DCore::QBuffer::DynamicRead);
        QVERIFY(backendBuffer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backendBuffer.unsetDirty();
        QVERIFY(!backendBuffer.isDirty());

        // WHEN
        frontendBuffer.setData(QByteArrayLiteral("LS9SL"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.data(), QByteArrayLiteral("LS9SL"));
        QVERIFY(backendBuffer.isDirty());
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1U);
        QCOMPARE(backendBuffer.pendingBufferUpdates().front().offset, -1);

        backendBuffer.pendingBufferUpdates().clear();

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backendBuffer.unsetDirty();
        QVERIFY(!backendBuffer.isDirty());

        // WHEN
        frontendBuffer.updateData(2, QByteArrayLiteral("LS5"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QVERIFY(!backendBuffer.pendingBufferUpdates().empty());
        QCOMPARE(backendBuffer.pendingBufferUpdates().front().offset, 2);
        QVERIFY(backendBuffer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backendBuffer.unsetDirty();
        QVERIFY(!backendBuffer.isDirty());
    }

    void checkBufferManagerReferenceCount()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;
        Qt3DCore::QBuffer buffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        // WHEN
        renderBuffer.setRenderer(&renderer);
        renderBuffer.setManager(&bufferManager);
        simulateInitializationSync(&buffer, &renderBuffer);

        // THEN
        QVERIFY(bufferManager.takeBuffersToRelease().empty());

        // WHEN
        bufferManager.removeBufferReference(renderBuffer.peerId());
        auto buffers = bufferManager.takeBuffersToRelease();

        // THEN
        QVERIFY(buffers.size() == 1);
        QVERIFY(buffers.first() == renderBuffer.peerId());
        QVERIFY(bufferManager.takeBuffersToRelease().empty());
    }

    void checkSetRendererDirtyOnInitialization()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;
        Qt3DCore::QBuffer buffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        renderBuffer.setRenderer(&renderer);
        renderBuffer.setManager(&bufferManager);

        // THEN
        QCOMPARE(renderer.dirtyBits(), 0);

        // WHEN
        simulateInitializationSync(&buffer, &renderBuffer);

        // THEN
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::BuffersDirty);
    }

    void checkHandlesMultipleUpdates()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;
        Qt3DCore::QBuffer buffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        const QByteArray initData("000000");
        buffer.setData(initData);

        renderBuffer.setRenderer(&renderer);
        renderBuffer.setManager(&bufferManager);

        simulateInitializationSync(&buffer, &renderBuffer);

        // THEN
        QCOMPARE(renderBuffer.data(), initData);
        renderBuffer.pendingBufferUpdates().clear();

        // WHEN
        buffer.updateData(0, QByteArray("012"));
        buffer.updateData(3, QByteArray("345"));
        renderBuffer.syncFromFrontEnd(&buffer, false);

        // THEN
        QCOMPARE(renderBuffer.pendingBufferUpdates().size(), 2U);
        QCOMPARE(renderBuffer.pendingBufferUpdates().front().offset, 0);
        QCOMPARE(renderBuffer.pendingBufferUpdates().front().data, QByteArray("012"));
        QCOMPARE(renderBuffer.pendingBufferUpdates().back().offset, 3);
        QCOMPARE(renderBuffer.pendingBufferUpdates().back().data, QByteArray("345"));
        QCOMPARE(renderBuffer.data(), QByteArray("012345"));
    }
};


QTEST_APPLESS_MAIN(tst_RenderBuffer)

#include "tst_buffer.moc"
