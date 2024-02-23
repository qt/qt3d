// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id : sceneMaterialEffect
    techniques : [
        // OpenGL 3.1
        Technique {
            graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 1; majorVersion : 3 }
            renderPasses : RenderPass {
                filterKeys : FilterKey { name : "pass"; value : "geometry" }
                shaderProgram : ShaderProgram {
                    id : sceneShaderGL3
                    vertexShaderCode:
                        "#version 140

                        in vec4 vertexPosition;
                        in vec3 vertexNormal;

                        out vec4 color0;
                        out vec3 position0;
                        out vec3 normal0;

                        uniform mat4 mvp;
                        uniform mat4 modelMatrix;
                        uniform mat3 modelNormalMatrix;
                        uniform vec4 meshColor;

                        void main()
                        {
                            color0 = meshColor;
                            position0 = vec3(modelMatrix * vertexPosition);
                            normal0 = normalize(modelNormalMatrix * vertexNormal);
                            gl_Position = mvp * vertexPosition;
                        }
                    "
                    fragmentShaderCode:
                        "#version 140

                        in vec4 color0;
                        in vec3 position0;
                        in vec3 normal0;

                        out vec4 color;
                        out vec4 position;
                        out vec4 normal;

                        void main()
                        {
                            color = color0;
                            position = vec4(position0, 0.0);
                            normal = vec4(normal0, 0.0);
                        }
                    "
                }
            }
        },
        // OpenGL 2.0
        Technique {
            graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 0; majorVersion : 2 }
            renderPasses : RenderPass {
                filterKeys : FilterKey { name : "pass"; value : "geometry" }
                shaderProgram : ShaderProgram {
                    id : sceneShaderGL2
                    vertexShaderCode:
                        "#version 110

                        attribute vec4 vertexPosition;
                        attribute vec3 vertexNormal;

                        varying vec4 color0;
                        varying vec3 position0;
                        varying vec3 normal0;

                        uniform mat4 mvp;
                        uniform mat4 modelMatrix;
                        uniform mat3 modelNormalMatrix;
                        uniform vec4 meshColor;

                        void main()
                        {
                            color0 = meshColor;
                            position0 = vec3(modelMatrix * vertexPosition);
                            normal0 = normalize(modelNormalMatrix * vertexNormal);
                            gl_Position = mvp * vertexPosition;
                        }
                    "
                    fragmentShaderCode:
                        "#version 110

                        varying vec4 color0;
                        varying vec3 position0;
                        varying vec3 normal0;

                        void main()
                        {
                            gl_FragData[0] = color0;
                            gl_FragData[1] = vec4(position0, 0);
                            gl_FragData[2] = vec4(normal0, 0);
                        }
                    "
                }
            }
        }
    ]
}
