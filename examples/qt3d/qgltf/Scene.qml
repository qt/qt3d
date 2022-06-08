// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    Entity {
        components: [
            Transform {
                translation: Qt.vector3d(-30, 0, 0)
                rotation: fromEulerAngles(-90, 180, 0)
            },
            SceneLoader {
                source: "qrc:/models/test_scene.qgltf"
            }
        ]
    }

    Entity {
        components: [
            SceneLoader {
                source: "qrc:/models/toyplane.qgltf"
            }
        ]
    }

    Entity {
        components: [
            Transform {
                translation: Qt.vector3d(0, -20, 0)
            },
            SceneLoader {
                source: "qrc:/models/trefoil.qgltf"
            }
        ]
    }
}
