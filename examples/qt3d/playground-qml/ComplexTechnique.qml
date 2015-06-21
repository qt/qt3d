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

Technique {
    annotations : [
        Annotation { name : "RenderingStyle"; value : "forward"},
        Annotation { name : "Enabled"; value : true}
    ]
    openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 1; majorVersion : 3 }
    renderPasses : [
        RenderPass {
            annotations : [Annotation {name : "Name"; value : "TextureLighting" }]
            bindings : [ // Add only the bindings needed for a shader
                ParameterMapping {parameterName: "vertexTexCoord"; shaderVariableName: "texCoord0"; bindingType: ParameterMapping.Attribute},
                ParameterMapping {parameterName: "tex"; shaderVariableName: "texture"; bindingType: ParameterMapping.Uniform},
                ParameterMapping {parameterName: "modelViewProjection"; shaderVariableName: "customMvp"; bindingType: ParameterMapping.StandardUniform}
            ]

            shaderProgram : ShaderProgram {
                id : textureShaderLighting
                vertexShaderCode: "
                #version 140
                in vec4 vertexPosition;
                in vec3 vertexNormal;
                in vec2 texCoord0;

                out vec2 texCoord;
                out vec3 worldPosition;
                out vec3 normal;

                uniform mat4 customMvp;
                uniform mat4 modelView;
                uniform mat3 modelViewNormal;

                void main()
                {
                    texCoord = texCoord0;
                    worldPosition = vec3(modelView * vertexPosition);
                    normal = normalize(modelViewNormal * vertexNormal);
                    gl_Position  = customMvp * vertexPosition;
                }"

                fragmentShaderCode: "
                #version 140
                in vec2 texCoord;
                in vec3 worldPosition;
                in vec3 normal;
                out vec4 fragColor;

                struct PointLight
                {
                    vec3 position;
                    vec3 direction;
                    vec4 color;
                    float intensity;
                };

                const int lightCount = 3;
                uniform PointLight pointLights[lightCount];
                uniform sampler2D texture;

                void main()
                {
                    vec4 color;
                    for (int i = 0; i < lightCount; i++) {
                        vec3 s = normalize(pointLights[i].position - worldPosition);
                        color += pointLights[i].color * (pointLights[i].intensity * max(dot(s, normal), 0.0));
                    }
                    color /= float(lightCount);
                    fragColor = texture(texture, texCoord) * color;
                }"
            }
        },
        RenderPass {
            annotations : [Annotation {name : "Name"; value : "Texture" }]
            shaderProgram : ShaderProgram {
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
                    fragColor = texture(tex, texCoord);
                }
                "
            }
        },
        RenderPass {
            annotations : [Annotation {name : "Name"; value : "Lighting" }]
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

                struct PointLight
                {
                    vec3 position;
                    vec3 direction;
                    vec4 color;
                    float intensity;
                };

                const int lightCount = 3;
                uniform PointLight pointLights[lightCount];

                void main()
                {
                    vec4 color;
                    for (int i = 0; i < lightCount; i++) {
                        vec3 s = normalize(pointLights[i].position - worldPosition);
                        color += pointLights[i].color * (pointLights[i].intensity * max(dot(s, normal), 0.0));
                    }
                    color /= float(lightCount);
                    fragColor = color;
                }"
            }
        },
        RenderPass {
            annotations : Annotation {name : "Name"; value : "Final" }
            shaderProgram : ShaderProgram {
                vertexShaderCode: "
                #version 140
                in vec4 vertexPosition;
                in vec2 vertexTexCoord;
                out vec2 texCoord;
                uniform mat4 modelViewProjection;

                void main()
                {
                    texCoord = vertexTexCoord;
                    gl_Position = modelViewProjection * vertexPosition;
                }
                "

                fragmentShaderCode: "
                #version 140
                in vec2 texCoord;
                out vec4 fragColor;
                uniform sampler2D gBuffer;

                void main()
                {
                    fragColor = texture(gBuffer, texCoord);
                }
                "
            }
        }

    ]
}
