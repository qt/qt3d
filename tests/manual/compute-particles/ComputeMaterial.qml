// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    property Buffer dataBuffer;
    property real particleStep: 0.4
    property real finalCollisionFactor: 0.2

    parameters: [
        Parameter { name: "particleStep"; value: particleStep },
        Parameter { name: "finalCollisionFactor"; value: finalCollisionFactor }
    ]

    effect: Effect {
        techniques: [
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            computeShaderCode: loadSource("qrc:/shaders/gl43/particles.comp")
                        }
                        // We set the buffer as the parameter data
                        parameters: [
                            Parameter { name: "Particles"; value: dataBuffer }
                        ]
                    }
                ]
                filterKeys: [
                    FilterKey { name: "type"; value: "compute" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }
            },
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/shaders/gl43/particles.vert")
                            fragmentShaderCode: loadSource("qrc:/shaders/gl43/particles.frag")
                        }
                        // We assume the mesh to be drawn will also receive
                        // Vertex buffers attributes that will be used to position and color
                    }
                ]
                filterKeys: [
                    FilterKey { name: "type"; value: "draw" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }
            },
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            computeShaderCode: loadSource("qrc:/shaders/gl45/particles.comp")
                        }
                        // We set the buffer as the parameter data
                        parameters: [
                            Parameter { name: "Particles"; value: dataBuffer }
                        ]
                    }
                ]
                filterKeys: [
                    FilterKey { name: "type"; value: "compute" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.RHI
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 1
                    minorVersion: 0
                }
            },
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/shaders/gl45/particles.vert")
                            fragmentShaderCode: loadSource("qrc:/shaders/gl45/particles.frag")
                        }
                        // We assume the mesh to be drawn will also receive
                        // Vertex buffers attributes that will be used to position and color
                    }
                ]
                filterKeys: [
                    FilterKey { name: "type"; value: "draw" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.RHI
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 1
                    minorVersion: 0
                }
            }
        ] // techniques
    }
}

