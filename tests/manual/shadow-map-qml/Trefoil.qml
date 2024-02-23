// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.1 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root
    property Material material

    Mesh {
        id: trefoilMesh
        source: "qrc:///assets/obj/trefoil.obj"
    }

    Transform {
        id: trefoilMeshTransform
        property real userAngle: 0.0
        rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
    }

    QQ2.NumberAnimation {
        target: trefoilMeshTransform

        running: true
        loops: QQ2.Animation.Infinite

        property: "userAngle"
        duration: 5000
        from: 360
        to: 0
    }

    components: [
        trefoilMesh,
        trefoilMeshTransform,
        material
    ]
}
