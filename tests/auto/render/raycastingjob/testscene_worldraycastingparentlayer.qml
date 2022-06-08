// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQml 2.14
import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Extras 2.14
import QtQuick.Window 2.14

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
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 2.0, 20.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    components: [
        RenderSettings {
            pickingSettings.pickMethod: PickingSettings.TrianglePicking

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
        },
        RayCaster {
            id: caster
            objectName: "caster"
            length: 20
            layers: [ parentLayer ]
            filterMode: RayCaster.DiscardAnyMatchingLayers
        }
    ]

    CylinderMesh {
        id: mesh
        radius: 1
        length: 3
        rings: 10
        slices: 10
    }
    PhongMaterial { id: material }

    Entity {
        property Transform transform: Transform {
            translation: Qt.vector3d(-5, 0, 0)
        }

        components: [mesh, material, transform]
    }

    Entity {
        components: [ Layer { id: parentLayer; recursive: true } ]

        Entity {
            id: rightEntity

            property Transform transform: Transform {
                translation: Qt.vector3d(5, 0, 0)
            }

            components: [mesh, material, transform]
        }
    }
}
