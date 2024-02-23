// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property string name: "Entity"
    property alias position: transform.translation
    property alias diffuse: material.diffuse

    components: [
        Transform { id: transform },
        SphereMesh { radius: 2 },
        PhongMaterial { id: material },
        ObjectPicker {
            onClicked: console.log("Clicked", root.name, pick.distance, pick.triangleIndex)
        }
    ]
}
