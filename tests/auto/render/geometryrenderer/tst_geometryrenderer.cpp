// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DCore/qgeometry.h>
#include "testrenderer.h"

class DummyGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
public:
    DummyGeometry(Qt3DCore::QNode *parent = nullptr)
        : Qt3DCore::QGeometry(parent)
    {}
};

class tst_RenderGeometryRenderer : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::GeometryRenderer renderGeometryRenderer;
        Qt3DRender::QGeometryRenderer geometryRenderer;
        Qt3DCore::QGeometry geometry;
        Qt3DRender::Render::GeometryRendererManager geometryRendererManager;
        TestRenderer renderer;

        geometryRenderer.setInstanceCount(1584);
        geometryRenderer.setVertexCount(1609);
        geometryRenderer.setIndexOffset(750);
        geometryRenderer.setFirstInstance(883);
        geometryRenderer.setIndexBufferByteOffset(96);
        geometryRenderer.setRestartIndexValue(65536);
        geometryRenderer.setPrimitiveRestartEnabled(true);
        geometryRenderer.setPrimitiveType(Qt3DRender::QGeometryRenderer::Patches);
        geometryRenderer.setGeometry(&geometry);
        geometryRenderer.setEnabled(false);
        geometryRenderer.setSortIndex(42.f);

        // WHEN
        renderGeometryRenderer.setRenderer(&renderer);
        renderGeometryRenderer.setManager(&geometryRendererManager);
        simulateInitializationSync(&geometryRenderer, &renderGeometryRenderer);

        // THEN
        QCOMPARE(renderGeometryRenderer.peerId(), geometryRenderer.id());
        QVERIFY(renderGeometryRenderer.isDirty());
        QCOMPARE(renderGeometryRenderer.instanceCount(), geometryRenderer.instanceCount());
        QCOMPARE(renderGeometryRenderer.vertexCount(), geometryRenderer.vertexCount());
        QCOMPARE(renderGeometryRenderer.indexOffset(), geometryRenderer.indexOffset());
        QCOMPARE(renderGeometryRenderer.firstInstance(), geometryRenderer.firstInstance());
        QCOMPARE(renderGeometryRenderer.indexBufferByteOffset(), geometryRenderer.indexBufferByteOffset());
        QCOMPARE(renderGeometryRenderer.restartIndexValue(), geometryRenderer.restartIndexValue());
        QCOMPARE(renderGeometryRenderer.primitiveRestartEnabled(), geometryRenderer.primitiveRestartEnabled());
        QCOMPARE(renderGeometryRenderer.primitiveType(), geometryRenderer.primitiveType());
        QCOMPARE(renderGeometryRenderer.geometryId(), geometry.id());
        QCOMPARE(renderGeometryRenderer.isEnabled(), false);
        QCOMPARE(renderGeometryRenderer.sortIndex(), geometryRenderer.sortIndex());
        QCOMPARE(renderGeometryRenderer.sortIndex(), 42.f);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::GeometryRenderer renderGeometryRenderer;
        Qt3DRender::Render::GeometryRendererManager geometryRendererManager;

        // THEN
        QVERIFY(renderGeometryRenderer.peerId().isNull());
        QVERIFY(renderGeometryRenderer.geometryId().isNull());
        QVERIFY(!renderGeometryRenderer.isDirty());
        QCOMPARE(renderGeometryRenderer.instanceCount(), 0);
        QCOMPARE(renderGeometryRenderer.vertexCount(), 0);
        QCOMPARE(renderGeometryRenderer.indexOffset(), 0);
        QCOMPARE(renderGeometryRenderer.firstInstance(), 0);
        QCOMPARE(renderGeometryRenderer.indexBufferByteOffset(), 0);
        QCOMPARE(renderGeometryRenderer.restartIndexValue(), -1);
        QCOMPARE(renderGeometryRenderer.primitiveRestartEnabled(), false);
        QCOMPARE(renderGeometryRenderer.primitiveType(), Qt3DRender::QGeometryRenderer::Triangles);
        QVERIFY(renderGeometryRenderer.geometryFactory().isNull());
        QVERIFY(!renderGeometryRenderer.isEnabled());
        QCOMPARE(renderGeometryRenderer.sortIndex(), -1.f);

        // GIVEN
        Qt3DRender::QGeometryRenderer geometryRenderer;
        Qt3DCore::QGeometry geometry;
        TestRenderer renderer;

        geometryRenderer.setInstanceCount(454);
        geometryRenderer.setVertexCount(350);
        geometryRenderer.setIndexOffset(427);
        geometryRenderer.setFirstInstance(383);
        geometryRenderer.setIndexBufferByteOffset(96);
        geometryRenderer.setRestartIndexValue(555);
        geometryRenderer.setPrimitiveRestartEnabled(true);
        geometryRenderer.setPrimitiveType(Qt3DRender::QGeometryRenderer::Patches);
        geometryRenderer.setGeometry(&geometry);
        geometryRenderer.setEnabled(true);
        geometryRenderer.setSortIndex(42.f);

        // WHEN
        renderGeometryRenderer.setRenderer(&renderer);
        renderGeometryRenderer.setManager(&geometryRendererManager);
        simulateInitializationSync(&geometryRenderer, &renderGeometryRenderer);
        renderGeometryRenderer.cleanup();

        // THEN
        QVERIFY(renderGeometryRenderer.geometryId().isNull());
        QVERIFY(!renderGeometryRenderer.isDirty());
        QCOMPARE(renderGeometryRenderer.instanceCount(), 0);
        QCOMPARE(renderGeometryRenderer.vertexCount(), 0);
        QCOMPARE(renderGeometryRenderer.indexOffset(), 0);
        QCOMPARE(renderGeometryRenderer.firstInstance(), 0);
        QCOMPARE(renderGeometryRenderer.indexBufferByteOffset(), 0);
        QCOMPARE(renderGeometryRenderer.restartIndexValue(), -1);
        QCOMPARE(renderGeometryRenderer.primitiveRestartEnabled(), false);
        QCOMPARE(renderGeometryRenderer.primitiveType(), Qt3DRender::QGeometryRenderer::Triangles);
        QVERIFY(!renderGeometryRenderer.isEnabled());
        QCOMPARE(renderGeometryRenderer.sortIndex(), -1.f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QGeometryRenderer frontEndRenderer;
        Qt3DRender::Render::GeometryRenderer backEndRenderer;
        TestRenderer renderer;
        backEndRenderer.setRenderer(&renderer);

        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setInstanceCount(2);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.instanceCount(), 2);
        QVERIFY(backEndRenderer.isDirty());
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setVertexCount(56);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.vertexCount(), 56);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setIndexOffset(65);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.indexOffset(), 65);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setFirstInstance(82);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.firstInstance(), 82);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setIndexBufferByteOffset(96);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.indexBufferByteOffset(), 96);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setRestartIndexValue(46);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.restartIndexValue(), 46);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setPrimitiveRestartEnabled(true);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.primitiveRestartEnabled(), true);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setPrimitiveType(Qt3DRender::QGeometryRenderer::LineLoop);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.primitiveType(), Qt3DRender::QGeometryRenderer::LineLoop);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        DummyGeometry geometry;
        const Qt3DCore::QNodeId geometryId = geometry.id();
        frontEndRenderer.setGeometry(&geometry);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.geometryId(), geometryId);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setGeometry(nullptr);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.geometryId(), Qt3DCore::QNodeId());
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setEnabled(true);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.isEnabled(), true);
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN
        frontEndRenderer.setSortIndex(42.f);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.sortIndex(), 42.f);
        QVERIFY(!backEndRenderer.isDirty());
    }

    void checkSetRendererDirtyOnInitialization()
    {
        // GIVEN
        Qt3DRender::Render::GeometryRendererManager geometryRendererManager;
        Qt3DRender::Render::GeometryRenderer renderGeometryRenderer;
        Qt3DRender::QGeometryRenderer geometryRenderer;
        TestRenderer renderer;

        renderGeometryRenderer.setRenderer(&renderer);
        renderGeometryRenderer.setManager(&geometryRendererManager);

        // THEN
        QCOMPARE(renderer.dirtyBits(), 0);

        // WHEN
        simulateInitializationSync(&geometryRenderer, &renderGeometryRenderer);

        // THEN
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::GeometryDirty);
    }
};

QTEST_APPLESS_MAIN(tst_RenderGeometryRenderer)

#include "tst_geometryrenderer.moc"
