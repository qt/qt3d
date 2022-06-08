// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.16
import Qt3D.Input 2.0
import Qt3D.Extras 2.16

import QtQuick 2.0 as QQ2
import QtQuick.Window 2.2 as W
import QtQuick.Scene3D 2.0

Entity {
    id: sceneRoot

    property Layer contentLayer: Layer {}
    property Layer debugLayer: Layer {}

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
        aspectRatio: _view.width * 0.5 / (_view.height * 0.5)
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 40.0, 5.0, -20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Camera {
        id: camera3
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _view.width * 0.5 / (_view.height * 0.5)
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
    components: [
        RenderSettings {
            Viewport {
                normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

                RenderSurfaceSelector {
                    ClearBuffers {
                        buffers : ClearBuffers.ColorDepthBuffer
                        NoDraw {}
                    }

                    Viewport {
                        normalizedRect: Qt.rect(0.0, 0.0, 0.5, 1.0)
                        CameraSelector {
                            camera: camera
                            LayerFilter { layers: sceneRoot.contentLayer }
                        }
                    }

                    Viewport {
                        normalizedRect: Qt.rect(0.5, 0.0, 0.5, 0.5)
                        CameraSelector {
                            camera: camera2
                            LayerFilter {
                                // To show Debug volumes
                                layers: [sceneRoot.contentLayer, sceneRoot.debugLayer]
                            }
                        }
                    }
                    Viewport {
                        normalizedRect: Qt.rect(0.5, 0.5, 0.5, 0.5)
                        CameraSelector {
                            camera: camera3
                            LayerFilter {
                                // To show Debug volumes
                                layers: [sceneRoot.contentLayer, sceneRoot.debugLayer]
                            }
                        }
                    }
                }
            }

            pickingSettings.pickMethod: PickingSettings.TrianglePicking
        },
        InputSettings {}
    ]

    CuboidMesh { id: cubeMesh }

    PickableEntity {
        id: cube1
        property real scaleFactor: isPressed ? 3.0 : 1.0
        QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 150; easing.type: QQ2.Easing.InQuad } }

        layer: sceneRoot.contentLayer
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
        layer: sceneRoot.contentLayer
        mesh: cubeMesh

        ambientColor: cube2.containsMouse ? "blue" : "red"
        scale: 1.5
        hoverEnabled: true

        onPressed: cube2.diffuseColor = "white"
        onReleased: cube2.diffuseColor = "red"

        property bool toggled: false
        onClicked: {
            console.log("Clicked cube 2", event.button)
            toggled = !toggled
        }
    }
    PickableEntity {
        id: cube3
        layer: sceneRoot.contentLayer
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

        components: [objectPicker]

        Entity {
            id: cube4
            property bool toggled: false
            property real scaleFactor: toggled ? 2.0 : 1.0
            QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 200; easing.type: QQ2.Easing.InQuad } }

            readonly property Transform transform: Transform {
                scale: cube4.scaleFactor
                translation: Qt.vector3d(3, 4, 0)
            }
            readonly property PhongMaterial material: PhongMaterial { diffuse: "red" }

            components: [cubeMesh, transform, material, sceneRoot.contentLayer]
        }
    }

    Entity {
        id: sphere1
        property bool toggled: false
        property real scaleFactor: toggled ? 2.0 : 1.0
        QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 200; easing.type: QQ2.Easing.InQuad } }

        readonly property ObjectPicker objectPicker: ObjectPicker {
            hoverEnabled: true
            onPressed: sphere1.toggled = !sphere1.toggled
            onClicked: console.log("Clicked sphere1")
            onEntered: sphere1.material.diffuse = "white"
            onExited: sphere1.material.diffuse = "yellow"
        }

        readonly property Transform transform: Transform {
            scale: sphere1.scaleFactor
            translation: Qt.vector3d(3, 6, 0)
        }
        readonly property PhongMaterial material: PhongMaterial { diffuse: "yellow" }

        readonly property GeometryRenderer sphereMesh: SphereMesh {  }

        components: [sphereMesh, transform, material, sceneRoot.contentLayer, objectPicker]
    }

    Entity {
        id: sphere2
        property bool toggled: false
        property real scaleFactor: toggled ? 2.0 : 1.0
        QQ2.Behavior on scaleFactor { QQ2.NumberAnimation { duration: 200; easing.type: QQ2.Easing.InQuad } }

        readonly property ObjectPicker objectPicker: ObjectPicker {
            hoverEnabled: true
            onPressed: sphere2.toggled = !sphere2.toggled
            onClicked: console.log("Clicked sphere2")
            onEntered: sphere2.material.diffuse = "white"
            onExited: sphere2.material.diffuse = "green"
        }
        readonly property PickingProxy pickingProxy: PickingProxy {
            view: CuboidGeometryView { xExtent: 2; yExtent: 2; zExtent: 2 }
        }

        readonly property Transform transform: Transform {
            scale: sphere2.scaleFactor
            translation: Qt.vector3d(-3, 6, 0)
        }
        readonly property PhongMaterial material: PhongMaterial { diffuse: "green" }

        readonly property GeometryRenderer sphereMesh: SphereMesh {  }

        components: [sphereMesh, transform, material, sceneRoot.contentLayer, objectPicker, pickingProxy]
    }
}
