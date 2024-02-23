// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 12.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: camera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    KeyboardDevice {
        id: keyboardDevice
    }

    SphereEntity {
        id: sphere1
        position: Qt.vector3d(-5, 0, 0)
        color: "red"
        input.onTabPressed: sphere2.input.focus = true
        input.focus: true
        input.sourceDevice: keyboardDevice
    }

    SphereEntity {
        id: sphere2
        color: "green"
        input.onTabPressed: sphere3.input.focus = true
        input.sourceDevice: keyboardDevice
    }

    SphereEntity {
        id: sphere3
        position: Qt.vector3d(5, 0, 0)
        color: "blue"
        input.onTabPressed: sphere1.input.focus = true
        input.sourceDevice: keyboardDevice
    }
}
