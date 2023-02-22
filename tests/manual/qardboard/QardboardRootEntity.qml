// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

Entity {
    id: root
    property alias fieldOfView: stereoCamera.fieldOfView
    property alias cameraPosition: stereoCamera.position
    property alias cameraDirection: stereoCamera.viewDirection
    property alias headlightDirection: headLight.worldDirection

    components: [
        RenderSettings {
            activeFrameGraph: QardboardFrameGraph {
                id: stereoFrameGraph
                leftCamera: stereoCamera.leftCamera
                rightCamera: stereoCamera.rightCamera
            }
        },
        InputSettings { },
        DirectionalLight {
            id: headLight
            worldDirection: stereoCamera.viewDirection.times(-1)
        }
    ]

    QardboardCamera {
        id: stereoCamera
    }
}
