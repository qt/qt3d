// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtQuick.Scene3D 2.0

Item {
    id: root
    property string currentFont
    property bool bold : false
    property bool italic : false

    Scene3D {
        id: scene3d
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: settingsPane.left
        focus: true
        aspects: ["input", "logic", "render"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        TextScene {
            id: textScene
            fontFamily: root.currentFont
            fontBold: bold
            fontItalic: italic
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            textScene.clicked()
        }
    }

    Item {
        id: settingsPane

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 400

        Rectangle {
            width: parent.width/2
            height:50
            color: root.bold ? "#000066" : "#222222"
            Text {
                anchors.centerIn: parent
                text: "Bold"
                font.pointSize: 20
                color: "#ffffff"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.bold = !root.bold
            }
        }

        Rectangle {
            x: parent.width/2
            width: parent.width/2
            height:50
            color: root.italic ? "#000066" : "#222222"
            Text {
                anchors.centerIn: parent
                text: "Italic"
                font.pointSize: 20
                color: "#ffffff"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: root.italic = !root.italic
            }
        }

        ListView {
            anchors.fill: parent
            anchors.topMargin: 50
            model: Qt.fontFamilies()

            delegate: Rectangle {
                height: fontFamilyText.height + 10
                width: parent.width
                color: (modelData == root.currentFont) ? "#000066" : "#222222"

                Text {
                    id: fontFamilyText
                    anchors.centerIn: parent
                    font.pointSize: 14
                    font.family: modelData
                    text: modelData
                    color: "#ffffff"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.currentFont = modelData;

                    }
                }
            }
        }
    }
}
