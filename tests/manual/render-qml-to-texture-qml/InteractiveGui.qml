// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

Rectangle {
    width: 1024
    height: 1024
    color: "white"
    focus: true

    Rectangle {
        id: rect
        x: 0
        y: 0
        width: 200
        height: 200
        color: "red"
    }

    Rectangle {
        id: rect2
        width: 300
        height: 100
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "blue"
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
    }
    Slider {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: 400
        value: 1
        minimumValue: 0
        maximumValue: 1

        onValueChanged: {
            rect.color = Qt.rgba(value, 0, 0, 1)
            rect2.color = Qt.rgba(0, 0, value, 1)
        }
    }
}
