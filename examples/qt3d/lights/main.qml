// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0 as Quick
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity
{
    components: [
        RenderSettings {
            activeFrameGraph: SimpleForwardRenderer {
                clearColor: Qt.rgba(0, 0, 0, 1)
                camera: camera
            }
        },
        InputSettings { }
    ]

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        position: Qt.vector3d( 0.0, 40.0, 300.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, -10.0, -1.0 )
    }

    FirstPersonCameraController {
        camera: camera
        linearSpeed: 1000.0
        acceleration: 0.1
        deceleration: 1.0
    }

    Entity {
        id: sun
        components: [
            DirectionalLight {
                color: Qt.rgba(0.8, 0.8, 0.8, 1.0)
                worldDirection: Qt.vector3d(-1, -1, 0)
            }
        ]
    }

    Entity {
        id: redLight
        components: [
            SphereMesh {
                radius: 2
            },
            Transform {
                translation: Qt.vector3d(2.0, 8.0, -2.0)
                Quick.SequentialAnimation on translation.y {
                    loops: Quick.Animation.Infinite
                    Quick.NumberAnimation { from: 8.0; to: 40.0; duration: 3000 }
                    Quick.NumberAnimation { from: 40.0; to: 8.0; duration: 3000 }
                }
            },
            PhongMaterial {
                diffuse: "red"
            },
            PointLight {
                color: Qt.rgba(1, 0, 0, 1)
            }
        ]
    }

    Entity {
        id: greenLight
        components: [
            SphereMesh {
                radius: 2
            },
            Transform {
                translation: Qt.vector3d(0.0, 3.0, 4.0)
                Quick.SequentialAnimation on translation.z {
                    loops: Quick.Animation.Infinite
                    Quick.NumberAnimation { from: 4.0; to: 40.0; duration: 5000 }
                    Quick.NumberAnimation { from: 40.0; to: 4.0; duration: 5000 }
                }
            },
            PhongMaterial {
                diffuse: "green"
            },
            PointLight {
                color: Qt.rgba(0, 1, 0, 1)
            }
        ]
    }

    Entity {
        id: spotLight
        components: [
            SphereMesh {
                radius: 2
            },
            Transform {
                translation: Qt.vector3d(-20.0, 40.0, 0.0)

                Quick.SequentialAnimation on translation {
                    loops: Quick.Animation.Infinite
                    running: true
                    Quick.Vector3dAnimation { from: Qt.vector3d(-40.0, 40.0, 0.0); to: Qt.vector3d(40.0, 40.0, 0.0); duration: 5000 }
                    Quick.Vector3dAnimation { from: Qt.vector3d(40.0, 40.0, 0.0); to: Qt.vector3d(-40.0, 40.0, 0.0); duration: 5000 }
                }
            },
            PhongMaterial {
                diffuse: "white"
            },
            SpotLight {
                localDirection: Qt.vector3d(0.0, -4.0, 0.0)
                Quick.SequentialAnimation on localDirection {
                    loops: Quick.Animation.Infinite
                    running: true
                    Quick.Vector3dAnimation { from: Qt.vector3d(0.0, -4.0, -4.0); to: Qt.vector3d(0.0, -4.0, 4.0); duration: 1000 }
                    Quick.Vector3dAnimation { from: Qt.vector3d(0.0, -4.0, 4.0); to: Qt.vector3d(0.0, -4.0, -4.0); duration: 1000 }
                }
                color: "white"
                cutOffAngle: 30
                constantAttenuation: 1
                intensity: 2.5
            }
        ]
    }

    PlaneEntity {
        id: floor

        width: 600
        height: 600
        resolution: Qt.size(20, 20)
        position: Qt.vector3d(0, -30, 0)

        material: NormalDiffuseMapMaterial {
            ambient: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
            diffuse: TextureLoader {
                source: "assets/textures/ceramic_small_diamond/ceramic_small_diamond_basecolor.png"
                format: Texture.SRGB8_Alpha8
                generateMipMaps: true
            }
            normal: TextureLoader {
                source: "assets/textures/ceramic_small_diamond/ceramic_small_diamond_normal.png"
                generateMipMaps: true
            }
            textureScale: 10
            shininess: 10
        }
    }

    Entity {
        components: [
            PhongMaterial {
                diffuse: "white"
                shininess: 50
            },
            Mesh {
                source: "assets/obj/toyplane.obj"
            }
        ]
    }
}
