// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Extras 2.0

Entity {
    id: root

    property Texture colorTexture
    readonly property int samples : colorTexture ? colorTexture.samples : 1

    Effect {
        id: materialEffect
        techniques : [
            // OpenGL 3.2
            Technique {
                graphicsApiFilter {
                    api : GraphicsApiFilter.OpenGL
                    profile : GraphicsApiFilter.CoreProfile
                    minorVersion : 3
                    majorVersion : 2
                }

                renderPasses : RenderPass {
                    filterKeys : FilterKey { name : "pass"; value : "final" }

                    shaderProgram : ShaderProgram {
                        vertexShaderCode:
                            "#version 150 core

                            in vec3 vertexPosition;
                            out vec2 texCoords;

                            void main() {
                                texCoords = vec2(0.5) + vec2(0.5) * vertexPosition.xz;
                                gl_Position = vec4(vertexPosition.x, vertexPosition.z, 0.0, 1.0);
                            }"
                        fragmentShaderCode:
                            "#version 150 core

                            uniform sampler2DMS source;

                            in vec2 texCoords;
                            out vec4 fragColor;

                            void main() {
                                vec4 c = vec4(0.0);
                                c += texelFetch(source, ivec2(gl_FragCoord), 0);
                                fragColor = vec4(c.rgb / max(c.a, 0.01), c.a);
                            }"
                    }
                }
            }
        ]
    }

    Material {
        id: materialWithoutTexture

        parameters: [
            Parameter { name: "source"; value: colorTexture },
            Parameter { name: "samples"; value: root.samples }
        ]

        effect: materialEffect
    }

    PlaneMesh {
        id: planeMesh
        width: 2.0
        height: 2.0
        meshResolution: Qt.size(2, 2)
    }

    components : [
        planeMesh,
        materialWithoutTexture
    ]
}
