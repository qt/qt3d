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

Viewport {
    width: parent.width;
    height: parent.height
    fillColor: "darkblue"
    objectName: "anim viewport"

    Camera {
        id: ccc
        eye: Qt.vector3d(0, 0, 200)
    }
    camera: ccc

    Effect {
        id: eee
        texture: "stonewal.jpg"
    }

    Mesh {
        id: test_mesh
        source: "cube_rotated.dae"
    }

    Item3D {
        id: test_item
        effect: eee
        mesh: test_mesh
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 5
        width: 200
        height: 150
        radius: 5
        Text {
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            text: test_item.animations.length + ( (test_item.animations.length==1)?" animation:":" animations:" )
        }
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 23
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.bottomMargin: 5
            border.color: "black"
            Item {
                anchors.fill: parent
                anchors.topMargin: 2
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.bottomMargin: 2
                ListView {
                    width: parent.width
                    height: parent.height
                    model: test_item.animations
                    clip: true
                    delegate: Item {
                        width: parent.width
                        height: 25
                        clip: true
                        Rectangle {
                            anchors.fill: parent
                            anchors.topMargin: 1
                            anchors.leftMargin: 1
                            anchors.rightMargin: 1
                            anchors.bottomMargin: 1
                            border.color: "black"
                            color: "lightgreen"
                            Text {
                                anchors.top: parent.top
                                anchors.topMargin: 1
                                anchors.left: parent.left
                                anchors.leftMargin: 1
                                text: name
                                font.pixelSize:18
                            }
                        }
                    }
                }
            }
        }
    }
}
