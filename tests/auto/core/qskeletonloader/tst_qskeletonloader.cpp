// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/private/qskeletonloader_p.h>
#include <QObject>
#include <QSignalSpy>
#include <testarbiter.h>

using namespace Qt3DCore;

class tst_QSkeletonLoader : public QSkeletonLoader
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        QSkeletonLoader skeleton;

        // THEN
        QCOMPARE(skeleton.source(), QUrl());
        QCOMPARE(skeleton.status(), QSkeletonLoader::NotReady);
        QCOMPARE(skeleton.isCreateJointsEnabled(), false);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        QSkeletonLoader skeleton;

        {
            // WHEN
            QSignalSpy spy(&skeleton, SIGNAL(sourceChanged(QUrl)));
            const QUrl newValue(QStringLiteral("qrc:/zergling.skel"));
            skeleton.setSource(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(skeleton.source(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            skeleton.setSource(newValue);

            // THEN
            QCOMPARE(skeleton.source(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&skeleton, SIGNAL(createJointsEnabledChanged(bool)));
            const bool newValue(true);
            skeleton.setCreateJointsEnabled(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(skeleton.isCreateJointsEnabled(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            skeleton.setCreateJointsEnabled(newValue);

            // THEN
            QCOMPARE(skeleton.isCreateJointsEnabled(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QSkeletonLoader skeleton;
        arbiter.setArbiterOnNode(&skeleton);

        {
            // WHEN
            skeleton.setSource(QUrl(QStringLiteral("qrc:/hydralisk.skel")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes()[0], &skeleton);

            arbiter.clear();
        }

        {
            // WHEN
            skeleton.setSource(QStringLiteral("qrc:/hydralisk.skel"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            skeleton.setCreateJointsEnabled(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes()[0], &skeleton);

            arbiter.clear();
        }

        {
            // WHEN
            skeleton.setCreateJointsEnabled(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }
};

QTEST_MAIN(tst_QSkeletonLoader)

#include "tst_qskeletonloader.moc"
