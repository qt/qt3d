// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Scene3D 2.0
import QtQuick.Controls 2.0

Item {

    Loader {
        id: loader
        anchors.fill: parent
        focus: true
        source: "Scene.qml"
    }

    Row {
        id: buttonRow
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: parent.height * 0.1
        }
        spacing: 10
        Button {
            text: "Scene 1";
            highlighted: loader.source === "Scene.qml"
            onClicked: loader.source = "Scene.qml"
        }
        Button {
            text: "Scene 2";
            highlighted: loader.source === "Scene2.qml"
            onClicked: loader.source = "Scene2.qml"
        }
    }
}
