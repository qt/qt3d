// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/private/keyboarddevice_p.h>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DInput/private/keyboardhandler_p.h>

class tst_KeyboardHandler : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::InputHandler inputHandler;

        auto keyboardDevice = new Qt3DInput::QKeyboardDevice;
        auto backendKeyboardDevice = inputHandler.keyboardDeviceManager()->getOrCreateResource(keyboardDevice->id());
        backendKeyboardDevice->setInputHandler(&inputHandler);
        simulateInitializationSync(keyboardDevice, backendKeyboardDevice);

        Qt3DInput::QKeyboardHandler keyboardHandler;
        auto backendKeyboardHandler = inputHandler.keyboardInputManager()->getOrCreateResource(keyboardHandler.id());
        backendKeyboardHandler->setInputHandler(&inputHandler);

        keyboardHandler.setFocus(true);
        keyboardHandler.setSourceDevice(keyboardDevice);

        // WHEN
        simulateInitializationSync(&keyboardHandler, backendKeyboardHandler);

        // THEN
        QCOMPARE(backendKeyboardHandler->peerId(), keyboardHandler.id());
        QCOMPARE(backendKeyboardHandler->isEnabled(), keyboardHandler.isEnabled());
        QCOMPARE(backendKeyboardDevice->lastKeyboardInputRequester(), backendKeyboardHandler->peerId());
        QCOMPARE(backendKeyboardHandler->keyboardDevice(), keyboardDevice->id());
    }

    void checkInitialState()
    {
        // GIVEN
        Qt3DInput::Input::KeyboardHandler backendKeyboardHandler;

        // THEN
        QVERIFY(backendKeyboardHandler.peerId().isNull());
        QCOMPARE(backendKeyboardHandler.isEnabled(), false);
        QCOMPARE(backendKeyboardHandler.focus(), false);
        QCOMPARE(backendKeyboardHandler.keyboardDevice(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::Input::InputHandler inputHandler;

        Qt3DInput::QKeyboardHandler keyboardHandler;
        keyboardHandler.setEnabled(false);
        Qt3DInput::Input::KeyboardHandler backendKeyboardHandler;
        backendKeyboardHandler.setInputHandler(&inputHandler);

        Qt3DInput::QKeyboardDevice device;
        Qt3DInput::Input::KeyboardDevice *backendKeyboardDevice =
            inputHandler.keyboardDeviceManager()->getOrCreateResource(device.id());
        backendKeyboardDevice->setInputHandler(&inputHandler);

        simulateInitializationSync(&keyboardHandler, &backendKeyboardHandler);

        // WHEN
        keyboardHandler.setSourceDevice(&device);
        backendKeyboardHandler.syncFromFrontEnd(&keyboardHandler, false);

        // THEN
        QCOMPARE(backendKeyboardHandler.keyboardDevice(), device.id());

        // WHEN (still disabled, nothing should happen)
        keyboardHandler.setFocus(true);
        backendKeyboardHandler.syncFromFrontEnd(&keyboardHandler, false);

        // THEN
        QVERIFY(backendKeyboardDevice->lastKeyboardInputRequester().isNull());

        // WHEN
        keyboardHandler.setEnabled(true);
        backendKeyboardHandler.syncFromFrontEnd(&keyboardHandler, false);

        // THEN
        QCOMPARE(backendKeyboardHandler.isEnabled(), true);

        // WHEN (now enabled, should request focus)
        keyboardHandler.setFocus(true);
        backendKeyboardHandler.syncFromFrontEnd(&keyboardHandler, false);

        // THEN
        QCOMPARE(backendKeyboardDevice->lastKeyboardInputRequester(), backendKeyboardHandler.peerId());
    }
};

QTEST_APPLESS_MAIN(tst_KeyboardHandler)

#include "tst_keyboardhandler.moc"
