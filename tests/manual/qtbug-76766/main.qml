// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick 2.12
import QtQuick.Scene3D 2.12
import Qt3D.Render 2.12

Item {
    anchors.fill: parent

    Scene3D {
        id: scene3d
        anchors.fill: parent
        focus: true
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        SceneRoot {
            id: root
        }
    }

    Image {
        width: 400
        fillMode: Image.PreserveAspectFit
        source: "qrc:/expected_output.png"
        Text {
            anchors.centerIn: parent
            color: "white"
            text: "This is the expected output"
        }
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border {
                color: "white"
                width: 2
            }
        }
    }
}
