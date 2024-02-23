// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.9
import Qt3D.Core 2.10
import Qt3D.Render 2.10
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

Entity {
    id: sceneRoot

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, 150.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController { camera: camera }

    components: [
        RenderSettings {
            activeFrameGraph: RenderSurfaceSelector {
                Viewport {
                    normalizedRect: Qt.rect(0, 0, 1, 1)

                    ClearBuffers {
                        buffers: ClearBuffers.ColorDepthBuffer
                        clearColor: Qt.rgba(0.6, 0.6, 0.6, 1.0)
                        NoDraw {}
                    }

                    FrustumCulling {
                        CameraSelector {
                            camera: camera
                            ProximityFilter {
                                entity: proximityTarget
                                distanceThreshold: 30
                            }
                        }
                    }
                }
            }
        },
        // Event Source will be set by the Qt3DQuickWindow
        InputSettings { }
    ]

    SphereMesh {
        id: sphereMesh
    }

    PhongMaterial {
        id: phongMaterial
        diffuse: "orange"
    }

    NodeInstantiator {
        id: instantiator
        model: 64
        Entity {
            readonly property real angle: Math.PI * 2.0 * model.index % 8
            readonly property real radius: 20
            readonly property real verticalStep: 10
            readonly property color meshColor: Qt.hsla(model.index / instantiator.count, 0.5, 0.5, 1.0);

            readonly property Transform transform: Transform {
                translation: Qt.vector3d(radius * Math.cos(angle),
                                         (-(instantiator.count / (8 * 2)) + model.index / 8) * verticalStep,
                                         radius * Math.sin(angle))
            }
            readonly property Material material: Material {
                effect: phongMaterial.effect
                parameters: Parameter { name: "kd"; value: meshColor }
            }
            readonly property GeometryRenderer mesh: sphereMesh
            components: [ transform, mesh, material ]
        }
    }

    Entity {
        id: proximityTarget
        readonly property Transform transform: Transform
        {
            property real y: 0;
            SequentialAnimation on y {
                NumberAnimation { from: -50; to: 50; duration: 2000; easing.type: Easing.InOutQuart }
                NumberAnimation { from: 50; to: -50; duration: 2000; easing.type: Easing.InOutQuart }
                loops: Animation.Infinite
            }
            translation: Qt.vector3d(0.0, y, 0.0)
        }
        components: [ sphereMesh, phongMaterial, transform ]
    }
}
