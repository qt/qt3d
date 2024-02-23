// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qaspectjob.h>

using namespace Qt3DCore;

class FakeAspectJob : public QAspectJob
{
public:
    void run() override {}
};

class tst_QAspectJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void shouldAddDependencies()
    {
        // GIVEN
        QAspectJobPtr job1(new FakeAspectJob);
        QAspectJobPtr job2(new FakeAspectJob);
        QAspectJobPtr job3(new FakeAspectJob);

        // THEN
        QVERIFY(job1->dependencies().empty());
        QVERIFY(job2->dependencies().empty());
        QVERIFY(job3->dependencies().empty());

        // WHEN
        job1->addDependency(job2);
        job1->addDependency(job3);

        // THEN
        QCOMPARE(job1->dependencies().size(), 2U);
        QCOMPARE(job1->dependencies().at(0).lock(), job2);
        QCOMPARE(job1->dependencies().at(1).lock(), job3);
        QVERIFY(job2->dependencies().empty());
        QVERIFY(job3->dependencies().empty());
    }

    void shouldRemoveDependencies()
    {
        // GIVEN
        QAspectJobPtr job1(new FakeAspectJob);
        QAspectJobPtr job2(new FakeAspectJob);
        QAspectJobPtr job3(new FakeAspectJob);

        job1->addDependency(job2);
        job1->addDependency(job3);

        // WHEN
        job1->removeDependency(job2);

        // THEN
        QCOMPARE(job1->dependencies().size(), 1U);
        QCOMPARE(job1->dependencies().at(0).lock(), job3);
    }

    void shouldClearNullDependencies()
    {
        // GIVEN
        QAspectJobPtr job1(new FakeAspectJob);
        QAspectJobPtr job2(new FakeAspectJob);
        QAspectJobPtr job3(new FakeAspectJob);

        job1->addDependency(job2);
        job1->addDependency(job3);

        // WHEN
        job2.clear();
        job1->removeDependency(QWeakPointer<QAspectJob>());

        // THEN
        QCOMPARE(job1->dependencies().size(), 1U);
        QCOMPARE(job1->dependencies().at(0).lock(), job3);
    }
};

QTEST_MAIN(tst_QAspectJob)

#include "tst_qaspectjob.moc"
