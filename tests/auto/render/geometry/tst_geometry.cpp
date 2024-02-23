// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testrenderer.h"
#include "testarbiter.h"

class DummyAttribute : public Qt3DCore::QAttribute
{
    Q_OBJECT
public:
    DummyAttribute(Qt3DCore::QNode *parent = nullptr)
        : Qt3DCore::QAttribute(parent)
    {}
};

class tst_RenderGeometry : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::Geometry renderGeometry;
        TestRenderer renderer;

        Qt3DCore::QGeometry geometry;
        Qt3DCore::QAttribute attr1;
        Qt3DCore::QAttribute attr2;
        Qt3DCore::QAttribute attr4;
        Qt3DCore::QAttribute attr3;

        geometry.addAttribute(&attr1);
        geometry.addAttribute(&attr2);
        geometry.addAttribute(&attr3);
        geometry.addAttribute(&attr4);
        geometry.setBoundingVolumePositionAttribute(&attr1);
        renderGeometry.setRenderer(&renderer);

        // WHEN
        simulateInitializationSync(&geometry, &renderGeometry);

        // THEN
        QCOMPARE(renderGeometry.peerId(), geometry.id());
        QCOMPARE(renderGeometry.isDirty(), true);
        QCOMPARE(renderGeometry.attributes().size(), 4);
        QCOMPARE(renderGeometry.boundingPositionAttribute(), attr1.id());

        Qt3DCore::QNodeIdVector attribs = Qt3DCore::qIdsForNodes(geometry.attributes());
        std::sort(std::begin(attribs), std::end(attribs));
        QCOMPARE(attribs, renderGeometry.attributes());
    }

    void checkSetRendererDirtyOnInitialization()
    {
        // GIVEN
        Qt3DRender::Render::Geometry renderGeometry;
        Qt3DCore::QGeometry geometry;
        TestRenderer renderer;

        renderGeometry.setRenderer(&renderer);

        // THEN
        QCOMPARE(renderer.dirtyBits(), 0);

        // WHEN
        simulateInitializationSync(&geometry, &renderGeometry);

        // THEN
        QCOMPARE(renderer.dirtyBits(), Qt3DRender::Render::AbstractRenderer::GeometryDirty);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Geometry renderGeometry;
        renderGeometry.setRenderer(&renderer);

        // THEN
        QCOMPARE(renderGeometry.isDirty(), false);
        QVERIFY(renderGeometry.attributes().isEmpty());
        QVERIFY(renderGeometry.peerId().isNull());
        QCOMPARE(renderGeometry.boundingPositionAttribute(), Qt3DCore::QNodeId());

        // GIVEN
        Qt3DCore::QGeometry geometry;
        Qt3DCore::QAttribute attr1;
        Qt3DCore::QAttribute attr2;
        Qt3DCore::QAttribute attr4;
        Qt3DCore::QAttribute attr3;
        geometry.setBoundingVolumePositionAttribute(&attr1);

        geometry.addAttribute(&attr1);
        geometry.addAttribute(&attr2);
        geometry.addAttribute(&attr3);
        geometry.addAttribute(&attr4);

        // WHEN
        simulateInitializationSync(&geometry, &renderGeometry);
        renderGeometry.cleanup();

        // THEN
        QCOMPARE(renderGeometry.isDirty(), false);
        QVERIFY(renderGeometry.attributes().isEmpty());
        QCOMPARE(renderGeometry.boundingPositionAttribute(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::Geometry renderGeometry;
        renderGeometry.setRenderer(&renderer);
        Qt3DCore::QGeometry geometry;

        simulateInitializationSync(&geometry, &renderGeometry);

        DummyAttribute attribute;

        // WHEN
        geometry.addAttribute(&attribute);
        renderGeometry.syncFromFrontEnd(&geometry, false);

        // THEN
        QCOMPARE(renderGeometry.attributes().size(), 1);
        QVERIFY(renderGeometry.isDirty());
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        renderGeometry.unsetDirty();
        QVERIFY(!renderGeometry.isDirty());

        // WHEN
        geometry.removeAttribute(&attribute);
        renderGeometry.syncFromFrontEnd(&geometry, false);

        // THEN
        QCOMPARE(renderGeometry.attributes().size(), 0);
        QVERIFY(renderGeometry.isDirty());
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);

        renderGeometry.unsetDirty();
        QVERIFY(!renderGeometry.isDirty());

        // WHEN
        geometry.setBoundingVolumePositionAttribute(&attribute);
        renderGeometry.syncFromFrontEnd(&geometry, false);

        // THEN
        QCOMPARE(renderGeometry.boundingPositionAttribute(), attribute.id());
        QVERIFY(!renderGeometry.isDirty());
        QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
    }

    void checkExtentTransmission()
    {
        // GIVEN
        TestRenderer renderer;
        TestArbiter arbiter;
        Qt3DRender::Render::Geometry renderGeometry;

        renderGeometry.setRenderer(&renderer);

        // WHEN
        renderGeometry.updateExtent(QVector3D(-1.0f, -1.0f, -1.0f), QVector3D(1.0f, 1.0f, 1.0f));

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);

        QCOMPARE(renderGeometry.min(), QVector3D(-1.0f, -1.0f, -1.0f));
        QCOMPARE(renderGeometry.max(), QVector3D(1.0f, 1.0f, 1.0f));
    }
};


QTEST_APPLESS_MAIN(tst_RenderGeometry)

#include "tst_geometry.moc"
