// Copyright (C) 2023 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.9
import QtQuick 2.0 as QQ2;

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 50.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController {
        camera: camera
        linearSpeed: 0.1 + camera.position.z
    }

    components: [
        RenderSettings {
            activeFrameGraph : ForwardRenderer {
                camera: camera
                clearColor: "black"
            }
        },
        InputSettings {
        }
    ]


    property bool odd: false
    QQ2.Timer {
        running: true
        interval: 1000
        onTriggered: {
            odd = !odd
        }
        repeat: true
    }

    NodeInstantiator {
        model: odd ? 5 : 1
        delegate: Entity {
            components: [
                Transform {
                    id: rot
                    translation: Qt.vector3d(-5, -20 + model.index * 5, 0)
                }
            ]

            Text2DEntity {
                id: text
                font.pointSize: 1
                text: "Text"
                width: 20
                height: 10
            }
        }
    }
}
