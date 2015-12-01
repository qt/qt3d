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
    id:root
    property color diffuse: Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
    property color specular: Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
    property color coolColor: Qt.rgba( 0.0, 0.0, 0.4, 1.0 )
    property color warmColor: Qt.rgba( 0.4, 0.4, 0.0, 1.0 )
    property real alpha: 0.25
    property real beta: 0.5
    property real shininess: 100.0

    ShaderProgram {
        id: gl3GoochShader
        vertexShaderCode: loadSource("qrc:/shaders/gl3/gooch.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/gl3/gooch.frag")
    }

    ShaderProgram {
        id: gl2es2GoochShader
        vertexShaderCode: loadSource("qrc:/shaders/es2/gooch.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/es2/gooch.frag")
    }

    effect: Effect {
        parameters: [
            Parameter { name: "kd";             value: root.diffuse },
            Parameter { name: "ks";             value: root.specular },
            Parameter { name: "kblue";          value: root.coolColor },
            Parameter { name: "kyellow";        value: root.warmColor },
            Parameter { name: "alpha";          value: root.alpha },
            Parameter { name: "beta";           value: root.beta },
            Parameter { name: "shininess";      value: root.shininess }
        ]

        techniques: [
            // GL 3 Technique
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 2
                }
                renderPasses: RenderPass {
                    shaderProgram: gl3GoochShader
                }
            },

            // GL 2 Technique
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    shaderProgram: gl2es2GoochShader
                }
            },

            // ES 2 Technique
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGLES
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 2
                    minorVersion: 0
                }
                renderPasses: RenderPass {
                    shaderProgram: gl2es2GoochShader
                }
            }
        ]
    }
}

