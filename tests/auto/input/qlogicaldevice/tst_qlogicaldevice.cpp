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

#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/private/qlogicaldevice_p.h>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAction>

#include "testarbiter.h"

class tst_QLogicalDevice: public QObject
{
    Q_OBJECT
public:
    tst_QLogicalDevice()
    {
        qRegisterMetaType<Qt3DCore::QNode *>();
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QLogicalDevice> logicalDevice(new Qt3DInput::QLogicalDevice());
        arbiter.setArbiterOnNode(logicalDevice.data());

        // WHEN
        Qt3DInput::QAction *action = new Qt3DInput::QAction(logicalDevice.data());
        QCoreApplication::processEvents();
        arbiter.clear();

        logicalDevice->addAction(action);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();

        // WHEN
        logicalDevice->removeAction(action);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();

        // WHEN
        Qt3DInput::QAxis *axis = new Qt3DInput::QAxis(logicalDevice.data());
        QCoreApplication::processEvents();
        arbiter.clear();

        logicalDevice->addAxis(axis);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();

        // WHEN
        logicalDevice->removeAxis(axis);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), logicalDevice.data());

        arbiter.clear();
    }

    void checkAxisBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QLogicalDevice> device(new Qt3DInput::QLogicalDevice);
        {
            // WHEN
            Qt3DInput::QAxis axis;
            device->addAxis(&axis);

            // THEN
            QCOMPARE(axis.parent(), device.data());
            QCOMPARE(device->axes().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(device->axes().empty());

        {
            // WHEN
            Qt3DInput::QLogicalDevice someOtherDevice;
            QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis(&someOtherDevice));
            device->addAxis(axis.data());

            // THEN
            QCOMPARE(axis->parent(), &someOtherDevice);
            QCOMPARE(device->axes().size(), 1);

            // WHEN
            device.reset();
            axis.reset();

            // THEN Should not crash when the axis is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkActionBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QLogicalDevice> device(new Qt3DInput::QLogicalDevice);
        {
            // WHEN
            Qt3DInput::QAction action;
            device->addAction(&action);

            // THEN
            QCOMPARE(action.parent(), device.data());
            QCOMPARE(device->actions().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(device->actions().empty());

        {
            // WHEN
            Qt3DInput::QLogicalDevice someOtherDevice;
            QScopedPointer<Qt3DInput::QAction> action(new Qt3DInput::QAction(&someOtherDevice));
            device->addAction(action.data());

            // THEN
            QCOMPARE(action->parent(), &someOtherDevice);
            QCOMPARE(device->actions().size(), 1);

            // WHEN
            device.reset();
            action.reset();

            // THEN Should not crash when the action is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QLogicalDevice)

#include "tst_qlogicaldevice.moc"
