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

import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.2 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration  {
        controlledCamera: camera
    }

    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        }
    ]

    TorusMesh {
        id: torusMesh
        radius: 5
        minorRadius: 1
        rings: 100
        slices: 20
    }

    Entity {
        id: torusEntityInlineShader

        property Transform transform:  Transform {
            Translate { translation: Qt.vector3d(-2.0, 0.0, 0.0) }
            Rotate {
                angle: 45
                axis: Qt.vector3d(1, 0, 0)
            }
        }

        // Shader defined inline
        property Material material: Material {

            parameters: Parameter { name: "finalColor"; value: "#e76140" /* Inferno Orange */ }

            effect: Effect {
                techniques: [
                    Technique {
                        openGLFilter {
                            api: OpenGLFilter.Desktop
                            majorVersion: 3
                            minorVersion: 2
                            profile: OpenGLFilter.Core
                        }

                        renderPasses: RenderPass {
                            shaderProgram: ShaderProgram {

                                vertexShaderCode: "
                                    #version 150

                                    in vec3 vertexPosition;
                                    in vec3 vertexNormal;

                                    out vec3 position;
                                    out vec3 normal;

                                    uniform mat4 modelView;
                                    uniform mat3 modelViewNormal;
                                    uniform mat4 mvp;

                                    void main()
                                    {
                                        normal = normalize(modelViewNormal * vertexNormal);
                                        position = vec3(modelView * vec4(vertexPosition, 1.0));
                                        gl_Position = mvp * vec4(vertexPosition, 1.0);
                                    }
                                "

                                fragmentShaderCode: "
                                    #version 150

                                    in vec3 normal;
                                    in vec3 position;

                                    uniform vec3 finalColor;

                                    out vec4 fragColor;

                                    void main()
                                    {
                                        vec3 n = normalize(normal);
                                        vec3 s = normalize(vec3(1.0, 0.0, 1.0) - position);
                                        vec3 v = normalize(-position);
                                        float diffuse = max(dot(s, n), 0.0);
                                        fragColor = vec4(diffuse * finalColor, 1.0);
                                    }
                                "

                            } // ShaderProgram
                        } // RenderPass
                    } // Technique
                ] // techniques
            } // Effect
        } // Material

        components: [ torusMesh, material, transform ]
    }

    Entity {
        id: torusEntitySeparateShader

        property Transform transform:  Transform {
            Translate { translation: Qt.vector3d(2.0, 0.0, 0.0) }
            Rotate {
                angle: 15
                axis: Qt.vector3d(1, 0, 0)
            }
        }

        // Shader defined in a separate file
        property Material material: Material {

            parameters: Parameter { name: "finalColor"; value: "yellow" }

            effect: Effect {
                techniques: [
                    Technique {
                        openGLFilter {
                            api: OpenGLFilter.Desktop
                            majorVersion: 3
                            minorVersion: 2
                            profile: OpenGLFilter.Core
                        }

                        renderPasses: RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/simple-shader.vert")
                                fragmentShaderCode: loadSource("qrc:/simple-shader.frag")
                            } // ShaderProgram
                        } // RenderPass
                    } // Technique
                ] // techniques
            } // Effect
        } // Material

        components: [ torusMesh, material, transform ]
    }
}
