// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.10
import QtQuick.Scene3D 2.0
import Qt3D.Core 2.10
import Qt3D.Render 2.13
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Item {
    width: 800
    height: 600

    Scene3D {
        anchors.fill: parent

        Entity {
            id: sceneRoot

            Camera {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: 16/9
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 0.0, 0.0, -4.0 )
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
                InputSettings { },
                DirectionalLight {
                    intensity: 0.8
                    worldDirection: camera.viewVector
                }
            ]


            NumberAnimation {
                target: cubeTransform
                property: "rotationY"
                duration: 10000
                from: 0
                to: 360

                loops: Animation.Infinite
                running: true
            }

            SharedGLTexture {
                id: shaderGLTexture
                textureId: _textureWidget.textureId
            }

            Entity {
                id: cubeEntity
                readonly property GeometryRenderer cuboid: CuboidMesh { }
                readonly property DiffuseMapMaterial material: DiffuseMapMaterial {
                    diffuse: shaderGLTexture
                }
                Transform {
                    id: cubeTransform
                    rotationX: 180
                }

                components: [ cuboid, material, cubeTransform ]
            }
        }
    }

    Grid {
        spacing: 10
        columns: 2
        Text { text: "Target: " + _nameMapper.targetName(shaderGLTexture.target) }
        Text { text: "Format: " + _nameMapper.formatName(shaderGLTexture.format) }
        Text { text: "Width: " + shaderGLTexture.width }
        Text { text: "Height: " + shaderGLTexture.height }
        Text { text: "Depth: " + shaderGLTexture.depth }
        Text { text: "Layers: " + shaderGLTexture.layers }
        Text { text: "GL Texture Id: " + shaderGLTexture.textureId }
    }

}
