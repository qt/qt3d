// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Examples 1.0

Entity {
    id: root

    property real x: 0.0
    property real y: 0.0
    property real z: 0.0
    property real scale: 1.0
    property real theta: 0.0
    property Material material

    components: [ transform, mesh, root.material ]

    Transform {
        id: transform
        translation: Qt.vector3d(root.x, root.y, root.z)
        rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), root.theta)
        scale: root.scale
    }

    TessellatedQuadMesh {
        id: mesh
    }
}
