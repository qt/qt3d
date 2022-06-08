// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id: root

    //! [0]
    property color maincolor: Qt.rgba(0.0, 0.0, 0.0, 1.0)

    parameters: [
        Parameter {
            name: "maincolor"
            value: Qt.vector3d(root.maincolor.r, root.maincolor.g, root.maincolor.b)
        }
    ]

    //! [0]

    effect: Effect {

        //! [1]
        property string vertex: "qrc:/shaders/gl3/simpleColor.vert"
        property string fragment: "qrc:/shaders/gl3/simpleColor.frag"
        property string vertexRHI: "qrc:/shaders/gl45/simpleColor.vert"
        property string fragmentRHI: "qrc:/shaders/gl45/simpleColor.frag"
        property string vertexES: "qrc:/shaders/es2/simpleColor.vert"
        property string fragmentES: "qrc:/shaders/es2/simpleColor.frag"
        //! [1]

        FilterKey {
            id: forward
            name: "renderingStyle"
            value: "forward"
        }

        //! [2]
        ShaderProgram {
            id: gl3Shader
            vertexShaderCode: loadSource(parent.vertex)
            fragmentShaderCode: loadSource(parent.fragment)
        }
        ShaderProgram {
            id: es2Shader
            vertexShaderCode: loadSource(parent.vertexES)
            fragmentShaderCode: loadSource(parent.fragmentES)
        }
        ShaderProgram {
            id: rhiShader
            vertexShaderCode: loadSource(parent.vertexRHI)
            fragmentShaderCode: loadSource(parent.fragmentRHI)
        }
        //! [2]

        techniques: [
            //! [3]
            // OpenGL 3.1
            Technique {
                filterKeys: [forward]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 1
                }
                renderPasses: RenderPass {
                    shaderProgram: gl3Shader
                }
            },
            //! [3]
            // OpenGL 2.0
            Technique {
                filterKeys: [forward]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    shaderProgram: es2Shader
                }
            },
            // ES 2.0
            Technique {
                filterKeys: [forward]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGLES
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    shaderProgram: es2Shader
                }
            },
            // RHI 1.0
            Technique {
                filterKeys: [forward]
                graphicsApiFilter {
                    api: GraphicsApiFilter.RHI
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 1
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    shaderProgram: rhiShader
                }
            }
        ]
    }
}


