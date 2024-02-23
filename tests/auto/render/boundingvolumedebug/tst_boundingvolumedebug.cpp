// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/private/boundingvolumedebug_p.h>
#include <Qt3DRender/private/qboundingvolumedebug_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/qnodepropertychange.h>
#include "testpostmanarbiter.h"
#include "testrenderer.h"

class tst_BoundingVolumeDebug : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::BoundingVolumeDebug boundingVolumeDebug;
        Qt3DRender::QBoundingVolumeDebug fbvH;
        fbvH.setRecursive(true);

        // WHEN
        boundingVolumeDebug.setPeer(&fbvH);

        // THEN
        QVERIFY(!boundingVolumeDebug.peerId().isNull());
        QCOMPARE(boundingVolumeDebug.isRecursive(), true);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::BoundingVolumeDebug boundingVolumeDebug;

        // THEN
        QVERIFY(boundingVolumeDebug.peerId().isNull());
        QCOMPARE(boundingVolumeDebug.isRecursive(), false);
        QCOMPARE(boundingVolumeDebug.radius(), 0.0f);
        QCOMPARE(boundingVolumeDebug.center(), QVector3D());

        // GIVEN
        Qt3DRender::QBoundingVolumeDebug fbvH;
        fbvH.setEnabled(true);

        // WHEN
        boundingVolumeDebug.updateFromPeer(&fbvH);
        boundingVolumeDebug.setRadius(427.0f);
        boundingVolumeDebug.setCenter(QVector3D(454.0f, 383.0f, 355.0f));
        boundingVolumeDebug.cleanup();

        // THEN
        QVERIFY(boundingVolumeDebug.peerId().isNull());
        QCOMPARE(boundingVolumeDebug.isRecursive(), false);
        QCOMPARE(boundingVolumeDebug.radius(), 0.0f);
        QCOMPARE(boundingVolumeDebug.center(), QVector3D());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::BoundingVolumeDebug boundingVolumeDebug;
        boundingVolumeDebug.setRenderer(&renderer);
        QVERIFY(!boundingVolumeDebug.isRecursive());

        // WHEN
        Qt3DCore::QNodePropertyChangePtr updateChange(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(true);
        updateChange->setPropertyName("recursive");
        boundingVolumeDebug.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(boundingVolumeDebug.isRecursive(), true);
        QVERIFY(renderer.dirtyBits() != 0);
    }

    void checkBackendPropertyNotifications()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::Render::BoundingVolumeDebug boundingVolumeDebug;
        Qt3DCore::QBackendNodePrivate::get(&boundingVolumeDebug)->setArbiter(&arbiter);

        // WHEN
        boundingVolumeDebug.setRadius(1340.0f);

        // THEN
        QCOMPARE(arbiter.events.count(), 1);
        Qt3DCore::QNodePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "radius");
        QCOMPARE(change->value().toFloat(), 1340.0f);

        arbiter.events.clear();

        // WHEN
        boundingVolumeDebug.setRadius(1340.0f);

        // THEN
        QCOMPARE(arbiter.events.count(), 0);


        // WHEN
        boundingVolumeDebug.setCenter(QVector3D(1.0f, 2.0f, 3.0f));

        // THEN
        QCOMPARE(arbiter.events.count(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "center");
        QCOMPARE(change->value().value<QVector3D>(), QVector3D(1.0f, 2.0f, 3.0f));

        arbiter.events.clear();

        // WHEN
        boundingVolumeDebug.setCenter(QVector3D(1.0f, 2.0f, 3.0f));


        // THEN
        QCOMPARE(arbiter.events.count(), 0);
    }
};


QTEST_APPLESS_MAIN(tst_BoundingVolumeDebug)

#include "tst_boundingvolumedebug.moc"
