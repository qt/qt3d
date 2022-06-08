// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/private/clock_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testarbiter.h>

class tst_Clock: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DAnimation::Animation::Clock backendClock;
        Qt3DAnimation::QClock clock;

        clock.setPlaybackRate(10.5);

        // WHEN
        simulateInitializationSync(&clock, &backendClock);

        // THEN
        QCOMPARE(backendClock.playbackRate(), clock.playbackRate());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DAnimation::Animation::Clock backendClock;

        // THEN
        QCOMPARE(backendClock.playbackRate(), 1.0);

        // GIVEN
        Qt3DAnimation::QClock clock;
        clock.setPlaybackRate(10.5);

        // WHEN
        simulateInitializationSync(&clock, &backendClock);
        backendClock.cleanup();

        // THEN
        QCOMPARE(backendClock.playbackRate(), 1.0);
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DAnimation::QClock clock;
        Qt3DAnimation::Animation::Clock backendClock;
        simulateInitializationSync(&clock, &backendClock);

        {
            // WHEN
            const bool newValue = false;
            clock.setEnabled(newValue);
            backendClock.syncFromFrontEnd(&clock, false);

            // THEN
            QCOMPARE(backendClock.isEnabled(), newValue);
        }
        {
            // WHEN
            const double newPlaybackRateValue = 2.0;
            clock.setPlaybackRate(newPlaybackRateValue);
            backendClock.syncFromFrontEnd(&clock, false);

            // THEN
            QCOMPARE(backendClock.playbackRate(), newPlaybackRateValue);
        }
    }
};

QTEST_APPLESS_MAIN(tst_Clock)

#include "tst_clock.moc"
