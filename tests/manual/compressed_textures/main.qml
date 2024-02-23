// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.12 as QQ2
import QtQuick.Scene3D 2.12
import QtQuick.Controls 2.12
import Qt3D.Core 2.12
import Qt3D.Render 2.12
import Qt3D.Input 2.12
import Qt3D.Extras 2.12

QQ2.Item {
    id: root
    width: 1280
    height: 720

    readonly property string textureSource: "file://" + _pathPrefix + "/" + textureSelector.textAt(textureSelector.currentIndex)

    Scene3D {
        anchors.fill: parent
        focus: true
        aspects: ["input", "logic"]

        Entity {
            id: sceneRoot

            Camera {
                id: camera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: 16/9
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d(0.0, 0.0, -2.0)
                upVector: Qt.vector3d(0.0, 1.0, 0.0)
                viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
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

            Entity {
                readonly property GeometryRenderer mesh: CuboidMesh { }
                readonly property DiffuseMapMaterial material: DiffuseMapMaterial {
                    diffuse: TextureLoader {
                        source: textureSource
                    }
                }
                components: [ mesh, material ]
            }
        }
    }

    ComboBox {
        id: textureSelector
        model: _texturesList
        width: 200
    }
}
