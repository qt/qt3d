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

import QtQuick 1.0
import Qt3D 1.0

Viewport {
    width: 640; height: 480
    id: viewPort

    MouseArea {
        anchors.fill: parent
        onClicked: program.scalex = 2.5 - program.scalex
    }

    Item3D {
        mesh: Mesh { source: "teapot.bez" }
        effect: program


        transform: Rotation3D {
            NumberAnimation on angle{
                running: true
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 18000
            }
            axis: Qt.vector3d(1, -0.3, 0)
        }

        ShaderProgram {
            id: program
            property real qt_Custom : 1.0
            property int customInt : 1
            property color customColor : "#3333ff"
            property real scalex : 1.0
            property real scaley : 1.0
            property real scalez : 1.0
            property real translationX : 0.0
            property real translationY : 0.0
            property real translationZ : 0.0

            property variant matrix4x4 :[
                scalex , 0.0, 0.0, translationX,
                0.0, scaley, 0.0, translationY,
                0.0, 0.0, scalez, translationZ,
                0.0, 0.0, 0.0, 1.0 ]

            SequentialAnimation on translationY {
                running: true
                loops: Animation.Infinite
                PauseAnimation { duration: 2000 }
                NumberAnimation { from: 1.0; to: 0.001; duration: 1500; easing.type:"OutBounce" }
                PauseAnimation { duration: 2000 }
                NumberAnimation { from: 0.001; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            }

            SequentialAnimation on customInt {
                running: true
                loops: Animation.Infinite
                PauseAnimation { duration: 2500 }
                NumberAnimation { from: 1.0; to: 0.001; duration: 1500; easing.type:"OutBounce" }
                PauseAnimation { duration: 2500 }
                NumberAnimation { from: 0.001; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            }

            texture: "qtlogo.png"

            vertexShaderSource: "matrix_animation.vsh"
            fragmentShaderSource: "matrix_animation.fsh"
        }
    }

}
