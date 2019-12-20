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
