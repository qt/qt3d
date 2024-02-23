// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/QLevelOfDetail>
#include <Qt3DRender/QLevelOfDetailBoundingSphere>
#include <Qt3DRender/private/levelofdetail_p.h>
#include <Qt3DRender/private/qlevelofdetail_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"
#include "testrenderer.h"

class tst_LevelOfDetail : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::LevelOfDetail renderLod;
        Qt3DRender::QLevelOfDetail lod;

        // WHEN
        renderLod.setRenderer(&renderer);
        simulateInitializationSync(&lod, &renderLod);

        // THEN
        QCOMPARE(renderLod.peerId(), lod.id());
        QVERIFY(renderLod.camera().isNull() && lod.camera() == nullptr);
        QCOMPARE(renderLod.currentIndex(), lod.currentIndex());
        QCOMPARE(renderLod.thresholdType(), lod.thresholdType());
        QCOMPARE(renderLod.thresholds(), lod.thresholds());
        QCOMPARE(renderLod.center(), lod.volumeOverride().center());
        QCOMPARE(renderLod.radius(), lod.volumeOverride().radius());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::LevelOfDetail renderLod;
        TestRenderer renderer;

        // THEN
        QCOMPARE(renderLod.camera(), Qt3DCore::QNodeId{});
        QCOMPARE(renderLod.currentIndex(), 0);
        QCOMPARE(renderLod.thresholdType(), Qt3DRender::QLevelOfDetail::DistanceToCameraThreshold);
        QVERIFY(renderLod.thresholds().empty());
        QCOMPARE(renderLod.radius(), 1.f);
        QCOMPARE(renderLod.center(), QVector3D{});
        QVERIFY(renderLod.peerId().isNull());

        // GIVEN
        Qt3DRender::QLevelOfDetail lod;
        lod.setThresholdType(Qt3DRender::QLevelOfDetail::ProjectedScreenPixelSizeThreshold);

        // WHEN
        renderLod.setRenderer(&renderer);
        simulateInitializationSync(&lod, &renderLod);

        // THEN
        QCOMPARE(renderLod.thresholdType(), lod.thresholdType());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QLevelOfDetail lod;
        Qt3DRender::Render::LevelOfDetail renderLod;
        renderLod.setRenderer(&renderer);
        simulateInitializationSync(&lod, &renderLod);

        // THEN
        QVERIFY(renderLod.thresholdType() != Qt3DRender::QLevelOfDetail::ProjectedScreenPixelSizeThreshold);
        QVERIFY(renderLod.camera().isNull());

        {
            // WHEN
            lod.setThresholdType(Qt3DRender::QLevelOfDetail::ProjectedScreenPixelSizeThreshold);
            renderLod.syncFromFrontEnd(&lod, false);

            // THEN
            QCOMPARE(renderLod.thresholdType(), Qt3DRender::QLevelOfDetail::ProjectedScreenPixelSizeThreshold);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::GeometryDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);        }

        {
            const QList<qreal> thresholds = { 20.f, 30.f, 40.f };

            // WHEN
            lod.setThresholds(thresholds);
            renderLod.syncFromFrontEnd(&lod, false);
            // THEN
            QCOMPARE(renderLod.thresholds(), thresholds);
        }

        {
            // WHEN
            const Qt3DRender::QLevelOfDetailBoundingSphere sphere(QVector3D(1.0f, 2.0f, 3.0f), 1.0f);
            // WHEN
            lod.setVolumeOverride(sphere);
            renderLod.syncFromFrontEnd(&lod, false);

            // THEN
            QCOMPARE(renderLod.center(), QVector3D(1., 2., 3.));
        }
    }
};


QTEST_APPLESS_MAIN(tst_LevelOfDetail)

#include "tst_levelofdetail.moc"
