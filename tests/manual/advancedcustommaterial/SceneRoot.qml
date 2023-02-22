// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import QtQuick 2.0 as QQ2


Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane: 0.1
        farPlane: 1000.0
        position: Qt.vector3d(0.0, 4.0, 15.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, -2.0, 0.0)
    }

    FirstPersonCameraController {
        camera: camera
    }

    Entity {
        DirectionalLight {
            id: directional
            worldDirection: Qt.vector3d(0.3, -1.0, 5.0).normalized();
            color: "#fff2a3"
            intensity: 0.01
        }
        Transform {
            id: lightpostransform
            translation: Qt.vector3d(0.0, 50.0, 60.0)
        }
        components: [lightpostransform, directional]
    }

    Entity {
        PointLight {
            id: pointL
            color: "#fff2a3"
        }
        Transform{
            id: plightpostransform
            translation: Qt.vector3d(0.0, 4.0, 15.0)
        }
        components: [plightpostransform, pointL]
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                clearColor: "black"
                camera: camera
            }
        },
        InputSettings { }
    ]

    Water { }
}
