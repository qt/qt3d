// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.2 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 22.5
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d(9.5, 7.0, 45.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(9.5, 0.5, 0.0)
    }

    FirstPersonCameraController { camera: camera }

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

    Material {
        id: instancedPhongMaterial
        effect: Effect {
            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        minorVersion: 2
                        majorVersion: 3
                    }
                    filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }
                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/gl3/instanced.vert")
                            fragmentShaderCode: loadSource("qrc:/gl3/instanced.frag")
                        }
                    }
                },
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.RHI
                        profile: GraphicsApiFilter.NoProfile
                        minorVersion: 1
                        majorVersion: 0
                    }
                    filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }
                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/gl45/instanced.vert")
                            fragmentShaderCode: loadSource("qrc:/gl45/instanced.frag")
                        }
                    }
                }
            ]
        }
    }

    // Create a GeometryRenderer component that uses the standard CylinderGeometry to
    // create the base vertex and index data buffers and attributes.
    GeometryRenderer {
        id: cylinderMeshInstanced
        enabled: instanceCount != 0

        geometry: CylinderGeometry {
            rings: 50
            slices: 30
            radius: 0.3
            length: 3.0

            attributes: [ instanceDataAttribute ]
        }

        // Use our buffer created from C++ as per-instance position data to render
        // many instances (copies) of the base cylinder geometry in a single OpenGL
        // draw call where supported (OpenGL >=3.3 or OpenGL ES 3). On older versions
        // of OpenGL the instancing is emulated on the CPU using multiple draw calls.
        QQ2.SequentialAnimation {
            running: true
            loops: QQ2.Animation.Infinite

            QQ2.NumberAnimation {
                target: cylinderMeshInstanced
                property: "instanceCount"
                duration: 5000
                from: 0
                to: _instanceBuffer.instanceCount
            }

            QQ2.PauseAnimation {
                duration: 3000
            }

            QQ2.NumberAnimation {
                target: cylinderMeshInstanced
                property: "instanceCount"
                duration: 5000
                from: _instanceBuffer.instanceCount
                to: 0
            }

            QQ2.PauseAnimation {
                duration: 3000
            }
        }

        Attribute {
            id: instanceDataAttribute
            name: "pos"
            attributeType: Attribute.VertexAttribute
            vertexBaseType: Attribute.Float
            vertexSize: 3
            divisor: 1
            buffer: _instanceBuffer
        }
    }

    Entity {
        id: torusEntity
        components: [ cylinderMeshInstanced, instancedPhongMaterial ]
    }
}
