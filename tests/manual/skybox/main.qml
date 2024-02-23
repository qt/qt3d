// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    components: [
        RenderSettings {
            ForwardRenderer {
                camera: basicCamera
                clearColor: "black"
                // Note: FrustumCulling should be disabled for proper rendering of the Skybox
                frustumCulling: false
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: basicCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 60
        aspectRatio: 16/9
        nearPlane:   0.01
        farPlane:    1000.0
        position: Qt.vector3d( 0.0, 0.0, 40.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
        upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
    }

    // So that the camera is rendered always at the same position as the camera
    SkyboxEntity {
        baseName: "qrc:/assets/cubemaps/miramar/miramar"
        extension: ".webp"
    }

    FirstPersonCameraController { camera: basicCamera }

    Entity {
        components: [
            SphereMesh { radius: 5 },
            PhongMaterial { diffuse: "dodgerblue" }
        ]
    }
}

