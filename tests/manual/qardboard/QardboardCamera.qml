// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qardboard 1.0

Entity {
    id: root
    property real convergence: 2000.0
    property real eyeSeparation: 35.0
    property real aspectRatio: _window.width / _window.height
    property real fieldOfView: 60.0
    property real nearPlane: 1.
    property real farPlane: 1000.0

    property vector3d position: Qt.vector3d(0.0, 0.0, 0.0)
    property vector3d viewDirection: Qt.vector3d(0.0, 0.0, 1.0)
    property vector3d viewUp: Qt.vector3d(0.0, 1.0, 0.0)
    property alias viewMatrix: eyeTransform.matrix

    readonly property real _fov2: Math.tan(fieldOfView * Math.PI / 180 * 0.5)
    readonly property real top: nearPlane * _fov2
    readonly property real a: aspectRatio * _fov2 * convergence

    DeviceOrientation {
        id: orientation
        enabled: true
    }

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

        function computeMatrix(roll, pitch, yaw) {
            var n = Qt.matrix4x4()
            n.rotate(-roll, Qt.vector3d(1, 0, 0))
            n.rotate(yaw, Qt.vector3d(0, 1, 0))
            var vd = n.times(stereoCamera.viewDirection)
            var vu = n.times(stereoCamera.viewUp)
            var vc = stereoCamera.position.minus(vd)

            var m = Qt.matrix4x4();
            m.translate(stereoCamera.position)
            var zAxis = stereoCamera.position.minus(vc).normalized()
            var xAxis = vu.crossProduct(zAxis).normalized();
            var yAxis = zAxis.crossProduct(xAxis);
            var r = Qt.matrix4x4(xAxis.x, yAxis.x, zAxis.x, 0,
                                 xAxis.y, yAxis.y, zAxis.y, 0,
                                 xAxis.z, yAxis.z, zAxis.z, 0,
                                 0, 0, 0, 1)
            return m.times(r);
        }

        matrix: computeMatrix(orientation.roll, orientation.pitch, orientation.yaw)
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
