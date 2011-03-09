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

//![1]
import Qt 4.7
import Qt3D 1.0
import Qt3D.Shapes 1.0

//![3]
StereoView {
    id: stereo
//![3]
    width: 640; height: 450
//![1]

/*
//![4]
    width: 1280; height: 450
    layout: StereoView.LeftRight
//![4]
*/

//![5]
    FloatingItem {
        anchors.fill: parent
        depth: -10
        Image {
            anchors.fill: parent
            source: "../textures/photos/niagara_falls.jpg"
        }
    }
//![5]

//![6]
    Viewport {
        anchors.fill: parent
        navigation: false

        camera: Camera {
            eye: Qt.vector3d(0, 0, 10)
            eyeSeparation: 0.08
        }
//![6]

//![8]
        Teapot {
            id: teapot
            position: Qt.vector3d(-1.0, -1.0, -5.0)
//![8]

            SequentialAnimation on y {
                running: true
                loops: Animation.Infinite
                PauseAnimation { duration: 500 }
                NumberAnimation { to : 1.0; duration: 300; easing.type:"OutQuad" }
                NumberAnimation { to : -1.0; duration: 300; easing.type:"OutBounce" }
            }
//![9]
        }

        Cube {
            id: cube
            position: Qt.vector3d(1.0, -0.5, 0.0)
//![9]

            transform: Rotation3D {
                axis: Qt.vector3d(1, 1, 1)

                NumberAnimation on angle {
                    running: true
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 5000
                }
            }

            effect: Effect {
                color: "#aaca00"
                texture: "../textures/qtlogo.png"
                decal: true
            }
//![10]
        }
//![10]
//![7]
    }
//![7]

//![11]
    Time { x: 5; y: 5 }
//![11]

//![12]
    FloatingItem {
        id: button
        x: 250; y: 50
        width: 300; height: 80
        depth: 4

        Rectangle {
            anchors.fill: parent
            color: "#90e020"
            opacity: 0.7
            radius: 5
        }

        Text {
            anchors.centerIn: parent
            color: "black"
            font.pointSize: 14
            text: "Drag Me!"
        }

        MouseArea {
            anchors.fill: parent
            drag.target: button
            drag.axis: Drag.XandYAxis
            drag.minimumX: 0
            drag.maximumX: stereo.viewport.width - parent.width
            drag.minimumY: 0
            drag.maximumY: stereo.viewport.height - parent.height
        }
    }
//![12]
//![2]
}
//![2]
