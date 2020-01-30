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
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qgeometryfactory.h>
#include "testrenderer.h"

class TestFactory : public Qt3DRender::QGeometryFactory
{
public:
    explicit TestFactory(int size)
        : m_size(size)
    {}

    Qt3DRender::QGeometry *operator ()() final
    {
        return nullptr;
    }

    bool operator ==(const Qt3DRender::QGeometryFactory &other) const final
    {
        const TestFactory *otherFactory = Qt3DRender::functor_cast<TestFactory>(&other);
        if (otherFactory != nullptr)
            return otherFactory->m_size == m_size;
        return false;
    }

    QT3D_FUNCTOR(TestFactory)

    private:
        int m_size;
};

class DummyGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT
public:
    DummyGeometry(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QGeometry(parent)
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
        Qt3DRender::QGeometry geometry;
        Qt3DRender::QGeometryFactoryPtr factory(new TestFactory(1200));
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
        geometryRenderer.setGeometryFactory(factory);
        geometryRenderer.setEnabled(false);

        // WHEN
        renderGeometryRenderer.setRenderer(&renderer);
        renderGeometryRenderer.setManager(&geometryRendererManager);
        simulateInitializationSync(&geometryRenderer, &renderGeometryRenderer);

        // THEN
        QCOMPARE(renderGeometryRenderer.peerId(), geometryRenderer.id());
        QCOMPARE(renderGeometryRenderer.isDirty(), true);
        QCOMPARE(renderGeometryRenderer.instanceCount(), geometryRenderer.instanceCount());
        QCOMPARE(renderGeometryRenderer.vertexCount(), geometryRenderer.vertexCount());
        QCOMPARE(renderGeometryRenderer.indexOffset(), geometryRenderer.indexOffset());
        QCOMPARE(renderGeometryRenderer.firstInstance(), geometryRenderer.firstInstance());
        QCOMPARE(renderGeometryRenderer.indexBufferByteOffset(), geometryRenderer.indexBufferByteOffset());
        QCOMPARE(renderGeometryRenderer.restartIndexValue(), geometryRenderer.restartIndexValue());
        QCOMPARE(renderGeometryRenderer.primitiveRestartEnabled(), geometryRenderer.primitiveRestartEnabled());
        QCOMPARE(renderGeometryRenderer.primitiveType(), geometryRenderer.primitiveType());
        QCOMPARE(renderGeometryRenderer.geometryId(), geometry.id());
        QCOMPARE(renderGeometryRenderer.geometryFactory(), factory);
        QCOMPARE(renderGeometryRenderer.isEnabled(), false);
        QVERIFY(*renderGeometryRenderer.geometryFactory() == *factory);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::GeometryRenderer renderGeometryRenderer;
        Qt3DRender::Render::GeometryRendererManager geometryRendererManager;

        // THEN
        QVERIFY(renderGeometryRenderer.peerId().isNull());
        QVERIFY(renderGeometryRenderer.geometryId().isNull());
        QCOMPARE(renderGeometryRenderer.isDirty(), false);
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

        // GIVEN
        Qt3DRender::QGeometryRenderer geometryRenderer;
        Qt3DRender::QGeometry geometry;
        Qt3DRender::QGeometryFactoryPtr factory(new TestFactory(1200));
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
        geometryRenderer.setGeometryFactory(factory);
        geometryRenderer.setEnabled(true);

        // WHEN
        renderGeometryRenderer.setRenderer(&renderer);
        renderGeometryRenderer.setManager(&geometryRendererManager);
        simulateInitializationSync(&geometryRenderer, &renderGeometryRenderer);
        renderGeometryRenderer.cleanup();

        // THEN
        QVERIFY(renderGeometryRenderer.geometryId().isNull());
        QCOMPARE(renderGeometryRenderer.isDirty(), false);
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
        Qt3DRender::QGeometryFactoryPtr factory(new TestFactory(1450));
        frontEndRenderer.setGeometryFactory(factory);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN
        QCOMPARE(backEndRenderer.geometryFactory(), factory);
        QVERIFY(backEndRenderer.isDirty());

        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        backEndRenderer.unsetDirty();
        QVERIFY(!backEndRenderer.isDirty());

        // WHEN we set an identical factory again
        Qt3DRender::QGeometryFactoryPtr factory2(new TestFactory(1450));
        frontEndRenderer.setGeometryFactory(factory2);
        backEndRenderer.syncFromFrontEnd(&frontEndRenderer, false);

        // THEN not dirty and still uses original factory
        QCOMPARE(backEndRenderer.geometryFactory(), factory);
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
