// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property vector3d translation: Qt.vector3d(0.0, 0.0, 0.0)
    property real scale: 1.0
    property real theta: 0.0
    property real phi: 0.0
    property Material material

    components: [ transform, mesh, root.material ]

    Transform {
        id: transform
        translation: root.translation
        rotation: fromEulerAngles(theta, phi, 0)
        scale: root.scale
    }

    Mesh {
        id: mesh
        source: "assets/obj/trefoil.obj"
    }
}
