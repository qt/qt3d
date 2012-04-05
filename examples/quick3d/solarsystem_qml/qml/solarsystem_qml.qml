/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D examples of the Qt Toolkit.
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
//! [1]
import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Viewport {
    width: 800; height: 600

    //! [0]
    SphereMesh {
        id: planetoid
        levelOfDetail: 6
        axis: Qt.YAxis
    }
    //! [0]

    //! [1]
    Item3D {
        id: sun
        Item3D {
            transform: [ Scale3D { scale: "0.3, 0.3, 0.3" } ]
            mesh: planetoid
        }
        effect: Effect { texture: "solar.jpg" }
        transform: [
            Rotation3D { id: spin; axis: "0,1,0"; angle: 0 }
        ]
        NumberAnimation { target: spin; property: "angle"; to: 360; duration: 10000; running: true; loops: Animation.Infinite }
    }
    //! [1]
    Item3D {
        id: earth_system
        transform: [
            Translation3D { id: earth_orbit_distance; translate: "2,0,0" },
            Rotation3D { id: earth_orbit; axis: "0,1,0"; angle: 0 },
            Rotation3D { id: orbit_tilt; axis: "0,0,1"; angle: 20 }
        ]
        NumberAnimation { target: earth_orbit; property: "angle"; to: 360; duration: 20000;
            running: true; loops: Animation.Infinite }
        Item3D {
            Item3D {
                id: earth
                Item3D {
                    transform: [ Scale3D { scale: "0.15, 0.15, 0.15" } ]
                    mesh: planetoid
                }
                transform: [
                    Rotation3D { id: earth_spin; axis: "0,1,0"; angle: 0 }
                ]
                NumberAnimation { target: earth_spin; property: "angle"; to: 360; duration: 2000;
                    running: true; loops: Animation.Infinite }
                effect: Effect { texture: "planet.jpg" }
            }
            Item3D {
                id: moon
                Item3D {
                    transform: [ Scale3D { scale: "0.09, 0.09, 0.09" } ]
                    mesh: planetoid
                }
                transform: [
                    Translation3D { id: moon_orbit_distance; translate: "0.5,0,0" },
                    Rotation3D { id: moon_orbit; angle: 0; axis: "0,1,0" },
                    Rotation3D { id: moon_orbit_tilt; angle: 90; axis: "1,0,0" }
                ]
                NumberAnimation { target: moon_orbit; property: "angle"; to: 360; duration: 8000;
                    running: true; loops: Animation.Infinite }
                effect: Effect { texture: "moon-texture.jpg" }
            }
        }
    }
    Rectangle {
        id: quit
        width: parent.width
        height: parent.height / 8
        color: "blue"
        opacity: 0.5
        Text {
            id: quit_button
            anchors.centerIn: parent
            text: qsTr("Quit")
            font.bold: true
            font.pointSize: 24
            color: "white"
        }
        MouseArea {
            onClicked: Qt.quit()
            anchors.fill: parent
        }
    }
}
