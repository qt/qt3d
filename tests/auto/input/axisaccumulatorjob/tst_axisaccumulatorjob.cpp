// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/axisaccumulator_p.h>
#include <Qt3DInput/private/axisaccumulatorjob_p.h>
#include <Qt3DInput/private/qabstractaxisinput_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAxisAccumulator>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"

class tst_AxisAccumulatorJob : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkIntegration()
    {
        // GIVEN
        const auto sourceAxisType = Qt3DInput::QAxisAccumulator::Velocity;
        const float axisValue = 1.0f;
        const float scale = 10.0f;
        const float dt = 0.1f;
        const float valueResultEnabled = 1.0f;
        const float valueResultDisabled = 0.0f;

        // Set up an axis
        Qt3DInput::QAxis *axis = new Qt3DInput::QAxis;
        Qt3DInput::Input::AxisManager axisManager;
        Qt3DInput::Input::Axis *backendAxis = axisManager.getOrCreateResource(axis->id());
        backendAxis->setEnabled(true);
        backendAxis->setAxisValue(axisValue);

        // Hook up a bunch of accumulators to this axis
        Qt3DInput::Input::AxisAccumulatorManager axisAccumulatorManager;
        QList<Qt3DInput::Input::AxisAccumulator *> accumulators;
        for (int i = 0; i < 10; ++i) {
            auto axisAccumulator = new Qt3DInput::QAxisAccumulator;
            Qt3DInput::Input::AxisAccumulator *backendAxisAccumulator
                    = axisAccumulatorManager.getOrCreateResource(axisAccumulator->id());
            accumulators.push_back(backendAxisAccumulator);

            axisAccumulator->setEnabled(i % 2 == 0); // Enable only even accumulators
            axisAccumulator->setSourceAxis(axis);
            axisAccumulator->setScale(scale);
            axisAccumulator->setSourceAxisType(sourceAxisType);
            simulateInitializationSync(axisAccumulator, backendAxisAccumulator);
        }

        // WHEN
        Qt3DInput::Input::AxisAccumulatorJob job(&axisAccumulatorManager, &axisManager);
        job.setDeltaTime(dt);
        job.run();


        // THEN
        for (const auto accumulator : accumulators) {
            QCOMPARE(accumulator->value(), accumulator->isEnabled() ? valueResultEnabled
                                                                    : valueResultDisabled);
        }
    }
};

QTEST_APPLESS_MAIN(tst_AxisAccumulatorJob)

#include "tst_axisaccumulatorjob.moc"
