// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DAnimation/qclipanimator.h>
#include <Qt3DAnimation/private/qanimationclip_p.h>
#include <Qt3DAnimation/private/qclipanimator_p.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class tst_QClipAnimator : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<Qt3DAnimation::QAbstractAnimationClip*>();
        qRegisterMetaType<Qt3DAnimation::QChannelMapper*>();
        qRegisterMetaType<Qt3DAnimation::QClock*>();
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QClipAnimator animator;

        // THEN
        QCOMPARE(animator.clip(), static_cast<Qt3DAnimation::QAbstractAnimationClip *>(nullptr));
        QCOMPARE(animator.channelMapper(), static_cast<Qt3DAnimation::QChannelMapper *>(nullptr));
        QCOMPARE(animator.clock(), static_cast<Qt3DAnimation::QClock*>(nullptr));
        QCOMPARE(animator.loopCount(), 1);
        QCOMPARE(animator.normalizedTime(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QClipAnimator animator;

        {
            // WHEN
            QSignalSpy spy(&animator, SIGNAL(clipChanged(Qt3DAnimation::QAbstractAnimationClip*)));
            auto newValue = new Qt3DAnimation::QAnimationClipLoader();
            animator.setClip(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(animator.clip(), newValue);
            QCOMPARE(newValue->parent(), &animator);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animator.setClip(newValue);

            // THEN
            QCOMPARE(animator.clip(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&animator, SIGNAL(channelMapperChanged(Qt3DAnimation::QChannelMapper*)));
            auto newValue = new Qt3DAnimation::QChannelMapper();
            animator.setChannelMapper(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(animator.channelMapper(), newValue);
            QCOMPARE(newValue->parent(), &animator);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animator.setChannelMapper(newValue);

            // THEN
            QCOMPARE(animator.channelMapper(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&animator, SIGNAL(clockChanged(Qt3DAnimation::QClock*)));
            auto clock = new Qt3DAnimation::QClock();
            animator.setClock(clock);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(animator.clock(), clock);
            QCOMPARE(clock->parent(), &animator);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animator.setClock(clock);

            // THEN
            QCOMPARE(animator.clock(), clock);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&animator, SIGNAL(loopCountChanged(int)));
            const int newValue = 5;
            animator.setLoopCount(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(animator.loopCount(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animator.setLoopCount(newValue);

            // THEN
            QCOMPARE(animator.loopCount(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkRunning()
    {
        // GIVEN
        Qt3DAnimation::QClipAnimator animator;
        animator.stop();

        {
            // WHEN
            animator.setRunning(true);

            // THEN
            QCOMPARE(animator.isRunning(), false);
        }

        {
            // WHEN
            Qt3DAnimation::QChannelMapper *mapper = new Qt3DAnimation::QChannelMapper;
            Qt3DAnimation::QAnimationClip *clip = new Qt3DAnimation::QAnimationClip;
            animator.setClip(clip);
            animator.setChannelMapper(mapper);
            animator.setRunning(true);

            // THEN
            QCOMPARE(animator.isRunning(), true);
        }
    }

    void checkPropertyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QClipAnimator animator;
        auto clip = new Qt3DAnimation::QAnimationClipLoader();
        arbiter.setArbiterOnNode(&animator);

        {
            // WHEN
            animator.setClip(clip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &animator);

            arbiter.clear();
        }

        {
            // WHEN
            animator.setClip(clip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        // GIVEN
        auto mapper = new Qt3DAnimation::QChannelMapper;
        {
            // WHEN
            animator.setChannelMapper(mapper);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &animator);

            arbiter.clear();
        }

        {
            // WHEN
            animator.setChannelMapper(mapper);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        // GIVEN
        auto clock = new Qt3DAnimation::QClock;
        {
            // WHEN
            animator.setClock(clock);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &animator);

            arbiter.clear();
        }

        {
            // WHEN
            animator.setClock(clock);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            animator.setLoopCount(10);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &animator);

            arbiter.clear();
        }

        {
            // WHEN
            animator.setLoopCount(10);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }
};

QTEST_MAIN(tst_QClipAnimator)

#include "tst_qclipanimator.moc"
