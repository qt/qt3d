// Copyright (C) 2021 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick.Window 2.0

Entity {
    id: sceneRoot

    Window {
        id: _view
        width: 600
        height: 600
        visible: true
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _view.width / 2 / _view.height
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -10.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Camera {
        id: camera2
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _view.width / 2 / _view.height
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -10.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController {
        camera: camera
    }

    DirectionalLight {
        worldDirection: camera.viewVector.times(-1)
    }

    // Draw 2 viewports
    components: [
        RenderSettings {
            RenderSurfaceSelector {
                surface: _view

                Viewport {
                    normalizedRect: Qt.rect(0.0, 0.0, 0.5, 1.0)
                    ClearBuffers {
                        buffers : ClearBuffers.ColorDepthBuffer
                        clearColor: "white"
                        CameraSelector {
                            camera: camera

                            LayerFilter {
                                layers: [ layer1 ]
                            }
                        }
                    }
                }

                Viewport {
                    normalizedRect: Qt.rect(0.5, 0.0, 0.5, 1.0)
                    CameraSelector {
                        camera: camera2

                        LayerFilter {
                            layers: [ layer2 ]
                        }
                    }
                }
            }
        }
    ]

    CuboidMesh { id: cubeMesh }

    Entity {
        readonly property ObjectPicker objectPicker: ObjectPicker {
            objectName: "Picker1"
            onClicked: console.log("o1")
        }
        readonly property Transform transform: Transform {
            scale: 4
        }
        readonly property PhongMaterial material: PhongMaterial { diffuse: "red" }
        readonly property Layer layer: Layer { id: layer1 }

        components: [cubeMesh, transform, material, objectPicker, layer ]
    }

    Entity {
        readonly property ObjectPicker objectPicker: ObjectPicker {
            objectName: "Picker2"
            onClicked: console.log("o2")
        }
        readonly property Transform transform: Transform {
            scale: 3
        }
        readonly property PhongMaterial material: PhongMaterial { diffuse: "green" }
        readonly property Layer layer: Layer { id: layer2 }

        components: [cubeMesh, transform, material, objectPicker, layer ]
    }
}
