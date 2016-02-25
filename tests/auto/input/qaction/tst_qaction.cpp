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

#include "testpostmanarbiter.h"

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

    ~tst_QAction()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DInput::QAction *>("action");

        Qt3DInput::QAction *defaultConstructed = new Qt3DInput::QAction();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DInput::QAction *namedaction = new Qt3DInput::QAction();
        namedaction->setName(QStringLiteral("fire"));
        QTest::newRow("namedAction") << namedaction;

        Qt3DInput::QAction *namedactionWithInputs = new Qt3DInput::QAction();
        Qt3DInput::QActionInput *actionInput1 = new Qt3DInput::QActionInput();
        Qt3DInput::QActionInput *actionInput2 = new Qt3DInput::QActionInput();
        Qt3DInput::QActionInput *actionInput3 = new Qt3DInput::QActionInput();
        namedactionWithInputs->setName("accelerate");
        namedactionWithInputs->addInput(actionInput1);
        namedactionWithInputs->addInput(actionInput2);
        namedactionWithInputs->addInput(actionInput3);
        QTest::newRow("namedActionWithInputs") << namedactionWithInputs;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DInput::QAction *, action);

        // WHEN
        Qt3DInput::QAction *clone = static_cast<Qt3DInput::QAction *>(QNode::clone(action));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(action->id(), clone->id());
        QCOMPARE(action->name(), clone->name());
        QCOMPARE(action->inputs().count(), clone->inputs().count());
        QCOMPARE(action->isActive(), clone->isActive());

        for (int i = 0, m = action->inputs().count(); i < m; ++i) {
            QCOMPARE(action->inputs().at(i)->id(), clone->inputs().at(i)->id());
        }

    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAction> action(new Qt3DInput::QAction());
        TestArbiter arbiter(action.data());

        // WHEN
        action->setName(QStringLiteral("454"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "name");
        QCOMPARE(change->value().toString(), QStringLiteral("454"));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3DInput::QActionInput *input = new Qt3DInput::QActionInput();
        action->addInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "input");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), input->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        action->removeInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "input");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), input->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();
    }

    void checkActivePropertyChanged()
    {
        // GIVEN
        QCOMPARE(isActive(), false);

        // Note: simulate backend change to frontend
        // WHEN
        Qt3DCore::QScenePropertyChangePtr valueChange(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        valueChange->setPropertyName("active");
        valueChange->setValue(true);
        sceneChangeEvent(valueChange);

        // THEN
        QCOMPARE(isActive(), true);
    }


protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QAction)

#include "tst_qaction.moc"
