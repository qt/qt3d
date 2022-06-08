// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

Rectangle {
    width: 1024
    height: 1024
    color: "white"

    TextRectangle {
        id: textRect
    }

    Rectangle {
        id: rect
        x: 0
        y: 0
        width: 200
        height: 200
        color: "red"

        ColorAnimation {
            target: rect
            loops: Animation.Infinite
            property: "color"
            from: "blue"
            to: "green"
            duration: 2000
            running: true
        }
    }

    Rectangle {
        id: rect2
        width: 300
        height: 100
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "red"

        ColorAnimation {
            target: rect2
            loops: Animation.Infinite
            property: "color"
            from: "red"
            to: "white"
            duration: 2000
            running: true
        }
    }

    Button {
        id: button
        width: 100
        height: 80
        anchors.top: rect.bottom
        anchors.left: parent.left
        text: "button"
        scale: 2.0
    }
    CheckBox {
        id: checkbox
        width: 200
        height: 180
        scale: 2.0
        text: "checkbox"
        checked: true
        anchors.top: parent.top
        anchors.right: parent.right

        Timer {
            interval: 1000
            repeat: true
            running: true
            onTriggered: {
                checkbox.checked = !checkbox.checked;
            }
        }
    }
    Slider {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: 400
        value: 0.0
        minimumValue: 0
        maximumValue: 1

        PropertyAnimation on value {
            loops: Animation.Infinite
            duration: 3000
            from: 0.0
            to: 1.0
            running: true
        }
    }
}
