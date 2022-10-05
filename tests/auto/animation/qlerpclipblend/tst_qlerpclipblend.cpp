// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DAnimation/qlerpclipblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qlerpclipblend_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QLerpClipBlend : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<Qt3DAnimation::QAbstractClipBlendNode*>();
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend lerpBlend;

        // THEN
        QCOMPARE(lerpBlend.blendFactor(), 0.0f);
        QCOMPARE(lerpBlend.startClip(), static_cast<Qt3DAnimation::QAbstractClipBlendNode *>(nullptr));
        QCOMPARE(lerpBlend.endClip(), static_cast<Qt3DAnimation::QAbstractClipBlendNode *>(nullptr));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QLerpClipBlend lerpBlend;

        {
            // WHEN
            QSignalSpy spy(&lerpBlend, SIGNAL(blendFactorChanged(float)));
            const float newValue = 0.5f;
            lerpBlend.setBlendFactor(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(lerpBlend.blendFactor(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            lerpBlend.setBlendFactor(newValue);

            // THEN
            QCOMPARE(lerpBlend.blendFactor(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&lerpBlend, SIGNAL(startClipChanged(Qt3DAnimation::QAbstractClipBlendNode*)));
            auto newValue = new Qt3DAnimation::QLerpClipBlend();
            lerpBlend.setStartClip(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(lerpBlend.startClip(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            lerpBlend.setStartClip(newValue);

            // THEN
            QCOMPARE(lerpBlend.startClip(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&lerpBlend, SIGNAL(endClipChanged(Qt3DAnimation::QAbstractClipBlendNode*)));
            auto newValue = new Qt3DAnimation::QLerpClipBlend();
            lerpBlend.setEndClip(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(lerpBlend.endClip(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            lerpBlend.setEndClip(newValue);

            // THEN
            QCOMPARE(lerpBlend.endClip(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkBlendFactorUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QLerpClipBlend lerpBlend;
        arbiter.setArbiterOnNode(&lerpBlend);

        {
            // WHEN
            lerpBlend.setBlendFactor(0.4f);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &lerpBlend);

            arbiter.clear();
        }

        {
            // WHEN
            lerpBlend.setBlendFactor(0.4f);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkStartClipUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QLerpClipBlend lerpBlend;
        arbiter.setArbiterOnNode(&lerpBlend);
        auto startClip = new Qt3DAnimation::QLerpClipBlend();

        {
            // WHEN
            lerpBlend.setStartClip(startClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &lerpBlend);

            arbiter.clear();
        }

        {
            // WHEN
            lerpBlend.setStartClip(startClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkEndClipUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QLerpClipBlend lerpBlend;
        arbiter.setArbiterOnNode(&lerpBlend);
        auto endClip = new Qt3DAnimation::QLerpClipBlend();

        {
            // WHEN
            lerpBlend.setEndClip(endClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &lerpBlend);

            arbiter.clear();
        }

        {
            // WHEN
            lerpBlend.setEndClip(endClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkStartClipBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DAnimation::QLerpClipBlend> lerpBlend(new Qt3DAnimation::QLerpClipBlend);
        {
            // WHEN
            Qt3DAnimation::QLerpClipBlend clip;
            lerpBlend->setStartClip(&clip);

            // THEN
            QCOMPARE(clip.parent(), lerpBlend.data());
            QCOMPARE(lerpBlend->startClip(), &clip);
        }
        // THEN (Should not crash and clip be unset)
        QVERIFY(lerpBlend->startClip() == nullptr);

        {
            // WHEN
            Qt3DAnimation::QLerpClipBlend someOtherLerpBlend;
            QScopedPointer<Qt3DAnimation::QLerpClipBlend> clip(new Qt3DAnimation::QLerpClipBlend(&someOtherLerpBlend));
            lerpBlend->setStartClip(clip.data());

            // THEN
            QCOMPARE(clip->parent(), &someOtherLerpBlend);
            QCOMPARE(lerpBlend->startClip(), clip.data());

            // WHEN
            lerpBlend.reset();
            clip.reset();

            // THEN Should not crash when the effect is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkEndClipBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DAnimation::QLerpClipBlend> lerpBlend(new Qt3DAnimation::QLerpClipBlend);
        {
            // WHEN
            Qt3DAnimation::QLerpClipBlend clip;
            lerpBlend->setEndClip(&clip);

            // THEN
            QCOMPARE(clip.parent(), lerpBlend.data());
            QCOMPARE(lerpBlend->endClip(), &clip);
        }
        // THEN (Should not crash and clip be unset)
        QVERIFY(lerpBlend->endClip() == nullptr);

        {
            // WHEN
            Qt3DAnimation::QLerpClipBlend someOtherLerpBlend;
            QScopedPointer<Qt3DAnimation::QLerpClipBlend> clip(new Qt3DAnimation::QLerpClipBlend(&someOtherLerpBlend));
            lerpBlend->setEndClip(clip.data());

            // THEN
            QCOMPARE(clip->parent(), &someOtherLerpBlend);
            QCOMPARE(lerpBlend->endClip(), clip.data());

            // WHEN
            lerpBlend.reset();
            clip.reset();

            // THEN Should not crash when the effect is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QLerpClipBlend)

#include "tst_qlerpclipblend.moc"
