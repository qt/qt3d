/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 2.0

// QTBUG-17418
Item {
    id: screen
    width: 720
    height: 480
    Rectangle {
        id: description
        border.width: 3
        radius: 5
        property variant radarPostion: recieverItem.localToWorld();
        x: radarPostion.x * view.width
        width: parent.width
        height: text.height + 10
        Text {
            id: text
            width: parent.width
            wrapMode: "WordWrap"
            text: "There should be a stationary lander model with a rotating radar dish attached.  \n" +
                  "The lander and attached radar dish should show no scaling or translation.\n" +
                  "There should be a second radar dish colored red, oscillating horizontally without scaling.\n" +
                  "There should be a third radar dish colored blue, scaling but not rotating.. \n" +
                  "It is acceptable if the scaling dish appears to be moving due to the scaling."
            anchors.centerIn: parent
        }
    }
    Rectangle {
        width: parent.width
        anchors.top: description.bottom
        anchors.bottom: screen.bottom
        anchors.left: screen.left
        anchors.right: screen.right
        color: "#aaccee"

        Viewport  {
            id: view
            anchors.fill: parent
            picking: true
            //        showPicking: true
            camera: Camera { eye: Qt.vector3d(0,15,50)}

            Effect {
                id: redEffect; material: red
                objectName: "redEffect"
            }

            Effect {
                id: blueEffect; material: blue
                objectName: "blueEffect"
            }

            Mesh {
                id:landerMesh
                source: "meshes/lunar-lander.3ds"
            }

            // A second lander mesh so we can have two different colors
            Mesh {
                id:landerMesh2
                source: "meshes/lunar-lander.3ds"
            }

            Item3D {
                mesh: landerMesh
                objectName: "lander"
                effect: Effect { material: green ; objectName: "greenEffect"}
                Item3D {
                    // Correct context loss from "pulling off" of graph
                    transform: [
                        Rotation3D { axis: Qt.vector3d(1.0, 0.0, 0.0) ; angle: -90},
                        Rotation3D {
                            id: radarSpin2 ; axis: Qt.vector3d(0.0, 1.0, 0.0);
                            angle: 0
                            origin: Qt.vector3d(0.55,0.0,0.0)
                            NumberAnimation on angle {
                                running: true; loops: Animation.Infinite
                                from: 360; to: 0; duration: 3000;
                            }
                        }
                    ]
                    Item3D { mesh: landerMesh; meshNode: "Rod.001" }
                    Item3D { mesh: landerMesh; meshNode: "Receiver.001" }
                    Item3D { mesh: landerMesh; meshNode: "Dish.001" }
                }
            }

            Item3D {
                // Check that you can duplicate items
                id: scalingRadarDish
                SequentialAnimation on scale {
                    loops: Animation.Infinite; running: true
                    NumberAnimation { from:1.0 ; to: 3.0; duration: 500 }
                    NumberAnimation { from:3.0 ; to: 1.0; duration: 500 }
                }
                // rotate object to remove ambiguity - scaling causes
                // movement, and if this is in the z axis, the scaling
                // could be confused with perspective.
                position: Qt.vector3d(3.0,0.0,0.0)
                transform: Rotation3D {
                    axis: Qt.vector3d(0.0,1.0,0.0)
                    angle: -90
                }
                Material {
                    // Distinctive green Color
                    id: green
                    ambientColor: "#448844"
                    diffuseColor: "#88EE44"
                    specularColor: "#88FF88"
                    shininess: 200
                }

                Material {
                    // Distinctive red Color
                    id: red
                    ambientColor: "#884422"
                    diffuseColor: "#EE4422"
                    specularColor: "#FF6644"
                    shininess: 200
                }

                Material {
                    // Distinctive blue Color
                    id: blue
                    ambientColor: "#224488"
                    diffuseColor: "#2244EE"
                    specularColor: "#4466FF"
                    shininess: 200
                }

                Item3D { mesh: landerMesh2; meshNode: "Rod.001"
                    effect: blueEffect
                }
                Item3D { id: recieverItem; mesh: landerMesh2; meshNode: "Receiver.001" ;
                    effect: blueEffect
                }
                Item3D { mesh: landerMesh2; meshNode: "Dish.001" ;
                    effect: blueEffect
                }
            }
            Item3D {
                // Check that you can duplicate items
                id: translatingRadarDish
                SequentialAnimation on x {
                    loops: Animation.Infinite
                    NumberAnimation { from: 0.0; to: 5.0 ; duration: 600 }
                    NumberAnimation { from: 5.0; to: 0.0 ; duration: 600 }
                }

                Item3D { mesh: landerMesh; meshNode: "Rod.001";
                    effect: redEffect
                    objectName: "Rod"
                }
                Item3D { mesh: landerMesh; meshNode: "Receiver.001";
                    effect: redEffect
                    objectName: "Receiver"
                }
                Item3D { mesh: landerMesh; meshNode: "Dish.001";
                    effect: redEffect
                    objectName: "Dish"
                }
            }
        }
    }
}
