// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DInput/private/action_p.h>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAction>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"

class DummyActionInput : public Qt3DInput::QActionInput
{
    Q_OBJECT
public:
    DummyActionInput(Qt3DCore::QNode *parent = nullptr)
        : Qt3DInput::QActionInput(parent)
    {}
};

class tst_Action : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::Action backendAction;
        Qt3DInput::QAction action;
        Qt3DInput::QActionInput actionInput;

        action.addInput(&actionInput);

        // WHEN
        simulateInitializationSync(&action, &backendAction);

        // THEN
        QCOMPARE(backendAction.peerId(), action.id());
        QCOMPARE(backendAction.isEnabled(), action.isEnabled());
        QCOMPARE(backendAction.inputs().size(), action.inputs().size());

        const int inputsCount = backendAction.inputs().size();
        if (inputsCount > 0) {
            for (int i = 0; i < inputsCount; ++i)
                QCOMPARE(backendAction.inputs().at(i), action.inputs().at(i)->id());
        }
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::Action backendAction;

        // THEN
        QVERIFY(backendAction.peerId().isNull());
        QCOMPARE(backendAction.actionTriggered(), false);
        QCOMPARE(backendAction.isEnabled(), false);
        QCOMPARE(backendAction.inputs().size(), 0);

        // GIVEN
        Qt3DInput::QAction action;
        Qt3DInput::QActionInput axisInput;

        action.addInput(&axisInput);

        // WHEN
        simulateInitializationSync(&action, &backendAction);
        backendAction.setActionTriggered(true);
        backendAction.cleanup();

        // THEN
        QCOMPARE(backendAction.actionTriggered(), false);
        QCOMPARE(backendAction.isEnabled(), false);
        QCOMPARE(backendAction.inputs().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::QAction action;
        Qt3DInput::Input::Action backendAction;
        simulateInitializationSync(&action, &backendAction);

        // WHEN
        action.setEnabled(false);
        backendAction.syncFromFrontEnd(&action, false);

        // THEN
        QCOMPARE(backendAction.isEnabled(), false);

        // WHEN
        DummyActionInput input;
        const Qt3DCore::QNodeId inputId = input.id();
        action.addInput(&input);
        backendAction.syncFromFrontEnd(&action, false);

        // THEN
        QCOMPARE(backendAction.inputs().size(), 1);
        QCOMPARE(backendAction.inputs().first(), inputId);

        // WHEN
        action.removeInput(&input);
        backendAction.syncFromFrontEnd(&action, false);

        // THEN
        QCOMPARE(backendAction.inputs().size(), 0);
    }

    void shouldNotActivateWhenDisabled()
    {
        // GIVEN
        Qt3DInput::Input::Action backendAction;
        backendAction.setEnabled(false);

        // WHEN
        backendAction.setActionTriggered(true);

        // THEN
        QVERIFY(!backendAction.actionTriggered());
    }
};

QTEST_APPLESS_MAIN(tst_Action)

#include "tst_action.moc"
