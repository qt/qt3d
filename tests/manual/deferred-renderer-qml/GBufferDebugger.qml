// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    readonly property Layer layer: debugLayer

    components : [
        Layer { id: debugLayer },

        PlaneMesh {
            width: 2.0
            height: 2.0
            meshResolution: Qt.size(2, 2)
        },

        Transform { // We rotate the plane so that it faces us
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
        },

        Material {
            effect : Effect {
                techniques: [
                    Technique {
                        graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.NoProfile; minorVersion : 0; majorVersion : 2 }
                        renderPasses: RenderPass {
                            filterKeys: FilterKey { name: "pass"; value: "final" }
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/debug_es2.vert")
                                fragmentShaderCode: loadSource("qrc:/debug_es2.frag")
                            }
                        }
                    },
                    Technique {
                        graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 2; majorVersion : 3 }
                        renderPasses: RenderPass {
                            filterKeys: FilterKey { name: "pass"; value: "final" }
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/debug_gl3.vert")
                                fragmentShaderCode: loadSource("qrc:/debug_gl3.frag")
                            }
                        }
                    }
                ]
            }
        }
    ]
}
