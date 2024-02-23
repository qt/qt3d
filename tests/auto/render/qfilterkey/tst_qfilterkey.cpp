// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/private/qfilterkey_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QFilterKey : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QFilterKey filterKey;

        // THEN
        QCOMPARE(filterKey.value(), QVariant());
        QCOMPARE(filterKey.name(), QString());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QFilterKey filterKey;

        {
            // WHEN
            QSignalSpy spy(&filterKey, SIGNAL(valueChanged(QVariant)));
            const QVariant newValue = QVariant("Tim");
            filterKey.setValue(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(filterKey.value(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            filterKey.setValue(newValue);

            // THEN
            QCOMPARE(filterKey.value(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&filterKey, SIGNAL(nameChanged(QString)));
            const QString newValue = QStringLiteral("Darius");
            filterKey.setName(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(filterKey.name(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            filterKey.setName(newValue);

            // THEN
            QCOMPARE(filterKey.name(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkValueUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QFilterKey filterKey;
        arbiter.setArbiterOnNode(&filterKey);

        {
            // WHEN
            filterKey.setValue(QVariant(427));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &filterKey);

            arbiter.clear();
        }

        {
            // WHEN
            filterKey.setValue(QVariant(427));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkNameUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QFilterKey filterKey;
        arbiter.setArbiterOnNode(&filterKey);

        {
            // WHEN
            filterKey.setName(QStringLiteral("Easton"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &filterKey);

            arbiter.clear();
        }

        {
            // WHEN
            filterKey.setName(QStringLiteral("Easton"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QFilterKey)

#include "tst_qfilterkey.moc"
