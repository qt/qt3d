// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.14
import QtQuick.Scene3D 2.14
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2

Item {
    id: main

    property real rotationValue: 0

    // If compositingMode == Scene3D.Underlay
    // Position in the QML file and sizing of the Scene3D
    // have no actual effect:
    // The 3D content will be drawn before any QtQuick content
    // and assume a FullScreen viewport
    Scene3D {
        id: scene3D
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        focus: true
        aspects: "input"
        compositingMode: _useUnderLayCompositingMode ? Scene3D.Underlay : Scene3D.FBO

        Logo {
            id: watch
        }
    }

    ColumnLayout {
        id: colorLayout
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: parent.width * 0.25
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: scene3D.top
        spacing: 5

        Text { text: qsTr("Appearance"); font.bold: true }
        Text { text: qsTr("Ambient color RGB") }
        RowLayout {
            Text { text: "R" }
            Slider {
                id: color_r
                Layout.fillWidth: true
                from: 0
                to: 255
                value: 128
            }
        }
        RowLayout {
            Text { text: "G" }
            Slider {
                id: color_g
                Layout.fillWidth: true
                from: 0
                to: 255
                value: 195
            }
        }
        RowLayout {
            Text { text: "B" }
            Slider {
                id: color_b
                Layout.fillWidth: true
                from: 0
                to: 255
                value: 66
            }
        }
        Text { text: qsTr("Shininess") }
        Slider {
            id: shining
            Layout.fillWidth: true
            from: 30
            to: 90
            value: 50
        }
    }

    ColumnLayout {
        id: transformLayout

        anchors.left: colorLayout.left
        anchors.right: colorLayout.right
        anchors.top: colorLayout.bottom
        anchors.topMargin: 10
        spacing: 5

        Text { text: qsTr("Item transform"); font.bold: true }
        Text { text: qsTr("Rotation") }
        RowLayout {
            Text { text: "X" }
            Slider {
                id: rotation_x
                Layout.fillWidth: true
                from: -45
                to: 45
                value: rotationValue
            }
        }
        RowLayout {
            Text { text: "Y" }
            Slider {
                id: rotation_y
                Layout.fillWidth: true
                from: -45
                to: 45
                value: rotationValue
            }
        }
        RowLayout {
            Text { text: "Z" }
            Slider {
                id: rotation_z
                Layout.fillWidth: true
                from: -45
                to: 45
                value: rotationValue
            }
        }

        RowLayout {
            CheckBox {id: animation; text: "Animation"; checked: false}
        }
    }

    ColumnLayout {
        id: cameraLayout

        anchors.left: colorLayout.left
        anchors.right: colorLayout.right
        anchors.top: transformLayout.bottom
        anchors.topMargin: 10
        spacing: 5

        Text { text: qsTr("Camera"); font.bold: true }
        Text { text: qsTr("View Ctr Z: ") + watch.cameraZ.toFixed(2) }
        Slider {
            id: viewCenter_z
            Layout.fillWidth: true
            from: 4
            to: 12
            value: 7.5
            onValueChanged: watch.setPositionZ(value)
        }
        Button {
            id: viewAll
            Layout.fillWidth: true
            text: qsTr("View All")
            onClicked: watch.viewLogo()
        }
    }

    RowLayout {
        id: compositingMode
        anchors.bottom: parent.bottom
        x: 5

        Text { text: qsTr("Compositing: ") + (scene3D.compositingMode == Scene3D.FBO ? "FBO" : "Underlay") }
    }

    SequentialAnimation {
        running: true
        paused: !animation.checked
        loops: Animation.Infinite

        NumberAnimation {
            target: main
            property: "rotationValue"
            easing.type: Easing.OutQuad
            duration: 1000
            from: 0
            to: 45
        }
        NumberAnimation {
            target: main
            property: "rotationValue"
            easing.type: Easing.InOutQuad
            duration: 1000
            from: 45
            to: -45
        }
        NumberAnimation {
            target: main
            property: "rotationValue"
            easing.type: Easing.InQuad
            duration: 1000
            from: -45
            to: 0
        }
    }
}
