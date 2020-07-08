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

#include <Qt3DInput/QAbstractPhysicalDevice>
#include <Qt3DInput/QButtonAxisInput>
#include <Qt3DInput/private/qbuttonaxisinput_p.h>

#include "testarbiter.h"
#include "testdevice.h"

class tst_QButtonAxisInput: public QObject
{
    Q_OBJECT
public:
    tst_QButtonAxisInput()
    {
        qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("Qt3DInput::QAbstractPhysicalDevice*");
    }

private Q_SLOTS:
    void shouldHaveDefaultState()
    {
        // GIVEN
        Qt3DInput::QButtonAxisInput axisInput;

        // THEN
        QVERIFY(axisInput.buttons().isEmpty());
        QCOMPARE(axisInput.scale(), 1.0f);
        QCOMPARE(axisInput.acceleration(), -1.0f);
        QCOMPARE(axisInput.deceleration(), -1.0f);
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QButtonAxisInput> axisInput(new Qt3DInput::QButtonAxisInput());
        arbiter.setArbiterOnNode(axisInput.data());

        // WHEN
        const QList<int> buttons = { 555 };
        axisInput->setButtons(buttons);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        axisInput->setScale(1340.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        TestDevice *device = new TestDevice(axisInput.data());

        axisInput->setSourceDevice(device);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        axisInput->setAcceleration(42.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();

        // WHEN
        axisInput->setDeceleration(43.0f);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axisInput.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QButtonAxisInput)

#include "tst_qbuttonaxisinput.moc"
