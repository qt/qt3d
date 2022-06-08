// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.15
import QtQuick.Scene2D 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "white"
    focus: true
    border.color: "black"
    enabled: false

    property alias colorR: color_r.value
    property alias colorG: color_g.value
    property alias colorB: color_b.value
    property alias shininess: shining.value

    property alias rotationX: rotation_x.value
    property alias rotationY: rotation_y.value
    property alias rotationZ: rotation_z.value

    property alias logoCentreZ: logoCenter_z.value


    QtObject {
        id: d
        property real rotationValue: 0
        property color textColor: root.enabled ? "black" : "gray"
    }

    SequentialAnimation {
        running: true
        paused: !animation.checked
        loops: Animation.Infinite

        NumberAnimation {
            target: d
            property: "rotationValue"
            easing.type: Easing.OutQuad
            duration: 1000
            from: 0
            to: 45
        }
        NumberAnimation {
            target: d
            property: "rotationValue"
            easing.type: Easing.InOutQuad
            duration: 1000
            from: 45
            to: -45
        }
        NumberAnimation {
            target: d
            property: "rotationValue"
            easing.type: Easing.InQuad
            duration: 1000
            from: -45
            to: 0
        }
    }

    ColumnLayout {
        id: tipLayout
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: root.top
        spacing: 5

        Text { text: "Middle click to (de)activate"; font.bold: true }
    }

    ColumnLayout {
        id: colorLayout
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: tipLayout.bottom
        anchors.topMargin: 20
        spacing: 5

        Text { text: "Appearance"; font.bold: true; color: d.textColor }
        Text { text: "Ambient color RGB"; color: d.textColor }
        RowLayout {
            Text { text: "R"; color: d.textColor }
            Slider {
                id: color_r
                Layout.fillWidth: true
                from: 0
                to: 255
                value: 128
            }
        }
        RowLayout {
            Text { text: "G"; color: d.textColor }
            Slider {
                id: color_g
                Layout.fillWidth: true
                from: 0
                to: 255
                value: 195
            }
        }
        RowLayout {
            Text { text: "B"; color: d.textColor }
            Slider {
                id: color_b
                Layout.fillWidth: true
                from: 0
                to: 255
                value: 66
            }
        }
        Text { text: "Shininess"; color: d.textColor }
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

        Text { text: "Item transform"; font.bold: true; color: d.textColor }
        Text { text: "Rotation"; color: d.textColor }
        RowLayout {
            Text { text: "X"; color: d.textColor }
            Slider {
                id: rotation_x
                Layout.fillWidth: true
                from: -45
                to: 45
                value: d.rotationValue
            }
        }
        RowLayout {
            Text { text: "Y"; color: d.textColor }
            Slider {
                id: rotation_y
                Layout.fillWidth: true
                from: -45
                to: 45
                value: d.rotationValue
            }
        }
        RowLayout {
            Text { text: "Z"; color: d.textColor }
            Slider {
                id: rotation_z
                Layout.fillWidth: true
                from: -45
                to: 45
                value: d.rotationValue
            }
        }

        RowLayout {
            CheckBox { id: animation; text: "Animation"; checked: false }
        }
    }

    ColumnLayout {
        id: cameraLayout

        anchors.left: colorLayout.left
        anchors.right: colorLayout.right
        anchors.top: transformLayout.bottom
        anchors.topMargin: 10
        spacing: 5

        Text { text: "Object"; font.bold: true; color: d.textColor }
        Text { text: "Logo Center Z"; color: d.textColor }
        Slider {
            id: logoCenter_z
            Layout.fillWidth: true
            from: -10
            to: 8
            value: 0
        }
    }
}
