// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/qtest.h>
#include <Qt3DAnimation/qabstractanimation.h>
#include <Qt3DAnimation/private/qabstractanimation_p.h>
#include <Qt3DCore/qnode.h>
#include <qobject.h>
#include <qsignalspy.h>

#include <private/qabstractanimation_p.h>

class TestAnimation : public Qt3DAnimation::QAbstractAnimation
{
public:
    explicit TestAnimation(Qt3DCore::QNode *parent = nullptr)
        : Qt3DAnimation::QAbstractAnimation(
              *new Qt3DAnimation::QAbstractAnimationPrivate(
                  Qt3DAnimation::QAbstractAnimation::KeyframeAnimation), parent)
    {

    }
};

class tst_QAbstractAnimation : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        TestAnimation abstractAnimation;

        // THEN
        QCOMPARE(abstractAnimation.animationName(), QString());
        QCOMPARE(abstractAnimation.animationType(),
                 Qt3DAnimation::QAbstractAnimation::KeyframeAnimation);
        QCOMPARE(abstractAnimation.position(), 0.0f);
        QCOMPARE(abstractAnimation.duration(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestAnimation abstractAnimation;

        {
            // WHEN
            QSignalSpy spy(&abstractAnimation, SIGNAL(animationNameChanged(QString)));
            const QString newValue = QString("test");
            abstractAnimation.setAnimationName(newValue);

            // THEN
            QCOMPARE(abstractAnimation.animationName(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            abstractAnimation.setAnimationName(newValue);

            // THEN
            QCOMPARE(abstractAnimation.animationName(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&abstractAnimation, SIGNAL(positionChanged(float)));
            const float newValue = 1.0f;
            abstractAnimation.setPosition(newValue);

            // THEN
            QCOMPARE(abstractAnimation.position(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            abstractAnimation.setPosition(newValue);

            // THEN
            QCOMPARE(abstractAnimation.position(), newValue);
            QCOMPARE(spy.size(), 0);

        }
    }

};

QTEST_APPLESS_MAIN(tst_QAbstractAnimation)

#include "tst_qabstractanimation.moc"
