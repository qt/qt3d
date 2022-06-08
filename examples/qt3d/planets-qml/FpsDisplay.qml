// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0

Item {
    id: fpsDisplayControl
    property bool hidden: true
    property real fps: 0.0

    onHiddenChanged: {
        if (fpsDisplayControl.hidden)
            fpsDisplay.color = "transparent"
        else
            fpsDisplay.color = "#000000FF"
    }

    onFpsChanged: {
        fpsDisplay.updateFps()
    }

    Rectangle {
        anchors.fill: parent
        id: fpsDisplay
        color: "transparent"

        property real maxFps: 60.0
        property color maxFpsColor: "#5500FF00"
        property color minFpsColor: "#55FF0000"

        function updateFps() {
            var scale = (fps > maxFps)?1.0:(fps/maxFps)
            var r = (1 - scale) * minFpsColor.r + scale * maxFpsColor.r
            var g = (1 - scale) * minFpsColor.g + scale * maxFpsColor.g
            var b = (1 - scale) * minFpsColor.b + scale * maxFpsColor.b
            var a = (1 - scale) * minFpsColor.a + scale * maxFpsColor.a
            fpsCauge.height = scale * fpsDisplay.height
            fpsCauge.color = Qt.rgba(r, g, b, a)
        }

        Rectangle {
            id: fpsCauge
            width: parent.width
            anchors.bottom: parent.bottom
            visible: !fpsDisplayControl.hidden
        }

        Text {
            id: fpsText
            text: ""+(fps | 0)
            font.family: "Helvetica"
            font.pixelSize: 16
            font.weight: Font.Light
            color: "white"
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            visible: !fpsDisplayControl.hidden
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            fpsDisplayControl.hidden = !fpsDisplayControl.hidden
        }
    }
}

