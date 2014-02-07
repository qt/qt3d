/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
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
import Qt3D.Shapes 2.0

Image {
    objectName: "tronscape image"
    property real eyeSeparation: 0.0   //Eye separation: 0.1 for 3d, 0.0 for standard
    property real repeatAnimationTimer: 30;  //Auto click the 'start' button this often

    id: theBackground
    width: parent.width
    height: parent.height
    source: "meshes/tronscape.png"

    focus: true
    Keys.onSpacePressed: {
        heartbeat.restart();
        qt3dButton.bounce = true;
        console.log("Hide Calc")
        if (calc.state=="hidden") {
            calc.state="shown"

        } else {
            calc.state = "hidden"
        };
    }
    Keys.onLeftPressed: {
        console.log("Rotate Key Pressed");
        heartbeat.restart();
        room.spinning = true;
    }
    Keys.onRightPressed: {
        console.log("Box Drop Key pressed");
        heartbeat.restart();
        smallCube.fallingCubeAnimation=true;
    }
    Keys.onUpPressed: {
        console.log("Info Key Pressed");
        heartbeat.restart();
        roboInfo.showVisible=true
    }
    Keys.onPressed: {
        heartbeat.restart();
        if (event.key==Qt.Key_A) {
            navigator.spindir=1
        } else if (event.key==Qt.Key_D) {
            navigator.spindir=2
        } else if (event.key==Qt.Key_W) {
            navigator.tiltdir=1
        } else if (event.key==Qt.Key_S) {
            navigator.tiltdir=2
        } else if (event.key==Qt.Key_Plus) {
            navigator.zoomdir=1
        } else if (event.key==Qt.Key_Minus) {
            navigator.zoomdir=2
        }
    }
    Keys.onReleased: {
        heartbeat.restart();
        if (event.key==Qt.Key_A) {
            navigator.spindir=0
        } else if (event.key==Qt.Key_D) {
            navigator.spindir=0
        } else if (event.key==Qt.Key_W) {
            navigator.tiltdir=0
        } else if (event.key==Qt.Key_S) {
            navigator.tiltdir=0
        } else if (event.key==Qt.Key_Plus) {
            navigator.zoomdir=0
        } else if (event.key==Qt.Key_Minus) {
            navigator.zoomdir=0
        }
    }

    Timer {
        id: navigator;
        interval: 5;
        running: Qt.application.active
        repeat: true
        property int tiltdir: 0 //1 is forward, 2 is back
        property int spindir: 0 //1 is right, 2 is left
        property int zoomdir: 0 //1 is in, 2 is out
        onTriggered: {
            if (tiltdir==1) {roomTilt.angle=roomTilt.angle+0.5}
            else if (tiltdir==2) {roomTilt.angle=roomTilt.angle-0.5}
            if (spindir==1) {roomSpin.angle=roomSpin.angle+0.5}
            else if (spindir==2) {roomSpin.angle=roomSpin.angle-0.5}
            if (spindir==1) {roomSpin.angle=roomSpin.angle+0.5}
            else if (spindir==2) {roomSpin.angle=roomSpin.angle-0.5}
            if (zoomdir==1) {
                if (theCamera.fieldOfView>13) {
                    theCamera.fieldOfView=theCamera.fieldOfView-0.5
                }
            } else if (zoomdir==2) {
                if (theCamera.fieldOfView<80) {
                    theCamera.fieldOfView=theCamera.fieldOfView+0.5
                }
            }
        }
    }


    Timer {
        id: heartbeat;
        interval: theBackground.repeatAnimationTimer*1000;
        running: Qt.application.active
        repeat: true
        onTriggered: {
            qt3dButton.bounce = true;
            console.log("Hide Calc")
            if (calc.state=="hidden") {
                calc.state="shown"
            } else {
                calc.state = "hidden"
            }
        }
    }

        Viewport {
            objectName: "Viewport 3D"
            id: viewport
            width: parent.width
            height: parent.height
            navigation: true
            picking: true
            //showPicking: true

            Item {
                visible: false
                id: buttonPanel
                state: "panelUp"
                states: [
                    State { name: "panelDown";},
                    State { name: "panelUp";}
                ]

                transitions: [
                    Transition {
                        from: "*"
                        to: "panelDown"
                        SequentialAnimation {
                            PropertyAction{target: buttonPanel; property: "visible"; value: true}
                            PauseAnimation{duration: 10}
                            PropertyAction{target: buttonPanel; property: "visible"; value: false}
                            PauseAnimation{duration: 10}
                            PropertyAction{target: buttonPanel; property: "visible"; value: true}
                            PauseAnimation{duration: 10}
                            PropertyAction{target: buttonPanel; property: "visible"; value: true}
                            PauseAnimation{duration: 10}
                            PropertyAction{target: buttonPanel; property: "visible"; value: false}
                            PauseAnimation{duration: 10}
                            PropertyAction{target: buttonPanel; property: "visible"; value: true}
                        }
                    },
                    Transition {
                        from: "*"
                        to: "panelUp"
                        SequentialAnimation {
                            onStarted: { console.log("Panel Up"); }
                            NumberAnimation {target: buttonPanel; property: opacity; to: 0; duration: 2000;}
                            PropertyAction{target: buttonPanel; property: "visible"; value: false}
                        }
                    }
                ]

                Rectangle {
                    id: buttonBox1
                    x: viewport.width-175
                    y: 10
                    width: 160
                    height: 60
                    opacity: 0.5
                    color: "black"
                    border.color: "#2D918D"
                    border.width: 2
                    radius: 5
                }
                PushButton {
                    source: "meshes/rotateButton2.png"
                    x: buttonBox1.x+10
                    y: buttonBox1.y+10
                    onPressed: {
                        console.log("Rotate Pressed");
                        room.spinning = true;
                        heartbeat.restart();
                    }
                }
                PushButton {
                    source: "meshes/cubeButton.png"
                    x: buttonBox1.x+60
                    y: buttonBox1.y+10
                    onPressed: {
                        console.log("Box button pressed");
                        smallCube.fallingCubeAnimation=true;
                        heartbeat.restart();
                    }
                }
                PushButton {
                    source: "meshes/infobutton.png"
                    x: buttonBox1.x+110
                    y: buttonBox1.y+10
                    onPressed: {
                        console.log("Info Button Pressed");
                        roboInfo.showVisible=true
                        heartbeat.restart();
                    }
                }
            }

            Rectangle {
                id: roboInfo
                x: 5
                y: 85
                width: 150
                height: 175
                opacity: 0.0
                color: "black"
                property bool showVisible: false
                border.color: "#2D918D"
                border.width: 2
                radius: 5
                Image {
                    source: "meshes/roboInfo.png"
                }

                SequentialAnimation {
                    running: roboInfo.showVisible && Qt.application.active
                    NumberAnimation {target: roboInfo; property: "opacity"; to: 0.7; duration: 500}
                    PauseAnimation {duration: 3000}
                    NumberAnimation {target: roboInfo; property: "opacity"; to: 0.0; duration: 500}
                    onCompleted: {roboInfo.showVisible=false}
                }
            }

            Calculator {
                id: calc
            }

            Image {
                id: qt3dButton
                source: "meshes/sQt3D_logo.png"
                property bool bounce: false

                x: 10
                y: 5
                width: 110
                height: 70

                SequentialAnimation {
                    running: qt3dButton.bounce && Qt.application.active
                    NumberAnimation {target: qt3dButton; property: "scale"; duration: 100; to: 1.2;}
                    NumberAnimation {target: qt3dButton; property: "scale"; duration: 100; to: 1;}
                    onCompleted: {qt3dButton.bounce = false}
                }

                MouseArea {
                    id: qt3dActivate
                    anchors.fill: parent
                    onClicked: {
                        heartbeat.restart();
                        qt3dButton.bounce = true;
                        console.log("Hide Calc")
                        if (calc.state=="hidden") {
                            calc.state="shown"
                        } else {
                            calc.state = "hidden"
                        }
                    }
                }
            }

            camera: Camera {
                id: theCamera
                eye: Qt.vector3d(0, 2, 10)
                nearPlane:2.5
                center: Qt.vector3d(0,2,0)
                fieldOfView: 12
                eyeSeparation: theBackground.eyeSeparation
            }

            Item3D {
                id: scene

                states: [
                    State { name: "qmlView"; when: calc.visible},
                    State { name: "threedView"; when: !calc.visible }
                ]

                transitions: [
                    Transition {
                        from: "*"
                        to: "qmlView"
                        SequentialAnimation {
                            PropertyAction{target: roomSpin; property: "angle"; value: 0}
                            PropertyAction{target: roomTilt; property: "angle"; value: 0}
                            PropertyAction{target: buttonPanel; property: "visible"; value: false}
                            PropertyAction{target: theCamera; property: "eye"; value: Qt.vector3d(0, 2, 10)}
                            PropertyAction{target: theCamera; property: "fieldOfView"; value: 11.45}
                            PropertyAction{target: spinSign; property: "angle"; value: 0}
                            PropertyAction{target: robot; property: "position"; value: Qt.vector3d(0,10,4)}
                            PropertyAction{target: pitch; property: "angle"; value: -15}
                            PropertyAction{target: roll; property: "angle"; value: 0}
                            PropertyAction{target: yaw; property: "angle"; value: 0}
                            PropertyAction{target: spinSign; property: "angle"; value: 0}
                            PropertyAction{target: innerCube; property: "scale"; value: 0.95}
                            PropertyAction{target: signCube; property: "scale"; value: 2.375}
                            PropertyAction{target: headroll; property: "angle"; value: -45}
                        }
                    },
                    Transition {
                        from: "*"
                        to: "threedView"
                        SequentialAnimation {
                            onStarted: {console.log("start")}
                            SequentialAnimation {
                                NumberAnimation { target: theCamera; property: "fieldOfView"; to: 20; duration: 1000; easing.type: "InOutQuad" }
                                ParallelAnimation {
                                    NumberAnimation { target: spinSign; property: "angle"; to: 720; duration: 500; easing.type: "InOutQuad"}
                                    NumberAnimation { target: innerCube; property: "scale"; to: 1.01; duration: 500}
                                }
                            }
                            PauseAnimation {duration: 1000}
                            ParallelAnimation {
                                NumberAnimation {target: theCamera; property: "fieldOfView"; to: 40;
                                    duration: 1000; easing.type: "InOutQuad"}
                                Vector3dAnimation {target: theCamera; property: "eye"; to: Qt.vector3d(0,4,10);
                                    duration: 2500; easing.type: "InOutQuad"
                                }
                            }
                            PauseAnimation {duration: 1000}
                            NumberAnimation {target: signCube; property: "scale"; to: 0.0; duration: 2500; easing.type: "InOutQuad"}
                            Vector3dAnimation {target: robot; property: "position"; to: Qt.vector3d(0,0,4); duration: 1000; easing.type: "OutBounce"}
                            PauseAnimation {duration: 500}
                            NumberAnimation { target:pitch; property: "angle"; to: -90; duration: 2500; easing.type: "InOutElastic"}
                            NumberAnimation { target: theCamera; property: "fieldOfView"; to: 20; duration: 1000; easing.type: "InOutQuad" }
                            NumberAnimation { target: headroll; property: "angle"; to: 0; duration: 500; easing.type: "OutElastic"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 00; duration: 100; easing.type: "OutQuad"}
                            NumberAnimation { target: theCamera; property: "fieldOfView"; to: 40; duration: 1000; easing.type: "InOutQuad" }
                            PauseAnimation{duration: 1000}
                            ParallelAnimation {
                                SequentialAnimation {
                                    NumberAnimation {target: roll; property: "angle"; to: -20; duration: 500; easing.type: "InOutBack"}
                                    PauseAnimation{duration: 1000}
                                    NumberAnimation {target: roll; property: "angle"; to: 0; duration: 500; easing.type: "InOutBack"}
                                }
                                Vector3dAnimation {target: robot; property: "position"; to: Qt.vector3d(0,0,0); duration: 2000; easing.type: "InOutBack"}
                            }
                            PauseAnimation{duration: 500}
                            SequentialAnimation {
                                PropertyAction{target: buttonPanel; property: "visible"; value: true}
                                PauseAnimation{duration: 50}
                                PropertyAction{target: buttonPanel; property: "visible"; value: false}
                                PauseAnimation{duration: 40}
                                PropertyAction{target: buttonPanel; property: "visible"; value: true}
                                PauseAnimation{duration: 100}
                                PropertyAction{target: buttonPanel; property: "visible"; value: false}
                                PauseAnimation{duration: 40}
                                PropertyAction{target: buttonPanel; property: "visible"; value: true}
                                PauseAnimation{duration: 150}
                                PropertyAction{target: buttonPanel; property: "visible"; value: false}
                                PauseAnimation{duration: 40}
                                PropertyAction{target: buttonPanel; property: "visible"; value: true}
                                PauseAnimation{duration: 300}
                                PropertyAction{target: buttonPanel; property: "visible"; value: false}
                                PauseAnimation{duration: 40}
                                PropertyAction{target: buttonPanel; property: "visible"; value: true}
                            }
                        }
                    }
                ]

                effect: Effect{}

                Light {position:  Qt.vector3d(20,5,00)}


                Item3D {
                    id: room
                    property bool spinning: false
                    transform: [
                        Rotation3D{id: roomTilt; angle: 0; axis: Qt.vector3d(1,0,0)},
                        Rotation3D{id: roomSpin; angle: 0; axis: Qt.vector3d(0,1,0)}
                    ]


                    NumberAnimation {
                        running: room.spinning && Qt.application.active
                        target: roomSpin
                        property: "angle"
                        to: 360
                        duration: 3000
                        easing.type: "InOutQuad"
                        onCompleted: {
                            room.spinning=false
                            roomSpin.angle=0
                        }
                    }

                    TronCube {
                        id: smallCube
                        property bool fallingCubeAnimation: false
                        position: Qt.vector3d(0,5,0)

                        onClicked: {
                            heartbeat.restart();
                            console.log("Small cube clicked");
                            smallCube.fallingCubeAnimation=true;
                        }

                        transform: [
                            Rotation3D {id: spinSmallCube; angle: 0; axis: Qt.vector3d(0,1,0)}
                        ]

                        SequentialAnimation {
                            running: smallCube.fallingCubeAnimation && Qt.application.active
                            onCompleted: {smallCube.fallingCubeAnimation=false;}
                            ParallelAnimation {
                                NumberAnimation {target: spinSmallCube; property: "angle"; to: 360; easing.type: "InOutQuad"; duration: 1750}
                                SequentialAnimation {
                                    Vector3dAnimation {target: smallCube; property: "position"; to: Qt.vector3d(0,3.2,0); easing.type: "InQuad"; duration: 250}
                                    PropertyAction {target: robotHead; property: "z"; value: -9.0;}
                                    ParallelAnimation {
                                        NumberAnimation {target: smallCube; property: "x"; to: 2; easing.type: "Linear"; duration: 1500}
                                        SequentialAnimation {
                                            NumberAnimation {target: smallCube; property: "y"; to: 4.5; easing.type: "OutQuad"; duration: 250}
                                            NumberAnimation {target: smallCube; property: "y"; to: 0.5; easing.type: "OutBounce"; duration: 1250}
                                        }
                                    }
                                }
                            }
                            PauseAnimation {duration: 500 }
                            NumberAnimation { target: robotHead; property: "z"; to: -9.2; easing.type: "OutBounce"; duration: 250 }
                            PauseAnimation {duration: 500 }
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: -30; duration: 70; easing.type: "InOutQuad"}
                            NumberAnimation { target: headyaw; property: "angle"; to: 00; duration: 100; easing.type: "OutQuad"}
                            NumberAnimation { target: yaw; property: "angle"; to: 90; duration: 250; easing.type: "InOutQuad"}
                            NumberAnimation { target: armBend; property: "angle"; to: -45; duration: 500; easing.type: "InOutQuad"}
                            ParallelAnimation {
                                NumberAnimation { target: robot; property: "x"; to: 0.5; duration: 500; easing.type: "InQuad"}
                                NumberAnimation { target: roll; property: "angle"; to: 30; duration: 500; easing.type: "InOutQuad"}
                            }
                            ParallelAnimation {
                                NumberAnimation { target: robot; property: "x"; to: 3.5; duration: 1000; easing.type: "Linear"}
                                NumberAnimation { target: smallCube; property: "x"; to: 5; duration: 1000; easing.type: "Linear"}
                            }
                            ParallelAnimation {
                                NumberAnimation { target: smallCube; property: "x"; to: 8; duration: 1000; easing.type: "Linear"}
                                SequentialAnimation {
                                    PauseAnimation { duration: 250}
                                    NumberAnimation { target: smallCube; property: "y"; to: -5; duration: 750; easing.type: "InQuad"}
                                }
                                ParallelAnimation {
                                    SequentialAnimation {
                                        NumberAnimation {target: roll; property: "angle"; to: -10; duration: 750; easing.type: "InOutBack"}
                                        PauseAnimation{duration: 1500}
                                        NumberAnimation {target: roll; property: "angle"; to: 0; duration: 750; easing.type: "InOutBack"}
                                    }
                                    Vector3dAnimation {target: robot; property: "position"; to: Qt.vector3d(0,0,0); duration: 3000; easing.type: "InOutBack"}
                                }
                            }
                            NumberAnimation { target: yaw; property: "angle"; to: 0; duration: 250; easing.type: "InOutQuad"}
                            NumberAnimation { target: armBend; property: "angle"; to: -90; duration: 500; easing.type: "InOutQuad"}
                            PauseAnimation { duration: 1000 }
                            PropertyAction {target: smallCube; property: "position"; value: Qt.vector3d(0,10,0)}
                            NumberAnimation { target: smallCube; property: "y"; to: 5; duration: 1000; easing.type: "OutQuad"}
                        }
                    }

                    TronCube {
                        scale: 10.0
                        onClicked: {
                            heartbeat.restart();
                            console.log("Big cube clicked");
                            room.spinning = true;
                        }
                    }

                    Cube {
                        id: signCube
                        scale: 2.375
                        position: Qt.vector3d(0,2,0)
                        effect: calcEffect
                        transform: [
                            Rotation3D{id: spinSign; angle: 180; axis:Qt.vector3d(0,1,0)}
                        ]

                        Cube {
                            id: innerCube
                            scale: 0.95
                            effect: signEffect
                        }

                        Effect {
                            id: signEffect
                            color: "#aaca00"
                            texture: "meshes/QML3DSign.png"
                            decal: true
                        }

                        Effect {
                            id: calcEffect
                            color: "#aaca00"
                            texture: "meshes/calculatorSign.png"
                            decal: true
                        }
                    }

                    Robot {
                        id: robot
                        position:  Qt.vector3d(0,0,4)

                        onClicked: {
                            heartbeat.restart();
                            console.log("robot has been clicked")
                            roboInfo.showVisible=true
                        }

                        transform: [
                            Rotation3D {id: correct; angle: 90; axis: Qt.vector3d(0,1,0)},
                            Rotation3D {id: pitch; angle: -90; axis: Qt.vector3d(0,0,1)},
                            Rotation3D {id: roll; angle: 0; axis: Qt.vector3d(1,0,0)},
                            Rotation3D {id: yaw; angle: 0; axis: Qt.vector3d(0,1,0)}
                        ]

                        Item3D {
                            id: robotArms
                            mesh: Mesh { source: "meshes/roboticArms.3ds"}
                            transform: Rotation3D{id: armBend; angle: -90;axis: Qt.vector3d(0,1,0)}
                            position: Qt.vector3d(.18, 0,-5.4)
                        }

                        Item3D {
                            id: robotHead
                            mesh: Mesh { source: "meshes/roboticHead.3ds" }
                            position: Qt.vector3d(0,0,-9.1)
                            transform: [
                                Rotation3D {id: headyaw; angle: 0; axis: Qt.vector3d(0,0,1)},
                                Rotation3D {id: headroll; angle: -45; axis: Qt.vector3d(1,0,0)},
                                Rotation3D {id: headpitch; angle: 0; axis: Qt.vector3d(0,1,0)}
                            ]
                        }
                    }
                }
            }
        }
}
