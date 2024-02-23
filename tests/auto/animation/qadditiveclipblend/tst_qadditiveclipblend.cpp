// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qadditiveclipblend_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QAdditiveClipBlend : public QObject
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
        Qt3DAnimation::QAdditiveClipBlend addBlend;

        // THEN
        QCOMPARE(addBlend.additiveFactor(), 0.0f);
        QCOMPARE(addBlend.baseClip(), static_cast<Qt3DAnimation::QAbstractClipBlendNode *>(nullptr));
        QCOMPARE(addBlend.additiveClip(), static_cast<Qt3DAnimation::QAbstractClipBlendNode *>(nullptr));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QAdditiveClipBlend addBlend;

        {
            // WHEN
            QSignalSpy spy(&addBlend, SIGNAL(additiveFactorChanged(float)));
            const float newValue = 0.5f;
            addBlend.setAdditiveFactor(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(addBlend.additiveFactor(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            addBlend.setAdditiveFactor(newValue);

            // THEN
            QCOMPARE(addBlend.additiveFactor(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&addBlend, SIGNAL(baseClipChanged(Qt3DAnimation::QAbstractClipBlendNode*)));
            auto newValue = new Qt3DAnimation::QAdditiveClipBlend();
            addBlend.setBaseClip(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(addBlend.baseClip(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            addBlend.setBaseClip(newValue);

            // THEN
            QCOMPARE(addBlend.baseClip(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&addBlend, SIGNAL(additiveClipChanged(Qt3DAnimation::QAbstractClipBlendNode*)));
            auto newValue = new Qt3DAnimation::QAdditiveClipBlend();
            addBlend.setAdditiveClip(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(addBlend.additiveClip(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            addBlend.setAdditiveClip(newValue);

            // THEN
            QCOMPARE(addBlend.additiveClip(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkBaseClipUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QAdditiveClipBlend addBlend;
        arbiter.setArbiterOnNode(&addBlend);
        auto baseClip = new Qt3DAnimation::QAdditiveClipBlend();

        {
            // WHEN
            addBlend.setBaseClip(baseClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &addBlend);

            arbiter.clear();
        }

        {
            // WHEN
            addBlend.setBaseClip(baseClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkAdditiveClipUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QAdditiveClipBlend addBlend;
        arbiter.setArbiterOnNode(&addBlend);
        auto additiveClip = new Qt3DAnimation::QAdditiveClipBlend();

        {
            // WHEN
            addBlend.setAdditiveClip(additiveClip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &addBlend);
            arbiter.clear();
        }

        {
            // WHEN
            addBlend.setAdditiveClip(additiveClip);

            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkBaseClipBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DAnimation::QAdditiveClipBlend> additiveBlend(new Qt3DAnimation::QAdditiveClipBlend);
        {
            // WHEN
            Qt3DAnimation::QAdditiveClipBlend clip;
            additiveBlend->setBaseClip(&clip);

            // THEN
            QCOMPARE(clip.parent(), additiveBlend.data());
            QCOMPARE(additiveBlend->baseClip(), &clip);
        }
        // THEN (Should not crash and clip be unset)
        QVERIFY(additiveBlend->baseClip() == nullptr);

        {
            // WHEN
            Qt3DAnimation::QAdditiveClipBlend someOtherAdditiveBlend;
            QScopedPointer<Qt3DAnimation::QAdditiveClipBlend> clip(new Qt3DAnimation::QAdditiveClipBlend(&someOtherAdditiveBlend));
            additiveBlend->setBaseClip(clip.data());

            // THEN
            QCOMPARE(clip->parent(), &someOtherAdditiveBlend);
            QCOMPARE(additiveBlend->baseClip(), clip.data());

            // WHEN
            additiveBlend.reset();
            clip.reset();

            // THEN Should not crash when the effect is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkAdditiveClipBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DAnimation::QAdditiveClipBlend> additiveBlend(new Qt3DAnimation::QAdditiveClipBlend);
        {
            // WHEN
            Qt3DAnimation::QAdditiveClipBlend clip;
            additiveBlend->setAdditiveClip(&clip);

            // THEN
            QCOMPARE(clip.parent(), additiveBlend.data());
            QCOMPARE(additiveBlend->additiveClip(), &clip);
        }
        // THEN (Should not crash and clip be unset)
        QVERIFY(additiveBlend->additiveClip() == nullptr);

        {
            // WHEN
            Qt3DAnimation::QAdditiveClipBlend someOtherAdditiveBlend;
            QScopedPointer<Qt3DAnimation::QAdditiveClipBlend> clip(new Qt3DAnimation::QAdditiveClipBlend(&someOtherAdditiveBlend));
            additiveBlend->setAdditiveClip(clip.data());

            // THEN
            QCOMPARE(clip->parent(), &someOtherAdditiveBlend);
            QCOMPARE(additiveBlend->additiveClip(), clip.data());

            // WHEN
            additiveBlend.reset();
            clip.reset();

            // THEN Should not crash when the effect is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAdditiveClipBlend)

#include "tst_qadditiveclipblend.moc"
