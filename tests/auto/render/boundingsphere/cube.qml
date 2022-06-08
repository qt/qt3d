// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: sceneRoot

    components: [ ]

    // Parent Entity
    Entity {

        components: [
            CuboidMesh { id: testMesh; objectName: "testMesh" },
            PhongMaterial { id: material }
        ]
    }

}
