// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Extras 2.9
import QtQuick.Window 2.0

Entity {
    id: sceneRoot

    Window {
        id: win
        width: 800
        height: 800
        visible: true
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    components: [
        RenderSettings {
            Viewport {
                normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

                RenderSurfaceSelector {

                    surface: win

                    ClearBuffers {
                        buffers : ClearBuffers.ColorDepthBuffer
                        NoDraw {}
                    }

                    CameraSelector {
                        camera: camera
                    }
                }
            }
        }
    ]

    Entity {
        components: [
            ShaderData {
                property vector3d eyePosition: Qt.vector3d(1.0, 1.0, 1.0);
                property int eyePositionTransformed: 0 // ModelToEye (enum aren't publicly exposed yet)
            },
            Transform {
                translation: Qt.vector3d(0.0, 5.0, 0.0)
            }
        ]
    }
}
