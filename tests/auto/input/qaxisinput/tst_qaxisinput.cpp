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

#include <Qt3DInput/QAxisInput>
#include <Qt3DInput/QAbstractPhysicalDevice>

#include "testpostmanarbiter.h"
#include "testdevice.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QAxisInput: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QAxisInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
    }

    ~tst_QAxisInput()
    {
        QNode::cleanup();
    }

private Q_SLOTS:
    void checkCloning_data()
    {
        QTest::addColumn<Qt3DInput::QAxisInput *>("axisInput");

        Qt3DInput::QAxisInput *defaultConstructed = new Qt3DInput::QAxisInput();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DInput::QAxisInput *axisInputWithKeysAndAxis = new Qt3DInput::QAxisInput();
        axisInputWithKeysAndAxis->setButtons(QVariantList() << QVariant((1 << 1) | (1 << 5)));
        axisInputWithKeysAndAxis->setAxis(383);
        axisInputWithKeysAndAxis->setScale(327.0f);
        QTest::newRow("axisInputWithKeys") << axisInputWithKeysAndAxis;

        Qt3DInput::QAxisInput *axisInputWithKeysAndSourceDevice = new Qt3DInput::QAxisInput();
        TestDevice *device = new TestDevice();
        axisInputWithKeysAndSourceDevice->setButtons(QVariantList() << QVariant((1 << 1) | (1 << 5)));
        axisInputWithKeysAndSourceDevice->setSourceDevice(device);
        axisInputWithKeysAndSourceDevice->setAxis(427);
        axisInputWithKeysAndAxis->setScale(355.0f);
        QTest::newRow("axisInputWithKeysAndSourceDevice") << axisInputWithKeysAndSourceDevice;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DInput::QAxisInput *, axisInput);

        // WHEN
        Qt3DInput::QAxisInput *clone = static_cast<Qt3DInput::QAxisInput *>(QNode::clone(axisInput));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(axisInput->id(), clone->id());
        QCOMPARE(axisInput->buttons(), clone->buttons());
        QCOMPARE(axisInput->axis(), clone->axis());
        QCOMPARE(axisInput->scale(), clone->scale());

        if (axisInput->sourceDevice() != Q_NULLPTR) {
            QVERIFY(clone->sourceDevice() != Q_NULLPTR);
            QCOMPARE(clone->sourceDevice()->id(), axisInput->sourceDevice()->id());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAxisInput> axisInput(new Qt3DInput::QAxisInput());
        TestArbiter arbiter(axisInput.data());

        // WHEN
        QVariantList buttons = QVariantList() << QVariant(555);
        axisInput->setButtons(buttons);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "buttons");
        QCOMPARE(change->value().toList(), buttons);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        axisInput->setScale(1340.0f);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "scale");
        QCOMPARE(change->value().toFloat(), 1340.0f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        axisInput->setAxis(350);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "axis");
        QCOMPARE(change->value().toInt(), 350);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        TestDevice *device = new TestDevice(axisInput.data());
        axisInput->setSourceDevice(device);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "sourceDevice");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), device->id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QAxisInput)

#include "tst_qaxisinput.moc"
