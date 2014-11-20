/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0
import Qt3D.Render 2.0
import QtQuick 2.2 as QQ2

Material {
    id: material

    property color ambientColor: "#cc2200";
    property color diffuseColor: "pink"
    property bool enabled: true
    property Texture2D texture;


    parameters: [
        Parameter { name: "ambient"; value: Qt.vector3d(material.ambientColor.r, material.ambientColor.g, material.ambientColor.b) },
        Parameter { name: "lightIntensity"; value: Qt.vector3d(0.5, 0.5, 0.5)},
        Parameter { name: "texture"; value: texture},
        Parameter { name: "PointLightBlock"; value: ShaderData {
                property color colorAmbient;
                property color colorDiffuse;
                property color colorSpecular;
                property real shininess: 1.0;
                property vector3d position: Qt.vector3d(1.0, 1.0, 0.0)
                property vector3d intensity: Qt.vector3d(0.7, 0.8, 0.6);

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
                    // TEXTURE PASS
                    RenderPass {
                        annotations: Annotation {name : "Name"; value : "Texture" }
                        bindings: ParameterMapping {parameterName: "texture"; shaderVariableName: "tex"; bindingType: ParameterMapping.Uniform}
                        shaderProgram: ShaderProgram {
                            vertexShaderCode : "
                            #version 140
                            in vec4 vertexPosition;
                            in vec2 vertexTexCoord;
                            out vec2 texCoord;

                            uniform mat4 mvp;

                            void main()
                            {
                                texCoord = vertexTexCoord;
                                gl_Position = mvp * vertexPosition;
                            }"

                            fragmentShaderCode: "
                            #version 140
                            in vec2 texCoord;
                            out vec4 fragColor;
                            uniform sampler2D tex;

                            void main()
                            {
                                fragColor = texture2D(tex, texCoord);
                            }
                            "
                        }
                    },
                    RenderPass {
                        annotations : [Annotation {name : "Name"; value : "Texture" }]
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

                            out vec3 worldPosition;
                            out vec3 normal;

                            uniform mat4 modelViewProjection;
                            uniform mat4 modelView;
                            uniform mat3 modelViewNormal;

                            void main()
                            {
                                worldPosition = vec3(modelView * vertexPosition);
                                normal = normalize(modelViewNormal * vertexNormal);
                                gl_Position = modelViewProjection * vertexPosition;
                            }
                            "

                            fragmentShaderCode: "
                            #version 140
                            in vec3 worldPosition;
                            in vec3 normal;
                            out vec4 fragColor;

                            uniform PointLightBlock
                            {
                                vec4 colorAmbient;
                                vec4 colorDiffuse;
                                vec4 colorSpecular;
                                float shininess;
                                vec3 position;
                                vec3 intensity;
                            } lightSource;


                            void main()
                            {
                                vec3 n = normalize(normal);
                                vec3 s = normalize(lightSource.position - worldPosition);
                                vec3 v = normalize(-worldPosition);
                                vec3 r = reflect(-s, n);

                                float diffuse = max(dot(s, n), 0.0);
                                float specular = step(diffuse, 0.0) * pow(max(dot(r, v), 0.0), lightSource.shininess);
                                fragColor = vec4(lightSource.intensity, 1.0) * (
                                                lightSource.colorAmbient +
                                                lightSource.colorDiffuse * diffuse +
                                                lightSource.colorSpecular * specular);
                            }"
                        }
                    }
                ]
            }
        ]
    }
}

