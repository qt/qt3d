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

#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAbstractPhysicalDevice>

#include "testpostmanarbiter.h"
#include "testdevice.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QActionInput: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QActionInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
    }

    ~tst_QActionInput()
    {
        QNode::cleanup();
    }

private Q_SLOTS:
    void checkCloning_data()
    {
        QTest::addColumn<Qt3DInput::QActionInput *>("actionInput");

        Qt3DInput::QActionInput *defaultConstructed = new Qt3DInput::QActionInput();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DInput::QActionInput *actionInputWithKeys = new Qt3DInput::QActionInput();
        actionInputWithKeys->setKeys(QVariantList() << QVariant((1 << 1) | (1 << 5)));
        QTest::newRow("actionInputWithKeys") << actionInputWithKeys;

        Qt3DInput::QActionInput *actionInputWithKeysAndSourceDevice = new Qt3DInput::QActionInput();
        TestDevice *device = new TestDevice();
        actionInputWithKeysAndSourceDevice->setKeys(QVariantList() << QVariant((1 << 1) | (1 << 5)));
        actionInputWithKeysAndSourceDevice->setSourceDevice(device);
        QTest::newRow("actionInputWithKeysAndSourceDevice") << actionInputWithKeysAndSourceDevice;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DInput::QActionInput *, actionInput);

        // WHEN
        Qt3DInput::QActionInput *clone = static_cast<Qt3DInput::QActionInput *>(QNode::clone(actionInput));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(actionInput->id(), clone->id());
        QCOMPARE(actionInput->keys(), clone->keys());

        if (actionInput->sourceDevice() != Q_NULLPTR) {
            QVERIFY(clone->sourceDevice() != Q_NULLPTR);
            QCOMPARE(clone->sourceDevice()->id(), actionInput->sourceDevice()->id());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QActionInput> actionInput(new Qt3DInput::QActionInput());
        TestArbiter arbiter(actionInput.data());

        // WHEN
        QVariantList keys = QVariantList() << QVariant(555);
        actionInput->setKeys(keys);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "keys");
        QCOMPARE(change->value().toList(), keys);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        TestDevice *device = new TestDevice(actionInput.data());
        actionInput->setSourceDevice(device);
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

QTEST_MAIN(tst_QActionInput)

#include "tst_qactioninput.moc"
