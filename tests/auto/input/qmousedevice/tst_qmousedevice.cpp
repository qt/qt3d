// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DInput/qmousedevice.h>
#include <Qt3DInput/qmouseevent.h>
#include <Qt3DInput/private/qmousedevice_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QMouseDevice : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DInput::QMouseDevice mouseDevice;

        // THEN
        QCOMPARE(mouseDevice.sensitivity(), 0.1f);
        QCOMPARE(mouseDevice.updateAxesContinuously(), false);
        QCOMPARE(mouseDevice.axisCount(), 4);
        QCOMPARE(mouseDevice.buttonCount(), 3);
        QCOMPARE(mouseDevice.axisNames(), QStringList()
                 << QStringLiteral("X")
                 << QStringLiteral("Y")
                 << QStringLiteral("WheelX")
                 << QStringLiteral("WheelY"));
        QCOMPARE(mouseDevice.buttonNames(), QStringList()
                 << QStringLiteral("Left")
                 << QStringLiteral("Right")
                 << QStringLiteral("Center"));

        QVERIFY(mouseDevice.axisIdentifier(QStringLiteral("X")) == Qt3DInput::QMouseDevice::X);
        QVERIFY(mouseDevice.axisIdentifier(QStringLiteral("Y")) == Qt3DInput::QMouseDevice::Y);
        QVERIFY(mouseDevice.axisIdentifier(QStringLiteral("WheelX")) == Qt3DInput::QMouseDevice::WheelX);
        QVERIFY(mouseDevice.axisIdentifier(QStringLiteral("WheelY")) == Qt3DInput::QMouseDevice::WheelY);

        QVERIFY(mouseDevice.buttonIdentifier(QStringLiteral("Left")) == Qt3DInput::QMouseEvent::LeftButton);
        QVERIFY(mouseDevice.buttonIdentifier(QStringLiteral("Right")) == Qt3DInput::QMouseEvent::RightButton);
        QVERIFY(mouseDevice.buttonIdentifier(QStringLiteral("Center")) == Qt3DInput::QMouseEvent::MiddleButton);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::QMouseDevice mouseDevice;

        {
            // WHEN
            QSignalSpy spy(&mouseDevice, SIGNAL(sensitivityChanged(float)));
            const float newValue = 0.5f;
            mouseDevice.setSensitivity(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mouseDevice.sensitivity(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mouseDevice.setSensitivity(newValue);

            // THEN
            QCOMPARE(mouseDevice.sensitivity(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&mouseDevice, SIGNAL(updateAxesContinuouslyChanged(bool)));
            const bool newValue = true;
            mouseDevice.setUpdateAxesContinuously(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mouseDevice.updateAxesContinuously(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mouseDevice.setUpdateAxesContinuously(newValue);

            // THEN
            QCOMPARE(mouseDevice.updateAxesContinuously(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkSensitivityUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DInput::QMouseDevice mouseDevice;
        arbiter.setArbiterOnNode(&mouseDevice);

        {
            // WHEN
            mouseDevice.setSensitivity(0.7f);
            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mouseDevice);
        }

        {
            // WHEN
            mouseDevice.setSensitivity(0.7f);

            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mouseDevice);
        }

    }

    void checkUpdateAxesContinuouslyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DInput::QMouseDevice mouseDevice;
        arbiter.setArbiterOnNode(&mouseDevice);

        {
            // WHEN
            mouseDevice.setUpdateAxesContinuously(true);
            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mouseDevice);
        }

        {
            // WHEN
            mouseDevice.setSensitivity(true);

            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mouseDevice);
        }

    }

};

QTEST_MAIN(tst_QMouseDevice)

#include "tst_qmousedevice.moc"
