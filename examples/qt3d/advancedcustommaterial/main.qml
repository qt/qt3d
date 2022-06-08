// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0
import QtQuick.Scene3D 2.0
import Qt3D.Render 2.0
import QtQuick.Controls 2.0


Item {

    Rectangle {
        id: scene
        property bool colorChange: true
        anchors.fill: parent
        color: "#2d2d2d"

        transform: Rotation {
            id: sceneRotation
            axis.x: 1
            axis.y: 0
            axis.z: 0
            origin.x: scene.width / 2
            origin.y: scene.height / 2
        }
        Rectangle {
            id: controlsbg
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.topMargin: 10
            anchors.rightMargin: 1720
            anchors.bottomMargin: 10
            color: "grey"
            Column {
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.topMargin: 5
                spacing: 10
                Rectangle {
                    id: slidertexscale
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: scaletext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "TEXTURE SCALE"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider1
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 1.0
                        from: 0.3
                    }
                }
                Rectangle {
                    id: slidertexturespeed
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: texturespeedtext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "TEXTURE SPEED"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider5
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 1.1
                        to: 4.0
                        from: 0.0
                    }
                }
                Rectangle {
                    id: sliderspecularity
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: specularitytext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "SPECULARITY"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider3
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 1.0
                        to: 3.0
                        from: 0.0
                    }
                }
                Rectangle {
                    id: sliderdistortion
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: distortiontext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "DISTORTION"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider7
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 0.015
                        to: 0.1
                        from: 0.0
                    }
                }
                Rectangle {
                    id: slidernormal
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: normaltext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "NORMAL AMOUNT"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider8
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 2.2
                        to: 4.0
                        from: 0.0
                    }
                }
                Rectangle {
                    id: sliderwavespeed
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: wawespeedtext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "WAVE SPEED"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider2
                        live: false
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 0.75
                        to: 4.0
                        from: 0.1
                    }
                }
                Rectangle {
                    id: sliderwaveheight
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: waweheighttext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "WAVE HEIGHT"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider6
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 0.2
                        to: 0.5
                        from: 0.02
                    }
                }
                Rectangle {
                    id: slidermeshrotation
                    width: 180
                    height: 60
                    color: "#2d2d2d"
                    Text {
                        id: meshrotationtext
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        text: "MESH ROTATION"
                        color: "white"
                        font.bold: true
                        font.pointSize: 12
                    }
                    Slider {
                        id: slider4
                        anchors.fill: parent
                        anchors.topMargin: 30
                        anchors.rightMargin: 10
                        anchors.leftMargin: 10
                        value: 35.0
                        to: 360.0
                        from: 0.0
                    }
                }
            }
        }

        Scene3D {
            id: scene3d
            anchors.fill: parent
            anchors.leftMargin: 200
            anchors.topMargin: 10
            anchors.rightMargin: 10
            anchors.bottomMargin: 10
            focus: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            SceneRoot {
                id: root
            }
        }
    }
}
