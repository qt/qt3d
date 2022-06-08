// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p.h>
#include <Qt3DInput/private/qabstractphysicaldevicebackendnode_p_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DInput/qinputaspect.h>
#include <Qt3DInput/private/qinputaspect_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/axissetting_p.h>
#include "testdevice.h"

class TestPhysicalDeviceBackendNode : public Qt3DInput::QAbstractPhysicalDeviceBackendNode
{
public:
    TestPhysicalDeviceBackendNode(Qt3DCore::QBackendNode::Mode mode = Qt3DCore::QBackendNode::ReadOnly)
        : Qt3DInput::QAbstractPhysicalDeviceBackendNode(mode)
    {}

    float axisValue(int axisIdentifier) const override
    {
        if (axisIdentifier == 883)
            return 883.0f;
        return 0.0f;
    }

    bool isButtonPressed(int buttonIdentifier) const override
    {
        if (buttonIdentifier == 454)
            return true;
        return false;
    }

};

class tst_QAbstractPhysicalDeviceBackendNode : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;

        // THEN
        QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.isEnabled(), false);
        QVERIFY(backendQAbstractPhysicalDeviceBackendNode.inputAspect() == nullptr);
        QVERIFY(backendQAbstractPhysicalDeviceBackendNode.peerId().isNull());
    }

    void checkAxisValue()
    {
        // GIVEN
        TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;

        // WHEN
        float axisValue = backendQAbstractPhysicalDeviceBackendNode.axisValue(883);
        // THEN
        QCOMPARE(axisValue, 883.0f);

        // WHEN
        axisValue = backendQAbstractPhysicalDeviceBackendNode.axisValue(454);
        // THEN
        QCOMPARE(axisValue, 0.0f);
    }

    void checkButtonPressed()
    {
        // GIVEN
        TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;

        // WHEN
        bool buttonPressed = backendQAbstractPhysicalDeviceBackendNode.isButtonPressed(883);
        // THEN
        QCOMPARE(buttonPressed, false);

        // WHEN
        buttonPressed = backendQAbstractPhysicalDeviceBackendNode.isButtonPressed(454);
        // THEN
        QCOMPARE(buttonPressed, true);
    }

    void checkCleanupState()
    {
        // GIVEN
        TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;
        Qt3DInput::QInputAspect aspect;

        // WHEN
        backendQAbstractPhysicalDeviceBackendNode.setEnabled(true);
        backendQAbstractPhysicalDeviceBackendNode.setInputAspect(&aspect);

        // THEN
        QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.inputAspect(), &aspect);

        // WHEN
        backendQAbstractPhysicalDeviceBackendNode.cleanup();

        // THEN
        QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.isEnabled(), false);
        QVERIFY(backendQAbstractPhysicalDeviceBackendNode.inputAspect() == nullptr);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        TestDevice physicalDeviceNode;

        {
            // WHEN
            TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;
            simulateInitializationSync(&physicalDeviceNode, &backendQAbstractPhysicalDeviceBackendNode);

            // THEN
            QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.isEnabled(), true);
            QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.peerId(), physicalDeviceNode.id());
        }
        {
            // WHEN
            TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;
            physicalDeviceNode.setEnabled(false);
            simulateInitializationSync(&physicalDeviceNode, &backendQAbstractPhysicalDeviceBackendNode);

            // THEN
            QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.peerId(), physicalDeviceNode.id());
            QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.isEnabled(), false);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        TestDevice physicalDeviceNode;
        TestPhysicalDeviceBackendNode backendQAbstractPhysicalDeviceBackendNode;
        Qt3DInput::QInputAspect aspect;
        backendQAbstractPhysicalDeviceBackendNode.setInputAspect(&aspect);
        simulateInitializationSync(&physicalDeviceNode, &backendQAbstractPhysicalDeviceBackendNode);

        {
            // WHEN
            const bool newValue = false;
            physicalDeviceNode.setEnabled(newValue);
            backendQAbstractPhysicalDeviceBackendNode.syncFromFrontEnd(&physicalDeviceNode, false);

            // THEN
            QCOMPARE(backendQAbstractPhysicalDeviceBackendNode.isEnabled(), newValue);
        }

        {
            Qt3DInput::QAxisSetting settings1;
            Qt3DInput::QAxisSetting settings2;

            settings1.setAxes(QList<int> { 883 });
            settings2.setAxes(QList<int> { 454 });
            Qt3DInput::QAbstractPhysicalDeviceBackendNodePrivate *priv = static_cast<Qt3DInput::QAbstractPhysicalDeviceBackendNodePrivate *>(
                        Qt3DCore::QBackendNodePrivate::get(&backendQAbstractPhysicalDeviceBackendNode));

            // Create backend resource
            {
                Qt3DInput::QInputAspectPrivate *aspectPrivate = static_cast<Qt3DInput::QInputAspectPrivate *>(Qt3DCore::QAbstractAspectPrivate::get(&aspect));
                Qt3DInput::Input::InputHandler *handler = aspectPrivate->m_inputHandler.data();
                Qt3DInput::Input::AxisSetting *backendSetting1 = handler->axisSettingManager()->getOrCreateResource(settings1.id());
                Qt3DInput::Input::AxisSetting *backendSetting2 = handler->axisSettingManager()->getOrCreateResource(settings2.id());
                simulateInitializationSync(&settings1, backendSetting1);
                simulateInitializationSync(&settings2, backendSetting2);
            }

            // Adding AxisSettings
            {
                // WHEN
                physicalDeviceNode.addAxisSetting(&settings1);
                backendQAbstractPhysicalDeviceBackendNode.syncFromFrontEnd(&physicalDeviceNode, false);

                // THEN
                QCOMPARE(priv->m_axisSettings.size(), 1);

                // WHEN
                physicalDeviceNode.addAxisSetting(&settings2);
                backendQAbstractPhysicalDeviceBackendNode.syncFromFrontEnd(&physicalDeviceNode, false);

                // THEN
                QCOMPARE(priv->m_axisSettings.size(), 2);
            }

            // Removing AxisSettings
            {
                // WHEN
                physicalDeviceNode.removeAxisSetting(&settings1);
                backendQAbstractPhysicalDeviceBackendNode.syncFromFrontEnd(&physicalDeviceNode, false);

                // THEN
                QCOMPARE(priv->m_axisSettings.size(), 1);

                // WHEN
                physicalDeviceNode.removeAxisSetting(&settings2);
                backendQAbstractPhysicalDeviceBackendNode.syncFromFrontEnd(&physicalDeviceNode, false);

                // THEN
                QCOMPARE(priv->m_axisSettings.size(), 0);
            }
        }
    }

};

QTEST_MAIN(tst_QAbstractPhysicalDeviceBackendNode)

#include "tst_qabstractphysicaldevicebackendnode.moc"
