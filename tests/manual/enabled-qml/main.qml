// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.0 as QQ2

Entity {
    id: sceneRoot
    property int counter: 0

    components: [
        RenderSettings {
            activeFrameGraph: Viewport {
                normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)
                enabled: counter !== 5

                RenderSurfaceSelector {

                    ClearBuffers {
                        buffers : ClearBuffers.ColorDepthBuffer
                        clearColor: "white"
                        enabled: counter !== 6
                    }

                    CameraSelector {
                        camera: camera
                        enabled: counter !== 7

                        LayerFilter {
                            enabled: counter === 12
                            layers: sceneLayer
                        }
                    }
                }
            }
        }
    ]

    QQ2.Timer {
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            counter += 1;
            if (counter > 15)
                counter = 0;
        }
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        enabled: counter !== 4
        position: Qt.vector3d(0, 10, 10)
        viewCenter: Qt.vector3d(0, 0, 0)
        upVector: Qt.vector3d(0, 1, 0)
    }

    // Sphere
    Entity {
        SphereMesh {
            id: sphereMesh
            enabled: counter !== 0
        }

        PhongMaterial {
            id: material
            ambient: "dodgerblue"
            diffuse: "chartreuse"
            specular: "ghostwhite"
            enabled: counter !== 1
        }

        Layer {
            id: sceneLayer
            enabled: counter !== 2
        }

        Transform {
            id: transform
            property real x: 0.0
            property real y: 0.0
            property real z: 0.0
            translation: Qt.vector3d(x, y, z)
            enabled: counter !== 3
        }

        QQ2.SequentialAnimation {
            running: true
            loops: QQ2.Animation.Infinite
            QQ2.NumberAnimation {
                target: transform; property: "x"
                from: 0; to: 5;
                duration: 550
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: transform; property: "y"
                from: 0; to: 5;
                duration: 650
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: transform; property: "z"
                from: 0; to: 5;
                duration: 350
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: transform; property: "x"
                from: 5; to: 0;
                duration: 550
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: transform; property: "y"
                from: 5; to: 0;
                duration: 350
                easing.type: QQ2.Easing.InOutQuad
            }
            QQ2.NumberAnimation {
                target: transform; property: "z"
                from: 5; to: 0;
                duration: 650
                easing.type: QQ2.Easing.InOutQuad
            }
        }

        components: [sphereMesh, material, sceneLayer, transform]
    }

    // Floor
    Entity {
        PlaneMesh {
            id: planeMesh
            width: 25.0
            height: 10
        }

        PhongMaterial {
            id: floorMaterial
            ambient: "lawngreen"
            diffuse: "forestgreen"
            specular: "floralwhite"
            shininess: 0.5
        }

        Layer {
            id: floorLayer
        }

        components: [planeMesh, floorMaterial, floorLayer]
    }
}

