/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

import Qt3D 2.0
import Qt3D.Renderer 2.0

Entity {
    id: root

    property real x: 0.0
    property real y: 0.0
    property real z: 0.0
    property real scale: 1.0
    property real theta: 0.0
    property real phi: 0.0
    property color tint: Qt.rgba( 0.0, 0.0, 0.0, 1.0 )
    property real intensity: 0.3
    property alias wireframe: material.wireframe

    components: [ transform, mesh, material, layer ]

    Layer {
        id: layer
        names: "wave"
    }

    Transform {
        id: transform
        Translate { dx: root.x; dy: root.y; dz: root.z }
        Scale { scale: root.scale }
        Rotate{ angle: root.theta; axis: Qt.vector3d(1.0, 0.0, 0.0) }
        Rotate{ angle: root.phi; axis: Qt.vector3d(0.0, 1.0, 0.0) }
    }

    WaveMaterial {
        id: material
        ambient: Qt.rgba( root.tint.r, root.tint.g, root.tint.b, 1.0 ) // Set color tint
        diffuse: Qt.rgba( root.intensity, root.intensity, root.intensity, 1.0 ) // Set how bright the wave is
    }

    PlaneMesh {
        id: mesh
        width: 25.0
        height: 5.0
        meshResolution: Qt.size( 125, 50 )
    }
}
