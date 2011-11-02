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

import QtQuick 1.0
import Qt3D 1.0

Rectangle {
    id: mainwindow
    width: 1024
    height: 768
    color: "#444444"
    property string targetMesh: "meshes/monkey.3ds";

    Item {
        width: parent.width; height: parent.height

        Viewport {
            anchors.fill: parent
            id: viewport
            picking: false
            blending: true

            camera: Camera {
                farPlane: 2000 // debugging
            }
            navigation: false

            Item3D {
                id: mainItem
                mesh: source_mesh
                effect: Effect {}
                transform: Rotation3D {
                    id: transformRotate
                    angle: 45
                    axis: Qt.vector3d(1, 0, 0)
                }
            }

            Mesh {
                id: source_mesh
                source: mainwindow.targetMesh
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 10
            anchors.bottom: parent.bottom
            radius: 10
            border.width: 1
            border.color: "black"
            color: "white"
            width: parent.width - 20
            height: 20

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: source_mesh.source
            }
        }
    }

    Column {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 32
        anchors.topMargin: 32
        width: 150
        height: 200

        Text {
            text: "Position";
            color: "#FFFFFF"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        ValueField {
            focus: true
            id: posX
            label: "X:"
            value: mainItem.position.x
            function update (f)  { mainItem.position.x = f; }
            function updateInc (f)  { mainItem.position.x += f; }
            Keys.onTabPressed:   { updateMe(); posY.focus = true; }
        }
        ValueField {
            id: posY
            label: "Y:"
            value: mainItem.position.y
            function update (f)  { mainItem.position.y = f; }
            function updateInc (f)  { mainItem.position.y += f; }
            Keys.onTabPressed:   { updateMe(); posZ.focus = true; }
        }
        ValueField {
            id: posZ
            label: "Z:"
            value: mainItem.position.z
            function update (f)  { mainItem.position.z = f; }
            function updateInc (f)  { mainItem.position.z += f; }
            Keys.onTabPressed:   { updateMe(); scale.focus = true; }
        }
    }
}
