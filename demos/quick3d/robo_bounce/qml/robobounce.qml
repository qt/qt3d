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
import Qt3D.Shapes 1.0
import "Calculator"
import "RobotCore"

Image {
    property real eyeSeparation: 0.0   //Eye separation: 0.1 for 3d, 0.0 for standard
    property real repeatAnimationTimer: 30;  //Auto click the 'start' button this often

    id: theBackground
    width: 640
    height: 480
    source: "Assets/tronscape.png"

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
        running: true
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
        running: true
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

    StereoView {
        id: stereo
        width: 640; height: 480
        //layout: StereoView.LeftRight

        Viewport {
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
                    source: "Assets/rotateButton2.png"
                    x: buttonBox1.x+10
                    y: buttonBox1.y+10
                    onPressed: {
                        console.log("Rotate Pressed");
                        room.spinning = true;
                        heartbeat.restart();
                    }
                }
                PushButton {
                    source: "Assets/cubeButton.png"
                    x: buttonBox1.x+60
                    y: buttonBox1.y+10
                    onPressed: {
                        console.log("Box button pressed");
                        smallCube.fallingCubeAnimation=true;
                        heartbeat.restart();
                    }
                }
                PushButton {
                    source: "Assets/infobutton.png"
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
                    source: "Assets/roboInfo.png"
                }

                SequentialAnimation {
                    running: roboInfo.showVisible
                    loops: Animation.infinite
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
                source: "Assets/sQt3D_logo.png"
                property bool bounce: false

                x: 10
                y: 5
                width: 110
                height: 70

                SequentialAnimation {
                    running: qt3dButton.bounce
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
                            PropertyAction{target: pitch; property: "angle"; value: 0}
                            PropertyAction{target: roll; property: "angle"; value: 75}
                            PropertyAction{target: yaw; property: "angle"; value: 90}
                            PropertyAction{target: spinSign; property: "angle"; value: 0}
                            PropertyAction{target: innerCube; property: "scale"; value: 0.95}
                            PropertyAction{target: signCube; property: "scale"; value: 2.375}
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
                            NumberAnimation { target:roll; property: "angle"; to: 0; duration: 2500; easing.type: "InOutElastic"}
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
                                    NumberAnimation {target: pitch; property: "angle"; to: -10; duration: 500; easing.type: "InOutBack"}
                                    PauseAnimation{duration: 1000}
                                    NumberAnimation {target: pitch; property: "angle"; to: 0; duration: 500; easing.type: "InOutBack"}
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
                        running: room.spinning
                        loops: Animation.infinite
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
                            running: smallCube.fallingCubeAnimation
                            loops: Animation.infinite
                            onCompleted: {smallCube.fallingCubeAnimation=false;}
                            ParallelAnimation {
                                NumberAnimation {target: spinSmallCube; property: "angle"; to: 360; easing.type: "InOutQuad"; duration: 1750}
                                SequentialAnimation {
                                    Vector3dAnimation {target: smallCube; property: "position"; to: Qt.vector3d(0,3.2,0); easing.type: "InQuad"; duration: 250}
                                    PropertyAction {target: robotHead; property: "y"; value: 9.00;}
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
                            NumberAnimation { target: robotHead; property: "y"; to: 9.2; easing.type: "OutBounce"; duration: 250 }
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
                            NumberAnimation { target: yaw; property: "angle"; to: 180; duration: 250; easing.type: "InOutQuad"}
                            NumberAnimation { target: armBend; property: "angle"; to: -45; duration: 500; easing.type: "InOutQuad"}
                            ParallelAnimation {
                                NumberAnimation { target: robot; property: "x"; to: 0.5; duration: 500; easing.type: "InQuad"}
                                NumberAnimation { target: pitch; property: "angle"; to: 30; duration: 500; easing.type: "InOutQuad"}
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
                                        NumberAnimation {target: pitch; property: "angle"; to: -10; duration: 750; easing.type: "InOutBack"}
                                        PauseAnimation{duration: 1500}
                                        NumberAnimation {target: pitch; property: "angle"; to: 0; duration: 750; easing.type: "InOutBack"}
                                    }
                                    Vector3dAnimation {target: robot; property: "position"; to: Qt.vector3d(0,0,0); duration: 3000; easing.type: "InOutBack"}
                                }
                            }
                            NumberAnimation { target: yaw; property: "angle"; to: 90; duration: 250; easing.type: "InOutQuad"}
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
                            texture: "Assets/QML3DSign.png"
                            decal: true
                        }

                        Effect {
                            id: calcEffect
                            color: "#aaca00"
                            texture: "Assets/calculatorSign.png"
                            decal: true
                        }
                    }

                    Robot {
                        id: robot
                        position:  Qt.vector3d(0,10,4)

                        onClicked: {
                            heartbeat.restart();
                            console.log("robot has been clicked")
                            roboInfo.showVisible=true
                        }

                        transform: [
                            Rotation3D {id: pitch; angle: 0; axis: Qt.vector3d(0,0,1)},
                            Rotation3D {id: roll; angle: 75; axis: Qt.vector3d(1,0,0)},
                            Rotation3D {id: yaw; angle: 90; axis: Qt.vector3d(0,1,0)}
                        ]

                        Item3D {
                            id: robotArms
                            mesh: Mesh { source: "Assets/roboticArms.3ds"}
                            transform: Rotation3D{id: armBend; angle: -90;axis: Qt.vector3d(0,0,1)}
                            position: Qt.vector3d(.18, 5.4,0)

                            SequentialAnimation {
                                running: false
                                loops: 100
                                NumberAnimation {target: armBend; property: "angle"; to: 0.0; duration: 1000; easing.type: "OutQuad" }
                                PauseAnimation {duration: 1000}
                                NumberAnimation {target: armBend; property: "angle"; to: -90.0; duration: 1000; easing.type: "OutQuad" }
                            }
                        }

                        Item3D {
                            id: robotHead
                            mesh: Mesh { source: "Assets/roboticHead.3ds" }
                            position: Qt.vector3d(0,9.1,0)
                            transform: [
                                Rotation3D {id: headpitch; angle: 0; axis: Qt.vector3d(0,0,1)},
                                Rotation3D {id: headroll; angle: 45; axis: Qt.vector3d(1,0,0)},
                                Rotation3D {id: headyaw; angle: 0; axis: Qt.vector3d(0,1,0)}
                            ]
                        }
                    }
                }
            }
        }
    }
}
