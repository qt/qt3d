// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Scene3D 2.0

Item {
    id: mainview
    width: 1280
    height: 768
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "white"

        Scene3D {
            anchors.fill: parent
            focus: true
            aspects: "input"

            SphereView {
                id: sphereView
            }
        }
    }

    property int frames: 0

    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            console.log("Frames done :" + frames)
            frames = 0
        }
        onRunningChanged: frames = 0
    }
}
