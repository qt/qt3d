// Copyright (C) 2022 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 6.0 as QQ2
import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15
import QtQuick.Scene3D 2.15

QQ2.Item {
    width: 300
    height: 300

    Scene3D {
        id: scene3d
        anchors.fill: parent
        aspects: ["render", "input", "logic"]

        Entity {
            id: sceneRoot

            Camera {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: scene3d.width / scene3d.height
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
                    renderPolicy: RenderSettings.OnDemand
                },
                InputSettings { }
            ]

            Entity {
                components: [
                    PhongMaterial {},
                    TorusMesh {
                        radius: 5
                        minorRadius: 1
                        rings: 100
                        slices: 20
                    },
                    Transform {
                        scale3D: Qt.vector3d(1.5, 1, 0.5)
                        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
                    }
                ]
            }
        }
    }
}
