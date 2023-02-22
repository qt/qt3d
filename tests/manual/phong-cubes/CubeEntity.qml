// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.0 as Quick
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property vector3d position: Qt.vector3d()
    property Material material

    components: [mesh, material, transform]

    CuboidMesh {
        id: mesh
        xExtent: 0.5
        yExtent: xExtent
        zExtent: xExtent
    }

    Transform {
        id: transform
        translation: root.position
        rotationZ: 45

        Quick.NumberAnimation on rotationY {
            from: 0; to: 360
            loops: Quick.Animation.Infinite
            duration: 5000
        }
    }
}
