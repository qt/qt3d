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
#include <Qt3DInput/private/axis_p.h>
#include <Qt3DInput/QAxisInput>
#include <Qt3DInput/QAxis>

class tst_Axis: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DInput::Input::Axis backendAxis;
        Qt3DInput::QAxis axis;
        Qt3DInput::QAxisInput axisInput;

        axis.addInput(&axisInput);
        axis.setName(QStringLiteral("L88"));

        // WHEN
        backendAxis.setPeer(&axis);

        // THEN
        QCOMPARE(backendAxis.peerUuid(), axis.id());
        QCOMPARE(backendAxis.isEnabled(), axis.isEnabled());
        QCOMPARE(backendAxis.name(), axis.name());
        QCOMPARE(backendAxis.inputs().size(), axis.inputs().size());

        const int inputsCount = backendAxis.inputs().size();
        if (inputsCount > 0) {
            for (int i = 0; i < inputsCount; ++i)
                QCOMPARE(backendAxis.inputs().at(i), axis.inputs().at(i)->id());
        }
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DInput::Input::Axis backendAxis;

        // THEN
        QVERIFY(backendAxis.peerUuid().isNull());
        QVERIFY(backendAxis.name().isEmpty());
        QCOMPARE(backendAxis.axisValue(), 0.0f);
        QCOMPARE(backendAxis.isEnabled(), false);
        QCOMPARE(backendAxis.inputs().size(), 0);

        // GIVEN
        Qt3DInput::QAxis axis;
        Qt3DInput::QAxisInput axisInput;

        axis.addInput(&axisInput);
        axis.setName(QStringLiteral("L88"));

        // WHEN
        backendAxis.updateFromPeer(&axis);
        backendAxis.setAxisValue(883.0f);
        backendAxis.cleanup();

        // THEN
        QVERIFY(backendAxis.peerUuid().isNull());
        QVERIFY(backendAxis.name().isEmpty());
        QCOMPARE(backendAxis.axisValue(), 0.0f);
        QCOMPARE(backendAxis.isEnabled(), false);
        QCOMPARE(backendAxis.inputs().size(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DInput::Input::Axis backendAxis;

        // WHEN
        Qt3DCore::QScenePropertyChangePtr updateChange(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QStringLiteral("LT1"));
        updateChange->setPropertyName("name");
        backendAxis.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxis.name(), QStringLiteral("LT1"));

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendAxis.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxis.isEnabled(), true);

        // WHEN
        Qt3DCore::QNodeId inputId = Qt3DCore::QNodeId::createId();
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeAdded, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("input");
        updateChange->setValue(QVariant::fromValue(inputId));
        backendAxis.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxis.inputs().size(), 1);
        QCOMPARE(backendAxis.inputs().first(), inputId);

        // WHEN
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeRemoved, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setPropertyName("input");
        updateChange->setValue(QVariant::fromValue(inputId));
        backendAxis.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendAxis.inputs().size(), 0);
    }
};

QTEST_APPLESS_MAIN(tst_Axis)

#include "tst_axis.moc"
