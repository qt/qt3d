// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.2 as QQ2
import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.12
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

QQ2.Item {
    id: root
    width: 1280
    height: 720

    property int i: 0
    readonly property var sources: ["https://codereview.qt-project.org/gitweb?p=qt/qt3d.git;a=blob_plain;hb=refs/heads/dev;f=examples/qt3d/planets-qml/images/solarsystemscope/earthmap2k.jpg", "qrc:/image.jpg", "qrc:/image2.jpg", "qrc:/wrongpath.jpg"]
    readonly property url textureSource: sources[i]

    QQ2.Timer {
        interval: 3000
        repeat: true
        running: true
        onTriggered: i = (i + 1) % sources.length
    }

    Scene3D {
        anchors.fill: parent
        focus: true

        aspects: ["input", "logic"]

        Entity {
            id: sceneRoot

            readonly property var textureModel: [texture1, texture2, texture3, texture4]

            readonly property Texture texture1: TextureLoader {
                source: textureSource
            }

            readonly property Texture texture2: TextureLoader {
                source: textureSource
            }

            readonly property Texture texture3: Texture2D {
                format: Texture.RGBA8_UNorm
                textureImages: TextureImage {
                    source: textureSource
                }
            }

            readonly property Texture texture4: Texture2D {
                format: Texture.RGBA8_UNorm
                textureImages: TextureImage {
                    source: textureSource
                }
            }

            Camera {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: 16/9
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 0.0, 20.0, -40.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            OrbitCameraController {
                camera: camera
            }

            components: [
                RenderSettings {
                    activeFrameGraph: ForwardRenderer {
                        clearColor: Qt.rgba(0, 0.5, 1, 1)
                        camera: camera
                    }
                },
                // Event Source will be set by the Qt3DQuickWindow
                InputSettings { }
            ]

            CuboidMesh { id: mesh }

            NodeInstantiator {
                id: instantiator
                model: sceneRoot.textureModel

                Entity {
                    readonly property Transform transform: Transform {
                        readonly property real angle: model.index / instantiator.count * Math.PI * 2
                        translation: Qt.vector3d(Math.cos(angle) * 10, 0, Math.sin(angle) * 10)
                        scale: 10
                    }

                    readonly property DiffuseMapMaterial material: DiffuseMapMaterial {
                        diffuse: model.modelData
                    }
                    components: [ mesh, material, transform ]
                }
            }
        }
    }

    QQ2.Text {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        font.pointSize: 20
        text: "Source (" + (i + 1) + " of " + sources.length + "): " + textureSource
    }

    QQ2.ListView {
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        readonly property var targets: []
        readonly property var status: []

        spacing: 30
        model: sceneRoot.textureModel
        width: parent.width / 4
        delegate: QQ2.Grid {
            spacing: 10
            columns: 2
            QQ2.Text { text: "Target: " + nameMapper.targetName(model.modelData.target) }
            QQ2.Text { text: "Format: " + nameMapper.formatName(model.modelData.format.toString()) }
            QQ2.Text { text: "Width: " + model.modelData.width }
            QQ2.Text { text: "Height: " + model.modelData.height }
            QQ2.Text { text: "Depth: " + model.modelData.depth}
            QQ2.Text { text: "Layers: " + model.modelData.layers}
            QQ2.Text { text: "Status: " + nameMapper.statusName(model.modelData.status.toString()) }
            QQ2.Text { text: "HandleType: " + nameMapper.handleTypeName(model.modelData.handleType) }
            QQ2.Text { text: "Handle: " + model.modelData.handle }
        }
    }
}
