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
import Qt3D.Input 2.0
import QtQuick 2.0 as QQ2

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _view.width / _view.height
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Configuration {
        controlledCamera: camera
    }

    components: FrameGraph {
        ForwardRenderer {
            camera: camera
            clearColor: "black"
        }
    }

    SphereMesh {
        id: sphereMesh
    }

    Entity {
        id: sphere1

        property real scaleFactor: objectPicker.pressed ? 3.0 : 1.0
        QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 150; easing.type: QQ2.Easing.InQuad } }

        property Transform transform: Transform {
            scale: sphere1.scaleFactor
            translation: Qt.vector3d(-8, 0, 0)
        }

        property Material material: PhongMaterial {
            ambient: "green"
            diffuse: "green"
        }

        property ObjectPicker objectPicker: ObjectPicker {
            hoverEnabled: true

            onPressed: sphere1.material.diffuse = "orange"
            onReleased:  sphere1.material.diffuse = "green"

            onEntered: sphere1.material.ambient = "blue"
            onExited: sphere1.material.ambient = "green"

            onClicked: console.log("Clicked Sphere 1")
        }

        components: [sphereMesh, material, transform, objectPicker]
    }

    Entity {
        id: sphere2

        property Material material: PhongMaterial {
            ambient: sphere2.objectPicker.containsMouse ? "blue" : "red"
        }

        property Transform transform: Transform {
            scale: 1.5
        }

        property ObjectPicker objectPicker: ObjectPicker {
            hoverEnabled: true

            onPressed: sphere2.material.diffuse = "white"
            onReleased: sphere2.material.diffuse = "red"

            onClicked: console.log("Clicked Sphere 2")
        }

        components: [sphereMesh, material, transform, objectPicker]
    }

    Entity {
        id: sphere3

        property Material material: PhongMaterial {
            diffuse: "yellow"
        }

        property bool toggled: false
        property real scaleFactor: toggled ? 5.0 : 0.5
        QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 200; easing.type: QQ2.Easing.InQuad } }

        property Transform transform: Transform {
            scale: sphere3.scaleFactor
            translation: Qt.vector3d(8, 0, 0)
        }

        property ObjectPicker objectPicker: ObjectPicker {
            hoverEnabled: false

            onPressed: sphere3.toggled = !sphere3.toggled

            onEntered: sphere3.material.ambient = "black"
            onExited: sphere3.material.ambient = "white"

            onClicked: console.log("Clicked Sphere 3")
        }

        components: [sphereMesh, material, transform, objectPicker]
    }
}
