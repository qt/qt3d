/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: root
    signal pressed(var event)
    signal clicked(var event)
    signal released(var event)
    signal entered()
    signal exited()

    property real x: 0
    property real y: 0
    property real z: 0
    property alias scale: transform.scale
    property alias hoverEnabled: objectPicker.hoverEnabled
    property alias recursive: debugVolume.recursive
    property alias diffuseColor: material.diffuse
    property alias ambientColor: material.ambient
    readonly property bool containsMouse: objectPicker.containsMouse
    readonly property bool isPressed: objectPicker.pressed

    property GeometryRenderer mesh;

    ObjectPicker {
        id: objectPicker
        onClicked: root.clicked(event)
        onPressed: root.pressed(event)
        onReleased: root.released(event)
        onEntered: root.entered()
        onExited: root.exited();
    }

    BoundingVolumeDebug { id: debugVolume }
    PhongMaterial { id: material }

    Transform {
        id: transform
        translation: Qt.vector3d(x, y, z)
    }

    Layer {
        id: layer
        names: "content"
    }

    components: [mesh, material, transform, debugVolume, objectPicker, layer]
}

