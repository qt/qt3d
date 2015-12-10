/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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
    property color ambient:  Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
    property color diffuse:  Qt.rgba( 0.7, 0.7, 0.7, 1.0 )
    property color specular: Qt.rgba( 0.01, 0.01, 0.01, 1.0 )
    property real shininess: 150.0
    property real alpha: 0.5


    ShaderProgram {
        id: gl3PhongAlphaShader
        vertexShaderCode: loadSource("qrc:/shaders/gl3/phong.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/gl3/phongalpha.frag")
    }

    ShaderProgram {
        id: gl2es2PhongAlphaShader
        vertexShaderCode: loadSource("qrc:/shaders/es2/phong.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/es2/phongalpha.frag")
    }

    effect: Effect {

        parameters: [
            Parameter { name: "alpha";  value: root.alpha },
            Parameter { name: "ka";   value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
            Parameter { name: "kd";   value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
            Parameter { name: "ks";  value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
            Parameter { name: "shininess"; value: root.shininess }
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
                        DepthMask { mask: false },
                        BlendState {
                            srcRGB: BlendState.SrcAlpha
                            dstRGB: BlendState.OneMinusSrcAlpha
                        },
                        BlendEquation {mode: BlendEquation.FuncAdd}
                    ]
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
                    shaderProgram: gl2es2PhongAlphaShader
                    renderStates: [
                        DepthMask { mask: false },
                        BlendState {
                            srcRGB: BlendState.SrcAlpha
                            dstRGB: BlendState.OneMinusSrcAlpha
                        },
                        BlendEquation {mode: BlendEquation.FuncAdd}
                    ]
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
                    shaderProgram: gl2es2PhongAlphaShader
                    renderStates: [
                        DepthMask { mask: false },
                        BlendState {
                            srcRGB: BlendState.SrcAlpha
                            dstRGB: BlendState.OneMinusSrcAlpha
                        },
                        BlendEquation {mode: BlendEquation.FuncAdd}
                    ]
                }
            }
        ]
    }
}

