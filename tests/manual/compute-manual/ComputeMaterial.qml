// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.14
import Qt3D.Render 2.14

Material {
    property Buffer dataBuffer;
    property real particleStep: 0.4
    property real finalCollisionFactor: 0.2

    parameters: [
        Parameter { name: "particleStep"; value: particleStep },
        Parameter { name: "finalCollisionFactor"; value: finalCollisionFactor }
    ]

    ShaderProgram {
        id: computeShader
        computeShaderCode: loadSource("qrc:/particles.comp")
    }

    ShaderProgram {
        id: drawShader
        vertexShaderCode: loadSource("qrc:/particles.vert")
        fragmentShaderCode: loadSource("qrc:/particles.frag")
    }

    effect: Effect {
        techniques: [
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: computeShader
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
                        shaderProgram: drawShader
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
            }
        ] // techniques
    }
}

