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
    property string name: "Interpolate Shader"
    property string icon: "teapot-logo.png"

    width: 640; height: 480

    Item3D {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: program

        transform: Rotation3D {
            NumberAnimation on angle{
                running: Qt.application.active
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 5000
            }
            axis: Qt.vector3d(1, -0.3, 0)
        }

        ShaderProgram {
            id: program
            texture: "basket.jpg"
            property variant texture2 : "qtlogo.png"
            property real interpolationFactor : 0.0

            SequentialAnimation on interpolationFactor {
                running: Qt.application.active
                loops: Animation.Infinite
                NumberAnimation { to : 1.0; duration: 750; }
                PauseAnimation { duration: 550 }
                NumberAnimation { to : 0.0; duration: 750; }
                PauseAnimation { duration: 550 }
            }

            SequentialAnimation on color{
                running: Qt.application.active
                loops: Animation.Infinite
                ColorAnimation {
                    from: "#aaca00"
                    to: "#0033ca"
                    duration: 500
                }
                ColorAnimation {
                    from: "#0033ca"
                    to: "#aaca00"
                    duration: 500
                }
            }

            vertexShader: "
                attribute highp vec4 qt_Vertex;
                attribute highp vec4 qt_MultiTexCoord0;
                uniform mediump mat4 qt_ModelViewProjectionMatrix;
                varying highp vec4 texCoord;

                void main(void)
                {
                    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
                    texCoord = qt_MultiTexCoord0;
                }
                "

            fragmentShader: "
                varying highp vec4 texCoord;
                uniform sampler2D qt_Texture0;
                uniform sampler2D texture2;
                uniform mediump vec4 qt_Color;
                uniform mediump float interpolationFactor;

                void main(void)
                {
                    mediump vec4 col1 = texture2D(qt_Texture0, texCoord.st);
                    mediump vec4 col2 = texture2D(texture2, texCoord.st);
                    gl_FragColor = mix(col1, col2, interpolationFactor);
                }
                "
        }
    }
}
