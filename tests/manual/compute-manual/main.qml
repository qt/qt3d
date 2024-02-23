// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.14
import QtQuick.Scene3D 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.1
import Qt3D.Render 2.14

Item {

    Scene3D {
        anchors.fill: parent
        aspects: ["input", "logic"]
        focus: true
        ParticlesScene {
            id: scene
            computeMode: runModeCombo.currentIndex === 0 ? ComputeCommand.Continuous : ComputeCommand.Manual
        }
    }

    RowLayout {
        id: colorLayout
        anchors.left: parent.left
        anchors.leftMargin: 35
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 35
        spacing: 15

        RowLayout {
            Text {
                text: "Run Mode:"
                color: "white"
            }
            ComboBox {
                id: runModeCombo
                model: ["Continuous", "Manual"]
            }
        }
        RowLayout {
            visible: runModeCombo.currentIndex === 1
            Text {
                color: "white"
                text: "Frames"
            }
            SpinBox {
                id: frameCountSpinBox
                value: 5
                from: 1
                to: 10
                stepSize: 1
            }
            Button {
                text: "Trigger"
                onClicked: {
                    // Trigger Compute Manual
                    scene.triggerCompute(frameCountSpinBox.value)
                }
            }
        }
    }
}
