// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import QtQuick.Scene3D 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    id: root

    Scene3D {
        anchors.fill: parent
        focus: true
        aspects: ["input", "logic", "render"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        Scene {
            id: scene
            width : root.lineWidth
        }
    }

    property real wMin : 0.5
    property real wMax : 64.0
    property real lineWidth : wMin * Math.exp(Math.log(wMax / wMin) * slider.value)

    Text {
        id: sliderLabel
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20
        font.pixelSize: 20
        color: "white"
        text: "width = " + lineWidth.toFixed(2)
    }

    Slider {
        id: slider
        anchors.top: sliderLabel.bottom
        anchors.right: parent.right
        anchors.margins: 20
        width: 200
        value: 0.0
    }

    Text {
        anchors.top: slider.bottom
        anchors.right: parent.right
        anchors.margins: 20
        color: scene.smooth ? "white" : "lightgray"
        text: scene.smooth ? "Line Smoothing enabled" : "Line Smoothing disabled"
        font.pixelSize: 20
        MouseArea {
            anchors.fill: parent
            onClicked: scene.smooth = !scene.smooth
        }
    }
}
