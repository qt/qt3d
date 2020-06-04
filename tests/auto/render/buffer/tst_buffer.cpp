/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/qbuffer_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"
#include "testrenderer.h"

class TestFunctor : public Qt3DRender::QBufferDataGenerator
{
public:
    explicit TestFunctor(int size)
        : m_size(size)
    {}

    QByteArray operator ()() final
    {
        return QByteArrayLiteral("454");
    }

    bool operator ==(const Qt3DRender::QBufferDataGenerator &other) const final
    {
        const TestFunctor *otherFunctor = Qt3DRender::functor_cast<TestFunctor>(&other);
        if (otherFunctor != nullptr)
            return otherFunctor->m_size == m_size;
        return false;
    }

    QT3D_FUNCTOR(TestFunctor)

private:
    int m_size;
};

class tst_RenderBuffer : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::Buffer renderBuffer;
        Qt3DRender::QBuffer buffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        buffer.setUsage(Qt3DRender::QBuffer::DynamicCopy);
        buffer.setData(QByteArrayLiteral("Corvette"));
        buffer.setDataGenerator(Qt3DRender::QBufferDataGeneratorPtr(new TestFunctor(883)));

        // WHEN
        renderBuffer.setRenderer(&renderer);
        renderBuffer.setManager(&bufferManager);
        simulateInitializationSync(&buffer, &renderBuffer);

        // THEN
        QCOMPARE(renderBuffer.peerId(), buffer.id());
        QCOMPARE(renderBuffer.isDirty(), true);
        QCOMPARE(renderBuffer.usage(), buffer.usage());
        QCOMPARE(renderBuffer.data(), buffer.data());
        QCOMPARE(renderBuffer.dataGenerator(), buffer.dataGenerator());
        QVERIFY(*renderBuffer.dataGenerator() == *buffer.dataGenerator());
        QCOMPARE(renderBuffer.pendingBufferUpdates().size(), 1);
        QCOMPARE(renderBuffer.pendingBufferUpdates().first().offset, -1);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::Buffer backendBuffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;

        // THEN
        QCOMPARE(backendBuffer.isDirty(), false);
        QCOMPARE(backendBuffer.usage(), Qt3DRender::QBuffer::StaticDraw);
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.peerId().isNull());
        QVERIFY(backendBuffer.dataGenerator().isNull());
        QVERIFY(backendBuffer.pendingBufferUpdates().empty());

        // GIVEN
        Qt3DRender::QBuffer frontendBuffer;

        // WHEN
        backendBuffer.setManager(&bufferManager);
        backendBuffer.setRenderer(&renderer);
        simulateInitializationSync(&frontendBuffer, &backendBuffer);

        // THEN
        QCOMPARE(backendBuffer.isDirty(), true);
        QCOMPARE(backendBuffer.usage(), Qt3DRender::QBuffer::StaticDraw);
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.dataGenerator().isNull());
        QVERIFY(backendBuffer.pendingBufferUpdates().empty());

        // WHEN
        frontendBuffer.setUsage(Qt3DRender::QBuffer::DynamicCopy);
        frontendBuffer.setData(QByteArrayLiteral("C7KR4"));
        frontendBuffer.setDataGenerator(Qt3DRender::QBufferDataGeneratorPtr(new TestFunctor(73)));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.usage(), Qt3DRender::QBuffer::DynamicCopy);
        QCOMPARE(backendBuffer.isDirty(), true);
        QCOMPARE(backendBuffer.data(), QByteArrayLiteral("C7KR4"));
        QVERIFY(!backendBuffer.dataGenerator().isNull());
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
        QCOMPARE(backendBuffer.usage(), Qt3DRender::QBuffer::StaticDraw);
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.dataGenerator().isNull());
        QVERIFY(backendBuffer.pendingBufferUpdates().empty());
    }


    void checkForceFullUploadOnFirstTime()
    {
        // GIVEN
        Qt3DRender::Render::Buffer backendBuffer;
        Qt3DRender::Render::BufferManager bufferManager;
        TestRenderer renderer;
        Qt3DRender::QBuffer frontendBuffer;

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
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1);
        Qt3DRender::QBufferUpdate fullUpdate = backendBuffer.pendingBufferUpdates().first();
        QCOMPARE(fullUpdate.offset, -1);
        QVERIFY(fullUpdate.data.isEmpty());
        QCOMPARE(frontendBuffer.data(), backendBuffer.data());

        backendBuffer.pendingBufferUpdates().clear();

        // WHEN
        frontendBuffer.updateData(1, QByteArray("00\0"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(frontendBuffer.data(), QByteArray("100456789\0"));
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1);
        fullUpdate = backendBuffer.pendingBufferUpdates().first();
        QCOMPARE(fullUpdate.offset, 1);
        QCOMPARE(fullUpdate.data, QByteArray("00\0"));
        QCOMPARE(frontendBuffer.data(), backendBuffer.data());

        // WHEN
        frontendBuffer.updateData(1, QByteArray("22\0"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, true);

        // THEN
        QCOMPARE(frontendBuffer.data(), QByteArray("122456789\0"));
        fullUpdate = backendBuffer.pendingBufferUpdates().first();
        QCOMPARE(fullUpdate.offset, -1);
        QVERIFY(fullUpdate.data.isEmpty());
        QCOMPARE(frontendBuffer.data(), backendBuffer.data());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QBuffer frontendBuffer;
        Qt3DRender::Render::Buffer backendBuffer;
        backendBuffer.setRenderer(&renderer);
        simulateInitializationSync(&frontendBuffer, &backendBuffer);

        // THEN
        QVERIFY(backendBuffer.data().isEmpty());
        QVERIFY(backendBuffer.usage() != Qt3DRender::QBuffer::DynamicRead);
        QVERIFY(!backendBuffer.isDirty());
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        frontendBuffer.setUsage(Qt3DRender::QBuffer::DynamicRead);
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.usage(), Qt3DRender::QBuffer::DynamicRead);
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
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1);
        QCOMPARE(backendBuffer.pendingBufferUpdates().first().offset, -1);

        backendBuffer.pendingBufferUpdates().clear();

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backendBuffer.unsetDirty();
        QVERIFY(!backendBuffer.isDirty());

        // WHEN
        Qt3DRender::QBufferDataGeneratorPtr functor(new TestFunctor(355));
        frontendBuffer.setDataGenerator(functor);
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.dataGenerator(), functor);
        QVERIFY(backendBuffer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backendBuffer.unsetDirty();
        QVERIFY(!backendBuffer.isDirty());

        // WHEN
        frontendBuffer.setSyncData(true);
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QCOMPARE(backendBuffer.isSyncData(), true);
        QVERIFY(!backendBuffer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::BuffersDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        // WHEN
        TestArbiter arbiter;
        Qt3DCore::QBackendNodePrivate::get(&backendBuffer)->setArbiter(&arbiter);
        backendBuffer.executeFunctor();

        // THEN
        QCOMPARE(arbiter.events.count(), 0);
        QCOMPARE(backendBuffer.pendingBufferUpdates().size(), 1);
        QCOMPARE(backendBuffer.pendingBufferUpdates().first().offset, -1);

        arbiter.events.clear();
        backendBuffer.pendingBufferUpdates().clear();

        // WHEN
        frontendBuffer.updateData(2, QByteArrayLiteral("LS5"));
        backendBuffer.syncFromFrontEnd(&frontendBuffer, false);

        // THEN
        QVERIFY(!backendBuffer.pendingBufferUpdates().empty());
        QCOMPARE(backendBuffer.pendingBufferUpdates().first().offset, 2);
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
        Qt3DRender::QBuffer buffer;
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
        Qt3DRender::QBuffer buffer;
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
        Qt3DRender::QBuffer buffer;
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
        QCOMPARE(renderBuffer.pendingBufferUpdates().size(), 2);
        QCOMPARE(renderBuffer.pendingBufferUpdates().first().offset, 0);
        QCOMPARE(renderBuffer.pendingBufferUpdates().first().data, QByteArray("012"));
        QCOMPARE(renderBuffer.pendingBufferUpdates().last().offset, 3);
        QCOMPARE(renderBuffer.pendingBufferUpdates().last().data, QByteArray("345"));
        QCOMPARE(renderBuffer.data(), QByteArray("012345"));
    }
};


QTEST_APPLESS_MAIN(tst_RenderBuffer)

#include "tst_buffer.moc"
