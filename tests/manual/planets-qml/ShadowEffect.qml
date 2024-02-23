// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    property Texture2D shadowTexture
    property PlanetsLight light

    parameters: [
        Parameter { name: "lightViewProjection"; value: root.light.lightViewProjection },
        Parameter { name: "lightPosition";  value: root.light.lightPosition },
        Parameter { name: "lightIntensity"; value: root.light.lightIntensity },
        Parameter { name: "shadowMapTexture"; value: root.shadowTexture }
    ]

    FilterKey { id: desktopkey; name: "name"; value: "Desktop" }
    FilterKey { id: shadowkey; name: "pass"; value: "shadowmap" }
    FilterKey { id: forwardkey; name : "pass"; value : "forward" }
    FilterKey { id: eskey; name: "name"; value: "ES2" }

    RenderPass {
        id: shadowpass
        filterKeys: [ shadowkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/gl3/shadowmap.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/gl3/shadowmap.frag")
        }

        renderStates: [
            PolygonOffset { scaleFactor: 4; depthSteps: 4 },
            DepthTest { depthFunction: DepthTest.Less }
        ]
    }

    RenderPass {
        id: glpass
        filterKeys: [ forwardkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/gl3/planetDShadow.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/gl3/planetDShadow.frag")
        }

        // no special render state set => use the default set of states
    }

    RenderPass {
        id: rhishadowpass
        filterKeys: [ shadowkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/rhi/shadowmap.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/rhi/shadowmap.frag")
        }

        renderStates: [
            PolygonOffset { scaleFactor: 4; depthSteps: 4 },
            DepthTest { depthFunction: DepthTest.Less }
        ]
    }


    RenderPass {
        id: rhipass
        filterKeys: [ forwardkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/rhi/planetDShadow.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/rhi/planetDShadow.frag")
        }

        // no special render state set => use the default set of states
    }

    RenderPass {
        id: espass
        filterKeys: [ forwardkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/es2/planetD.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/es2/planetD.frag")
        }

        // no special render state set => use the default set of states
    }


    techniques: [
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 2
            }

            filterKeys: [ desktopkey ]

            renderPasses: [ shadowpass, glpass ]
        },
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                majorVersion: 2
            }

            filterKeys: [ eskey ]

            renderPasses: [ espass ]
        },
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGLES
                majorVersion: 2
                minorVersion: 0
            }

            filterKeys: [ eskey ]

            renderPasses: [ espass ]
        },
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.RHi
                majorVersion: 1
                minorVersion: 0
            }

            filterKeys: [ desktopkey ]

            renderPasses: [ rhishadowpass, rhipass ]
        }
    ]
}
