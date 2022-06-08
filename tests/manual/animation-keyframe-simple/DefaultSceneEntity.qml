// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property Camera camera

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: root.camera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Entity {
        components: [
            PointLight { intensity: 0.5 },
            Transform { translation: camera.position }
        ]
    }
}
