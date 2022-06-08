// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController { camera: camera }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
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
            matrix: {
                var m = Qt.matrix4x4();
                m.rotate(45, Qt.vector3d(1, 0, 0));
                m.translate(Qt.vector3d(-2.0, 0.0, 0.0));
                return m;
            }
        }

        Timer {
            property bool toggle: false
            interval: 2000
            repeat: true
            triggeredOnStart: true
            running: true

            onTriggered: {
                // Toggle shader code each time we are triggered
                if (toggle) {
                    console.log("Setting color fragment shader");
                    changingShader.fragmentShaderCode = "
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
                    ";
                } else {
                    console.log("Setting greyscale fragment shader");
                    changingShader.fragmentShaderCode = "
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

                            float luminance = fragColor.r * 0.299 + fragColor.g * 0.587 + fragColor.b * 0.114;
                            fragColor = vec4( luminance, luminance, luminance, 1.0f );
                        }
                    ";
                }

                toggle = !toggle;
            }
        }

        // Shader defined inline
        property Material material: Material {

            parameters: Parameter { name: "finalColor"; value: "#e76140" /* Inferno Orange */ }

            effect: Effect {
                techniques: [
                    Technique {
                        filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }

                        graphicsApiFilter {
                            api: GraphicsApiFilter.OpenGL
                            majorVersion: 3
                            minorVersion: 2
                            profile: GraphicsApiFilter.CoreProfile
                        }

                        renderPasses: RenderPass {
                            shaderProgram: ShaderProgram {
                                id: changingShader

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

                                fragmentShaderCode: ""

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

        property Transform transform: Transform {
            matrix: {
                var m = Qt.matrix4x4();
                m.rotate(15, Qt.vector3d(1, 0, 0));
                m.translate(Qt.vector3d(2.0, 0.0, 0.0));
                return m;
            }
        }

        // Shader defined in a separate file
        property Material material: Material {

            parameters: Parameter { name: "finalColor"; value: "yellow" }

            effect: Effect {
                techniques: [
                    Technique {
                        filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }

                        graphicsApiFilter {
                            api: GraphicsApiFilter.OpenGL
                            majorVersion: 3
                            minorVersion: 2
                            profile: GraphicsApiFilter.CoreProfile
                        }

                        renderPasses: RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/simple-shader.vert")
                                fragmentShaderCode: loadSource("qrc:/simple-shader.frag")
                            } // ShaderProgram
                        } // namespace RenderPass
                    } // Technique
                ] // techniques
            } // Effect
        } // Material

        components: [ torusMesh, material, transform ]
    }
}
