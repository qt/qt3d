// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id:root
    property color ambient:  Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
    property color diffuse:  Qt.rgba( 0.7, 0.7, 0.7, 1.0 )
    property color specular: Qt.rgba( 0.95, 0.95, 0.95, 1.0 )
    property real shininess: 150.0
    property real alpha: 0.5


    ShaderProgram {
        id: gl3PhongAlphaShader
        vertexShaderCode: loadSource("qrc:/default.vert")
        fragmentShaderCode: loadSource("qrc:/phongalpha.frag")
    }

    effect: Effect {

        parameters: [
            Parameter { name: "alpha";  value: root.alpha },
            Parameter { name: "ka";   value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
            Parameter { name: "kd";   value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
            Parameter { name: "ks";  value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
            Parameter { name: "shininess"; value: root.shininess },
            Parameter { name: "lightPosition"; value: Qt.vector4d(1.0, 1.0, 0.0, 1.0) },
            Parameter { name: "lightIntensity"; value: Qt.vector3d(1.0, 1.0, 1.0) }
        ]

        techniques: [
            // GL 3 Technique
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 1
                }
                renderPasses: RenderPass {
                    shaderProgram: gl3PhongAlphaShader
                    renderStates: [
                        NoDepthMask { },
                        BlendEquationArguments {
                            sourceRgb: BlendEquationArguments.SourceAlpha
                            destinationRgb: BlendEquationArguments.OneMinusSourceAlpha
                        },
                        BlendEquation {blendFunction: BlendEquation.Add}
                    ]
                    filterKeys: FilterKey { name: "pass"; value: "material" }
                }
            }
        ]
    }
}

