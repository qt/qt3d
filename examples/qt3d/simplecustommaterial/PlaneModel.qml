// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    components: [transform, mesh, material]

    SimpleMaterial {
        id: material
        maincolor: "red"
    }

    Transform {
        id: transform
        rotationX: 45
    }

    PlaneMesh {
        id: mesh
        width: 1.0
        height: 1.0
        meshResolution: Qt.size(2, 2)
    }
}
