// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/QRayCaster>
#include <Qt3DRender/private/qabstractraycaster_p.h>
#include <Qt3DRender/QLayer>

#include "testarbiter.h"

class MyRayCaster : public Qt3DRender::QRayCaster
{
    Q_OBJECT
public:
    MyRayCaster(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QRayCaster(parent)
    {
        qRegisterMetaType<Qt3DRender::QAbstractRayCaster::Hits>("Hits");
    }

private:
    friend class tst_RayCaster;
};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QRayCaster : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QRayCaster()
    {
    }

    ~tst_QRayCaster()
    {
        QMetaObject::invokeMethod(this, "_q_cleanup", Qt::DirectConnection);
    }

private Q_SLOTS:

    void checkState()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRayCaster> rayCaster(new Qt3DRender::QRayCaster());

        QVERIFY(!rayCaster->isEnabled());
        QVERIFY(rayCaster->direction().length() > 0.f);
        QCOMPARE(rayCaster->runMode(), Qt3DRender::QAbstractRayCaster::SingleShot);

        // WHEN
        rayCaster->trigger();

        // THEN
        QVERIFY(rayCaster->isEnabled());

        // WHEN
        rayCaster->setEnabled(false);
        rayCaster->trigger(QVector3D(1., 2., 3.), QVector3D(1., 0., 0.), 10.f);

        // THEN
        QVERIFY(rayCaster->isEnabled());
        QCOMPARE(rayCaster->origin(), QVector3D(1., 2., 3.));
        QCOMPARE(rayCaster->direction(), QVector3D(1., 0., 0.));
        QCOMPARE(rayCaster->length(), 10.f);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QRayCaster> rayCaster(new Qt3DRender::QRayCaster());
        arbiter.setArbiterOnNode(rayCaster.data());

        // WHEN
        rayCaster->setOrigin({1., 1., 1.});
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), rayCaster.data());

        arbiter.clear();

        // WHEN
        auto layer = new Qt3DRender::QLayer(rayCaster.data());
        QCoreApplication::processEvents();
        rayCaster->addLayer(layer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);

        arbiter.clear();

        // WHEN
        layer = new Qt3DRender::QLayer(rayCaster.data());
        QCoreApplication::processEvents();
        rayCaster->addLayer(layer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);

        arbiter.clear();

        // WHEN
        layer = rayCaster->layers().at(0);
        rayCaster->removeLayer(layer);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);

        arbiter.clear();
    }

    void checkLayerBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRayCaster> rayCaster(new Qt3DRender::QRayCaster);
        {
            // WHEN
            Qt3DRender::QLayer layer;
            rayCaster->addLayer(&layer);

            // THEN
            QCOMPARE(layer.parent(), rayCaster.data());
            QCOMPARE(rayCaster->layers().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(rayCaster->layers().empty());

        {
            // WHEN
            Qt3DRender::QRayCaster someOtherRayCaster;
            QScopedPointer<Qt3DRender::QLayer> layer(new Qt3DRender::QLayer(&someOtherRayCaster));
            rayCaster->addLayer(layer.data());

            // THEN
            QCOMPARE(layer->parent(), &someOtherRayCaster);
            QCOMPARE(rayCaster->layers().size(), 1);

            // WHEN
            rayCaster.reset();
            layer.reset();

            // THEN Should not crash when the layer is destroyed (tests for failed removal of destruction helper)
        }
    }


    void checkBackendUpdates()
    {
        // GIVEN
        QScopedPointer<MyRayCaster> rayCaster(new MyRayCaster());
        QSignalSpy spy(rayCaster.data(), SIGNAL(hitsChanged(Hits)));
        Qt3DRender::QRayCaster::Hits hits;

        QVERIFY(spy.isValid());

        // WHEN
        Qt3DRender::QAbstractRayCasterPrivate *d = Qt3DRender::QAbstractRayCasterPrivate::get(rayCaster.data());
        d->dispatchHits(hits);

        // THEN
        // Check that the QRayCaster triggers the expected signal
        QCOMPARE(spy.size(), 1);
    }
};

QTEST_MAIN(tst_QRayCaster)

#include "tst_qraycaster.moc"
