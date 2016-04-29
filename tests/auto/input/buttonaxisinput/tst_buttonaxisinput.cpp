/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
#include <qbackendnodetester.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qnodepropertychange.h>
#include <Qt3DInput/private/buttonaxisinput_p.h>
#include <Qt3DInput/QButtonAxisInput>
#include <Qt3DCore/qnodepropertychange.h>
#include "testdevice.h"

namespace {

void compareKeys(const QVector<int> &backendKeys, const QVariantList &frontendKeys)
{
    QCOMPARE(backendKeys.size(), frontendKeys.size());
    for (int i = 0, m = backendKeys.size(); i < m; ++i) {
        QCOMPARE(backendKeys.at(i), frontendKeys.at(i).toInt());
    }
}

}

class tst_ButtonAxisInput: public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::ButtonAxisInput backendAxisInput;
        Qt3DInput::QButtonAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setButtons(QVariantList() << QVariant(1 << 8));
        axisInput.setAxis(327);
        axisInput.setScale(0.5f);
        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        simulateInitialization(&axisInput, &backendAxisInput);

        // THEN
        QCOMPARE(backendAxisInput.peerId(), axisInput.id());
        QCOMPARE(backendAxisInput.isEnabled(), axisInput.isEnabled());
        compareKeys(backendAxisInput.buttons(), axisInput.buttons());
        QCOMPARE(backendAxisInput.axis(), axisInput.axis());
        QCOMPARE(backendAxisInput.scale(), axisInput.scale());
        QCOMPARE(backendAxisInput.sourceDevice(), sourceDevice.id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::ButtonAxisInput backendAxisInput;

        // THEN
        QVERIFY(backendAxisInput.peerId().isNull());
        QCOMPARE(backendAxisInput.scale(), 0.0f);
        QVERIFY(backendAxisInput.buttons().isEmpty());
        QCOMPARE(backendAxisInput.axis(), 0);
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());

        // GIVEN
        Qt3DInput::QButtonAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setButtons(QVariantList() << QVariant(1 << 8));
        axisInput.setAxis(327);
        axisInput.setScale(0.5f);
        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        backendAxisInput.updateFromPeer(&axisInput);
        backendAxisInput.cleanup();

        // THEN
        QVERIFY(backendAxisInput.peerId().isNull());
        QCOMPARE(backendAxisInput.scale(), 0.0f);
        QVERIFY(backendAxisInput.buttons().isEmpty());
        QCOMPARE(backendAxisInput.axis(), 0);
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::Input::ButtonAxisInput backendAxisInput;


        // WHEN
        Qt3DCore::QNodePropertyChangePtr updateChange(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(32);
        updateChange->setPropertyName("axis");
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.axis(), 32);

        // WHEN
        updateChange.reset(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariantList() << QVariant(64));
        updateChange->setPropertyName("buttons");
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        compareKeys(backendAxisInput.buttons(), QVariantList() << QVariant(64));

        // WHEN
        updateChange.reset(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(0.5f);
        updateChange->setPropertyName("scale");
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.scale(), 0.5f);

        // WHEN
        updateChange.reset(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.isEnabled(), true);

        // WHEN
        TestDevice device;
        updateChange.reset(new Qt3DCore::QNodePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("sourceDevice");
        updateChange->setValue(QVariant::fromValue(device.id()));
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.sourceDevice(), device.id());
    }
};

QTEST_APPLESS_MAIN(tst_ButtonAxisInput)

#include "tst_buttonaxisinput.moc"
