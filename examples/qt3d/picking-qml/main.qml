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
import QtQuick.Window 2.2 as W
import QtQuick.Scene3D 2.0

W.Window
{
    id: _view
    visible: true
    width: 1024
    height: 768

    Scene3D {
//        anchors.fill: parent
        id: sceneItem
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        width: parent.width / 2
        height: parent.height / 2
        focus: true
        aspects: "input"

        Entity {
            id: sceneRoot

            Camera {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: _view.width * 0.5 / _view.height
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 0.0, 0.0, -40.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            Camera {
                id: camera2
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: _view.width * 0.5 / _view.height
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 40.0, 5.0, -20.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            FirstPersonCameraController {
                camera: camera
            }

            // Draw 2 viewports
            // one with the content, the other with content + debug volumes
            components: FrameGraph {
                Viewport {
                    ClearBuffer {
                        buffers : ClearBuffer.ColorDepthBuffer
                        NoDraw {}
                    }
                    Viewport {
                        rect: Qt.rect(0.0, 0.0, 0.5, 1.0)
                        CameraSelector {
                            camera: camera
                            LayerFilter { layers: "content"}
                        }
                    }
                    Viewport {
                        rect: Qt.rect(0.5, 0.0, 0.5, 1.0)
                        CameraSelector {
                            camera: camera2
                            LayerFilter {
                                // To show Debug volumes
                                layers: ["content", "debug"]
                            }
                        }
                    }
                }
            }

            CuboidMesh { id: cubeMesh }

            PickableEntity {
                id: cube1
                property real scaleFactor: isPressed ? 3.0 : 1.0
                QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 150; easing.type: QQ2.Easing.InQuad } }

                mesh: cubeMesh
                scale: cube1.scaleFactor
                x: -8

                ambientColor: "green"
                diffuseColor: "green"

                hoverEnabled: true
                onPressed: cube1.diffuseColor = "orange"
                onReleased:  cube1.diffuseColor = "green"
                onEntered: cube1.ambientColor = "blue"
                onExited: cube1.ambientColor = "green"
                onClicked: console.log("Clicked cube 1")
            }

            PickableEntity {
                id: cube2
                mesh: cubeMesh

                ambientColor: cube2.containsMouse ? "blue" : "red"
                scale: 1.5
                hoverEnabled: true

                onPressed: cube2.diffuseColor = "white"
                onReleased: cube2.diffuseColor = "red"

                property bool toggled: false
                onClicked: {
                    console.log("Clicked cube 2")
                    toggled = !toggled
                    sceneItem.width = _view.width * (toggled ? .7 : .5)
                    sceneItem.height = _view.height * (toggled ? .7 : .5)
                    console.log(sceneItem.width, sceneItem.height)
                }
            }
            PickableEntity {
                id: cube3
                mesh: cubeMesh

                diffuseColor: "yellow"

                property bool toggled: false
                property real scaleFactor: toggled ? 5.0 : 0.5
                QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 200; easing.type: QQ2.Easing.InQuad } }

                scale: cube3.scaleFactor
                x: 8

                onPressed: cube3.toggled = !cube3.toggled
                onEntered: cube3.ambientColor = "black"
                onExited: cube3.ambientColor = "white"
                onClicked: console.log("Clicked cube 3")
            }

            Entity {
                readonly property ObjectPicker objectPicker: ObjectPicker {
                    hoverEnabled: true
                    onPressed: cube4.toggled = !cube4.toggled
                    onClicked: console.log("Clicked cube 4's parent Entity")
                    onEntered: cube4.material.diffuse = "white"
                    onExited: cube4.material.diffuse = "blue"
                }

                readonly property BoundingVolumeDebug debugVolume: BoundingVolumeDebug {
                    recursive: true
                }

                components: [objectPicker, debugVolume]

                Entity {
                    id: cube4
                    property bool toggled: false
                    property real scaleFactor: toggled ? 2.0 : 1.0
                    QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 200; easing.type: QQ2.Easing.InQuad } }

                    readonly property Transform transform: Transform {
                        scale: cube4.scaleFactor
                        translation: Qt.vector3d(3, 4, 0)
                    }
                    readonly property Layer layer: Layer { names: "content" }
                    readonly property BoundingVolumeDebug debugVolume: BoundingVolumeDebug {}
                    readonly property PhongMaterial material: PhongMaterial { diffuse: "red" }

                    components: [cubeMesh, transform, material, debugVolume, layer]
                }
            }
        }

        QQ2.Behavior on width { QQ2.NumberAnimation { duration: 200 } }
        QQ2.Behavior on height { QQ2.NumberAnimation { duration: 200 } }
    }
}
