// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DAnimation/qblendedclipanimator.h>
#include <Qt3DAnimation/private/qblendedclipanimator_p.h>
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class tst_QBlendedClipAnimator : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DAnimation::QChannelMapper*>("QChannelMapper *");
        qRegisterMetaType<Qt3DAnimation::QAbstractClipBlendNode*>("QAbstractClipBlendNode *");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;

        // THEN
        QVERIFY(blendedClipAnimator.blendTree() == nullptr);
        QVERIFY(blendedClipAnimator.channelMapper() == nullptr);
        QCOMPARE(blendedClipAnimator.isRunning(), false);
        QCOMPARE(blendedClipAnimator.loopCount(), 1);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;

        {
            // WHEN
            QSignalSpy spy(&blendedClipAnimator, SIGNAL(blendTreeChanged(QAbstractClipBlendNode *)));
            Qt3DAnimation::QLerpClipBlend newValue;
            blendedClipAnimator.setBlendTree(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(blendedClipAnimator.blendTree(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            blendedClipAnimator.setBlendTree(&newValue);

            // THEN
            QCOMPARE(blendedClipAnimator.blendTree(), &newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&blendedClipAnimator, SIGNAL(channelMapperChanged(QChannelMapper *)));
            Qt3DAnimation::QChannelMapper newValue;
            blendedClipAnimator.setChannelMapper(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(blendedClipAnimator.channelMapper(), &newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            blendedClipAnimator.setChannelMapper(&newValue);

            // THEN
            QCOMPARE(blendedClipAnimator.channelMapper(), &newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&blendedClipAnimator, SIGNAL(runningChanged(bool)));
            const bool newValue = true;
            blendedClipAnimator.setRunning(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(blendedClipAnimator.isRunning(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            blendedClipAnimator.setRunning(newValue);

            // THEN
            QCOMPARE(blendedClipAnimator.isRunning(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&blendedClipAnimator, SIGNAL(loopCountChanged(int)));
            const int newValue = 5;
            blendedClipAnimator.setLoopCount(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(blendedClipAnimator.loopCount(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            blendedClipAnimator.setLoopCount(newValue);

            // THEN
            QCOMPARE(blendedClipAnimator.loopCount(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&blendedClipAnimator, SIGNAL(normalizedTimeChanged(float)));
            const float newValue = 0.5;
            blendedClipAnimator.setNormalizedTime(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(blendedClipAnimator.normalizedTime(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            blendedClipAnimator.setNormalizedTime(newValue);

            // THEN
            QCOMPARE(blendedClipAnimator.normalizedTime(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&blendedClipAnimator, SIGNAL(normalizedTimeChanged(float)));
            const float newValue = -0.01f; // Invalid
            blendedClipAnimator.setNormalizedTime(newValue);
            const float oldValue = blendedClipAnimator.normalizedTime();

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(blendedClipAnimator.normalizedTime(), oldValue);
            QCOMPARE(spy.size(), 0);

            // WHEN
            spy.clear();
            blendedClipAnimator.setNormalizedTime(1.01f); // Invalid

            // THEN
            QCOMPARE(blendedClipAnimator.normalizedTime(), oldValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkBlendTreeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        arbiter.setArbiterOnNode(&blendedClipAnimator);
        Qt3DAnimation::QLerpClipBlend blendRoot;

        {
            // WHEN
            blendedClipAnimator.setBlendTree(&blendRoot);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &blendedClipAnimator);

            arbiter.clear();
        }

        {
            // WHEN
            blendedClipAnimator.setBlendTree(&blendRoot);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkBlendTreeBookkeeping()
    {
        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;

        {
            // WHEN
            Qt3DAnimation::QLerpClipBlend blendRoot;
            blendedClipAnimator.setBlendTree(&blendRoot);

            QCOMPARE(blendedClipAnimator.blendTree(), &blendRoot);
        }

        // THEN -> should not crash
        QVERIFY(blendedClipAnimator.blendTree() == nullptr);
    }

    void checkChannelMapperUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        arbiter.setArbiterOnNode(&blendedClipAnimator);

        Qt3DAnimation::QChannelMapper channelMapper;
        {
            // WHEN
            blendedClipAnimator.setChannelMapper(&channelMapper);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &blendedClipAnimator);

            arbiter.clear();
        }

        {
            // WHEN
            blendedClipAnimator.setChannelMapper(&channelMapper);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkChannelMapperBookkeeping()
    {
        // GIVEN
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;

        {
            // WHEN
            Qt3DAnimation::QChannelMapper channelMapper;
            blendedClipAnimator.setChannelMapper(&channelMapper);

            QCOMPARE(blendedClipAnimator.channelMapper(), &channelMapper);
        }

        // THEN -> should not crash
        QVERIFY(blendedClipAnimator.channelMapper() == nullptr);
    }

    void checkRunningUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        arbiter.setArbiterOnNode(&blendedClipAnimator);

        {
            // WHEN
            blendedClipAnimator.setRunning(true);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &blendedClipAnimator);

            arbiter.clear();
        }

        {
            // WHEN
            blendedClipAnimator.setRunning(true);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLoopsUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        arbiter.setArbiterOnNode(&blendedClipAnimator);

        {
            // WHEN
            blendedClipAnimator.setLoopCount(1584);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &blendedClipAnimator);

            arbiter.clear();
        }

        {
            // WHEN
            blendedClipAnimator.setLoopCount(1584);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkNormalizedTimeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QBlendedClipAnimator blendedClipAnimator;
        arbiter.setArbiterOnNode(&blendedClipAnimator);

        {
            // WHEN
            blendedClipAnimator.setNormalizedTime(0.5f);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &blendedClipAnimator);

            arbiter.clear();
        }

        {
            // WHEN
            blendedClipAnimator.setNormalizedTime(0.5f);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }
};

QTEST_MAIN(tst_QBlendedClipAnimator)

#include "tst_qblendedclipanimator.moc"
