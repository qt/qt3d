// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.16
import Qt3D.Render 2.16
import Qt3D.Input 2.0
import Qt3D.Extras 2.16

import QtQuick 2.0 as QQ2


Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            id: renderSettings
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "transparent"
            }
        },
        InputSettings { }
    ]

    // normal entity, BV computed from geometry
    Entity {
        id: sphereEntity1
        components: [
            SphereMesh { radius: 4 },
            PhongMaterial { diffuse: "blue" },
            Transform { translation: Qt.vector3d(mainAnimation.x, 9, 0) }
        ]
    }

    // override BV of the entity with one from another (non-rendered) geometry
    Entity {
        id: sphereEntity2
        components: [
            SphereMesh { radius: 4 },
            PhongMaterial { diffuse: "red" },
            Transform { translation: Qt.vector3d(mainAnimation.x, -9, 0) },
            BoundingVolume { view: SphereGeometryView { radius: 2 } }
        ]
    }

    // override BV with known specific values (can also be done by using a separate BoundingVolume)
    Entity {
        id: sphereEntity3
        components: [
            SphereMesh {
                radius: 4
                minPoint: Qt.vector3d(-.5, -.5, -.5)
                maxPoint: Qt.vector3d(.5, .5, .5)
            },
            PhongMaterial { diffuse: "green" },
            Transform { translation: Qt.vector3d(mainAnimation.x, 0 , 0) }
        ]
    }

    QQ2.SequentialAnimation {
        id: mainAnimation

        property real x: 0
        loops: QQ2.Animation.Infinite
        running: true

        QQ2.NumberAnimation {
            target: mainAnimation
            property: "x"
            duration: 2000
            from: -25
            to: 25
            easing.type: Easing.InOutQuad
        }

        QQ2.NumberAnimation {
            target: mainAnimation
            property: "x"
            duration: 2000
            from: 25
            to: -25
            easing.type: Easing.InOutQuad
        }
    }
}
