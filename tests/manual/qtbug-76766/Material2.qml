// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id: root

    property color color:  Qt.rgba(0.15, 0.35, 0.50, 1.0)

    parameters: [
        Parameter { name: "color"; value: Qt.vector3d(root.color.r, root.color.g, root.color.b) }
    ]


    effect: Effect {
        property string vertex: "qrc:/shaders/shader.vert"
        property string fragment: "qrc:/shaders/shader.frag"

        FilterKey {
            id: forward
            name: "renderingStyle"
            value: "forward"
        }

        ShaderProgram {
            id: gl3Shader
            vertexShaderCode: loadSource(parent.vertex)
            fragmentShaderCode: loadSource(parent.fragment)
        }

        techniques: [
            // OpenGL 3.1
            Technique {
                filterKeys: [ forward ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 3
                }
                renderPasses: [
                    RenderPass {
                        filterKeys: [
                            FilterKey {
                                name: "pass"
                                value: 1
                            }
                        ]
                        shaderProgram: gl3Shader
                        renderStates: [
                            NoDepthMask {},
                            BlendEquation {},
                            BlendEquationArguments {
                                sourceRgb: BlendEquationArguments.One
                                destinationRgb: BlendEquationArguments.One
                                sourceAlpha: BlendEquationArguments.Zero
                                destinationAlpha: BlendEquationArguments.One
                            }
                        ]
                    }
                ]
            }
        ]
    }
}


