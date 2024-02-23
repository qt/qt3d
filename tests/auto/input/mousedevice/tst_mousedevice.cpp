// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DInput/qmousedevice.h>
#include <Qt3DInput/private/qmousedevice_p.h>
#include <Qt3DInput/private/mousedevice_p.h>
#include "qbackendnodetester.h"

class tst_MouseDevice : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DInput::Input::MouseDevice backendMouseDevice;

        // THEN
        QCOMPARE(backendMouseDevice.isEnabled(), false);
        QVERIFY(backendMouseDevice.peerId().isNull());
        QVERIFY(backendMouseDevice.inputHandler() == nullptr);
        QCOMPARE(backendMouseDevice.mouseState().xAxis, 0.0f);
        QCOMPARE(backendMouseDevice.mouseState().yAxis, 0.0f);
        QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
        QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
        QCOMPARE(backendMouseDevice.mouseState().leftPressed, false);
        QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
        QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
        QCOMPARE(backendMouseDevice.previousPos(), QPointF());
        QCOMPARE(backendMouseDevice.wasPressed(), false);
        QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
        QCOMPARE(backendMouseDevice.updateAxesContinuously(), false);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DInput::QMouseDevice mouseDevice;
        mouseDevice.setSensitivity(0.8f);

        {
            // WHEN
            Qt3DInput::Input::MouseDevice backendMouseDevice;
            simulateInitializationSync(&mouseDevice, &backendMouseDevice);

            // THEN
            QCOMPARE(backendMouseDevice.isEnabled(), true);
            QCOMPARE(backendMouseDevice.peerId(), mouseDevice.id());
            QVERIFY(backendMouseDevice.inputHandler() == nullptr);
            QCOMPARE(backendMouseDevice.mouseState().xAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().yAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().leftPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
            QCOMPARE(backendMouseDevice.previousPos(), QPointF());
            QCOMPARE(backendMouseDevice.wasPressed(), false);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.8f);
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), false);
        }
        {
            // WHEN
            Qt3DInput::Input::MouseDevice backendMouseDevice;
            mouseDevice.setEnabled(false);
            simulateInitializationSync(&mouseDevice, &backendMouseDevice);

            // THEN
            QCOMPARE(backendMouseDevice.peerId(), mouseDevice.id());
            QCOMPARE(backendMouseDevice.isEnabled(), false);
        }
    }

    void checkMouseMoveStatesUpdates()
    {
        // GIVEN
        Qt3DInput::Input::MouseDevice backendMouseDevice;

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QMouseEvent)(QMouseEvent::MouseButtonPress,
                                                           QPointF(400.0, 400.0),
                                                           QPointF(400.0, 400.0),
                                                           QPointF(400.0, 400.0),
                                                           Qt::LeftButton,
                                                           Qt::LeftButton,
                                                           Qt::NoModifier);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateMouseEvent(&event);

            // THEN
            // Note: axis are only modified when moving (> 1 event)
            QCOMPARE(backendMouseDevice.mouseState().xAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().yAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().leftPressed, true);
            QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
            QCOMPARE(backendMouseDevice.previousPos(), QPointF(400.0, 400.0));
            QCOMPARE(backendMouseDevice.wasPressed(), true);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), false);
        }

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QMouseEvent)(QMouseEvent::MouseMove,
                                                           QPointF(600.0, 600.0),
                                                           QPointF(600.0, 600.0),
                                                           QPointF(600.0, 600.0),
                                                           Qt::LeftButton,
                                                           Qt::LeftButton,
                                                           Qt::NoModifier);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateMouseEvent(&event);

            // THEN
            QCOMPARE(backendMouseDevice.mouseState().xAxis, (600.0f - 400.0f) * 0.1f);
            QCOMPARE(backendMouseDevice.mouseState().yAxis, (400.0f - 600.0f) * 0.1f);
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().leftPressed, true);
            QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
            QCOMPARE(backendMouseDevice.previousPos(), QPointF(600.0, 600.0));
            QCOMPARE(backendMouseDevice.wasPressed(), true);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), false);
        }

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QMouseEvent)(QMouseEvent::MouseButtonRelease,
                                                           QPointF(800.0, 800.0),
                                                           QPointF(800.0, 800.0),
                                                           QPointF(800.0, 800.0),
                                                           Qt::LeftButton,
                                                           Qt::NoButton,
                                                           Qt::NoModifier);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateMouseEvent(&event);

            // THEN
            QCOMPARE(backendMouseDevice.mouseState().xAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().yAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().leftPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
            QCOMPARE(backendMouseDevice.previousPos(), QPointF(800.0, 800.0));
            QCOMPARE(backendMouseDevice.wasPressed(), false);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), false);
        }

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QMouseEvent)(QMouseEvent::MouseMove,
                                                           QPointF(900.0, 900.0),
                                                           QPointF(900.0, 900.0),
                                                           QPointF(900.0, 900.0),
                                                           Qt::NoButton,
                                                           Qt::NoButton,
                                                           Qt::NoModifier);

            // THEN -> no axes update
            backendMouseDevice.updateMouseEvent(&event);
            QCOMPARE(backendMouseDevice.mouseState().xAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().yAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().leftPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
            QCOMPARE(backendMouseDevice.previousPos(), QPointF(900.0, 900.0));
            QCOMPARE(backendMouseDevice.wasPressed(), false);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), false);
        }

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QMouseEvent)(QMouseEvent::MouseMove,
                                                           QPointF(1000.0, 1000.0),
                                                           QPointF(1000.0, 1000.0),
                                                           QPointF(1000.0, 1000.0),
                                                           Qt::NoButton,
                                                           Qt::NoButton,
                                                           Qt::NoModifier);

            Qt3DInput::QMouseDevice mouseDevice;
            mouseDevice.setUpdateAxesContinuously(true);
            backendMouseDevice.syncFromFrontEnd(&mouseDevice, false);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateMouseEvent(&event);

            // THEN
            QCOMPARE(backendMouseDevice.mouseState().xAxis, (1000.0f - 900.0f) * 0.1f);
            QCOMPARE(backendMouseDevice.mouseState().yAxis, (900.0f - 1000.0f) * 0.1f);
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().leftPressed,false);
            QCOMPARE(backendMouseDevice.mouseState().rightPressed, false);
            QCOMPARE(backendMouseDevice.mouseState().centerPressed, false);
            QCOMPARE(backendMouseDevice.previousPos(), QPointF(1000.0, 1000.0));
            QCOMPARE(backendMouseDevice.wasPressed(), false);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), true);
        }
    }

#if QT_CONFIG(wheelevent)
    void checkMouseWheelState()
    {
        // GIVEN
        Qt3DInput::Input::MouseDevice backendMouseDevice;

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QWheelEvent)(QPointF(500.0, 500.0),
                                                           QPointF(), QPoint(),
                                                           QPoint(0, 120),
                                                           Qt::NoButton,
                                                           Qt::NoModifier,
                                                           Qt::NoScrollPhase,
                                                           false);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateWheelEvent(&event);

            // THEN
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.1f * 120);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
        }

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QWheelEvent)(QPointF(500.0, 500.0),
                                                           QPointF(), QPoint(),
                                                           QPoint(120, 0),
                                                           Qt::NoButton,
                                                           Qt::NoModifier,
                                                           Qt::NoScrollPhase,
                                                           false);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateWheelEvent(&event);

            // THEN
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.1f * 120);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
        }

        {
            // WHEN
            auto event = QT_PREPEND_NAMESPACE(QWheelEvent)(QPointF(500.0, 500.0),
                                                           QPointF(), QPoint(), QPoint(),
                                                           Qt::NoButton,
                                                           Qt::NoModifier,
                                                           Qt::NoScrollPhase,
                                                           false);
            backendMouseDevice.resetMouseAxisState();
            backendMouseDevice.updateWheelEvent(&event);

            // THEN
            QCOMPARE(backendMouseDevice.mouseState().wXAxis, 0.0f);
            QCOMPARE(backendMouseDevice.mouseState().wYAxis, 0.0f);
            QCOMPARE(backendMouseDevice.sensitivity(), 0.1f);
        }
    }
#endif

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DInput::QMouseDevice mouseDevice;
        Qt3DInput::Input::MouseDevice backendMouseDevice;
        simulateInitializationSync(&mouseDevice, &backendMouseDevice);

        {
            // WHEN
            const bool newValue = false;
            mouseDevice.setEnabled(newValue);
            backendMouseDevice.syncFromFrontEnd(&mouseDevice, false);

            // THEN
            QCOMPARE(backendMouseDevice.isEnabled(), newValue);
        }
        {
            // WHEN
            const float newValue = 99.0f;
            mouseDevice.setSensitivity(newValue);
            backendMouseDevice.syncFromFrontEnd(&mouseDevice, false);

            // THEN
            QCOMPARE(backendMouseDevice.sensitivity(), newValue);
        }
        {
            // WHEN
            const bool newValue = true;
            mouseDevice.setUpdateAxesContinuously(newValue);
            backendMouseDevice.syncFromFrontEnd(&mouseDevice, false);

            // THEN
            QCOMPARE(backendMouseDevice.updateAxesContinuously(), newValue);
        }
    }

};

QTEST_MAIN(tst_MouseDevice)

#include "tst_mousedevice.moc"
