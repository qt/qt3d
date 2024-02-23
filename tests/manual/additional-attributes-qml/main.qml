// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.2 as QQ2
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
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    readonly property Buffer translationBuffer: Buffer {
        data: {
            var array = new Float32Array(20 * 3)
            for (var i = 0; i < 20; ++i) {
                var j = i * 3;
                array[j] = i * 4.0;
                array[j + 1] = 0.0;
                array[j + 2] = 0.0;
            }
            return array;
        }
    }

    readonly property Material instancedMaterial: Material {
        effect: Effect {
            techniques: Technique {
                filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }

                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    majorVersion: 3
                    minorVersion: 2
                    profile: GraphicsApiFilter.CoreProfile
                }

                renderPasses: RenderPass {
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: "
                            #version 150
                            in vec3 vertexPosition;
                            in vec3 translation;
                            uniform mat4 mvp;

                            void main()
                            {
                               gl_Position = mvp * vec4(vertexPosition + translation, 1.0);
                            }
                        "

                        fragmentShaderCode: "
                            #version 150

                            out vec4 fragColor;

                            void main()
                            {
                                fragColor = vec4(1.0, 0.0, 0.0, 1.0);
                            }
                        "
                    }
                }
            }
        }
    }

    GeometryRenderer {
        id: torusMesh
        instanceCount: 20
        geometry: torusGeometry
    }

    TorusGeometry {
        id: torusGeometry
        minorRadius: 0.5
        attributes: [
            Attribute {
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 3
                byteStride: 3 * 4 // vector3d
                byteOffset: 0
                divisor: 1 // One translation per instance
                name: "translation"
                buffer: translationBuffer
            }
        ]
    }

    // Doesn't work
//    TorusMesh {
//        id: torusMesh
//        geometry.attributes: [
//            Attribute {
//                attributeType: Attribute.VertexAttribute
//                vertexBaseType: Attribute.Float
//                vertexSize: 3
//                byteStride: 3 * 4 // vector3d
//                byteOffset: 0
//                divisor: 1 // One translation per instance
//                name: "translation"
//                buffer: translationBuffer
//            }
//        ]
//    }

    Transform {
        id: torusTransform
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
    }

    Entity {
        id: torusEntity
        components: [ torusMesh, instancedMaterial, torusTransform ]
    }
}
