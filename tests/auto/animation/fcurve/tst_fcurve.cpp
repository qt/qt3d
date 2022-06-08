// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <private/fcurve_p.h>

using namespace Qt3DAnimation;
using namespace Qt3DAnimation::Animation;

class tst_FCurve : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // WHEN
        FCurve fcurve;

        // THEN
        QCOMPARE(fcurve.keyframeCount(), 0);
        QCOMPARE(fcurve.startTime(), 0.0f);
        QCOMPARE(fcurve.endTime(), 0.0f);
    }

    void checkAddingKeyframes()
    {
        // GIVEN
        FCurve fcurve;

        // WHEN
        const Keyframe kf0{0.0f, {-5.0f, 0.0f}, {5.0f, 0.0f}, QKeyFrame::BezierInterpolation};
        fcurve.appendKeyframe(0.0f, kf0);

        // THEN
        QCOMPARE(fcurve.keyframeCount(), 1);
        QCOMPARE(fcurve.startTime(), 0.0f);
        QCOMPARE(fcurve.endTime(), 0.0f);

        // WHEN
        const Keyframe kf1{5.0f, {45.0f, 5.0f}, {55.0f, 5.0f}, QKeyFrame::BezierInterpolation};
        fcurve.appendKeyframe(50.0f, kf1);

        // THEN
        QCOMPARE(fcurve.keyframeCount(), 2);
        QCOMPARE(fcurve.startTime(), 0.0f);
        QCOMPARE(fcurve.endTime(), 50.0f);
        QCOMPARE(fcurve.keyframe(0), kf0);
        QCOMPARE(fcurve.keyframe(1), kf1);
    }

    void checkClearKeyframes()
    {
        // GIVEN
        FCurve fcurve;
        fcurve.appendKeyframe(0.0f, Keyframe{0.0f, {-5.0f, 0.0f}, {5.0f, 0.0f}, QKeyFrame::BezierInterpolation});
        fcurve.appendKeyframe(50.0f, Keyframe{5.0f, {45.0f, 5.0f}, {55.0f, 5.0f}, QKeyFrame::BezierInterpolation});

        // WHEN
        fcurve.clearKeyframes();

        // THEN
        QCOMPARE(fcurve.keyframeCount(), 0);
        QCOMPARE(fcurve.startTime(), 0.0f);
        QCOMPARE(fcurve.endTime(), 0.0f);
    }

    void checkEvaluateAtTime_data()
    {

        QTest::addColumn<float>("time");
        QTest::addColumn<float>("expectedValue");

        QTest::addRow("Constant_before_time") << 0.0f << 1.0f;
        QTest::addRow("Constant_1.5") << 1.5f << 1.0f;
        QTest::addRow("Constant_2.5") << 2.5f << 2.0f;
        QTest::addRow("Constant_after_time") << 3.5f << 4.0f;
    }

    void checkEvaluateAtTime()
    {
        QFETCH(float, time);
        QFETCH(float, expectedValue);

        FCurve curve;
        curve.appendKeyframe(1.0, Keyframe{1.0, {0.0, 0.0}, {0.0,0.0}, QKeyFrame::ConstantInterpolation});
        curve.appendKeyframe(2.0, Keyframe{2.0, {0.0, 0.0}, {0.0,0.0}, QKeyFrame::ConstantInterpolation});
        curve.appendKeyframe(3.0, Keyframe{4.0, {0.0, 0.0}, {0.0,0.0}, QKeyFrame::ConstantInterpolation});

        float actualValue = curve.evaluateAtTime(time);

        QCOMPARE(expectedValue, actualValue);
    }
};

QTEST_APPLESS_MAIN(tst_FCurve)

#include "tst_fcurve.moc"
