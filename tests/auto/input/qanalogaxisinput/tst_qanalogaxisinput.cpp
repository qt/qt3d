// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QAbstractPhysicalDevice>
#include <Qt3DInput/private/qanalogaxisinput_p.h>

#include "testdevice.h"
#include <testarbiter.h>

class tst_QAnalogAxisInput: public QObject
{
    Q_OBJECT
public:
    tst_QAnalogAxisInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QAnalogAxisInput> axisInput(new Qt3DInput::QAnalogAxisInput());
        arbiter.setArbiterOnNode(axisInput.data());

        // WHEN
        axisInput->setAxis(350);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        TestDevice *device = new TestDevice(axisInput.data());
        QCoreApplication::processEvents();

        axisInput->setSourceDevice(device);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QAnalogAxisInput)

#include "tst_qanalogaxisinput.moc"
