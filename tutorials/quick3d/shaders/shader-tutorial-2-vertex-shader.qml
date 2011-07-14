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

Viewport {
    width: 640; height: 480

    TutorialTeapot {
        id: teapot
        effect: program
        xRotation: 30
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

        attribute highp vec4 qt_MultiTexCoord0;
        varying highp vec4 texCoord;
        uniform mediump float textureOffsetX;
        uniform mediump float squashFactor;

        void main(void)
        {
            const float modelSize = 2.0;
            const float modelBottom = -1.0;
            float newY = max(qt_Vertex.y - squashFactor * modelSize,
                               qt_Vertex.y * 0.01 + modelBottom);
            gl_Position = qt_ModelViewProjectionMatrix *
                                  vec4(qt_Vertex.x, newY, qt_Vertex.zw);

            texCoord = vec4(-qt_MultiTexCoord0.s
                            - textureOffsetX,
                            -qt_MultiTexCoord0.t,
                            qt_MultiTexCoord0.pq);
        }
        "
        fragmentShader: "
        varying highp vec4 texCoord;
        uniform sampler2D qt_Texture0;

        void main(void)
        {
            mediump vec4 textureColor = texture2D(qt_Texture0, texCoord.st);
            gl_FragColor = textureColor;
        }
        "
    }
}
