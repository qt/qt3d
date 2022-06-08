// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id: root

    property color ambient: Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
    property color diffuse: Qt.rgba( 0.7, 0.7, 0.7, 1.0 )
    property color specular: Qt.rgba( 0.95, 0.95, 0.95, 1.0 )
    property real shininess: 150.0
    property real lineWidth: 1.0
    property color lineColor: Qt.rgba( 1.0, 1.0, 1.0, 1.0 )
    property var innerTessLevel: [1.0, 1.0 ]
    property var outerTessLevel: [1.0, 1.0, 1.0, 1.0]

    parameters: [
        Parameter { name: "ambient"; value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
        Parameter { name: "diffuse"; value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
        Parameter { name: "specular"; value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
        Parameter { name: "shininess"; value: root.shininess },
        Parameter { name: "line.width"; value: root.lineWidth },
        Parameter { name: "line.color"; value: root.lineColor },
        Parameter { name: "inner[0]"; value: root.innerTessLevel },
        Parameter { name: "outer[0]"; value: root.outerTessLevel }
    ]
}
