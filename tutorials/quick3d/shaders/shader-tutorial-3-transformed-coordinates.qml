/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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


    Item3D {
        z: -8.0
        transform: [
            Rotation3D {
                NumberAnimation on angle {
                    running: true; loops: Animation.Infinite
                    from: 0; to: 360; duration: 5000
                }
                axis: Qt.vector3d(0, 0, 1.0)
            }
        ]

        TutorialTeapot {id: teapot1; effect: program; y:2.0; x:0.0}
        TutorialTeapot {id: teapot2; effect: program; y:-1.0; x:-1.732}
        TutorialTeapot {id: teapot3; effect: program; y:-1.0; x:1.732}
    }

    ShaderProgram {
        id: program
        texture: "textures/qtlogo.png"

        property real textureOffsetX : 0.0
        property real squashFactor : 0.0
        NumberAnimation on textureOffsetX
        {
            running: true; loops: Animation.Infinite
            from: 0.0; to: 1.0;
            duration: 1000
        }

        SequentialAnimation on squashFactor
        {
            running: true; loops: Animation.Infinite
            NumberAnimation {
                from: 0.0; to: 1.0; duration: 1000
                easing.type: "InQuad"
            }
            PauseAnimation {
                duration: 250
            }
            NumberAnimation {
                from: 1.0; to: 0.0; duration: 1000
                easing.type: "OutQuad"
            }
        }


        vertexShader: "
        attribute highp vec4 qt_Vertex;
        uniform mediump mat4 qt_ModelViewProjectionMatrix;

        attribute mediump vec4 qt_MultiTexCoord0;
        varying mediump vec4 texCoord;

        void main(void)
        {
            const float modelBottom = -4.0;

            vec4 workingPosition = qt_ModelViewProjectionMatrix * qt_Vertex;
            float newY = max(workingPosition.y,
                               workingPosition.y * 0.15 + modelBottom);
            workingPosition.y = newY;
            gl_Position = workingPosition;

            texCoord = -qt_MultiTexCoord0;
        }
        "
        fragmentShader: "
        varying mediump vec4 texCoord;
        uniform sampler2D qt_Texture0;

        void main(void)
        {
            mediump vec4 textureColor = texture2D(qt_Texture0, texCoord.st);
            gl_FragColor = textureColor;
        }
        "
    }
}
