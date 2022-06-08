// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qaxisaccumulator.h>
#include <Qt3DInput/private/qaxisaccumulator_p.h>

#include "testarbiter.h"

class tst_QAxisAccumulator: public Qt3DInput::QAxisAccumulator
{
    Q_OBJECT
public:
    tst_QAxisAccumulator()
    {
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QAxisAccumulator> accumulator(new Qt3DInput::QAxisAccumulator());
        arbiter.setArbiterOnNode(accumulator.data());
        Qt3DInput::QAxis *axis = new Qt3DInput::QAxis;

        // WHEN
        accumulator->setSourceAxis(axis);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), accumulator.data());

        arbiter.clear();


        // WHEN
        accumulator->setScale(2.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), accumulator.data());

        arbiter.clear();

        // WHEN
        accumulator->setSourceAxisType(Qt3DInput::QAxisAccumulator::Acceleration);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), accumulator.data());

        arbiter.clear();
    }

    void checkAxisInputBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAxisAccumulator> accumulator(new Qt3DInput::QAxisAccumulator);
        {
            // WHEN
            Qt3DInput::QAxis axis;
            accumulator->setSourceAxis(&axis);

            // THEN
            QCOMPARE(axis.parent(), accumulator.data());
            QCOMPARE(accumulator->sourceAxis(), &axis);
        }

        // THEN (Should not crash and parameter be unset)
        QVERIFY(accumulator->sourceAxis() == nullptr);

        {
            // WHEN
            Qt3DInput::QAxisAccumulator someOtherAccumulator;
            QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis(&someOtherAccumulator));
            accumulator->setSourceAxis(axis.data());

            // THEN
            QCOMPARE(axis->parent(), &someOtherAccumulator);
            QCOMPARE(accumulator->sourceAxis(), axis.data());

            // WHEN
            accumulator.reset();
            axis.reset();

            // THEN Should not crash when the input is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAxisAccumulator)

#include "tst_qaxisaccumulator.moc"
