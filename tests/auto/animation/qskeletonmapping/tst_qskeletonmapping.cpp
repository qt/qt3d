// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DAnimation/qskeletonmapping.h>
#include <Qt3DAnimation/private/qskeletonmapping_p.h>
#include <Qt3DCore/qskeleton.h>
#include <Qt3DCore/qentity.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

class tst_QSkeletonMapping : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<Qt3DCore::QAbstractSkeleton*>();
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QSkeletonMapping mapping;

        // THEN
        QCOMPARE(mapping.skeleton(), static_cast<Qt3DCore::QAbstractSkeleton *>(nullptr));
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QSkeletonMapping mapping;

        {
            // WHEN
            QSignalSpy spy(&mapping, SIGNAL(skeletonChanged(Qt3DCore::QAbstractSkeleton*)));
            auto newValue = new Qt3DCore::QSkeleton();
            mapping.setSkeleton(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mapping.skeleton(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mapping.setSkeleton(newValue);

            // THEN
            QCOMPARE(mapping.skeleton(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkPropertyUpdateChanges()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QSkeletonMapping mapping;
        arbiter.setArbiterOnNode(&mapping);

        {
            // WHEN
            auto target = new Qt3DCore::QSkeleton();
            mapping.setSkeleton(target);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mapping);

            arbiter.clear();

            // WHEN
            mapping.setSkeleton(target);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QSkeletonMapping)

#include "tst_qskeletonmapping.moc"
