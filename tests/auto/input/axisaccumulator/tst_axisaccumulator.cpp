/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/axisaccumulator_p.h>
#include <Qt3DInput/private/qabstractaxisinput_p.h>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAxisAccumulator>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"

class tst_AxisAccumulator: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::AxisAccumulator backendAccumulator;
        Qt3DInput::QAxisAccumulator axisAccumulator;
        Qt3DInput::QAxis axis;

        axisAccumulator.setSourceAxis(&axis);
        axisAccumulator.setSourceAxisType(Qt3DInput::QAxisAccumulator::Velocity);
        axisAccumulator.setScale(2.0f);

        // WHEN
        simulateInitialization(&axisAccumulator, &backendAccumulator);

        // THEN
        QCOMPARE(backendAccumulator.peerId(), axisAccumulator.id());
        QCOMPARE(backendAccumulator.isEnabled(), axisAccumulator.isEnabled());
        QCOMPARE(backendAccumulator.sourceAxisType(), axisAccumulator.sourceAxisType());
        QCOMPARE(backendAccumulator.sourceAxisId(), axisAccumulator.sourceAxis()->id());
        QCOMPARE(backendAccumulator.scale(), axisAccumulator.scale());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::AxisAccumulator backendAxisAccumulator;

        // THEN
        QVERIFY(backendAxisAccumulator.peerId().isNull());
        QCOMPARE(backendAxisAccumulator.isEnabled(), false);
        QCOMPARE(backendAxisAccumulator.value(), 0.0f);
        QCOMPARE(backendAxisAccumulator.scale(), 1.0f);
        QCOMPARE(backendAxisAccumulator.sourceAxisId(), Qt3DCore::QNodeId());
        QCOMPARE(backendAxisAccumulator.sourceAxisType(), Qt3DInput::QAxisAccumulator::Velocity);

        // GIVEN
        Qt3DInput::QAxisAccumulator axisAccumulator;
        Qt3DInput::QAxis axis;

        axisAccumulator.setSourceAxis(&axis);
        axisAccumulator.setScale(2.0f);
        axisAccumulator.setSourceAxisType(Qt3DInput::QAxisAccumulator::Acceleration);
        axisAccumulator.setEnabled(true);

        // WHEN
        simulateInitialization(&axisAccumulator, &backendAxisAccumulator);
        backendAxisAccumulator.cleanup();

        // THEN
        QCOMPARE(backendAxisAccumulator.isEnabled(), false);
        QCOMPARE(backendAxisAccumulator.value(), 0.0f);
        QCOMPARE(backendAxisAccumulator.scale(), 1.0f);
        QCOMPARE(backendAxisAccumulator.sourceAxisId(), Qt3DCore::QNodeId());
        QCOMPARE(backendAxisAccumulator.sourceAxisType(), Qt3DInput::QAxisAccumulator::Velocity);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::Input::AxisAccumulator backendAxisAccumulator;
        Qt3DCore::QPropertyUpdatedChangePtr updateChange;

        // WHEN
        updateChange.reset(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendAxisAccumulator.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisAccumulator.isEnabled(), true);

        // WHEN
        Qt3DInput::QAxis axis;
        const Qt3DCore::QNodeId axisId = axis.id();
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setPropertyName("sourceAxis");
        updateChange->setValue(QVariant::fromValue(axisId));
        backendAxisAccumulator.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisAccumulator.sourceAxisId(), axisId);

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setPropertyName("sourceAxisType");
        updateChange->setValue(Qt3DInput::QAxisAccumulator::Acceleration);
        backendAxisAccumulator.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisAccumulator.sourceAxisType(), Qt3DInput::QAxisAccumulator::Acceleration);

        // WHEN
        updateChange = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
        updateChange->setPropertyName("scale");
        updateChange->setValue(3.0f);
        backendAxisAccumulator.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisAccumulator.scale(), 3.0f);
    }

    void checkValuePropertyBackendNotification()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DInput::Input::AxisAccumulator backendAxisAccumulator;
        backendAxisAccumulator.setEnabled(true);
        Qt3DCore::QBackendNodePrivate::get(&backendAxisAccumulator)->setArbiter(&arbiter);

        // WHEN
        backendAxisAccumulator.setValue(454.0f);

        // THEN
        QCOMPARE(backendAxisAccumulator.value(), 454.0f);
        QCOMPARE(arbiter.events.count(), 1);
        Qt3DCore::QPropertyUpdatedChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
        QCOMPARE(change->propertyName(), "value");
        QCOMPARE(change->value().toFloat(), backendAxisAccumulator.value());

        arbiter.events.clear();

        // WHEN
        backendAxisAccumulator.setValue(454.0f);

        // THEN
        QCOMPARE(backendAxisAccumulator.value(), 454.0f);
        QCOMPARE(arbiter.events.count(), 0);

        arbiter.events.clear();
    }

    void shouldNotChangeValueWhenDisabled()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DInput::Input::AxisAccumulator backendAxisAccumulator;
        Qt3DCore::QBackendNodePrivate::get(&backendAxisAccumulator)->setArbiter(&arbiter);
        backendAxisAccumulator.setEnabled(false);

        // WHEN
        backendAxisAccumulator.setValue(454.0f);

        // THEN
        QCOMPARE(backendAxisAccumulator.value(), 0.0f);
        QCOMPARE(arbiter.events.count(), 0);
    }
};

QTEST_APPLESS_MAIN(tst_AxisAccumulator)

#include "tst_axisaccumulator.moc"
