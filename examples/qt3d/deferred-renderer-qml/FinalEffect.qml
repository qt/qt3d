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

Effect {
    techniques : [
        // OpenGL 3.1
        Technique {
            graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 1; majorVersion : 3 }
            parameters:  Parameter { name: "PointLightBlock"; value: ShaderData {
                    property ShaderDataArray lights: ShaderDataArray {
                        // hard coded lights until we have a way to filter
                        // ShaderData in a scene
                        values: [sceneEntity.light, sphere1.light, sphere2.light, light3.light, light4.light]
                    }
                }
            }
            renderPasses : RenderPass {
                filterKeys : FilterKey { name : "pass"; value : "final" }
                shaderProgram : ShaderProgram {
                    id : finalShaderGL3
                    vertexShaderCode:
                        "#version 140

                        in vec4 vertexPosition;
                        uniform mat4 modelMatrix;

                        void main()
                        {
                            gl_Position = modelMatrix * vertexPosition;
                        }
                        "
                    fragmentShaderCode:
                        "#version 140

                        uniform sampler2D color;
                        uniform sampler2D position;
                        uniform sampler2D normal;
                        uniform vec2 winSize;

                        out vec4 fragColor;

                        struct PointLight
                        {
                            vec3 position;
                            vec3 direction;
                            vec4 color;
                            float intensity;
                        };

                        const int lightCount = 5;
                        uniform PointLightBlock {
                             PointLight lights[lightCount];
                        };

                        void main()
                        {
                            vec2 texCoord = gl_FragCoord.xy / winSize;
                            vec4 col = texture(color, texCoord);
                            vec3 pos = texture(position, texCoord).xyz;
                            vec3 norm = texture(normal, texCoord).xyz;

                            vec4 lightColor;
                            for (int i = 0; i < lightCount; i++) {
                                vec3 s = normalize(lights[i].position - pos);
                                lightColor += lights[i].color * (lights[i].intensity * max(dot(s, norm), 0.0));
                            }
                            lightColor /= float(lightCount);
                            fragColor = col;
                        }
                                "
                }
            }
        },
        // OpenGL 2.0 with FBO extension
        Technique {
            graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.NoProfile; minorVersion : 0; majorVersion : 2 }
            parameters: Parameter { name: "pointLights"; value: ShaderData {
                                property ShaderDataArray lights: ShaderDataArray {
                                    // hard coded lights until we have a way to filter
                                    // ShaderData in a scene
                                    values: [sceneEntity.light, sphere1.light, sphere2.light, light3.light, light4.light]
                                }
                            }
                        }
            renderPasses : RenderPass {
                filterKeys : FilterKey { name : "pass"; value : "final" }
                shaderProgram : ShaderProgram {
                    id : finalShaderGL2
                    vertexShaderCode:
                        "#version 110

                        attribute vec4 vertexPosition;
                        uniform mat4 modelMatrix;

                        void main()
                        {
                            gl_Position = modelMatrix * vertexPosition;
                        }
                                "
                    fragmentShaderCode:
                        "#version 110

                        uniform sampler2D color;
                        uniform sampler2D position;
                        uniform sampler2D normal;
                        uniform vec2 winSize;

                        struct PointLight
                        {
                            vec3 position;
                            vec3 direction;
                            vec4 color;
                            float intensity;
                        };

                        const int lightCount = 5;
                        uniform struct
                        {
                            PointLight lights[lightCount];
                        };

                        void main()
                        {
                            vec2 texCoord = gl_FragCoord.xy / winSize;
                            vec4 col = texture2D(color, texCoord);
                            vec3 pos = texture2D(position, texCoord).xyz;
                            vec3 norm = texture2D(normal, texCoord).xyz;

                            vec4 lightColor;
                            for (int i = 0; i < lightCount; i++) {
                                vec3 s = lights[i].position - pos);
                                lightColor += lights[i].color * (lights[i].intensity * max(dot(s, norm), 0.0));
                            }
                            lightColor /= float(lightCount);
                            gl_FragColor = col * lightColor;
                        }
                    "
                }
            }
        }]
}
