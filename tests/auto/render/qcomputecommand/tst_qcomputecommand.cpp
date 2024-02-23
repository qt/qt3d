// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/private/qcomputecommand_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QComputeCommand : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QComputeCommand computeCommand;

        // THEN
        QCOMPARE(computeCommand.workGroupX(), 1);
        QCOMPARE(computeCommand.workGroupY(), 1);
        QCOMPARE(computeCommand.workGroupZ(), 1);
        QCOMPARE(computeCommand.runType(), Qt3DRender::QComputeCommand::Continuous);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QComputeCommand computeCommand;

        {
            // WHEN
            QSignalSpy spy(&computeCommand, SIGNAL(workGroupXChanged()));
            const int newValue = 128;
            computeCommand.setWorkGroupX(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(computeCommand.workGroupX(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            computeCommand.setWorkGroupX(newValue);

            // THEN
            QCOMPARE(computeCommand.workGroupX(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&computeCommand, SIGNAL(workGroupYChanged()));
            const int newValue = 256;
            computeCommand.setWorkGroupY(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(computeCommand.workGroupY(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            computeCommand.setWorkGroupY(newValue);

            // THEN
            QCOMPARE(computeCommand.workGroupY(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&computeCommand, SIGNAL(workGroupZChanged()));
            const int newValue = 512;
            computeCommand.setWorkGroupZ(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(computeCommand.workGroupZ(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            computeCommand.setWorkGroupZ(newValue);

            // THEN
            QCOMPARE(computeCommand.workGroupZ(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&computeCommand, SIGNAL(runTypeChanged()));
            const Qt3DRender::QComputeCommand::RunType newValue = Qt3DRender::QComputeCommand::Manual;
            computeCommand.setRunType(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(computeCommand.runType(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            computeCommand.setRunType(newValue);

            // THEN
            QCOMPARE(computeCommand.runType(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkWorkGroupXUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QComputeCommand computeCommand;
        arbiter.setArbiterOnNode(&computeCommand);

        {
            // WHEN
            computeCommand.setWorkGroupX(256);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);

            arbiter.clear();
        }

        {
            // WHEN
            computeCommand.setWorkGroupX(256);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorkGroupYUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QComputeCommand computeCommand;
        arbiter.setArbiterOnNode(&computeCommand);

        {
            // WHEN
            computeCommand.setWorkGroupY(512);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);

            arbiter.clear();
        }

        {
            // WHEN
            computeCommand.setWorkGroupY(512);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkWorkGroupZUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QComputeCommand computeCommand;
        arbiter.setArbiterOnNode(&computeCommand);

        {
            // WHEN
            computeCommand.setWorkGroupZ(64);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);

            arbiter.clear();
        }

        {
            // WHEN
            computeCommand.setWorkGroupZ(64);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkRunTypeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QComputeCommand computeCommand;
        arbiter.setArbiterOnNode(&computeCommand);

        {
            // WHEN
            computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);

            arbiter.clear();
        }

        {
            // WHEN
            computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkTrigger()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QComputeCommand computeCommand;
        arbiter.setArbiterOnNode(&computeCommand);
        computeCommand.setRunType(Qt3DRender::QComputeCommand::Manual);
        computeCommand.setEnabled(false);
        QCoreApplication::processEvents();
        arbiter.clear();

        {
            // WHEN
            computeCommand.trigger(1);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);
            QCOMPARE(computeCommand.isEnabled(), true);

            computeCommand.setEnabled(false);
            QCoreApplication::processEvents();
            arbiter.clear();
        }

        {
            // WHEN
            computeCommand.trigger(2);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);
            QCOMPARE(computeCommand.isEnabled(), true);

            computeCommand.setEnabled(false);
            QCoreApplication::processEvents();
            arbiter.clear();
        }

        {
            // WHEN
            computeCommand.trigger(10, 11, 12, 1);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &computeCommand);
            QCOMPARE(computeCommand.isEnabled(), true);
            QCOMPARE(computeCommand.workGroupX(), 10);
            QCOMPARE(computeCommand.workGroupY(), 11);
            QCOMPARE(computeCommand.workGroupZ(), 12);

            computeCommand.setEnabled(false);
            arbiter.clear();
        }
    }

};

QTEST_MAIN(tst_QComputeCommand)

#include "tst_qcomputecommand.moc"
