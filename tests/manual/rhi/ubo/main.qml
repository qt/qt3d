// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.15 as QQ2
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15

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

    OrbitCameraController {
        camera: camera
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
                showDebugOverlay: true
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Material {
        id: material
        effect: Effect {
            techniques: [
                Technique {
                    filterKeys: FilterKey {
                        name: "renderingStyle"
                        value: "forward"
                    }
                    graphicsApiFilter {
                        api: GraphicsApiFilter.RHI
                        majorVersion: 1
                        minorVersion: 0
                    }
                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: "
                            #version 450

                            layout(location = 0) in vec3 vertexPosition;

                            layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
                              mat4 viewMatrix;
                              mat4 projectionMatrix;
                              mat4 uncorrectedProjectionMatrix;
                              mat4 clipCorrectionMatrix;
                              mat4 viewProjectionMatrix;
                              mat4 inverseViewMatrix;
                              mat4 inverseProjectionMatrix;
                              mat4 inverseViewProjectionMatrix;
                              mat4 viewportMatrix;
                              mat4 inverseViewportMatrix;
                              vec4 textureTransformMatrix;
                              vec3 eyePosition;
                              float aspectRatio;
                              float gamma;
                              float exposure;
                              float time;
                            };

                            layout(std140, binding = 1) uniform qt3d_command_uniforms {
                              mat4 modelMatrix;
                              mat4 inverseModelMatrix;
                              mat4 modelViewMatrix;
                              mat3 modelNormalMatrix;
                              mat4 inverseModelViewMatrix;
                              mat4 modelViewProjection;
                              mat4 inverseModelViewProjectionMatrix;
                            };

                            void main()
                            {
                                gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
                            }
                            "

                            fragmentShaderCode: "
                            #version 450

                            layout(location = 0) out vec4 fragColor;

                            layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
                              mat4 viewMatrix;
                              mat4 projectionMatrix;
                              mat4 uncorrectedProjectionMatrix;
                              mat4 clipCorrectionMatrix;
                              mat4 viewProjectionMatrix;
                              mat4 inverseViewMatrix;
                              mat4 inverseProjectionMatrix;
                              mat4 inverseViewProjectionMatrix;
                              mat4 viewportMatrix;
                              mat4 inverseViewportMatrix;
                              vec4 textureTransformMatrix;
                              vec3 eyePosition;
                              float aspectRatio;
                              float gamma;
                              float exposure;
                              float time;
                            };

                            struct MyStruct {
                                vec4 factor;
                            };

                            layout(std140, binding = 2) uniform CustomUBO1 {
                                vec4 baseColor;
                            };

                            layout(std140, binding = 3) uniform CustomUBO2 {
                                MyStruct innerStruct;
                                vec4 accentColor;
                            };

                            void main()
                            {
                                fragColor = baseColor * abs(sin(time)) + accentColor * innerStruct.factor;
                            }
                            "
                        }
                    }
                }
            ]
        }

        parameters: [
            // User provided UBO
            Parameter {
                name: "CustomUBO1"
                value: Buffer {
                    data: {
                        var bufferData = new Float32Array(4);
                        bufferData[0] = 0.5;
                        bufferData[1] = 0.5;
                        bufferData[2] = 0.5;
                        bufferData[3] = 1.0;
                        return bufferData
                    }
                }
            },
            // Regular parameter that maps to UBO member
            Parameter {
                name: "accentColor"
                value: "yellow"
            },
            Parameter {
                name: "innerStruct"
                value: ShaderData {
                    property vector4d factor: Qt.vector4d(0.5, 0.5, 0.5, 0.0)
                }
            }
        ]
    }

    TorusMesh {
        id: torusMesh
        radius: 5
        minorRadius: 1
        rings: 100
        slices: 20
    }

    Transform {
        id: torusTransform
        scale3D: Qt.vector3d(1.5, 1, 0.5)
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
    }

    Entity {
        id: torusEntity
        components: [ torusMesh, material, torusTransform ]
    }

    //    QQ2.NumberAnimation {
    //        target: torusTransform
    //        property: "userAngle"
    //        duration: 10000
    //        from: 0
    //        to: 360

    //        loops: QQ2.Animation.Infinite
    //        running: true
    //    }
}
