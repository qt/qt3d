/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 1.0

Rectangle {
    id: mainwindow
    width: parent.width
    height: parent.height

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

    Item {
        width: parent.width; height: parent.height

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
 /*       MainButton {
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
        }*/

        //Button 2
        MainButton {
            id: button2
            anchors.top: parent.top
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
