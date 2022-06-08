// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DRender/qproximityfilter.h>
#include <Qt3DRender/private/qproximityfilter_p.h>
#include <Qt3DRender/private/proximityfilter_p.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"

class tst_ProximityFilter : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::ProximityFilter backendProximityFilter;

        // THEN
        QCOMPARE(backendProximityFilter.isEnabled(), false);
        QVERIFY(backendProximityFilter.peerId().isNull());
        QCOMPARE(backendProximityFilter.distanceThreshold(), 0.0f);
        QCOMPARE(backendProximityFilter.entityId(), Qt3DCore::QNodeId());
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::QProximityFilter proximityFilter;
        Qt3DCore::QEntity entity;
        proximityFilter.setDistanceThreshold(1340.0f);
        proximityFilter.setEntity(&entity);

        {
            // WHEN
            Qt3DRender::Render::ProximityFilter backendProximityFilter;
            backendProximityFilter.setRenderer(&renderer);
            simulateInitializationSync(&proximityFilter, &backendProximityFilter);

            // THEN
            QCOMPARE(backendProximityFilter.isEnabled(), true);
            QCOMPARE(backendProximityFilter.peerId(), proximityFilter.id());
            QCOMPARE(backendProximityFilter.distanceThreshold(), 1340.f);
            QCOMPARE(backendProximityFilter.entityId(), entity.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
        renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        {
            // WHEN
            Qt3DRender::Render::ProximityFilter backendProximityFilter;
            backendProximityFilter.setRenderer(&renderer);
            proximityFilter.setEnabled(false);
            simulateInitializationSync(&proximityFilter, &backendProximityFilter);

            // THEN
            QCOMPARE(backendProximityFilter.peerId(), proximityFilter.id());
            QCOMPARE(backendProximityFilter.isEnabled(), false);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::QProximityFilter proximityFilter;
        Qt3DRender::Render::ProximityFilter backendProximityFilter;
        TestRenderer renderer;
        backendProximityFilter.setRenderer(&renderer);
        simulateInitializationSync(&proximityFilter, &backendProximityFilter);

        {
             // WHEN
             const bool newValue = false;
             proximityFilter.setEnabled(newValue);
             backendProximityFilter.syncFromFrontEnd(&proximityFilter, false);

             // THEN
            QCOMPARE(backendProximityFilter.isEnabled(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
             // WHEN
             const float newValue = 383.0f;
             proximityFilter.setDistanceThreshold(newValue);
             backendProximityFilter.syncFromFrontEnd(&proximityFilter, false);


             // THEN
            QCOMPARE(backendProximityFilter.distanceThreshold(), newValue);
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
        {
             // WHEN
             Qt3DCore::QEntity e;
             proximityFilter.setEntity(&e);
             backendProximityFilter.syncFromFrontEnd(&proximityFilter, false);

             // THEN
            QCOMPARE(backendProximityFilter.entityId(), e.id());
            QVERIFY(renderer.dirtyBits() & Qt3DRender::Render::AbstractRenderer::FrameGraphDirty);
            renderer.clearDirtyBits(Qt3DRender::Render::AbstractRenderer::AllDirty);
        }
    }

};

QTEST_MAIN(tst_ProximityFilter)

#include "tst_proximityfilter.moc"
