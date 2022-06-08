// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.1
import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import Qt3D.Extras 2.15

Entity {
    id: root
    objectName: "root"

    // Use the renderer configuration specified in ForwardRenderer.qml
    // and render from the mainCamera
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: mainCamera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    BasicCamera {
        id: mainCamera
        position: Qt.vector3d( -5.17253, 2.95727, 6.65948 )
        viewCenter: Qt.vector3d( 6.73978, -2.50545, -10.6525 )
    }

    FirstPersonCameraController { camera: mainCamera }

    Lights { }

    Entity {
        components: [
            PointLight {
                enabled: parent.enabled
                color: "black"
                intensity: 0
            },
            EnvironmentLight {
                enabled: parent.enabled

                irradiance: TextureLoader {
                    source: "qrc:/assets/cubemaps/default/default_irradiance.dds"
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
                specular: TextureLoader {
                    source: "qrc:/assets/cubemaps/default/default_specular.dds"
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
            }
        ]
    }

    Entity {
        id: floor

        components: [
            Mesh {
                source: "qrc:/assets/obj/plane-10x10.obj"
            },
            MetalRoughMaterial {
                baseColor: TextureLoader {
                    source: "qrc:/assets/textures/ceramic_small_diamond/ceramic_small_diamond_basecolor.png"
                    format: Texture.SRGB8_Alpha8
                    generateMipMaps: true
                }
                metalness: TextureLoader { source: "qrc:/assets/textures/ceramic_small_diamond/ceramic_small_diamond_metallic.png"; generateMipMaps: true }
                roughness: TextureLoader { source: "qrc:/assets/textures/ceramic_small_diamond/ceramic_small_diamond_roughness.png"; generateMipMaps: true }
                normal: TextureLoader { source: "qrc:/assets/textures/ceramic_small_diamond/ceramic_small_diamond_normal.png"; generateMipMaps: true }
                ambientOcclusion: TextureLoader { source: "qrc:/assets/textures/ceramic_small_diamond/ceramic_small_diamond_ambient_occlusion.png" }
            }
        ]
    }

    TrefoilKnot {
        id: trefoilKnot
        y: 1.5
        z: -2
        scale: 0.2

        ParallelAnimation {
            loops: Animation.Infinite
            running: true

            NumberAnimation {
                target: trefoilKnot
                property: "theta"
                from: 0; to: 360
                duration: 5000
            }

            NumberAnimation {
                target: trefoilKnot
                property: "phi"
                from: 0; to: 360
                duration: 5000
            }
        }
    }

    Mesh {
        id: matSphere
        source: "qrc:/assets/obj/material-sphere.obj"
    }

    Entity {
        id: sphere1

        components: [
            matSphere1Transform,
            matSphere,
            matSphere1Material
        ]

        Transform {
            id: matSphere1Transform
            translation: Qt.vector3d(-3, 0, 0)
            rotationY: -90
        }

        MetalRoughMaterial {
            id: matSphere1Material
            baseColor:  TextureLoader {
                source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png"
                format: Texture.SRGB8_Alpha8
                generateMipMaps: true
            }
            metalness: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png"; generateMipMaps: true }
            roughness: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_roughness.png"; generateMipMaps: true}
            normal: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_normal.png"; generateMipMaps: true }
            ambientOcclusion: TextureLoader { source: "qrc:/assets/textures/no-ao.png" }
        }
    }

    Entity {
        id: sphere2

        components: [
            matSphere2Transform,
            matSphere,
            matSphere2Material
        ]

        Transform {
            id: matSphere2Transform
            translation: Qt.vector3d(-1.5, 0, 0)
            rotationY: -90
        }

        MetalRoughMaterial {
            id: matSphere2Material
            baseColor:  TextureLoader {
                source: "qrc:/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_basecolor.png"
                format: Texture.SRGB8_Alpha8
                generateMipMaps: true
            }
            metalness: TextureLoader { source: "qrc:/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_metallic.png"; generateMipMaps: true }
            roughness: TextureLoader { source: "qrc:/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_roughness.png"; generateMipMaps: true }
            normal: TextureLoader { source: "qrc:/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_normal.png"; generateMipMaps: true }
            ambientOcclusion: TextureLoader { source: "qrc:/assets/textures/no-ao.png" }
        }
    }

    Entity {
        id: sphere3

        components: [
            matSphere3Transform,
            matSphere,
            matSphere3Material
        ]

        Transform {
            id: matSphere3Transform
            translation: Qt.vector3d(0, 0, 0)
            rotationY: -90
        }

        MetalRoughMaterial {
            id: matSphere3Material
            baseColor:  TextureLoader {
                source: "qrc:/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_basecolor.png"
                format: Texture.SRGB8_Alpha8
                generateMipMaps: true
            }
            metalness: TextureLoader { source: "qrc:/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_metallic.png"; generateMipMaps: true }
            roughness: TextureLoader { source: "qrc:/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_roughness.png"; generateMipMaps: true }
            normal: TextureLoader { source: "qrc:/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_normal.png"; generateMipMaps: true }
            ambientOcclusion: TextureLoader { source: "qrc:/assets/textures/no-ao.png" }
        }
    }

    Entity {
        id: sphere4

        components: [
            matSphere4Transform,
            matSphere,
            matSphere4Material
        ]

        Transform {
            id: matSphere4Transform
            translation: Qt.vector3d(1.5, 0, 0)
            rotationY: -90
        }

        MetalRoughMaterial {
            id: matSphere4Material
            baseColor:  TextureLoader {
                source: "qrc:/assets/textures/copper_brushed/copper_brushed_basecolor.png"
                format: Texture.SRGB8_Alpha8
                generateMipMaps: true
            }
            metalness: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_metallic.png"; generateMipMaps: true }
            roughness: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_roughness.png"; generateMipMaps: true }
            normal: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_normal.png"; generateMipMaps: true }
            ambientOcclusion: TextureLoader { source: "qrc:/assets/textures/no-ao.png" }
        }
    }

    Entity {
        id: sphere5

        components: [
            matSphere5Transform,
            matSphere,
            matSphere5Material
        ]

        Transform {
            id: matSphere5Transform
            translation: Qt.vector3d(3, 0, 0)
            rotationY: -90
        }

        MetalRoughMaterial {
            id: matSphere5Material
            baseColor:  TextureLoader {
                source: "qrc:/assets/textures/gold_leaf_waste/gold_leaf_waste_basecolor.png"
                format: Texture.SRGB8_Alpha8
                generateMipMaps: true
            }
            metalness: TextureLoader { source: "qrc:/assets/textures/gold_leaf_waste/gold_leaf_waste_metallic.png"; generateMipMaps: true }
            roughness: TextureLoader { source: "qrc:/assets/textures/gold_leaf_waste/gold_leaf_waste_roughness.png"; generateMipMaps: true }
            normal: TextureLoader { source: "qrc:/assets/textures/gold_leaf_waste/gold_leaf_waste_normal.png"; generateMipMaps: true }
            ambientOcclusion: TextureLoader { source: "qrc:/assets/textures/no-ao.png" }
        }
    }
}
