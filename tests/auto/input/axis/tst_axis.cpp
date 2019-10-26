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
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/private/qabstractaxisinput_p.h>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QAxis>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"

class DummyAxisInput : public Qt3DInput::QAbstractAxisInput
{
    Q_OBJECT
public:
    DummyAxisInput(Qt3DCore::QNode *parent = nullptr)
        : Qt3DInput::QAbstractAxisInput(*new Qt3DInput::QAbstractAxisInputPrivate, parent)
    {}
};

class tst_Axis: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::Axis backendAxis;
        Qt3DInput::QAxis axis;
        Qt3DInput::QAnalogAxisInput axisInput;

        axis.addInput(&axisInput);

        // WHEN
        simulateInitializationSync(&axis, &backendAxis);

        // THEN
        QCOMPARE(backendAxis.peerId(), axis.id());
        QCOMPARE(backendAxis.isEnabled(), axis.isEnabled());
        QCOMPARE(backendAxis.inputs().size(), axis.inputs().size());

        const int inputsCount = backendAxis.inputs().size();
        if (inputsCount > 0) {
            for (int i = 0; i < inputsCount; ++i)
                QCOMPARE(backendAxis.inputs().at(i), axis.inputs().at(i)->id());
        }
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::Axis backendAxis;

        // THEN
        QVERIFY(backendAxis.peerId().isNull());
        QCOMPARE(backendAxis.axisValue(), 0.0f);
        QCOMPARE(backendAxis.isEnabled(), false);
        QCOMPARE(backendAxis.inputs().size(), 0);

        // GIVEN
        Qt3DInput::QAxis axis;
        Qt3DInput::QAnalogAxisInput axisInput;

        axis.addInput(&axisInput);

        // WHEN
        simulateInitializationSync(&axis, &backendAxis);
        backendAxis.setAxisValue(883.0f);
        backendAxis.cleanup();

        // THEN
        QCOMPARE(backendAxis.axisValue(), 0.0f);
        QCOMPARE(backendAxis.isEnabled(), false);
        QCOMPARE(backendAxis.inputs().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::QAxis axis;
        Qt3DInput::Input::Axis backendAxis;
        simulateInitializationSync(&axis, &backendAxis);

        // WHEN
        axis.setEnabled(false);
        backendAxis.syncFromFrontEnd(&axis, false);

        // THEN
        QCOMPARE(backendAxis.isEnabled(), false);

        // WHEN
        DummyAxisInput input;
        const Qt3DCore::QNodeId inputId = input.id();
        axis.addInput(&input);
        backendAxis.syncFromFrontEnd(&axis, false);

        // THEN
        QCOMPARE(backendAxis.inputs().size(), 1);
        QCOMPARE(backendAxis.inputs().first(), inputId);

        // WHEN
        axis.removeInput(&input);
        backendAxis.syncFromFrontEnd(&axis, false);

        // THEN
        QCOMPARE(backendAxis.inputs().size(), 0);
    }

    void shouldNotChangeValueWhenDisabled()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DInput::Input::Axis backendAxis;
        Qt3DCore::QBackendNodePrivate::get(&backendAxis)->setArbiter(&arbiter);
        backendAxis.setEnabled(false);

        // WHEN
        backendAxis.setAxisValue(454.0f);

        // THEN
        QCOMPARE(backendAxis.axisValue(), 0.0f);
        QCOMPARE(arbiter.events.count(), 0);
    }
};

QTEST_APPLESS_MAIN(tst_Axis)

#include "tst_axis.moc"
