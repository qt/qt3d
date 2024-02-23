// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    property Texture2D shadowTexture
    property Light light

    parameters: [
        Parameter { name: "lightViewProjection"; value: root.light.lightViewProjection },
        Parameter { name: "lightPosition";  value: root.light.lightPosition },
        Parameter { name: "lightIntensity"; value: root.light.lightIntensity },

        Parameter { name: "shadowMapTexture"; value: root.shadowTexture }
    ]

    techniques: [
        Technique {
            renderPasses: [
                RenderPass {
                    filterKeys: [ FilterKey { name: "pass"; value: "shadowmap" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/shadowmap.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/shadowmap.frag")
                    }

                    renderStates: [
                        PolygonOffset { scaleFactor: 4; units: 4 },
                        DepthTest { func: DepthTest.Less }
                    ]
                },

                RenderPass {
                    filterKeys: [ FilterKey { name: "pass"; value: "forward" } ]

                    bindings: [
                        ParameterMapping { parameterName: "ambient";    shaderVariableName: "ka"; bindingType: ParameterMapping.Uniform },
                        ParameterMapping { parameterName: "diffuse";    shaderVariableName: "kd"; bindingType: ParameterMapping.Uniform },
                        ParameterMapping { parameterName: "specular";   shaderVariableName: "ks"; bindingType: ParameterMapping.Uniform }
                    ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/ads.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/ads.frag")
                    }
                }
            ]
        }
    ]
}
