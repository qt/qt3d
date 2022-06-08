// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import QtQuick 2.3 as QQ2
import Qt3D.Extras 2.0

Entity {
    id: root
    property alias position: transform.translation
    property alias color: material.diffuse
    property alias input: input

    PhongMaterial { id: material }

    SphereMesh { id: sphereMesh }

    Transform {
        id: transform
        scale: root.input.focus ? 2 : 1

        QQ2.Behavior on scale {
            QQ2.NumberAnimation {
                duration: 250
            }
        }
    }

    KeyboardHandler { id: input }

    components: [material, sphereMesh, transform, input]
}
