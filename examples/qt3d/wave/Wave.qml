// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.16
import Qt3D.Render 2.16
import Qt3D.Extras 2.15

Entity {
    id: root

    property real x: 0.0
    property real y: 0.0
    property real z: 0.0
    property real scale: 1.0
    property real theta: 0.0
    property real phi: 0.0
    property color tint: Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
    property real intensity: 0.3
    property alias wireframe: material.wireframe
    property Layer layer: null

    components: [ transform, mesh, material, layer ]

    Transform {
        id: transform
        translation: Qt.vector3d(root.x, root.y, root.z);
        rotation: fromEulerAngles(root.theta, root.phi, 0)
        scale: root.scale
    }

    WaveMaterial {
        id: material
        ambient: Qt.rgba( root.tint.r, root.tint.g, root.tint.b, 1.0 ) // Set color tint
        diffuse: Qt.rgba( root.intensity, root.intensity, root.intensity, 1.0 ) // Set how bright the wave is
    }

    PlaneMesh {
        id: mesh
        width: 25.0
        height: 5.0
        meshResolution: Qt.size( 125, 50 )
    }
}
