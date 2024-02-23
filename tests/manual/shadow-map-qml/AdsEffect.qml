// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    property Texture2D shadowTexture
    property ShadowMapLight light

    // These parameters act as default values for the effect. They take
    // priority over any parameters specified in the RenderPasses below
    // (none provided in this example). In turn these parameters can be
    // overwritten by specifying them in a Material that references this
    // effect.
    // The priority order is:
    //
    // Material -> Effect -> Technique -> RenderPass -> GLSL default values
    parameters: [
        Parameter { name: "lightViewProjection"; value: root.light.lightViewProjection },
        Parameter { name: "lightPosition";  value: root.light.lightPosition },
        Parameter { name: "lightIntensity"; value: root.light.lightIntensity },
        Parameter { name: "shadowMapTexture"; value: root.shadowTexture }
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
                RenderPass {
                    filterKeys: [ FilterKey { name: "pass"; value: "shadowmap" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/shadowmap.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/shadowmap.frag")
                    }

                    renderStates: [
                        PolygonOffset { scaleFactor: 4; depthSteps: 4 },
                        DepthTest { depthFunction: DepthTest.Less }
                    ]
                },

                RenderPass {
                    filterKeys: [ FilterKey { name : "pass"; value : "forward" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/ads.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/ads.frag")
                    }

                    // no special render state set => use the default set of states
                }
            ]
        },
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGLES
                majorVersion: 3
                minorVersion: 0
            }

            renderPasses: [
                RenderPass {
                    filterKeys: [ FilterKey { name: "pass"; value: "shadowmap" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/es3/shadowmap.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/es3/shadowmap.frag")
                    }

                    renderStates: [
                        PolygonOffset { scaleFactor: 4; depthSteps: 4 },
                        DepthTest { depthFunction: DepthTest.Less }
                    ]
                },

                RenderPass {
                    filterKeys: [ FilterKey { name : "pass"; value : "forward" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/es3/ads.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/es3/ads.frag")
                    }
                }
            ]
        },
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.RHI
                majorVersion: 1
                minorVersion: 0
            }

            renderPasses: [
                RenderPass {
                    filterKeys: [ FilterKey { name: "pass"; value: "shadowmap" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/rhi/shadowmap.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/rhi/shadowmap.frag")
                    }

                    renderStates: [
                        PolygonOffset { scaleFactor: 4; depthSteps: 4 },
                        DepthTest { depthFunction: DepthTest.Less }
                    ]
                },

                RenderPass {
                    filterKeys: [ FilterKey { name : "pass"; value : "forward" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/rhi/ads.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/rhi/ads.frag")
                    }
                }
            ]
        }
    ]
}
