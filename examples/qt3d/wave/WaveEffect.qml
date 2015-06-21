/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

import Qt3D 2.0
import Qt3D.Renderer 2.0

Effect {
    id: root

    property bool wireframe: false

    // These parameters act as default values for the effect. They take
    // priority over any parameters specified in the RenderPasses below
    // (none provided in this example). In turn these parameters can be
    // overwritten by specifying them in a Material that references this
    // effect.
    // The priority order is:
    //
    // Material -> Effect -> Technique -> RenderPass -> GLSL default values
    parameters: [
        Parameter { name: "ambient"; value: Qt.vector3d( 0.1, 0.1, 0.1 ) },
        Parameter { name: "diffuse"; value: Qt.vector3d( 0.7, 0.7, 0.7 ) }
    ]

    techniques: [
        Technique {
            openGLFilter {
                api: OpenGLFilter.Desktop
                profile: OpenGLFilter.Core
                majorVersion: 3
                minorVersion: 2
            }

            annotations: [ Annotation { name: "renderingStyle"; value: "forward" } ]

            parameters: [
                Parameter { name: "light.position"; value: Qt.vector4d( 0.0, 0.0, 0.0, 1.0 ) },
                Parameter { name: "light.intensity"; value: Qt.vector3d( 1.0, 1.0, 1.0 ) },
                Parameter { name: "line.width"; value: 0.8 },
                Parameter { name: "line.color"; value: Qt.vector4d( 1.0, 1.0, 1.0, 1.0 ) }
            ]

            renderPasses: [
                RenderPass {
                    bindings: [
                        ParameterMapping { parameterName: "ambient";  shaderVariableName: "ka"; bindingType: ParameterMapping.Uniform },
                        ParameterMapping { parameterName: "diffuse";  shaderVariableName: "kd"; bindingType: ParameterMapping.Uniform }
                    ]

                    shaderProgram: wireframe ? wireframeShader : standardShader

                    // Disable depth testing and use simple additive blending
                    renderStates: [
                        DepthTest { func: DepthTest.Always },
                        DepthMask { mask: true },
                        BlendState {
                            srcRGB: BlendState.One
                            dstRGB: BlendState.OneMinusSrcColor
                        }
                    ]

                    ShaderProgram {
                        id: standardShader
                        vertexShaderCode:   loadSource("qrc:/shaders/ribbon.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/ribbon.frag")
                    }

                    ShaderProgram {
                        id: wireframeShader
                        vertexShaderCode:   loadSource("qrc:/shaders/ribbon.vert")
                        geometryShaderCode: loadSource("qrc:/shaders/robustwireframe.geom")
                        fragmentShaderCode: loadSource("qrc:/shaders/ribbonwireframe.frag")
                    }
                }
            ]
        }
    ]
}
