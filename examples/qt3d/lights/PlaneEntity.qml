// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property alias position: transform.translation
    property alias scale: transform.scale
    property alias width: mesh.width
    property alias height: mesh.height
    property alias resolution: mesh.meshResolution
    property Material material

    components: [ transform, mesh, root.material ]

    Transform { id: transform }

    PlaneMesh {
        id: mesh
        width: 1.0
        height: 1.0
        meshResolution: Qt.size(2, 2)
    }
}
