// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0
import QtQuick.Scene2D 2.9
import QtQuick.Window 2.0 as QW2
import Qt3D.Extras 2.9


QQ2.Item {
    width: 1024
    height: 768

    Scene3D {
        id: scene
        focus: true
        anchors.fill: parent

        aspects: ["input", "logic"]

        Entity {
            id: sceneRoot

            Camera {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: scene.width / scene.height
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 0.0, 0.0, 6.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            Scene2D {
                id: qmlTexture
                output: RenderTargetOutput {
                    attachmentPoint: RenderTargetOutput.Color0
                    texture: Texture2D {
                        id: offscreenTexture
                        width: 1024
                        height: 1024
                        format: Texture.RGBA8_UNorm
                        generateMipMaps: true
                        magnificationFilter: Texture.Linear
                        minificationFilter: Texture.LinearMipMapLinear
                        wrapMode {
                            x: WrapMode.ClampToEdge
                            y: WrapMode.ClampToEdge
                        }
                    }

                }

                entities: [plane1]

                mouseEnabled: plane1.picker.pressed

                InteractiveGui {

                }
            }

            FirstPersonCameraController {
                id: controller
                camera: camera
                enabled: !plane1.picker.pressed
            }

            components: [
                RenderSettings {
                    pickingSettings.pickMethod: PickingSettings.TrianglePicking
                    activeFrameGraph:
                        ForwardRenderer {
                            camera: camera
                    }
                },
                InputSettings {}
            ]

            PlaneMesh {
                id: planeMesh
                width: 4
                height: 4
                mirrored: true
            }

            Entity {
                id: plane1

                property Transform transform: Transform {
                    scale: 1
                    translation: Qt.vector3d(0,0,0)
                    rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
                }

                property Material material: TextureMaterial {
                    texture: offscreenTexture
                }

                property ObjectPicker picker: ObjectPicker {
                    hoverEnabled: true
                    dragEnabled: true
                }

                components: [planeMesh, material, transform, picker]
            }
        }
    }
}
