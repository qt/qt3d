// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import Qt3D.Core 2.0
import Qt3D.Render 2.9
import Qt3D.Input 2.0
import QtQuick 2.2 as QQ2
import QtQuick.Scene2D 2.9
import QtQuick.Window 2.0 as QW2
import Qt3D.Extras 2.9
import QtMultimedia 5.6 as QMM
import QtQuick.Dialogs 1.0

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _window.width / _window.height
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 3.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    Scene2D {
        output: RenderTargetOutput {
            attachmentPoint: RenderTargetOutput.Color0
            texture: Texture2D {
                id: offscreenTexture
                width: 1024
                height: 1024
                format: Texture.RGBA8_UNorm
                generateMipMaps: false
                magnificationFilter: Texture.Linear
                minificationFilter: Texture.Linear
                wrapMode {
                    x: WrapMode.ClampToEdge
                    y: WrapMode.ClampToEdge
                }
            }
        }
        QQ2.Rectangle {
            width: 400
            height: 300
            color: "green"

            QMM.MediaPlayer {
                id: player
                autoPlay: false
                loops: QMM.MediaPlayer.Infinite
            }

            QMM.VideoOutput {
                id: videoOutput
                source: player
                anchors.fill: parent
            }
        }
    }

    FirstPersonCameraController {
        camera: camera
    }

    components: [
        RenderSettings {
            activeFrameGraph:
                ForwardRenderer {
                    camera: camera
                }
        },
        InputSettings {}
    ]

    CuboidMesh {
        id: mesh
    }

    Entity {
        id: entity

        property Transform transform: Transform {
            scale: 1
            translation: Qt.vector3d(0,0,0)
        }

        property Material material: TextureMaterial {
            texture: offscreenTexture
        }

        components: [mesh, material, transform]
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a video"
        folder: shortcuts.home
        onAccepted: {
            visible = false
            player.source = fileDialog.fileUrl
            player.play()
        }
        onRejected: {
            Qt.quit()
        }
        QQ2.Component.onCompleted: {
            visible = true
        }
    }
}
