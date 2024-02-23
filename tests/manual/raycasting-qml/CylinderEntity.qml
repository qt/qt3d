// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property alias translation: transform.translation
    property alias color: material.diffuse

    components: [ mesh, material, transform ]

    CylinderMesh {
        id: mesh
        radius: 1
        length: 3
        rings: 100
        slices: 20
    }

    Transform {
        id: transform
        rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
    }

    PhongMaterial {
        id: material
        diffuse: "green"
    }
}
