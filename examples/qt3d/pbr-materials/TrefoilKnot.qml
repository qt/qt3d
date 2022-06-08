// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.9

Entity {
    id: root

    property real x: 0.0
    property real y: 0.0
    property real z: 0.0
    property real scale: 1.0
    property real theta: 0.0
    property real phi: 0.0

    components: [ transform, mesh, material ]

    MetalRoughMaterial {
        id: material
        baseColor: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )
        metalness: 0.2
        roughness: 0.5
    }

    Transform {
        id: transform
        translation: Qt.vector3d(root.x, root.y, root.z)
        rotationX: theta
        rotationY: phi
        scale: root.scale
    }

    Mesh {
        id: mesh
        source: "qrc:/assets/obj/trefoil.obj"
    }
}
