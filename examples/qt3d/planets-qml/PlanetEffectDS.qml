/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
            openGLFilter {
                api: OpenGLFilter.Desktop
                profile: OpenGLFilter.Core
                majorVersion: 3
                minorVersion: 2
            }

            annotations: [ Annotation { name: "name"; value: "Desktop" } ]

            renderPasses: [
                RenderPass {
                    annotations: [ Annotation { name: "pass"; value: "shadowmap" } ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/gl3/shadowmap.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/gl3/shadowmap.frag")
                    }

                    renderStates: [
                        PolygonOffset { factor: 4; units: 4 },
                        DepthTest { func: DepthTest.Less }
                    ]
                },
                RenderPass {
                    annotations: [ Annotation { name : "pass"; value : "forward" } ]

                    bindings: [
                        // Uniforms (those provided by the user)
                        ParameterMapping {
                            parameterName: "ambient";
                            shaderVariableName: "ka";
                            bindingType: ParameterMapping.Uniform
                        }
                    ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/gl3/planetD.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/gl3/planetDS.frag")
                    }

                    // no special render state set => use the default set of states
                }
            ]
        },
        Technique {
            openGLFilter {
                api: OpenGLFilter.Desktop
                majorVersion: 2
            }

            annotations: [ Annotation { name: "name"; value: "ES2" } ]

            renderPasses: [
                RenderPass {
                    annotations: [ Annotation { name : "pass"; value : "forward" } ]

                    bindings: [
                        // Uniforms (those provided by the user)
                        ParameterMapping {
                            parameterName: "ambient";
                            shaderVariableName: "ka";
                            bindingType: ParameterMapping.Uniform
                        }
                    ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/es2/planetD.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/es2/planetDS.frag")
                    }

                    // no special render state set => use the default set of states
                }
            ]
        },
        Technique {
            openGLFilter {
                api: OpenGLFilter.ES
                majorVersion: 2
                minorVersion: 0
            }

            annotations: [ Annotation { name: "name"; value: "ES2" } ]

            renderPasses: [
                RenderPass {
                    annotations: [ Annotation { name : "pass"; value : "forward" } ]

                    bindings: [
                        // Uniforms (those provided by the user)
                        ParameterMapping {
                            parameterName: "ambient";
                            shaderVariableName: "ka";
                            bindingType: ParameterMapping.Uniform
                        }
                    ]

                    shaderProgram: ShaderProgram {
                        vertexShaderCode:   loadSource("qrc:/shaders/es2/planetD.vert")
                        fragmentShaderCode: loadSource("qrc:/shaders/es2/planetDS.frag")
                    }

                    // no special render state set => use the default set of states
                }
            ]
        }
    ]
}
