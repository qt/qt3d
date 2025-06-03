// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/QScreenRayCaster>

#include "testarbiter.h"

class MyRayCaster : public Qt3DRender::QScreenRayCaster
{
    Q_OBJECT
public:
    MyRayCaster(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QScreenRayCaster(parent)
    {
        qRegisterMetaType<Qt3DRender::QAbstractRayCaster::Hits>("Hits");
    }

private:
    friend class tst_RayCaster;
};

class tst_QScreenRayCaster : public QObject
{
    Q_OBJECT
public:
    tst_QScreenRayCaster()
    {
    }

    ~tst_QScreenRayCaster()
    {
        QMetaObject::invokeMethod(this, "_q_cleanup", Qt::DirectConnection);
    }

private Q_SLOTS:

    void checkState()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QScreenRayCaster> rayCaster(new Qt3DRender::QScreenRayCaster());

        QVERIFY(!rayCaster->isEnabled());
        QCOMPARE(rayCaster->runMode(), Qt3DRender::QAbstractRayCaster::SingleShot);

        // WHEN
        rayCaster->trigger();

        // THEN
        QVERIFY(rayCaster->isEnabled());

        // WHEN
        rayCaster->setEnabled(false);
        rayCaster->trigger(QPoint(200, 200));

        // THEN
        QVERIFY(rayCaster->isEnabled());
        QCOMPARE(rayCaster->position(), QPoint(200, 200));
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QScreenRayCaster> rayCaster(new Qt3DRender::QScreenRayCaster());
        arbiter.setArbiterOnNode(rayCaster.data());

        // WHEN
        rayCaster->setPosition({200, 200});
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), rayCaster.data());

        arbiter.clear();
    }

    void checkBackendUpdates_data()
    {
        QTest::addColumn<QByteArray>("signalPrototype");
        QTest::addColumn<QByteArray>("propertyName");

        QTest::newRow("hits")
                << QByteArray(SIGNAL(hitsChanged(Hits)))
                << QByteArrayLiteral("hits");
    }
};

QTEST_MAIN(tst_QScreenRayCaster)

#include "tst_qscreenraycaster.moc"
