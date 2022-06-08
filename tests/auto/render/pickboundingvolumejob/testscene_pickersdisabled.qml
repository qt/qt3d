// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick.Window 2.0

Entity {
    id: sceneRoot

    Window {
        id: win
        width: 600
        height: 600
        visible: true
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -40.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    components: [
        RenderSettings {
            Viewport {
                normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

                RenderSurfaceSelector {

                    surface: win

                    ClearBuffers {
                        buffers : ClearBuffers.ColorDepthBuffer
                        NoDraw {}
                    }

                    CameraSelector {
                        camera: camera
                    }
                }
            }
        }
    ]

    CuboidMesh { id: cubeMesh }
    PhongMaterial { id: material }

    // Entity 1
    Entity {
        property ObjectPicker picker: ObjectPicker {
            objectName: "Picker1"
            enabled: false
        }

        property Transform transform: Transform {
            translation: Qt.vector3d(5, 0, 0)
            scale: 2.0
        }

        components: [cubeMesh, material, picker, transform]
    }

    // Entity 2
    Entity {
        property ObjectPicker picker: ObjectPicker {
            objectName: "Picker2"
            enabled: false
        }

        property Transform transform: Transform {
            translation: Qt.vector3d(-5, 0, 0)
            scale: 2.0
        }

        components: [cubeMesh, material, picker, transform]
    }
}
