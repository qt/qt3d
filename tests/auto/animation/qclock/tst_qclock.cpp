// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/private/qclock_p.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class tst_QClock : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<Qt3DAnimation::QClock*>();
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QClock clock;

        // THEN
        QCOMPARE(clock.playbackRate(), 1.0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QClock clock;

        // WHEN
        QSignalSpy spy(&clock, SIGNAL(playbackRateChanged(double)));
        const double newValue = 5.5;
        clock.setPlaybackRate(newValue);

        // THEN
        QVERIFY(spy.isValid());
        QCOMPARE(clock.playbackRate(), newValue);
        QCOMPARE(spy.size(), 1);

        // WHEN
        spy.clear();
        clock.setPlaybackRate(newValue);

        // THEN
        QCOMPARE(clock.playbackRate(), newValue);
        QCOMPARE(spy.size(), 0);
    }

    void checkPropertyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QClock clock;
        arbiter.setArbiterOnNode(&clock);

        {
            // WHEN
            clock.setPlaybackRate(10.5f);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &clock);

            arbiter.clear();
        }

        {
            // WHEN
            clock.setPlaybackRate(10.5f);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QClock)

#include "tst_qclock.moc"
