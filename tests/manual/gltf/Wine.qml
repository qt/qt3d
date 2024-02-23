// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property vector3d position: Qt.vector3d(0, 0, 0)
    property real angleX: 0
    property real angleY: 0
    property real angleZ: 0
    property real scale: 1

    components: [
        Transform {
            translation: root.position
            rotation: fromEulerAngles(root.angleX, root.angleY, root.angleZ)
            scale: root.scale
        },
        SceneLoader {
            source: "qrc:/assets/gltf/wine/wine.gltf"
        }
    ]
}
