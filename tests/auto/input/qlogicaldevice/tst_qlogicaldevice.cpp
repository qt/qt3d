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

#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAction>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QLogicalDevice: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QLogicalDevice()
    {
    }

    ~tst_QLogicalDevice()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DInput::QLogicalDevice *>("logicalDevice");

        Qt3DInput::QLogicalDevice *defaultConstructed = new Qt3DInput::QLogicalDevice();
        QTest::newRow("defaultConstructed") << defaultConstructed;

        Qt3DInput::QLogicalDevice *logicalDeviceWithActions = new Qt3DInput::QLogicalDevice();
        logicalDeviceWithActions->addAction(new Qt3DInput::QAction());
        logicalDeviceWithActions->addAction(new Qt3DInput::QAction());
        logicalDeviceWithActions->addAction(new Qt3DInput::QAction());
        QTest::newRow("logicalDeviceWithActions") << logicalDeviceWithActions;

        Qt3DInput::QLogicalDevice *logicalDeviceWithAxes = new Qt3DInput::QLogicalDevice();
        logicalDeviceWithAxes->addAxis(new Qt3DInput::QAxis());
        logicalDeviceWithAxes->addAxis(new Qt3DInput::QAxis());
        logicalDeviceWithAxes->addAxis(new Qt3DInput::QAxis());
        QTest::newRow("logicalDeviceWithAxes") << logicalDeviceWithAxes;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DInput::QLogicalDevice *, logicalDevice);

        // WHEN
        Qt3DInput::QLogicalDevice *clone = static_cast<Qt3DInput::QLogicalDevice *>(QNode::clone(logicalDevice));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(logicalDevice->id(), clone->id());
        const int axesCount = logicalDevice->axes().count();
        const int actionsCount = logicalDevice->actions().count();
        QCOMPARE(axesCount, clone->axes().count());
        QCOMPARE(actionsCount, clone->actions().count());

        if (axesCount > 0) {
            for (int i = 0; i < axesCount; ++i)
                QCOMPARE(logicalDevice->axes().at(i)->id(), clone->axes().at(i)->id());
        }

        if (actionsCount > 0) {
            for (int i = 0; i < actionsCount; ++i)
                QCOMPARE(logicalDevice->actions().at(i)->id(), clone->actions().at(i)->id());
        }
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QLogicalDevice> logicalDevice(new Qt3DInput::QLogicalDevice());
        TestArbiter arbiter(logicalDevice.data());

        // WHEN
        Qt3DInput::QAction *action = new Qt3DInput::QAction(logicalDevice.data());
        logicalDevice->addAction(action);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "action");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), action->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        logicalDevice->removeAction(action);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "action");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), action->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();

        // WHEN
        Qt3DInput::QAxis *axis = new Qt3DInput::QAxis(logicalDevice.data());
        logicalDevice->addAxis(axis);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "axis");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), axis->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        logicalDevice->removeAxis(axis);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "axis");
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), axis->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QLogicalDevice)

#include "tst_qlogicaldevice.moc"
