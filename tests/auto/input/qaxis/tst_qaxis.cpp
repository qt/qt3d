// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/private/qaxis_p.h>
#include <testarbiter.h>

class tst_QAxis: public QObject
{
    Q_OBJECT
public:
    tst_QAxis()
    {
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis());
        arbiter.setArbiterOnNode(axis.data());

        // WHEN
        Qt3DInput::QAbstractAxisInput *input = new Qt3DInput::QAnalogAxisInput();
        axis->addInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axis.data());

        arbiter.clear();

        // WHEN
        axis->removeInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axis.data());

        arbiter.clear();
    }

    void checkAxisInputBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis);
        {
            // WHEN
            Qt3DInput::QAnalogAxisInput input;
            axis->addInput(&input);

            // THEN
            QCOMPARE(input.parent(), axis.data());
            QCOMPARE(axis->inputs().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(axis->inputs().empty());

        {
            // WHEN
            Qt3DInput::QAxis someOtherAxis;
            QScopedPointer<Qt3DInput::QAbstractAxisInput> input(new Qt3DInput::QAnalogAxisInput(&someOtherAxis));
            axis->addInput(input.data());

            // THEN
            QCOMPARE(input->parent(), &someOtherAxis);
            QCOMPARE(axis->inputs().size(), 1);

            // WHEN
            axis.reset();
            input.reset();

            // THEN Should not crash when the input is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAxis)

#include "tst_qaxis.moc"
