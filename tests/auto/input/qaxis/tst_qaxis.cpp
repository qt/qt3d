/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAxisInput>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QAxis: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QAxis()
    {
    }

    ~tst_QAxis()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DInput::QAxis *>("axis");

        Qt3DInput::QAxis *defaultConstructed = new Qt3DInput::QAxis();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DInput::QAxis *namedAxis = new Qt3DInput::QAxis();
        namedAxis->setName(QStringLiteral("moveForward"));
        QTest::newRow("namedAxis") << namedAxis;

        Qt3DInput::QAxis *namedAxisWithInputs = new Qt3DInput::QAxis();
        Qt3DInput::QAxisInput *axisInput1 = new Qt3DInput::QAxisInput();
        Qt3DInput::QAxisInput *axisInput2 = new Qt3DInput::QAxisInput();
        Qt3DInput::QAxisInput *axisInput3 = new Qt3DInput::QAxisInput();
        namedAxisWithInputs->setName("moveBackward");
        namedAxisWithInputs->addInput(axisInput1);
        namedAxisWithInputs->addInput(axisInput2);
        namedAxisWithInputs->addInput(axisInput3);
        QTest::newRow("namedAxisWithInputs") << namedAxisWithInputs;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DInput::QAxis *, axis);

        // WHEN
        Qt3DInput::QAxis *clone = static_cast<Qt3DInput::QAxis *>(QNode::clone(axis));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(axis->id(), clone->id());
        QCOMPARE(axis->name(), clone->name());
        QCOMPARE(axis->inputs().count(), clone->inputs().count());

        for (int i = 0, m = axis->inputs().count(); i < m; ++i) {
            QCOMPARE(axis->inputs().at(i)->id(), clone->inputs().at(i)->id());
        }

    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis());
        TestArbiter arbiter(axis.data());

        // WHEN
        axis->setName(QStringLiteral("454"));
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "name");
        QCOMPARE(change->value().toString(), QStringLiteral("454"));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        Qt3DInput::QAxisInput *input = new Qt3DInput::QAxisInput();
        axis->addInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "input");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), input->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        axis->removeInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "input");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), input->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QAxis)

#include "tst_qaxis.moc"
