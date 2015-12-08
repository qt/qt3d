/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    property Buffer dataBuffer;
    property real particleStep: 0.4
    property real finalCollisionFactor: 0.2

    parameters: [
        Parameter { name: "particleStep"; value: particleStep },
        Parameter { name: "finalCollisionFactor"; value: finalCollisionFactor }
    ]

    ShaderProgram {
        id: computeShader
        computeShaderCode: loadSource("qrc:/particles.comp")
    }

    ShaderProgram {
        id: drawShader
        vertexShaderCode: loadSource("qrc:/particles.vert")
        fragmentShaderCode: loadSource("qrc:/particles.frag")
    }

    effect: Effect {
        techniques: [
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: computeShader
                        // We set the buffer as the parameter data
                        parameters: [
                            Parameter { name: "Particles"; value: dataBuffer }
                        ]
                    }
                ]
                annotations: [
                    Annotation { name: "type"; value: "compute" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }
            },
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: drawShader
                        // We assume the mesh to be drawn will also receive
                        // Vertex buffers attributes that will be used to position and color
                        bindings: [
                            ParameterMapping { parameterName: "particlePosition";  shaderVariableName: "particlePosition"; bindingType: ParameterMapping.Attribute },
                            ParameterMapping { parameterName: "particleColor";  shaderVariableName: "particleColor"; bindingType: ParameterMapping.Attribute }
                        ]
                    }
                ]
                annotations: [
                    Annotation { name: "type"; value: "draw" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }
            }
        ] // techniques
    }
}

