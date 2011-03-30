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

Viewport {
    width: 640; height: 480

    property string name: "Animation Shader"
    property string icon: "images/teapot-logo.png"
    property bool run: false

    Item3D {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: program

        transform: Rotation3D {
            NumberAnimation on angle{
                running: run
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
            SequentialAnimation on qt_Custom {
                running: run
                loops: Animation.Infinite
                PauseAnimation { duration: 700 }
                NumberAnimation { from: 1.0; to: 0.001; duration: 1500; easing.type:"OutBounce" }
                PauseAnimation { duration: 700 }
                NumberAnimation { from: 0.001; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            }

            texture: "images/qtlogo.png"

            SequentialAnimation on color {
                running: run
                loops: Animation.Infinite
                ColorAnimation {
                    from: "#00008a"
                    to: "#003333"
                    duration: 5000
                }
                ColorAnimation {
                    from: "#003333"
                    to: "#00008a"
                    duration: 5000
                }
            }

            vertexShader: "
                attribute highp vec4 qt_Vertex;
                attribute highp vec4 qt_MultiTexCoord0;
                uniform mediump mat4 qt_ModelViewProjectionMatrix;
                varying highp vec4 texCoord;
                uniform highp float qt_Custom;

                void main(void)
                {
                    gl_Position = qt_ModelViewProjectionMatrix * (qt_Vertex *
                    vec4(1.0, qt_Custom, 1.0, 1.0));
                    texCoord =  -qt_MultiTexCoord0 + vec4(qt_Custom, 0.0, 0.0, 0.0);

                }
                "
            fragmentShader: "
                varying highp vec4 texCoord;
                uniform sampler2D qt_Texture0;
                uniform mediump vec4 qt_Color;

                void main(void)
                {
                    mediump vec4 col = texture2D(qt_Texture0, texCoord.st);
                    gl_FragColor = vec4(clamp(qt_Color.rgb * (1.0 - col.a) +
                    col.rgb, 0.0, 1.0), 1.0);
                }
                "
        }
    }
}
