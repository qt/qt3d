// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DRender/qcamera.h>
#include <QObject>
#include <QSignalSpy>

class tst_QFirstPersonCameraController : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DExtras::QFirstPersonCameraController firstPersonCameraController;

        // THEN
        QVERIFY(firstPersonCameraController.camera() == nullptr);
        QCOMPARE(firstPersonCameraController.linearSpeed(), 10.0f);
        QCOMPARE(firstPersonCameraController.lookSpeed(), 180.0f);
        QCOMPARE(firstPersonCameraController.acceleration(), -1.0f);
        QCOMPARE(firstPersonCameraController.deceleration(), -1.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DExtras::QFirstPersonCameraController firstPersonCameraController;

        {
            // WHEN
            QSignalSpy spy(&firstPersonCameraController, SIGNAL(cameraChanged()));
            Qt3DRender::QCamera *newValue = new Qt3DRender::QCamera(&firstPersonCameraController);
            firstPersonCameraController.setCamera(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.camera(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            firstPersonCameraController.setCamera(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.camera(), newValue);
            QCOMPARE(spy.count(), 0);

            // WHEN
            spy.clear();
            // Check node bookeeping
            delete newValue;

            // THEN
            QCOMPARE(spy.count(), 1);
            QVERIFY(firstPersonCameraController.camera() == nullptr);
        }
        {
            // WHEN
            QSignalSpy spy(&firstPersonCameraController, SIGNAL(linearSpeedChanged()));
            const float newValue = 300.0f;
            firstPersonCameraController.setLinearSpeed(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.linearSpeed(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            firstPersonCameraController.setLinearSpeed(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.linearSpeed(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&firstPersonCameraController, SIGNAL(lookSpeedChanged()));
            const float newValue = 0.001f;
            firstPersonCameraController.setLookSpeed(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.lookSpeed(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            firstPersonCameraController.setLookSpeed(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.lookSpeed(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&firstPersonCameraController, SIGNAL(accelerationChanged(float)));
            const float newValue = 0.001f;
            firstPersonCameraController.setAcceleration(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.acceleration(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            firstPersonCameraController.setAcceleration(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.acceleration(), newValue);
            QCOMPARE(spy.count(), 0);

        }
        {
            // WHEN
            QSignalSpy spy(&firstPersonCameraController, SIGNAL(decelerationChanged(float)));
            const float newValue = 0.001f;
            firstPersonCameraController.setDeceleration(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.deceleration(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            firstPersonCameraController.setDeceleration(newValue);

            // THEN
            QCOMPARE(firstPersonCameraController.deceleration(), newValue);
            QCOMPARE(spy.count(), 0);

        }
    }

};

QTEST_APPLESS_MAIN(tst_QFirstPersonCameraController)

#include "tst_qfirstpersoncameracontroller.moc"
