// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root
    property real xPos: 0
    property real yPos: 0
    property real zPos: 0
    property Material material

    SphereMesh {
        id: sphereMesh
        rings: 15
        slices: 16
        radius: 0.5
    }

    Transform {
        id: sphereTransform
        Translate {
            dx: xPos
            dy: yPos
            dz: zPos
        }
    }

    components: [
        sphereMesh,
        sphereTransform,
        material
    ]
}
