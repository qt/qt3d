// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property alias position: transform.translation
    property alias scale: transform.scale
    property alias material: gooch
    property alias source: mesh.source

    components: [
        Transform { id: transform },
        Mesh { id: mesh },
        GoochMaterial { id: gooch }
    ]
}
