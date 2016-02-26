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

Technique {
    annotations : [
        Annotation { name : "RenderingStyle"; value : "forward"},
        Annotation { name : "Enabled"; value : true}
    ]
    graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 1; majorVersion : 3 }
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
                BlendEquation {blendFunction: BlendEquation.Add},
                CullFace { mode : CullFace.Back },
                DepthTest { depthFunction : DepthTest.LessOrEqual}
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
