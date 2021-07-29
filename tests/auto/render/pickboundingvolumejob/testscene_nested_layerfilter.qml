/****************************************************************************
**
** Copyright (C) 2021 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
                        clearColor: "red"
                        CameraSelector {
                            camera: camera

                            LayerFilter {
                                // Any of
                                layers: [ layer1, layer3 ]

                                LayerFilter {
                                    filterMode: LayerFilter.DiscardAnyMatchingLayers
                                    layers: [layer2]
                                }
                            }
                        }
                    }
                }

                Viewport {
                    normalizedRect: Qt.rect(0.5, 0.0, 0.5, 1.0)
                    CameraSelector {
                        camera: camera2

                        LayerFilter {
                            layers: [ layerRecursive ]

                            LayerFilter {
                                filterMode: LayerFilter.DiscardAnyMatchingLayers
                                layers: [layer1]
                            }
                        }
                    }
                }
            }

            pickingSettings {
                pickMethod: PickingSettings.TrianglePicking
            }
        }
    ]

    CuboidMesh { id: cubeMesh }

    Layer { id: layerRecursive; recursive: true }
    Layer { id: layer1 }
    Layer { id: layer2 }
    Layer { id: layer3 }

    Entity {
        id: rootSceneEntity
        components: [layerRecursive]

        // Selected by Viewport 1
        Entity {
            readonly property ObjectPicker objectPicker: ObjectPicker {
                objectName: "Picker1"
                onClicked: console.log("o1 " + pick.position)
            }
            readonly property Transform transform: Transform {
                scale: 2
                translation: Qt.vector3d(-1, 0, 0)
            }
            readonly property PhongMaterial material: PhongMaterial { diffuse: "red" }

            components: [cubeMesh, transform, material, objectPicker, layer1 ]
        }

        // Selected by Viewport 2
        Entity {
            readonly property ObjectPicker objectPicker: ObjectPicker {
                objectName: "Picker2"
                onClicked: console.log("o2" + pick.position)
            }
            readonly property Transform transform: Transform {
                scale: 3
                translation: Qt.vector3d(-2, 3, 5)
            }
            readonly property PhongMaterial material: PhongMaterial { diffuse: "green" }

            components: [cubeMesh, transform, material, objectPicker, layer2 ]
        }

        // Not Selected by Any Viewport
        Entity {
            readonly property ObjectPicker objectPicker: ObjectPicker {
                objectName: "Picker3"
                onClicked: console.log("o3" + pick.position)
            }
            readonly property Transform transform: Transform {
                scale: 4
            }
            readonly property PhongMaterial material: PhongMaterial { diffuse: "blue" }

            components: [cubeMesh, transform, material, objectPicker, layer1, layer2 ]
        }

        // Both Viewport
        Entity {
            readonly property ObjectPicker objectPicker: ObjectPicker {
                objectName: "Picker4"
                onClicked: console.log("o4" + pick.position)
            }
            readonly property Transform transform: Transform {
                scale: 1
                translation: Qt.vector3d(0, -2, -1)
            }
            readonly property PhongMaterial material: PhongMaterial { diffuse: "orange" }

            components: [cubeMesh, transform, material, objectPicker, layer3 ]
        }

        // Viewport 1
        Entity {
            readonly property ObjectPicker objectPicker: ObjectPicker {
                objectName: "Picker5"
                onClicked: console.log("o5" + pick.position)
            }
            readonly property Transform transform: Transform {
                scale: 2
                translation: Qt.vector3d(0, 1, 3)
            }
            readonly property PhongMaterial material: PhongMaterial { diffuse: "purple" }

            components: [cubeMesh, transform, material, objectPicker, layer3, layer1 ]
        }
    }
}
