// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.10
import QtQuick 2.0

Entity {
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: "white"
                camera: mainCamera
            }
        },
        InputSettings { }
    ]

    Camera {
        id: mainCamera
        position: Qt.vector3d(0.0, 0.0, 7.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    FirstPersonCameraController {
        camera: mainCamera
    }

    Entity {
        components: [
            PointLight {},
            Transform { translation: mainCamera.position }
        ]
    }

    CubeEntity {
        position: Qt.vector3d(-1, 1, 0)
        material: DiffuseSpecularMaterial {}
    }

    CubeEntity {
        position: Qt.vector3d(0, 1, 0)
        material: DiffuseSpecularMaterial {
            diffuse: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png" }
        }
    }

    CubeEntity {
        position: Qt.vector3d(1, 1, 0)
        material: DiffuseSpecularMaterial {
            diffuse: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png" }
            specular: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png" }
        }
    }

    CubeEntity {
        position: Qt.vector3d(-1, 0, 0)
        material: DiffuseSpecularMaterial {
            alphaBlending: true
            diffuse: Qt.rgba(0.7, 0.7, 0.7, 0.5)
        }
    }

    CubeEntity {
        position: Qt.vector3d(0, 0, 0)
        material: DiffuseSpecularMaterial {
            normal: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png" }
            diffuse: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png" }
        }
    }

    CubeEntity {
        position: Qt.vector3d(1, 0, 0)
        material: DiffuseSpecularMaterial {
            alphaBlending: true
            normal: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png" }
            diffuse: TextureLoader { source: "qrc:/assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png" }
        }
    }

    CubeEntity {
        position: Qt.vector3d(-1, -1, 0)
        material: DiffuseSpecularMaterial {
            normal: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_normal.png" }
            diffuse: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_basecolor.png" }
            specular: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_metallic.png" }
        }
    }

    CubeEntity {
        position: Qt.vector3d(0, -1, 0)
        material: DiffuseSpecularMaterial {
            normal: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_normal.png" }
            diffuse: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_basecolor.png" }
            specular: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_metallic.png" }
        }
    }

    CubeEntity {
        position: Qt.vector3d(1, -1, 0)
        material: DiffuseSpecularMaterial {
            normal: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_normal.png" }
            diffuse: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_basecolor.png" }
            specular: TextureLoader { source: "qrc:/assets/textures/copper_brushed/copper_brushed_metallic.png" }
        }
    }
}
