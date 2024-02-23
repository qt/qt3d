// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.4 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 800/600
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer{
                camera: camera
                clearColor: Qt.rgba(0.0, 0.5, 1, 1)
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Entity {
        id: lightEntity
        property Transform transform: Transform {
            translation: Qt.vector3d(1.0, 1.0, 0.0)
        }

        property PointLight light: PointLight {
            id: light
            color: "white"
            constantAttenuation: 1.0
            linearAttenuation: 0.0
            quadraticAttenuation: 0.0
        }
        components: [transform, light]
    }

    TorusMesh {
        id: torusMesh
        radius: 5
        minorRadius: 1
        rings: 100
        slices: 20
    }

    SphereMesh {
        id: sphereMesh
        radius: 3
    }

    Transform {
        id: sphereTransform
        translation: Qt.vector3d(10, 0, 0)
    }

    Transform {
        id: cylinderTransform
        translation: Qt.vector3d(-10, 0, 0)
    }

    Entity {
        id: torusEntity
        components: [ torusMesh, phongMaterial ]
    }

    Entity {
        id: sphereEntity
        components: [ sphereMesh, alphaMaterial, sphereTransform ]
    }

    Entity {
        id: cylinderEntity
        components: [ sphereMesh, alphaMaterial, cylinderTransform ]
    }

    PhongAlphaMaterial {
        id: alphaMaterial
        shininess: 75.0
        ambient: "black"
        diffuse: "blue"
        specular: "white"

        QQ2.NumberAnimation {
            duration: 2000
            loops: QQ2.Animation.Infinite
            target: alphaMaterial
            property: "alpha"
            from: 0.0
            to: 1.0
            running: true
        }
    }

    PhongMaterial {
        id: phongMaterial
    }
}
