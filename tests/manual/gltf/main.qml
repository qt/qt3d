// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 20.0, -120.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 10.0, 0.0 )
    }

    FirstPersonCameraController { camera: camera }

    RenderSettings {
        id : renderSettings
        activeFrameGraph : Viewport {
            CameraSelector {
                camera: camera
                ClearBuffers {
                    buffers : ClearBuffers.ColorDepthBuffer
                    clearColor: "black"
                    SortPolicy {
                        sortTypes: [ SortPolicy.BackToFront ]
                    }
                }
            }
        }
    }

    components: [ renderSettings ]

    Wine {
        id: wineRack
        position: Qt.vector3d(-60.0, 0.0, 50.0)
        angleX: 180
    }
}
