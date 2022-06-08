// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/private/qaction_p.h>
#include <Qt3DInput/private/qactioninput_p.h>
#include <testarbiter.h>

// We need to call QNode::clone which is protected
// We need to call QAction::sceneChangeEvent which is protected
// So we sublcass QNode instead of QObject
class tst_QAction: public Qt3DInput::QAction
{
    Q_OBJECT
public:
    tst_QAction()
    {
    }

private Q_SLOTS:



    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QAction> action(new Qt3DInput::QAction());
        arbiter.setArbiterOnNode(action.data());

        // WHEN
        Qt3DInput::QActionInput *input = new Qt3DInput::QActionInput();
        action->addInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), action.data());

        arbiter.clear();

        // WHEN
        action->removeInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), action.data());
    }

    void checkActionInputBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAction> action(new Qt3DInput::QAction);
        {
            // WHEN
            Qt3DInput::QActionInput input;
            action->addInput(&input);

            // THEN
            QCOMPARE(input.parent(), action.data());
            QCOMPARE(action->inputs().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(action->inputs().empty());

        {
            // WHEN
            Qt3DInput::QAction someOtherAction;
            QScopedPointer<Qt3DInput::QActionInput> input(new Qt3DInput::QActionInput(&someOtherAction));
            action->addInput(input.data());

            // THEN
            QCOMPARE(input->parent(), &someOtherAction);
            QCOMPARE(action->inputs().size(), 1);

            // WHEN
            action.reset();
            input.reset();

            // THEN Should not crash when the input is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAction)

#include "tst_qaction.moc"
