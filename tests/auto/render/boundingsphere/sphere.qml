// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    components: [ ]

    // Parent Entity
    Entity {

        components: [
            SphereMesh { id: testMesh; objectName: "testMesh" },
            PhongMaterial { id: material }
        ]
    }

}
