// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.4 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    components: [ mesh, phongMaterial, transform ]

    CuboidMesh {
        id: mesh
        yzMeshResolution: Qt.size(2, 2)
        xzMeshResolution: Qt.size(2, 2)
        xyMeshResolution: Qt.size(2, 2)
    }

    GoochMaterial {
        id: phongMaterial
    }

    Transform {
        id: transform
        property real userAngle: 0.0
        scale: 4
        rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), userAngle)
    }

    QQ2.ColorAnimation {
        target: phongMaterial
        property: "warm"
        from: "red"
        to: "blue"
        duration: 2500
        loops: QQ2.Animation.Infinite
        running: true
    }

    QQ2.NumberAnimation {
        target: transform
        property: "userAngle"
        duration: 750
        loops: QQ2.Animation.Infinite
        running: true
        easing.type: QQ2.Easing.InOutQuad
        from: 0
        to: 360
    }
}
