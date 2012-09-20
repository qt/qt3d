/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0

//! [1]
Viewport  {
    width: parent.width
    height: parent.height
    fillColor: "black"

    camera: Camera { eye: Qt.vector3d(21,7,19)}
//! [1]

//! [2]
    Item3D {
        transform: LookAt { subject: focalPenguin}
        position: Qt.vector3d(0, 1.7, -6)

        Item3D {
            id: monkey
            scale: 2
            mesh: Mesh { source: "meshes/monkey.3ds"; options: "ForceSmooth"}
            effect: Effect { material: gold}
            transform: Rotation3D {angle: 90; axis: Qt.vector3d(1,0,0);}
        }
    }
//! [2]

//! [3]
    Item3D
    {
        Penguin {position: Qt.vector3d(-1.5,0,1)}
        Penguin {position: Qt.vector3d(1.5,0,1)}
        Penguin {id: focalPenguin; position: Qt.vector3d(-0.5,0,2)}
        Penguin {position: Qt.vector3d(0.5,0,2)}

        transform: [
            Rotation3D {id: swivel1; angle: -20; axis: Qt.vector3d(0,1,0)}
        ]

        SequentialAnimation {
            running:  Qt.application.active
            loops: Animation.Infinite
            NumberAnimation {target: swivel1; property: "angle"; to: 20; duration: 1200; easing.type: "OutQuad"}
            NumberAnimation {target: swivel1; property: "angle"; to: -20; duration: 1200; easing.type: "OutQuad"}
        }
    }
//! [3]

    Cube {
        scale: 12.0
        y: -6.5
        z: -2

        effect: Effect {
            color: "#aaca00"
            texture: "marble.png"
            decal: true
        }
    }

    Item3D
    {
        Penguin {position: Qt.vector3d(-2.5,0,2)}
        Penguin {position: Qt.vector3d(2.5,0,2)}
        Penguin {position: Qt.vector3d(-1.5,0,3)}
        Penguin {position: Qt.vector3d(1.5,0,3)}
        Penguin {position: Qt.vector3d(-0.5,0,3.5)}
        Penguin {position: Qt.vector3d(0.5,0,3.5)}

        transform: [
            Rotation3D {id: swivel2; angle: 20; axis: Qt.vector3d(0,1,0)}
        ]

        SequentialAnimation {
            running: Qt.application.active
            loops: Animation.Infinite
            NumberAnimation {target: swivel2; property: "angle"; to: -20; duration: 1200; easing.type: "OutQuad"}
            NumberAnimation {target: swivel2; property: "angle"; to: 20; duration: 1200; easing.type: "OutQuad"}
        }
    }

    Material {
        id: gold
        ambientColor: "#EEDD00"
        specularColor: "#FFFFFF"
        shininess: 200
    }
}
