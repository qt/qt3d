// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Extras 2.10

Entity {
    id: root

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: mainCamera
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    Camera {
        id: mainCamera
        position: Qt.vector3d(1, 0.15, 0)
        viewCenter: Qt.vector3d(0, 0.15, 0)
        fieldOfView: 60
    }

    OrbitCameraController {
        camera: mainCamera
        linearSpeed: 10
        lookSpeed: 180
    }

    Entity {
        components: [
            PointLight {
                enabled: parent.enabled
                color: "black"
                intensity: 0
            },
            EnvironmentLight {
                enabled: parent.enabled

                irradiance: TextureLoader {
                    source: "qrc:/assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
                specular: TextureLoader {
                    source: "qrc:/assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
                    wrapMode {
                        x: WrapMode.ClampToEdge
                        y: WrapMode.ClampToEdge
                    }
                    generateMipMaps: false
                }
            }
        ]

        SkyboxEntity {
            baseName: "qrc:/assets/envmaps/cedar-bridge/cedar_bridge_irradiance"
            extension: ".dds"
        }
    }
}
