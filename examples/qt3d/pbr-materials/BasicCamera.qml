// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Camera {
    id: mainCamera
    objectName: "mainCamera"

    projectionType: CameraLens.PerspectiveProjection
    fieldOfView: 22.5
    aspectRatio: _window.width / _window.height
    onAspectRatioChanged: function(aspectRatio) { console.log( "aspectRatio = " + aspectRatio ) }
    nearPlane:   0.01
    farPlane:    1000.0
    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
}
