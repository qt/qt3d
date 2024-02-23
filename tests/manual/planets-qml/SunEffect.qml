// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    FilterKey { id: desktopkey; name: "name"; value: "Desktop" }
    FilterKey { id: forwardkey; name : "pass"; value : "forward" }
    FilterKey { id: eskey; name: "name"; value: "ES2" }

    RenderPass {
        id: glpass
        filterKeys: [ forwardkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/gl3/sun.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/gl3/sun.frag")
        }

        // no special render state set => use the default set of states
    }

    RenderPass {
        id: rhipass
        filterKeys: [ forwardkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/rhi/sun.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/rhi/sun.frag")
        }

        // no special render state set => use the default set of states
    }

    RenderPass {
        id: espass
        filterKeys: [ forwardkey ]

        shaderProgram: ShaderProgram {
            vertexShaderCode:   loadSource("qrc:/shaders/es2/sun.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/es2/sun.frag")
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

            renderPasses: [ glpass ]
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
                api: GraphicsApiFilter.RHI
                majorVersion: 1
                minorVersion: 0
            }

            filterKeys: [ desktopkey ]

            renderPasses: [ rhipass ]
        }
    ]
}
