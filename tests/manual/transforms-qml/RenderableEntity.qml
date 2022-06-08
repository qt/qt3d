// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property Material material: null
    property alias meshSource: mesh.source
    property alias translation: transform.translation
    property alias rotation: transform.rotation
    property alias scale: transform.scale

    components: [ transform, mesh, material ]

    Transform {
        id: transform
    }

    Mesh {
        id: mesh
    }
}
