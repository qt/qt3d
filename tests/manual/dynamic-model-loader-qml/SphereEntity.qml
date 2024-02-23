// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.4 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    components: [ mesh, material, transform ]

    SphereMesh {
        id: mesh
        radius: 3
    }

    Transform {
        id: transform
        scale: 3
    }

    QQ2.ColorAnimation {
        target: material
        property: "diffuse"
        from: "yellow"
        to: "orange"
        duration: 1500
        loops: QQ2.Animation.Infinite
        running: true
    }

    QQ2.NumberAnimation {
        target: transform
        property: "scale"
        duration: 1500
        loops: QQ2.Animation.Infinite
        running: true
        easing.type: QQ2.Easing.InOutQuad
        from: 1
        to: 1.5
    }
}
