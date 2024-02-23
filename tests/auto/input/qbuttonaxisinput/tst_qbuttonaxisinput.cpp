// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QAbstractPhysicalDevice>
#include <Qt3DInput/QButtonAxisInput>
#include <Qt3DInput/private/qbuttonaxisinput_p.h>

#include "testarbiter.h"
#include "testdevice.h"

class tst_QButtonAxisInput: public QObject
{
    Q_OBJECT
public:
    tst_QButtonAxisInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
    }

private Q_SLOTS:
    void shouldHaveDefaultState()
    {
        // GIVEN
        Qt3DInput::QButtonAxisInput axisInput;

        // THEN
        QVERIFY(axisInput.buttons().isEmpty());
        QCOMPARE(axisInput.scale(), 1.0f);
        QCOMPARE(axisInput.acceleration(), -1.0f);
        QCOMPARE(axisInput.deceleration(), -1.0f);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QButtonAxisInput> axisInput(new Qt3DInput::QButtonAxisInput());
        arbiter.setArbiterOnNode(axisInput.data());

        // WHEN
        const QList<int> buttons = { 555 };
        axisInput->setButtons(buttons);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        axisInput->setScale(1340.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        TestDevice *device = new TestDevice(axisInput.data());

        axisInput->setSourceDevice(device);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        axisInput->setAcceleration(42.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        axisInput->setDeceleration(43.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QButtonAxisInput)

#include "tst_qbuttonaxisinput.moc"
