// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property vector3d lightPosition: Qt.vector3d(30.0, 30.0, 0.0)
    property vector3d lightIntensity: Qt.vector3d(1.0, 1.0, 1.0)

    readonly property Camera lightCamera: lightCamera
    readonly property matrix4x4 lightViewProjection: lightCamera.projectionMatrix.times(lightCamera.matrix)

    Camera {
        id: lightCamera
        objectName: "lightCameraLens"
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 1280 / 768 //mainview.width / mainview.height
        nearPlane: 0.1
        farPlane: 200.0
        position: root.lightPosition
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }
}
