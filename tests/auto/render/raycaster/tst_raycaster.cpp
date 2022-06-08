// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/raycaster_p.h>
#include <Qt3DRender/qpickevent.h>
#include <Qt3DRender/qraycaster.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"
#include "testrenderer.h"

class tst_RayCaster : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::RayCaster rayCaster;
        Qt3DRender::QRayCaster caster;
        caster.setRunMode(Qt3DRender::QRayCaster::Continuous);
        caster.setOrigin(QVector3D(1., 2., 3.));
        caster.setDirection(QVector3D(1., 0., 0.));
        caster.setLength(42.f);

        // WHEN
        rayCaster.setRenderer(&renderer);
        simulateInitializationSync(&caster, &rayCaster);

        // THEN
        QVERIFY(!rayCaster.peerId().isNull());
        QCOMPARE(rayCaster.runMode(), Qt3DRender::QRayCaster::Continuous);
        QCOMPARE(rayCaster.origin(), QVector3D(1., 2., 3.));
        QCOMPARE(rayCaster.direction(), QVector3D(1., 0., 0.));
        QCOMPARE(rayCaster.length(), 42.f);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::RayCaster rayCaster;

        // THEN
        QVERIFY(rayCaster.peerId().isNull());
        QCOMPARE(rayCaster.runMode(), Qt3DRender::QRayCaster::SingleShot);

        // GIVEN
        Qt3DRender::QRayCaster caster;
        caster.setRunMode(Qt3DRender::QRayCaster::Continuous);

        // WHEN
        rayCaster.setRenderer(&renderer);
        simulateInitializationSync(&caster, &rayCaster);
        rayCaster.cleanup();

        // THEN
        QCOMPARE(rayCaster.runMode(), Qt3DRender::QRayCaster::SingleShot);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        {
            Qt3DRender::QRayCaster caster;
            Qt3DRender::Render::RayCaster rayCaster;
            rayCaster.setRenderer(&renderer);
            simulateInitializationSync(&caster, &rayCaster);

            // WHEN
            caster.setRunMode(Qt3DRender::QRayCaster::Continuous);
            rayCaster.syncFromFrontEnd(&caster, false);

            // THEN
            QCOMPARE(rayCaster.runMode(), Qt3DRender::QRayCaster::Continuous);
            QVERIFY(renderer.dirtyBits() != 0);
        }
    }
};


QTEST_APPLESS_MAIN(tst_RayCaster)

#include "tst_raycaster.moc"
