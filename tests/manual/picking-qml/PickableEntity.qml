// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    signal pressed(var event)
    signal clicked(var event)
    signal released(var event)
    signal entered()
    signal exited()

    property Layer layer
    property real x: 0
    property real y: 0
    property real z: 0
    property alias scale: transform.scale
    property alias hoverEnabled: objectPicker.hoverEnabled
    property alias diffuseColor: material.diffuse
    property alias ambientColor: material.ambient
    property alias pickingEnabled: objectPicker.enabled
    readonly property bool containsMouse: objectPicker.containsMouse
    readonly property bool isPressed: objectPicker.pressed

    property GeometryRenderer mesh;

    ObjectPicker {
        id: objectPicker
        onClicked: root.clicked(pick)
        onPressed: root.pressed(pick)
        onReleased: root.released(pick)
        onEntered: root.entered()
        onExited: root.exited();
    }

    PhongMaterial { id: material }

    Transform {
        id: transform
        translation: Qt.vector3d(x, y, z)
    }

    components: [mesh, material, transform, objectPicker, layer]
}

