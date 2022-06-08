// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property Layer layer
    property Material visualMaterial;
    property real rotateAngle: 0.0
    property vector3d rotateAxis: Qt.vector3d(1.0, 0.0, 0.0)
    property vector3d center;
    property vector3d normal;
    readonly property vector4d equation: Qt.vector4d(normal.x,
                                                     normal.y,
                                                     normal.z,
                                                     -(normal.x * center.x +
                                                       normal.y * center.y +
                                                       normal.z * center.z))

    PlaneMesh {
        id: mesh
        width: 20.0
        height: 20.0
        meshResolution: Qt.size(2, 2)
    }

    Transform {
        id: transform
        translation: root.center
        rotation: fromAxisAndAngle(root.rotateAxis, root.rotateAngle)
    }

    components: [visualMaterial, mesh, transform, layer]
}

