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

Effect {
    techniques : [
        // OpenGL 3.1
        Technique {
            openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.Core; minorVersion : 1; majorVersion : 3 }
            parameters:  Parameter { name: "PointLightBlock"; value: ShaderData {
                    property ShaderDataArray lights: ShaderDataArray {
                        // hard coded lights until we have a way to filter
                        // ShaderData in a scene
                        values: [sceneEntity.light, sphere1.light, sphere2.light, light3.light, light4.light]
                    }
                }
            }
            renderPasses : RenderPass {
                annotations : Annotation { name : "pass"; value : "final" }
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
                            fragColor = col * lightColor;
                        }
                                "
                }
            }
        },
        // OpenGL 2.0 with FBO extension
        Technique {
            openGLFilter {api : OpenGLFilter.Desktop; profile : OpenGLFilter.None; minorVersion : 0; majorVersion : 2 }
            parameters: Parameter { name: "pointLights"; value: ShaderData {
                                property ShaderDataArray lights: ShaderDataArray {
                                    // hard coded lights until we have a way to filter
                                    // ShaderData in a scene
                                    values: [sceneEntity.light, sphere1.light, sphere2.light, light3.light, light4.light]
                                }
                            }
                        }
            renderPasses : RenderPass {
                annotations : Annotation { name : "pass"; value : "final" }
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
