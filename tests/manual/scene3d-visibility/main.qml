// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick 2.14
import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Input 2.14
import Qt3D.Extras 2.14
import QtQuick.Scene3D 2.14

Item {
    id: root

    Timer {
        running: true
        repeat: true
        interval: 2000
        onTriggered: scene3d.visible = !scene3d.visible
    }

    // Scene3D
    Scene3D {
        id: scene3d
        visible: false
        readonly property double halfWidth: width * 0.5

        focus: true
        anchors.fill: parent
        // Make sure to define the input aspect if we want to handle inputs
        aspects: ["render", "input"]
        multisample: false

        Entity { // Root
            id: sceneRoot
            components: [
                RenderSettings {
                    activeFrameGraph: ForwardRenderer {
                        id: forwardRenderer
                        camera: mainCamera
                        clearColor: "yellow"
                    }
                },
                // Event Source is the Scene3D in that case
                InputSettings { }
            ]

            Camera {
                id: mainCamera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( 0.0, 0.0, -40.0 )
                upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            }

            PhongMaterial {
                id: material
            }

            TorusMesh {
                id: torusMesh
                radius: 5
                minorRadius: 1
                rings: 100
                slices: 20
            }

            SphereMesh {
                id: sphereMesh
                radius: 3
            }

            Transform {
                id: torusTransform
                scale3D: Qt.vector3d(1.5, 1, 0.5)
                rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
            }

            Transform {
                id: sphereTransform
                property real userAngle: 0.0
                matrix: {
                    var m = Qt.matrix4x4();
                    m.rotate(userAngle, Qt.vector3d(0, 1, 0));
                    m.translate(Qt.vector3d(20, 0, 0));
                    return m;
                }

                onWorldMatrixChanged: console.log("Sphere WorldMatrix " + worldMatrix)
            }

            NumberAnimation {
                target: sphereTransform
                property: "userAngle"
                duration: 1000
                from: 0
                to: 360
                loops: Animation.Infinite
                running: true
            }

            Entity {
                id: torusEntity
                components: [ torusMesh, material, torusTransform ]
            }
            Entity {
                id: sphereEntity
                components: [ sphereMesh, material, sphereTransform ]
            }
        }
    }
}
