// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0 as QQ2;

Entity {
    id: sceneRoot

    property real width : 1
    property alias smooth : lineWidth.smooth

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 2.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    FirstPersonCameraController {
        camera: camera
        linearSpeed: 1.0
    }

    components: [
        RenderSettings {
            activeFrameGraph : RenderStateSet {
                renderStates: [
                    PointSize {
                        value: sceneRoot.width
                        sizeMode: PointSize.Fixed
                    },
                    LineWidth {
                        id: lineWidth
                        value: sceneRoot.width / 3.0
                    }

                ]

                ForwardRenderer {
                    camera: camera
                    clearColor: "black"
                }
            }
        },
        InputSettings {
        }
    ]

    SphereGeometry {
        id: sphere
        rings: 4
        slices: 4
        radius: 1
    }

    Entity {
        components: [
            GeometryRenderer {
                // abuse PlaneGeometry to give us some lines
                geometry: sphere
                primitiveType: GeometryRenderer.LineStrip
            },
            PhongMaterial {
                ambient: "white"
            }
        ]
    }

    Entity {
        components: [
            GeometryRenderer {
                // abuse PlaneGeometry to give us some points
                geometry: sphere
                primitiveType: GeometryRenderer.Points
            },
            PhongMaterial {
                ambient: "red"
            }
        ]
    }

}
