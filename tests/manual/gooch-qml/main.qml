// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.2 as QQ2

Entity {
    id: sceneRoot

    FirstPersonCameraController { camera: mainCamera }

    components: [
        RenderSettings {
            ForwardRenderer {
                camera: mainCamera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: mainCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 22.5
        aspectRatio: _window.width / _window.height
        nearPlane:   0.01
        farPlane:    1000.0
        position:   Qt.vector3d( 0.0, 0.0, 15.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
        upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
    }

    MyEntity {
        id: myEntity

        source: "assets/obj/trefoil.obj"
        scale: 1.0
        material.diffuse: Qt.rgba( 1.0, 0.75, 1.0, 1.0 )
        material.specular: Qt.rgba( 0.2, 0.2, 0.2, 1.0 )
        material.alpha: 0.2
        material.beta: 0.6
    }
}
