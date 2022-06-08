// Copyright (C) 2017 Juan José Casafranca.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.9
import Qt3D.Render 2.9

Material {
    property Buffer dataBuffer;
    property int inputSize;

    parameters: [
        Parameter { name: "inputSize"; value: inputSize}
    ]

    ShaderProgram {
        id: computeShaderGL43
        computeShaderCode: loadSource("qrc:/gl43/bufferSetter.comp")
    }

    ShaderProgram {
        id: computeShaderGL45
        computeShaderCode: loadSource("qrc:/gl45/bufferSetter.comp")
    }

    effect: Effect {
        techniques: [
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: computeShaderGL43
                        // We set the buffer as the parameter data
                        parameters: [
                            Parameter { name: "input"; value: dataBuffer }
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
                        shaderProgram: computeShaderGL45
                        // We set the buffer as the parameter data
                        parameters: [
                            Parameter { name: "input"; value: dataBuffer }
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
            }
        ] // techniques
    }
}
