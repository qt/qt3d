/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
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

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.2 as QQ2

Material {
    id: material

    property color ambientColor: "#cc2200";
    property color diffuseColor: "pink"
    property bool enabled: true
    property Texture2D texture;

    QQ2.Timer {
        id: testTimer
        interval: 5000
        repeat: true
        running: true
        property bool even: true
        onTriggered: {
            console.log("Triggered");
            even = !even
            pointLightBlockShaderData.u.values[0].t = (even) ? null : shaderDataT
        }
    }

    ShaderData {
        id: shaderDataT
        property real a: testTimer.even ? 1.0 : 0.0
        property real b: 5.0
        property var r: [Qt.vector3d(1, 1, 1), Qt.vector3d(2, 2, 2), Qt.vector3d(3, 3, 3), Qt.vector3d(4, 4, 4)]
    }

    parameters: [
        Parameter { name: "ambient"; value: Qt.vector3d(material.ambientColor.r, material.ambientColor.g, material.ambientColor.b) },
        Parameter { name: "lightIntensity"; value: Qt.vector3d(0.5, 0.5, 0.5)},
        Parameter { name: "texture"; value: texture},
        Parameter { name: "PointLightBlock"; value: ShaderData {
                id: pointLightBlockShaderData
                property color colorAmbient;
                property color colorDiffuse;
                property color colorSpecular;
                property real shininess: 1.0;
                property vector3d position: Qt.vector3d(1.0, 1.0, 0.0)
                property vector3d intensity: Qt.vector3d(0.7, 0.8, 0.6);

                property ShaderData s: ShaderData {
                    property real innerV: 3.0
                    property var innerVec3Array: [Qt.vector3d(1, 1, 1), Qt.vector3d(2, 2, 2), Qt.vector3d(3, 3, 3), Qt.vector3d(4, 4, 4)]
                }

                property ShaderDataArray u: ShaderDataArray {
                    id: array
                    ShaderData {
                        property real innerV: 2.0
                        property vector3d innerVec3
                        property var innerVec3Array: [Qt.vector3d(1, 1, 1), Qt.vector3d(2, 2, 2), Qt.vector3d(3, 3, 3), Qt.vector3d(4, 4, 4)]
                        property ShaderData t: shaderDataT
                            property ShaderDataArray c: ShaderDataArray {
                                ShaderData {
                                    property real a: 3.0
                                    property real b: 4.0
                                    property var r: [Qt.vector3d(1, 1, 1), Qt.vector3d(2, 2, 2), Qt.vector3d(3, 3, 3), Qt.vector3d(4, 4, 4)]
                                }
                                ShaderData {
                                    property real a: 2.0
                                    property real b: 3.0
                                    property var r: [Qt.vector3d(2, 2, 2), Qt.vector3d(1, 1, 1), Qt.vector3d(4, 4, 4), Qt.vector3d(3, 3, 3)]
                                }
                            }
                    }
                    ShaderData {
                        property real innerV: 3.2
                        property var innerVec3Array: [Qt.vector3d(1, 0, 1), Qt.vector3d(0, 2, 2), Qt.vector3d(3, 0, 3), Qt.vector3d(4, 0, 4)]
                        property ShaderData t: ShaderData {
                            property real a: 1.0
                            property real b: 5.0
                            property var r: [Qt.vector3d(1, 1, 1), Qt.vector3d(2, 2, 2), Qt.vector3d(3, 3, 3), Qt.vector3d(4, 4, 4)]
                        }
                    }
                    ShaderData {
                        property real innerV: 0.2
                        property var innerVec3Array: [Qt.vector3d(1, 0, 1), Qt.vector3d(0, 2, 2), Qt.vector3d(3, 0, 3), Qt.vector3d(4, 0, 4)]
                    }
                }

                QQ2.ColorAnimation on colorAmbient { from: "blue"; to: "yellow"; duration: 1000; loops: QQ2.Animation.Infinite }
                QQ2.ColorAnimation on colorDiffuse { from: "red"; to: "green"; duration: 1000; loops: QQ2.Animation.Infinite }
                QQ2.ColorAnimation on colorSpecular { from: "white"; to: "orange"; duration: 1000; loops: QQ2.Animation.Infinite }
                QQ2.NumberAnimation on shininess { from: 0; to: 1.0; duration: 1000; loops: QQ2.Animation.Infinite }

            } }
    ]

    effect : Effect {
        parameters : Parameter { name: "diffuse"; value: Qt.vector3d(material.diffuseColor.r, material.diffuseColor.g, material.diffuseColor.b)}

        techniques : [
            // OpenGL 3.1 Technique
            Technique {
                graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 1; majorVersion : 3 }

                annotations: [
                    Annotation { name : "RenderingStyle"; value : "forward"},
                    Annotation { name : "Enabled"; value: enabled }
                ]

                parameters : Parameter { name : "lightPos"; value : Qt.vector4d(10.0, 10.0, 0.0, 1.0)}

                renderPasses : [
                    // COLOR PASS
                    RenderPass {
                        annotations: Annotation {name: "Name"; value: "ColorMaterial"}

                        shaderProgram: ShaderProgram {
                            id: diffuseShader
                            vertexShaderCode: loadSource("qrc:/shaders/diffuse.vert")
                            fragmentShaderCode: loadSource("qrc:/shaders/diffuse.frag")
                        }
                    },
                    // TEXTURE PASS + UBO
                    RenderPass {
                        annotations : [Annotation {name : "Name"; value : "Texture" }]

                        renderStates : [BlendEquationArguments {sourceRgb: BlendEquationArguments.One; destinationRgb : BlendEquationArguments.One},
                                        BlendEquation {blendFunction: BlendEquation.Add},

                            CullFace { mode : CullFace.Back },
                            DepthTest { depthFunction : DepthTest.LessOrEqual}
                        ]
                        shaderProgram : ShaderProgram {
                            vertexShaderCode: "
                            #version 140
                            in vec4 vertexPosition;
                            in vec3 vertexNormal;
                            in vec2 vertexTexCoord;

                            out vec3 worldPosition;
                            out vec3 normal;
                            out vec2 texCoord;

                            uniform mat4 modelViewProjection;
                            uniform mat4 modelView;
                            uniform mat3 modelViewNormal;

                            void main()
                            {
                                texCoord = vertexTexCoord;
                                worldPosition = vec3(modelView * vertexPosition);
                                normal = normalize(modelViewNormal * vertexNormal);
                                gl_Position = modelViewProjection * vertexPosition;
                            }
                            "

                            fragmentShaderCode: "
                            #version 140
                            in vec3 worldPosition;
                            in vec3 normal;
                            in vec2 texCoord;
                            out vec4 fragColor;

                            struct subStruct
                            {
                                float a;
                                float b;
                                float r[4];
                            };

                            struct innerStruct
                            {
                                float innerV;
                                vec3  innerVec3;
                                vec3  innerVec3Array[4];
                                subStruct t;
                                subStruct c[2];
                            };

                            uniform PointLightBlock
                            {
                                vec4 colorAmbient;
                                vec4 colorDiffuse;
                                vec4 colorSpecular;
                                float shininess;
                                vec3 position;
                                vec3 intensity;
                                innerStruct s;
                                innerStruct u[4];
                            } lightSource;

                            uniform sampler2D tex;

                            void main()
                            {
                                vec3 n = normalize(normal);
                                vec3 s = normalize(lightSource.position - worldPosition);
                                vec3 v = normalize(-worldPosition);
                                vec3 r = reflect(-s, n);

                                float diffuse = max(dot(s, n), 0.0);
                                float specular = step(diffuse, 0.0) * pow(max(dot(r, v), 0.0), lightSource.shininess);

                                vec3 sum = vec3(0.0, 0.0, 0.0);

                                for (int i = 0; i < 4; ++i) {
                                    sum += lightSource.s.innerVec3Array[i];
                                }

                                float tmp = 0;

                                for (int i = 0; i < 4; ++i) {
                                    tmp += lightSource.u[i].innerV;
                                }

                                if (sum == vec3(10.0, 10.0, 10.0))
                                    fragColor = vec4(lightSource.intensity, 1.0) * (
                                                    lightSource.colorAmbient * lightSource.s.innerV +
                                                    lightSource.colorDiffuse * diffuse +
                                                    lightSource.colorSpecular * specular) * 0.2 + texture(tex, texCoord);
                                else
                                    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
                            }"
                        }
                    }
                ]
            }
        ]
    }
}

