// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DAnimation/qclipblendvalue.h>
#include <Qt3DAnimation/qanimationcliploader.h>
#include <Qt3DAnimation/private/qclipblendvalue_p.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class tst_QClipBlendValue : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<Qt3DAnimation::QAbstractAnimationClip*>();
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QClipBlendValue clipBlendNode;

        // THEN;
        QCOMPARE(clipBlendNode.clip(), static_cast<Qt3DAnimation::QAbstractAnimationClip *>(nullptr));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QClipBlendValue clipBlendNode;

        {
            // WHEN
            QSignalSpy spy(&clipBlendNode, SIGNAL(clipChanged(Qt3DAnimation::QAbstractAnimationClip*)));
            auto newValue = new Qt3DAnimation::QAnimationClipLoader();
            clipBlendNode.setClip(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(clipBlendNode.clip(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            clipBlendNode.setClip(newValue);

            // THEN
            QCOMPARE(clipBlendNode.clip(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkClipUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QClipBlendValue clipBlendNode;
        arbiter.setArbiterOnNode(&clipBlendNode);
        auto clip = new Qt3DAnimation::QAnimationClipLoader();

        {
            // WHEN
            clipBlendNode.setClip(clip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &clipBlendNode);

            arbiter.clear();
        }

        {
            // WHEN
            clipBlendNode.setClip(clip);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkStartClipBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DAnimation::QClipBlendValue> clipBlendNode(new Qt3DAnimation::QClipBlendValue);
        {
            // WHEN
            Qt3DAnimation::QAnimationClipLoader clip;
            clipBlendNode->setClip(&clip);

            // THEN
            QCOMPARE(clip.parent(), clipBlendNode.data());
            QCOMPARE(clipBlendNode->clip(), &clip);
        }
        // THEN (Should not crash and clip be unset)
        QVERIFY(clipBlendNode->clip() == nullptr);

        {
            // WHEN
            Qt3DAnimation::QClipBlendValue someOtherNode;
            QScopedPointer<Qt3DAnimation::QAnimationClipLoader> clip(new Qt3DAnimation::QAnimationClipLoader(&someOtherNode));
            clipBlendNode->setClip(clip.data());

            // THEN
            QCOMPARE(clip->parent(), &someOtherNode);
            QCOMPARE(clipBlendNode->clip(), clip.data());

            // WHEN
            clipBlendNode.reset();
            clip.reset();

            // THEN Should not crash when the effect is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QClipBlendValue)

#include "tst_qclipblendvalue.moc"
