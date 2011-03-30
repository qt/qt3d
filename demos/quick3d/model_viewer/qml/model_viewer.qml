/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import Qt3D 1.0

Rectangle {
    id: mainwindow
    width: 800
    height: 500

    gradient: Gradient {
         GradientStop { position: 0.0; color: "#300000" }
         GradientStop { position: 0.3; color: "darkred" }
         GradientStop { position: 1.0; color: "#300000" }
    }

    states: [
        State {
            name: "Open"

            PropertyChanges { target: mainwindow; x: -800}

        },
        State {
            name: "NotImplemented"

            //PropertyChanges { target: container; y: 0 }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.OutQuad; duration: 600 }
    }


    Viewport {
        visible: false
        x: 800
        id: viewport1
        width: 800; height: 500
        picking: false

        camera: Camera {
            eye: Qt.vector3d(0, 0, 350)
            farPlane: 2000
        }


    Text {
        id: imgSrc
        text: "Image Source"; font.family: "Helvetica"; font.pointSize: 22; color: "white"
        anchors.top: parent.top
        z: 10
    }

    LongButton {
        //width: 800
        id: imgSrcButton
        anchors.top: imgSrc.bottom
        anchors.topMargin: 2
        z: 10
    }
    Text {
        id: displayOptions
        anchors.top: imgSrcButton.bottom
        anchors.topMargin: 2
        text: "Display Options"; font.family: "Helvetica"; font.pointSize: 22; color: "white"
        z: 10
    }
    FourButtons {
        id: displayOptionsButton
        anchors.top: displayOptions.bottom
        anchors.topMargin: 2
        z: 10
    }
    Text {
        id: texture
        anchors.top: displayOptionsButton.bottom
        anchors.topMargin: 2
        text: "Texture"; font.family: "Helvetica"; font.pointSize:22; color: "white"
        z: 10
    }
    LongButton {
        id: textureButton
        anchors.top: texture.bottom
        anchors.topMargin: 2
        z: 10
    }
    Text {
        id: navigation
        anchors.top: textureButton.bottom
        anchors.topMargin: 2
        text: "Navigation"; font.family: "Helvetica"; font.pointSize: 22; color: "white"
        z: 10
    }
    FourButtons {
        id: navigationButton
        anchors.top: navigation.bottom
        anchors.topMargin: 2
        z: 10
    }
    Text {
        id: background
        anchors.top: navigationButton.bottom
        anchors.topMargin: 2
        text: "Background"; font.family: "Helvetica"; font.pointSize: 22; color: "white"
        z: 10
    }
    LongButton {
        id: backgroundButton
        anchors.top: background.bottom
        anchors.topMargin: 2
        z: 10
    }

        //Button 1
        MainButton {
            id: button1c
            anchors.top: parent.top
            imageSource: "images/cog.svg"


            MouseArea {
                //property bool on: false
                anchors.fill: parent
                onClicked: {
                    //source_mesh.options =  "ForceSmooth";
                    //source_mesh.source = textInput.text;
                    parent.bounce = true;

                    if (mainwindow.state == "")mainwindow.state = "Open"; else mainwindow.state = "";
                }
            }
        }

    }

    Item {
        width: 800; height: 500

        Viewport {
            anchors.fill: parent
            id: viewport
            picking: false
            blending: true

            camera: Camera {
                eye: Qt.vector3d(0, 0, 350)
                farPlane: 2000
            }

            Item3D {
                id: mainItem
                scale: 50
                mesh: source_mesh
                effect: Effect {}
                cullFaces: "CullBackFaces"
            }

            Mesh {
                id: source_mesh
                source: "meshes/monkey.3ds"
            }

            MouseArea {
                id: mouseArea
                property bool rotating: false
                property int startX: 0
                property int startY: 0
                property variant startEye
                property variant startCenter
                property variant startUpVector
                anchors.fill: parent
                onPressed: {
                    if (mouse.button == Qt.LeftButton) {
                        rotating = true;
                        startX = mouse.x;
                        startY = mouse.y;
                        startEye = viewport.camera.eye;
                        startCenter = viewport.camera.center;
                        startUpVector = viewport.camera.upVector;
                    }
                }
                onReleased: {
                    if (mouse.button == Qt.LeftButton)
                        rotating = false;
                }
                onPositionChanged: {
                    if (rotating) {
                        var deltaX = mouse.x - startX;
                        var deltaY = mouse.y - startY;
                        var angleAroundY = deltaX * 90 / mouseArea.width;
                        var angleAroundX = deltaY * 90 / mouseArea.height;

                        viewport.camera.eye = startEye;
                        viewport.camera.center = startCenter;
                        viewport.camera.upVector = startUpVector;
                        viewport.camera.tiltPanRollCenter
                            (-angleAroundX, -angleAroundY, 0);
                    }
                }
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 15
            anchors.bottom: parent.bottom
            radius: 10
            border.width: 1
            border.color: "black"
            color: "white"
            width: 500
            height: 25
            //Enter text here
            TextInput {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                id: textInput
                text: source_mesh.source
                activeFocusOnPress: false
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!textInput.activeFocus) {
                            textInput.forceActiveFocus();
                            textInput.openSoftwareInputPanel();
                        } else {
                            textInput.focus = false;
                        }
                    }
                    onPressAndHold: textInput.closeSoftwareInputPanel();
                }
            }
        }


        //Button 1
        MainButton {
            id: button1
            anchors.top: parent.top
            imageSource: "images/cog.svg"

            MouseArea {
                //property bool on: false
                anchors.fill: parent
                onClicked: {
                    //source_mesh.options =  "ForceSmooth";
                    //source_mesh.source = textInput.text;
                    parent.bounce = true;

                    if (mainwindow.state == "")mainwindow.state = "Open"; else mainwindow.state = "";
                }
            }
        }

        //Button 2
        MainButton {
            id: button2
            anchors.top: button1.bottom
            imageSource: "images/zoomin.svg";
            property bool zoom: false
            property real changeScale: 1.1

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    button2.changeScale = 1.1
                    button2.zoom = true
                    parent.bounce = true;
                }
                onPressAndHold: {
                    button2.changeScale = 2
                    button2.zoom = true
                    parent.bounce = true;
                }

            }

            SequentialAnimation {
                running: button2.zoom
                loops: 1
                NumberAnimation { target: mainItem; property: "scale"; to: mainItem.scale*button2.changeScale; duration: 50;}
                onCompleted: button2.zoom = false
            }
        }

        //Buton 3
        MainButton {
            id: button3
            anchors.top: button2.bottom
            imageSource: "images/zoomout.svg"
            property bool zoom: false
            property real changeScale: 1.1

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    button3.changeScale = 1.1
                    button3.zoom = true
                    parent.bounce = true;
                }
                onPressAndHold: {
                    button3.changeScale = 2
                    button3.zoom = true
                    parent.bounce = true;
                }
            }

            SequentialAnimation {
                running: button3.zoom
                loops: 1
                NumberAnimation { target: mainItem; property: "scale"; to: mainItem.scale/button3.changeScale; duration: 50;}
                onCompleted: button3.zoom = false
            }
        }

        //Buton 4
        MainButton {
            id: button4
            anchors.top: button3.bottom
            imageSource: "images/arrow.svg"

            MouseArea {
                anchors.fill: parent
                onClicked: {

                    parent.bounce = true;

                    source_mesh.source = textInput.text;
                }
            }
        }


    }
}
