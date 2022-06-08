// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <private/functionrangefinder_p.h>

using namespace Qt3DAnimation::Animation;

class tst_FunctionRangeFinder : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        QList<float> data;

        // WHEN
        FunctionRangeFinder finder(&data);

        // THEN
        QCOMPARE(finder.rangeSize(), 2);
        QCOMPARE(finder.isAscending(), true);
        QCOMPARE(finder.correlationThreshold(), 1);
    }

    void checkConstructionWithData_data()
    {
        QTest::addColumn<QList<float>>("x");
        QTest::addColumn<int>("correlationThreshold");
        QTest::addColumn<bool>("ascending");

        QVector<float> data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
        int correlationThreshold = 1;
        bool ascending = true;
        QTest::newRow("10 entries, ascending") << data << correlationThreshold << ascending;
        data.clear();

        data.resize(10000);
        for (int i = 0; i < 10000; ++i)
            data[i] = float(i);
        ascending = true;
        correlationThreshold = std::pow(float(10000), 0.25f);
        QTest::newRow("10k entries, ascending") << data << correlationThreshold << ascending;
        data.clear();

        data.resize(10000);
        for (int i = 0; i < 10000; ++i)
            data[10000 - i - 1] = float(i);
        ascending = false;
        correlationThreshold = std::pow(float(10000), 0.25f);
        QTest::newRow("10k entries, descending") << data << correlationThreshold << ascending;
        data.clear();
    }

    void checkConstructionWithData()
    {
        // GIVEN
        QFETCH(QList<float>, x);
        QFETCH(int, correlationThreshold);
        QFETCH(bool, ascending);

        // WHEN
        FunctionRangeFinder finder(&x);

        // THEN
        QCOMPARE(finder.rangeSize(), 2);
        QCOMPARE(finder.isAscending(), ascending);
        QCOMPARE(finder.correlationThreshold(), correlationThreshold);
    }

    void checkFindLowerBound_data()
    {
        QTest::addColumn<QList<float>>("x");
        QTest::addColumn<QList<float>>("needles");
        QTest::addColumn<QList<int>>("lowerBounds");

        QVector<float> data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
        QVector<float> needles = { 2.5f };
        QVector<int> lowerBounds = { 1 };
        QTest::newRow("10 entries, ascending") << data << needles << lowerBounds;
        data.clear();
        needles.clear();
        lowerBounds.clear();

        data.resize(10000);
        for (int i = 0; i < 10000; ++i)
            data[i] = float(i);

        needles.push_back(23.75f);
        lowerBounds.push_back(23);

        needles.push_back(500.03f);
        lowerBounds.push_back(500);

        needles.push_back(500.2f);
        lowerBounds.push_back(500);

        needles.push_back(501.4f); // Will trigger hunt codepath as previous test is correlated
        lowerBounds.push_back(501);

        needles.push_back(3405.123f);
        lowerBounds.push_back(3405);

        QTest::newRow("10k entries, ascending") << data << needles << lowerBounds;
        data.clear();
        needles.clear();
        lowerBounds.clear();

        data.resize(10);
        for (int i = 0; i < 10; ++i)
            data[10 - i - 1] = float(i);

        needles.push_back(8.1f);
        lowerBounds.push_back(0);

        needles.push_back(7.2f);
        lowerBounds.push_back(1);

        needles.push_back(0.5f);
        lowerBounds.push_back(8);

        QTest::newRow("10 entries, descending") << data << needles << lowerBounds;
        data.clear();
    }

    void checkFindLowerBound()
    {
        // GIVEN
        QFETCH(QList<float>, x);
        QFETCH(QList<float>, needles);
        QFETCH(QList<int>, lowerBounds);
        FunctionRangeFinder finder(&x);

        for (int i = 0; i < needles.size(); ++i) {
            // WHEN
            int result = finder.findLowerBound(needles[i]);

            // THEN
            QCOMPARE(result, lowerBounds[i]);
        }
    }
};

QTEST_APPLESS_MAIN(tst_FunctionRangeFinder)

#include "tst_functionrangefinder.moc"
