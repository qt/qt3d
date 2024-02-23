// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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

    // 2 Textures can reference the same texture image
    // (Though it's up to the user to make sure that the images are compatible with the textures)
    // If the 2 Textures contain the exact same number of images and filtering options
    // Qt3D will only generate 1 actual GL texture, if they differ, 2 will be generated
    TextureImage {
        id: mainImage
        source: "qrc:/image.png"
    }

    readonly property GeometryRenderer mesh: CuboidMesh { }

    Entity {
        id: leftCube

        readonly property Transform transform: Transform {
            translation: Qt.vector3d(-10, 0, 0)
            scale: 10
        }

        readonly property DiffuseMapMaterial material: DiffuseMapMaterial {
            diffuse: Texture2D {
                generateMipMaps: false
                minificationFilter: Texture.Nearest
                magnificationFilter: Texture.Nearest
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                textureImages: [mainImage]
            }
        }
        components: [ mesh, material, transform ]
    }

    Entity {
        id: rightCube

        readonly property Transform transform: Transform {
            translation: Qt.vector3d(10, 0, 0)
            scale: 10
        }

        readonly property DiffuseMapMaterial material: DiffuseMapMaterial {
            diffuse: Texture2D {
                generateMipMaps: true
                minificationFilter: Texture.LinearMipMapLinear
                magnificationFilter: Texture.Linear
                maximumAnisotropy: 16.0
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
                textureImages: [mainImage]
            }
        }
        components: [ mesh, material, transform ]
    }
}
