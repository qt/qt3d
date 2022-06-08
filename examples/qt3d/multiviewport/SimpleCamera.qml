// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property vector3d position: Qt.vector3d(0.0, 0.0, 10.0)
    property vector3d viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    property vector3d upVector: Qt.vector3d(0.0, 1.0, 0.0)
    property CameraLens lens: null

    components: [lens, transform]

    Transform {
        id: transform
        matrix: {
            var m = Qt.matrix4x4();
            m.translate(root.position)
            var zAxis = root.position.minus(root.viewCenter).normalized()
            var xAxis = root.upVector.crossProduct(zAxis).normalized();
            var yAxis = zAxis.crossProduct(xAxis);
            var r = Qt.matrix4x4(xAxis.x, yAxis.x, zAxis.x, 0,
                                 xAxis.y, yAxis.y, zAxis.y, 0,
                                 xAxis.z, yAxis.z, zAxis.z, 0,
                                 0, 0, 0, 1)
            return m.times(r);
        }
    }
}
