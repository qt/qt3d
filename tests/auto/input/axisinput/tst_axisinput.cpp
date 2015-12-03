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
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DInput/private/axisinput_p.h>
#include <Qt3DInput/QAxisInput>
#include <Qt3DCore/qscenepropertychange.h>
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

class tst_AxisInput: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::AxisInput backendAxisInput;
        Qt3DInput::QAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setKeys(QVariantList() << QVariant(1 << 8));
        axisInput.setAxis(327);
        axisInput.setScale(0.5f);
        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        backendAxisInput.setPeer(&axisInput);

        // THEN
        QCOMPARE(backendAxisInput.peerUuid(), axisInput.id());
        QCOMPARE(backendAxisInput.isEnabled(), axisInput.isEnabled());
        compareKeys(backendAxisInput.keys(), axisInput.keys());
        QCOMPARE(backendAxisInput.axis(), axisInput.axis());
        QCOMPARE(backendAxisInput.scale(), axisInput.scale());
        QCOMPARE(backendAxisInput.sourceDevice(), sourceDevice.id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::AxisInput backendAxisInput;

        // THEN
        QVERIFY(backendAxisInput.peerUuid().isNull());
        QCOMPARE(backendAxisInput.scale(), 0.0f);
        QVERIFY(backendAxisInput.keys().isEmpty());
        QCOMPARE(backendAxisInput.axis(), 0);
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());

        // GIVEN
        Qt3DInput::QAxisInput axisInput;
        TestDevice sourceDevice;

        axisInput.setKeys(QVariantList() << QVariant(1 << 8));
        axisInput.setAxis(327);
        axisInput.setScale(0.5f);
        axisInput.setSourceDevice(&sourceDevice);

        // WHEN
        backendAxisInput.updateFromPeer(&axisInput);
        backendAxisInput.cleanup();

        // THEN
        QVERIFY(backendAxisInput.peerUuid().isNull());
        QCOMPARE(backendAxisInput.scale(), 0.0f);
        QVERIFY(backendAxisInput.keys().isEmpty());
        QCOMPARE(backendAxisInput.axis(), 0);
        QCOMPARE(backendAxisInput.isEnabled(), false);
        QCOMPARE(backendAxisInput.sourceDevice(), Qt3DCore::QNodeId());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::Input::AxisInput backendAxisInput;

        // WHEN
        Qt3DCore::QScenePropertyChangePtr updateChange(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(32);
        updateChange->setPropertyName("axis");
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.axis(), 32);

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariantList() << QVariant(64));
        updateChange->setPropertyName("keys");
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        compareKeys(backendAxisInput.keys(), QVariantList() << QVariant(64));

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(0.5f);
        updateChange->setPropertyName("scale");
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.scale(), 0.5f);

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.isEnabled(), true);

        // WHEN
        TestDevice device;
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("sourceDevice");
        updateChange->setValue(QVariant::fromValue(device.id()));
        backendAxisInput.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxisInput.sourceDevice(), device.id());
    }
};

QTEST_APPLESS_MAIN(tst_AxisInput)

#include "tst_axisinput.moc"
