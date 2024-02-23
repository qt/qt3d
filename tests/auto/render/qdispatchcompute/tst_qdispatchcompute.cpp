// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DRender/private/qdispatchcompute_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QDispatchCompute : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QDispatchCompute dispatchCompute;

        // THEN
        QCOMPARE(dispatchCompute.workGroupX(), 1);
        QCOMPARE(dispatchCompute.workGroupY(), 1);
        QCOMPARE(dispatchCompute.workGroupZ(), 1);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QDispatchCompute dispatchCompute;

        {
            // WHEN
            QSignalSpy spy(&dispatchCompute, SIGNAL(workGroupXChanged()));
            const int newValue = 128;
            dispatchCompute.setWorkGroupX(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(dispatchCompute.workGroupX(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            dispatchCompute.setWorkGroupX(newValue);

            // THEN
            QCOMPARE(dispatchCompute.workGroupX(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&dispatchCompute, SIGNAL(workGroupYChanged()));
            const int newValue = 1024;
            dispatchCompute.setWorkGroupY(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(dispatchCompute.workGroupY(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            dispatchCompute.setWorkGroupY(newValue);

            // THEN
            QCOMPARE(dispatchCompute.workGroupY(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&dispatchCompute, SIGNAL(workGroupZChanged()));
            const int newValue = 32;
            dispatchCompute.setWorkGroupZ(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(dispatchCompute.workGroupZ(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            dispatchCompute.setWorkGroupZ(newValue);

            // THEN
            QCOMPARE(dispatchCompute.workGroupZ(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkWorkGroupXUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QDispatchCompute dispatchCompute;
        arbiter.setArbiterOnNode(&dispatchCompute);

        {
            // WHEN
            dispatchCompute.setWorkGroupX(883);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &dispatchCompute);

            arbiter.clear();
        }

        {
            // WHEN
            dispatchCompute.setWorkGroupX(883);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorkGroupYUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QDispatchCompute dispatchCompute;
        arbiter.setArbiterOnNode(&dispatchCompute);

        {
            // WHEN
            dispatchCompute.setWorkGroupY(1340);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &dispatchCompute);

            arbiter.clear();
        }

        {
            // WHEN
            dispatchCompute.setWorkGroupY(1340);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorkGroupZUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QDispatchCompute dispatchCompute;
        arbiter.setArbiterOnNode(&dispatchCompute);

        {
            // WHEN
            dispatchCompute.setWorkGroupZ(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &dispatchCompute);

            arbiter.clear();
        }

        {
            // WHEN
            dispatchCompute.setWorkGroupZ(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QDispatchCompute)

#include "tst_qdispatchcompute.moc"
