// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root
    property real convergence: 2000.0
    property real eyeSeparation: 35.0
    property real aspectRatio: _window.width / _window.height
    property real fieldOfView: 60.0
    property real nearPlane: 10.0
    property real farPlane: 10000.0

    property vector3d viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    property vector3d position: Qt.vector3d(0.0, 0.0, 1.0)
    property vector3d upVector: Qt.vector3d(0.0, 1.0, 0.0)

    readonly property real _fov2: Math.tan(fieldOfView * Math.PI / 180 * 0.5)
    readonly property real top: nearPlane * _fov2
    readonly property real a: aspectRatio * _fov2 * convergence

    CameraLens {
        id: leftEyeLens
        projectionType: CameraLens.FrustumProjection
        nearPlane : root.nearPlane
        farPlane : root.farPlane
        left: -(a - eyeSeparation * 0.5) * nearPlane / convergence
        right: (a + eyeSeparation * 0.5) * nearPlane / convergence
        top: root.top
        bottom: -root.top
    }

    CameraLens {
        id: rightEyeLens
        projectionType: CameraLens.FrustumProjection
        nearPlane : root.nearPlane
        farPlane : root.farPlane
        left: -(a + eyeSeparation * 0.5) * nearPlane / convergence
        right: (a - eyeSeparation * 0.5) * nearPlane / convergence
        top: root.top
        bottom: -root.top
    }

    Transform {
        id: eyeTransform

        matrix: {
            var m = Qt.matrix4x4();
            m.translate(root.position)
            var zAxis = root.position.minus(root.viewCenter).normalized()
            var xAxis = root.upVector.crossProduct(zAxis).normalized();
            var yAxis = zAxis.crossProduct(xAxis);
            var r = Qt.matrix4x4(xAxis.x, yAxis.x, zAxis.x, 0,
                                 xAxis.y, yAxis.y, zAxis.y, 0,
                                 xAxis.z, yAxis.z, zAxis.z, 0,
                                 0, 0, 0, 1)
            return m.times(r);
        }
    }

    components: [ eyeTransform ]

    property Entity leftCamera: Entity {
        components: [ leftEyeLens ]
    }

    property Entity rightCamera: Entity {
        id: rightCameraEntity
        components: [ rightEyeLens ]
    }
}
