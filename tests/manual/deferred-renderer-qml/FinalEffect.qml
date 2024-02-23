// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    techniques : [
        // OpenGL 3.1
        Technique {
            graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.CoreProfile; minorVersion : 1; majorVersion : 3 }
            renderPasses : RenderPass {
                filterKeys : FilterKey { name : "pass"; value : "final" }
                shaderProgram : ShaderProgram {
                    id : finalShaderGL3
                    vertexShaderCode:  loadSource("qrc:/final_gl3.vert")
                    fragmentShaderCode: loadSource("qrc:/final_gl3.frag")
                }
            }
        },
        // OpenGL 2.0 with FBO extension
        Technique {
            graphicsApiFilter {api : GraphicsApiFilter.OpenGL; profile : GraphicsApiFilter.NoProfile; minorVersion : 0; majorVersion : 2 }
            renderPasses : RenderPass {
                filterKeys : FilterKey { name : "pass"; value : "final" }
                shaderProgram : ShaderProgram {
                    id : finalShaderGL2
                    vertexShaderCode: loadSource("qrc:/final_es2.vert")
                    fragmentShaderCode:  loadSource("qrc:/final_es2.frag")
                }
            }
        }]
}
