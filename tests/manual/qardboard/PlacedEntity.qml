// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property double angle: 0
    property double radius: 10
    property var mesh
    property alias color: material.diffuse
    property vector3d center: Qt.vector3d(0, 0, 0)

    components: [
        Transform {
            id: tr
            matrix: {
                var ra = 0// (root.angle) * Math.PI / 180
                var m = Qt.matrix4x4()
                m.rotate(root.angle, Qt.vector3d(0, 1, 0))
                m.translate(Qt.vector3d(root.center.x + root.radius * Math.sin(ra), root.center.y, root.center.z - root.radius * Math.cos(ra)))
                return m
            }
        }
    ]

    Entity {
        PhongMaterial {
            id: material
        }
        components: [ material, mesh ]
    }
}
