/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Renderer 2.0
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
                openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 1; majorVersion : 3 }

                annotations: [
                    Annotation { name : "RenderingStyle"; value : "forward"},
                    Annotation { name : "Enabled"; value: enabled }
                ]

                parameters : Parameter { name : "lightPos"; value : Qt.vector4d(10.0, 10.0, 0.0, 1.0)}

                renderPasses : [
                    // COLOR PASS
                    RenderPass {
                        annotations: Annotation {name: "Name"; value: "ColorMaterial"}

                        bindings: [ // Add only the bindings needed for a shader
                            ParameterMapping {parameterName: "ambient"; shaderVariableName: "ka"; bindingType: ParameterMapping.Uniform},
                            ParameterMapping {parameterName: "diffuse"; shaderVariableName: "kd"; bindingType: ParameterMapping.Uniform},
                            ParameterMapping {parameterName: "lightPos"; shaderVariableName: "lightPosition"; bindingType: ParameterMapping.Uniform},
                            ParameterMapping {parameterName: "lightIntensity"; shaderVariableName: "lightIntensity"; bindingType: ParameterMapping.Uniform}
                        ]

                        shaderProgram: ShaderProgram {
                            id: diffuseShader
                            vertexShaderCode: loadSource("qrc:/shaders/diffuse.vert")
                            fragmentShaderCode: loadSource("qrc:/shaders/diffuse.frag")
                        }
                    },
                    // TEXTURE PASS + UBO
                    RenderPass {
                        annotations : [Annotation {name : "Name"; value : "Texture" }]
                        bindings: ParameterMapping {parameterName: "texture"; shaderVariableName: "tex"; bindingType: ParameterMapping.Uniform}
                        renderStates : [BlendState {srcRGB: BlendState.One; dstRGB : BlendState.One},
                            BlendEquation {mode: BlendEquation.FuncAdd},
                            CullFace { mode : CullFace.Back },
                            DepthTest { func : DepthTest.LessOrEqual}
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

