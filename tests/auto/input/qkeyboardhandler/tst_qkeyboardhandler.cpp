// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DInput/private/qkeyboardhandler_p.h>
#include <testarbiter.h>

class tst_QKeyboardHandler : public QObject
{
    Q_OBJECT
public:
    tst_QKeyboardHandler()
    {
        qRegisterMetaType<Qt3DInput::QKeyboardDevice*>("Qt3DInput::QKeyboardDevice*");
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QKeyboardHandler> keyboardHandler(new Qt3DInput::QKeyboardHandler);
        arbiter.setArbiterOnNode(keyboardHandler.data());

        // WHEN
        keyboardHandler->setFocus(true);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), keyboardHandler.data());

        arbiter.clear();

        // WHEN
        auto device = new Qt3DInput::QKeyboardDevice(keyboardHandler.data());
        QCoreApplication::processEvents();
        arbiter.clear();

        keyboardHandler->setSourceDevice(device);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), keyboardHandler.data());

        arbiter.clear();
    }

    void checkSourceDeviceBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QKeyboardHandler> keyboardHandler(new Qt3DInput::QKeyboardHandler);
        {
            // WHEN
            Qt3DInput::QKeyboardDevice device;
            keyboardHandler->setSourceDevice(&device);

            // THEN
            QCOMPARE(device.parent(), keyboardHandler.data());
            QCOMPARE(keyboardHandler->sourceDevice(), &device);
        }
        // THEN (Should not crash and effect be unset)
        QVERIFY(keyboardHandler->sourceDevice() == nullptr);

        {
            // WHEN
            Qt3DInput::QKeyboardHandler someOtherKeyboardHandler;
            QScopedPointer<Qt3DInput::QKeyboardDevice> device(new Qt3DInput::QKeyboardDevice(&someOtherKeyboardHandler));
            keyboardHandler->setSourceDevice(device.data());

            // THEN
            QCOMPARE(device->parent(), &someOtherKeyboardHandler);
            QCOMPARE(keyboardHandler->sourceDevice(), device.data());

            // WHEN
            keyboardHandler.reset();
            device.reset();

            // THEN Should not crash when the device is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QKeyboardHandler)

#include "tst_qkeyboardhandler.moc"
