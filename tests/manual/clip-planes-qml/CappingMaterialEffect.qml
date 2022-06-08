// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {

    property ShaderData sectionsData;

    parameters: [
        Parameter { name: "ka"; value: "black" },
        Parameter { name: "kd"; value: "blue" },
        Parameter { name: "ks"; value: "white" },
        Parameter { name: "shininess"; value: 100 },
        Parameter { name: "lightPosition"; value: Qt.vector4d(1.0, 1.0, 0.0, 1.0) },
        Parameter { name: "lightIntensity"; value: Qt.vector3d(1.0, 1.0, 1.0) },
        Parameter { name: "sectionsData"; value: sectionsData }
    ]

    techniques: [
        Technique {

            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 2
            }

            renderPasses: [
                // Capping pass: generate planes given clipping plane equations
                // to be rendered with the phong material
                RenderPass {
                    filterKeys: FilterKey { name: "pass"; value: "capping" }
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource("qrc:/capping.vert")
                        geometryShaderCode: loadSource("qrc:/capping.geom")
                        fragmentShaderCode: loadSource("qrc:/capping.frag")
                    }
                }
            ]
        }
    ]
}

