// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    components: RenderSettings {
        ForwardRenderer {}
        renderPolicy: RenderSettings.Always
    }

    Entity {
        components: [
            PlaneMesh {
                width: 2.0
                height: 2.0
                meshResolution: Qt.size(2, 2)
            },

            Transform { // Rotate the plane so that it faces us
                rotation: fromAxisAndAngle(Qt.vector3d(1.0, 0.0, 0.0), 90)
            },

            Material {
                parameters: Parameter { name: "winsize"; value: Qt.vector2d(_window.width, _window.height) }

                effect: Effect {
                    techniques: Technique {
                        filterKeys: [ FilterKey{ name: "renderingStyle"; value: "forward" } ]

                        graphicsApiFilter {
                            minorVersion: 3
                            majorVersion: 2
                            api: GraphicsApiFilter.OpenGL
                            profile: GraphicsApiFilter.CoreProfile
                        }

                        renderPasses: RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/plasma.vert")
                                fragmentShaderCode: loadSource("qrc:/plasma.frag")
                            }
                        }
                    }
                }
            }
        ]
    }
}
