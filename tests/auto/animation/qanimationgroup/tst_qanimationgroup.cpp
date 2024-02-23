// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DAnimation/qanimationgroup.h>
#include <qobject.h>
#include <qsignalspy.h>

class tst_QAnimationGroup : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QAnimationGroup animationGroup;

        // THEN
        QCOMPARE(animationGroup.name(), QString());
        QCOMPARE(animationGroup.position(), 0.0f);
        QCOMPARE(animationGroup.duration(), 0.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QAnimationGroup animationGroup;

        {
            // WHEN
            QSignalSpy spy(&animationGroup, SIGNAL(nameChanged(QString)));
            const QString newValue = QString("group");
            animationGroup.setName(newValue);

            // THEN
            QCOMPARE(animationGroup.name(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationGroup.setName(newValue);

            // THEN
            QCOMPARE(animationGroup.name(), newValue);
            QCOMPARE(spy.size(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&animationGroup, SIGNAL(positionChanged(float)));
            const float newValue = 2.0f;
            animationGroup.setPosition(newValue);

            // THEN
            QCOMPARE(animationGroup.position(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            animationGroup.setPosition(newValue);

            // THEN
            QCOMPARE(animationGroup.position(), newValue);
            QCOMPARE(spy.size(), 0);

        }
    }

};

QTEST_APPLESS_MAIN(tst_QAnimationGroup)

#include "tst_qanimationgroup.moc"
